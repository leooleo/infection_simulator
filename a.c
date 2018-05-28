#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void print_system_state(){
    int i = 0;
    int j = 1;
    while(1){
        i++;
        j++;
        printf("i: %d j: %d\r",i,j);
        fflush(stdout);            
        sleep(1);
    }
}
void main(){
    print_system_state();
}