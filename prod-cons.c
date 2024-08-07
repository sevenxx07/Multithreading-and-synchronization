#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

struct input_t{
    int num;
    char *word;
};
struct node{
    struct input_t data;
    struct node *next;
};
struct queue{
    int count;
    struct node *head;
    struct node *tail;
};

sem_t semFull;

pthread_mutex_t mutexList;
pthread_mutex_t mutexPrint;

int error_input = 0;
struct queue q;
int num_con;

void enqueue(struct input_t input){
    struct node *tmp;
    tmp = malloc(sizeof(struct node));
    tmp->data = input;
    tmp->next = NULL;
    if(q.tail != NULL){ // is not empty
        q.tail->next = tmp;
        q.tail = tmp;
    }else{
        q.head = q.tail = tmp;
    }
    q.count++;
}

struct input_t dequeue(){
    struct node *tmp;
    if(q.head == NULL){
        struct input_t t;
        return t;
    }
    struct input_t t = q.head->data;
    tmp = q.head;   
    q.head = q.head->next;
    q.count--;
    if(q.count == 0){
        q.head = NULL;
        q.tail = NULL;
    }
    free(tmp);
    return t;
}

void* producer() {
    int ret = 0;
    int x = -1;
    char *text = '\0';
    while ((ret = scanf("%d %ms", &x, &text)) == 2) {
        if(x < 0){
            pthread_mutex_lock(&mutexList);
            error_input = 1;
            free(text);
            pthread_mutex_unlock(&mutexList);
        }else{
            struct input_t input;
            input.num = x;
            input.word = text;

            pthread_mutex_lock(&mutexList);
            enqueue(input);
            sem_post(&semFull);
            pthread_mutex_unlock(&mutexList);
        }
    }
    if(ret != EOF){
        pthread_mutex_lock(&mutexList);
        error_input = 1;
        pthread_mutex_unlock(&mutexList);
    }
    for (int i = 0; i < num_con; i++){
       sem_post(&semFull);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (1) {
        sem_wait(&semFull);
        pthread_mutex_lock(&mutexList);
        if(q.tail == NULL){
            pthread_mutex_unlock(&mutexList);
            break;
        }

        struct input_t t = dequeue();

        pthread_mutex_unlock(&mutexList);
        pthread_mutex_lock(&mutexPrint); 

        printf("Thread %d:", *(int*)arg + 1);
        for(int j = 0; j < t.num; j++){
            printf(" %s", t.word);
        }
        printf("\n");
        free(t.word);

        pthread_mutex_unlock(&mutexPrint);
    }
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    q.count = 0;
    q.head = NULL;
    q.tail = NULL;
    pthread_mutex_init(&mutexPrint, NULL);
    pthread_mutex_init(&mutexList, NULL);
    sem_init(&semFull, 0, 0);

    //getting how many consuments should program have
    char *p;
    if(argv[1] != NULL){
        long arg = strtol(argv[1], &p, 10);
        num_con = arg;
    }else{
        num_con = 1;
    }
    if(num_con < 1 || num_con > sysconf(_SC_NPROCESSORS_ONLN)){
        return 1;
    }

    //initializing threads
    pthread_t producent;
    if (pthread_create(&producent, NULL, &producer, NULL) != 0) {
            return 1;
    }
    pthread_t consuments[num_con];
    for (int i = 0; i < num_con; i++) {
        int* a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&consuments[i], NULL, &consumer, a) != 0) {
            return 1;
        }
    }
    //joining threads
    if (pthread_join(producent, NULL) != 0) {
        return 1;
    }
    
    for (int i = 0; i < num_con; i++) {
        if (pthread_join(consuments[i], NULL) != 0) {
            return 1;
        }
        fprintf(stderr, "Thread %d has finished execution\n", i);
    }

    sem_destroy(&semFull);
    pthread_mutex_destroy(&mutexList);
    pthread_mutex_destroy(&mutexPrint);
    return error_input;
}