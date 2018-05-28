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
#define clear() printf("\033[H\033[J")

#define num_max_cells    40
#define num_max_bacteria 40
#define num_white_cells 4
#define initial_cells_count 10
#define initial_bacteria_count 10

semaphore cell_semaphore;
semaphore bacteria_semaphore;

pthread_mutex_t cells_count_lock     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bacteria_count_lock  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resources_lock       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cell_action_lock     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bacteria_action_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_lock           = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t white_kill_lock      = PTHREAD_MUTEX_INITIALIZER;
// A variavel de condição sinaliza se o corpo precisa de recursos
pthread_cond_t needing_resources     = PTHREAD_COND_INITIALIZER;
// Body resources define the shared resources by cells and bacteria
int num_body_resources  = 100;
int cells_count         = 0;
int bacteria_count      = 0;
int white_cell_killings = 0;

int produce_random_number(){
    return 2*(rand()%10 + 5);
}

void print_system_state(){
    lock(&print_lock);
    clear();
    printf("\tCell count:                 %d\n", cells_count);
    printf("\tBacteria count:             %d\n",bacteria_count);
    printf("\tResources count:            %d\n",num_body_resources);

    lock(&white_kill_lock);
    printf("\tWhite cells intend to kill: %d\n",white_cell_killings);
    unlock(&white_kill_lock);

    if(bacteria_count == num_max_bacteria){
        printf("\tINFECÇÃO DOMINOU O CORPO: aperte qualquer tecla para encerrar a execução");
        getchar();
        exit(0);
    }
    if(bacteria_count == 0 || cells_count == num_max_bacteria){
        printf("\tCORPO GANHOU DA INFECÇÃO: aperte qualquer tecla para encerrar a execução");
        getchar();
        exit(0);
    }
    unlock(&print_lock);
}
void * white_cell_action(void * arg) {
    sleep(1);
    while(1){
        lock(&white_kill_lock);
        // Pretende matar 2 bacterias
        white_cell_killings++;
        unlock(&white_kill_lock);
        sleep(2);
    }    
    
}
void * bacteria_action(void * arg) {
    int id = *((int *) arg);
    espera_bacteria:
    down(&bacteria_semaphore);

    lock(&bacteria_count_lock);
    bacteria_count++;
    unlock(&bacteria_count_lock);

    while(1){
        lock(&bacteria_action_lock);
        if(num_body_resources <= 0){
            // Bacteria morre de fome
            bacteria_count--;
            unlock(&bacteria_action_lock);
            print_system_state();
            signal(&needing_resources);
            goto espera_bacteria;   
        }
        if(white_cell_killings > 0){
            // Se o macrofago precisa matar...
            lock(&white_kill_lock);
            white_cell_killings--;
            unlock(&white_kill_lock);

            bacteria_count--;
            unlock(&bacteria_action_lock);
            print_system_state();
            
            goto espera_bacteria;
        }
        else{
            // Se já não estiver no número máximo de bacterias faça a mitose
            if(bacteria_count < num_max_bacteria){
                // Bacteria usa um recurso para fazer mitose
                num_body_resources--;
                up(&bacteria_semaphore);                
                sleep(1);          
            }
        }
        print_system_state();  
        unlock(&bacteria_action_lock);
        sleep(1);
    }
}

void * cell_action(void * arg){
    int id = *((int *) arg);
    espera_celula:
    down(&cell_semaphore);
    
    lock(&cells_count_lock);
        cells_count++;        
    unlock(&cells_count_lock);
    while(1){        
        lock(&cell_action_lock);
        if(num_body_resources <= 0) {
            // Célula morre se não há recurso para ela            
            lock(&cells_count_lock);
                cells_count--;                
            unlock(&cells_count_lock);            
            print_system_state();
            unlock(&cell_action_lock);
            // Volta para estado de espera_celula
            goto espera_celula;
        }
        else {
            if(num_body_resources < 30){
                // Avisa que o corpo precisa de recursos                
                signal(&needing_resources);
            }
            // Se já não estiver no número máximo de células faça a mitose
            if(cells_count < num_max_cells){
                // Célula usa dois recursos para fazer mitose
                lock(&resources_lock);
                num_body_resources -= 2;
                unlock(&resources_lock);                
                up(&cell_semaphore);
            }
            sleep(1);
        }
        print_system_state();
        sleep(1);
        unlock(&cell_action_lock);            
    }
    pthread_exit(0);
}

void * system_producer_action(void * arg) {
    while(1){
        lock(&resources_lock);
            while (num_body_resources > 0){
                // Espera enquanto não for necessário
                wait(&needing_resources,&resources_lock);
            }
            
            num_body_resources = produce_random_number();
            printf("\tCorpo produzindo %d recursos...\n",num_body_resources, cells_count );            
            if(cells_count == 0){
                up(&cell_semaphore);
            }
            sleep(2);
            
        unlock(&resources_lock);
    }      
}

int main() {
    int i;
    int *id;

    time_t t;
    srand((unsigned) time(&t));

    pthread_t cells[num_max_cells];
    pthread_t bacteria[num_max_bacteria];
    pthread_t white_cells[num_white_cells];
    pthread_t system_producer;

    // Initialize semaphores
    sem_init(&cell_semaphore,    0, initial_cells_count);
    sem_init(&bacteria_semaphore,0, initial_bacteria_count);

    pthread_create(&system_producer, NULL, system_producer_action,(void *) (0));

    for (i = 0; i < num_max_cells ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&cells[i], NULL, cell_action,(void *) (id));
    }
    for (i = 0; i < num_max_bacteria ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&bacteria[i], NULL, bacteria_action,(void *) (id));
    }
    for (i = 0; i < num_white_cells ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&white_cells[i], NULL, white_cell_action,(void *) (id));
    }
    for (i = 0; i < num_max_bacteria ; i++) {
        pthread_join(cells[i],NULL);    
    }

    return 0;
}