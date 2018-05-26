/*
  Universidade de Brasília
  aluno:     Léo Moraes da Silva
  matrícula: 16/0011795
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define lock   pthread_mutex_lock
#define unlock pthread_mutex_unlock
#define wait   pthread_cond_wait
#define signal pthread_cond_signal
#define broadcast pthread_cond_broadcast
#define semaphore sem_t
#define down sem_wait
#define up   sem_post
#define try_down sem_trywait

#define num_max_cells 35
#define initial_cells_count 1
// Body resources define the shared resources by cells and bacteria
int num_body_resources = 3;
int cells_count = 0;

semaphore cell_semaphore;
pthread_mutex_t resources_lock   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cells_count_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t action = PTHREAD_MUTEX_INITIALIZER;
// A variavel de condição sinaliza se o corpo precisa de recursos
pthread_cond_t needing_resources = PTHREAD_COND_INITIALIZER;

int produce_random_number(){  
    return (rand()%5 + 1);
}

void * cell_action(void * arg){
    int id = *((int *) arg);
    down(&cell_semaphore);
    printf("Célula %dº spawna\n",id);
    
    lock(&cells_count_lock);
        cells_count++;
        printf("Cell count %d\n",cells_count);
    unlock(&cells_count_lock);
    while(1){        
        lock(&action);
        if(num_body_resources <= 0) {
            // Célula morre se não há recurso para ela
            printf("Célula %dº morre por falta de recursos\n",id);
            lock(&cells_count_lock);
                cells_count--;
                printf("Cell count %d\n",cells_count);
            unlock(&cells_count_lock);

            // Avisa que o corpo precisa de recursos
            signal(&needing_resources);
            unlock(&action);
            break;            
        }
        else {
            lock(&resources_lock);
                num_body_resources--;
                printf("Celula %dº fazendo mitose(Estado recursos atual: %d)\n",id,num_body_resources);
                up(&cell_semaphore);
            unlock(&resources_lock);
            sleep(1);
        }
        unlock(&action);            
    }
    pthread_exit(0);
}

void * system_producer_action(void * arg) {
    while(1){
        lock(&action);
            while (num_body_resources > 0){
                // Espera enquanto não for necessário
                wait(&needing_resources,&action);
            }
            lock(&resources_lock);
            num_body_resources = produce_random_number();
            printf("Corpo produziu %d recursos\n",num_body_resources );                    
            sleep(2);
            unlock(&resources_lock);
        unlock(&action);
    }
        
}

int main() {
    int i;
    int *id;

    time_t t;
    srand((unsigned) time(&t));

    pthread_t cells[num_max_cells];
    pthread_t system_producer;

    // Initialize semaphores
    sem_init(&cell_semaphore,0,initial_cells_count);

    pthread_create(&system_producer, NULL, system_producer_action,(void *) (0));
    for (i = 0; i < num_max_cells ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&cells[i], NULL, cell_action,(void *) (id));
    }
    for (i = 0; i < num_max_cells ; i++) {
        pthread_join(cells[i],NULL);    
    }

    return 0;
}