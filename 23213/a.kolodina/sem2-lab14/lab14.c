#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t parent_semaphore, child_semaphore;

void* child_thread(void* arg) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&child_semaphore); 
        printf("Child thread %d\n", i + 1);
        sem_post(&parent_semaphore);
    }
    return NULL;
}

int main() {
    pthread_t thread;
    sem_init(&parent_semaphore, 0, 1);  
    sem_init(&child_semaphore, 0, 0); 

    pthread_create(&thread, NULL, child_thread, NULL);

    for (int i = 0; i < 10; i++) {
        sem_wait(&parent_semaphore); 
        printf("Parent thread %d\n", i + 1);
        sem_post(&child_semaphore); 
    }

    pthread_join(thread, NULL);
    sem_destroy(&parent_semaphore);
    sem_destroy(&child_semaphore);
    pthread_exit(NULL);
}
