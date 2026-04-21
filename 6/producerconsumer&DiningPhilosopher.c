#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define B 5
#define N 5

int buffer[B], in=0, out=0;
sem_t empty, full, mutex, forks[N];

void *producer(void *a){
    for(int i=0;i<10;i++){
        sem_wait(&empty); sem_wait(&mutex);
        buffer[in]=i;
        printf("Produced: %d at buffer[%d]\n", i, in);
        in=(in+1)%B;
        sem_post(&mutex); sem_post(&full);
        sleep(1);
    } return NULL;
}

void *consumer(void *a){
    for(int i=0;i<10;i++){
        sem_wait(&full); sem_wait(&mutex);
        printf("Consumed: %d from buffer[%d]\n", buffer[out], out);
        out=(out+1)%B;
        sem_post(&mutex); sem_post(&empty);
        sleep(2);
    } return NULL;
}



void *philosopher(void *a){
    int id=*(int*)a, left=id, right=(id+1)%N;
    for(int i=0;i<3;i++){
        printf("Philosopher %d is thinking.\n", id); sleep(1);

        if(id==N-1){
            sem_wait(&forks[right]);
            printf("Philosopher %d picked up right fork %d.\n", id, right);
            sem_wait(&forks[left]);
            printf("Philosopher %d picked up left fork %d.\n", id, left);
        } else {
            sem_wait(&forks[left]);
            printf("Philosopher %d picked up left fork %d.\n", id, left);
            sem_wait(&forks[right]);
            printf("Philosopher %d picked up right fork %d.\n", id, right);
        }

        printf("Philosopher %d is eating.\n", id); sleep(2);

        sem_post(&forks[left]); sem_post(&forks[right]);
        printf("Philosopher %d put down forks %d and %d.\n", id, left, right);
    } return NULL;
}


int main(){
    int ch; pthread_t p,c,ph[N]; int id[N];

    printf("\n1. Producer-Consumer\n2. Dining Philosophers\nChoice: ");
    scanf("%d",&ch);

    if(ch==1){
        sem_init(&empty,0,B); sem_init(&full,0,0); sem_init(&mutex,0,1);
        pthread_create(&p,0,producer,0); pthread_create(&c,0,consumer,0);
        pthread_join(p,0); pthread_join(c,0);
    }
    else if(ch==2){
        for(int i=0;i<N;i++) sem_init(&forks[i],0,1);
        for(int i=0;i<N;i++){ id[i]=i; pthread_create(&ph[i],0,philosopher,&id[i]); }
        for(int i=0;i<N;i++) pthread_join(ph[i],0);
    }
    else printf("Invalid choice!\n");

    return 0;
}
