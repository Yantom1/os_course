#include "segel.h"
#include "request.h"
#include "queue.h"
#include <pthread.h>

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

pthread_mutex_t m;
pthread_cond_t cond;
pthread_cond_t block_cond;

Queue wait_queue = NULL;
Queue worker_queue = NULL;

int* thread_static;
int* thread_dynamic;
int* thread_total;

void getargs(int *port, int *thread_num, int *queue_size, char* schedalg , int argc, char *argv[])
{
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *thread_num = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    strcpy(schedalg, argv[4]);
}

void* thread_main(void* args){
    int thread_index =((int *)args)[0];

    while(1){
        pthread_mutex_lock(&m);
        while(queue_empty(wait_queue)){
            pthread_cond_wait(&cond, &m);
        }
        // handle
        struct timeval arrival = queue_head_arrival_time(wait_queue);
        int fd = dequeue(wait_queue);
        enqueue(worker_queue, fd, arrival);
        pthread_mutex_unlock(&m);

        struct timeval handle_time;
        gettimeofday(&handle_time,NULL);

        requestHandle(fd, arrival, handle_time, thread_static, thread_dynamic, thread_total, thread_index);
        Close(fd);

        pthread_mutex_lock(&m);
        dequeue_index(worker_queue, queue_find(worker_queue,fd));
        pthread_cond_signal(&block_cond);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // TODO: input check for new inputs?
    int listenfd, connfd, port, clientlen, threads_num, queue_max_size;
    struct sockaddr_in clientaddr;
    char schedalg[7];
    getargs(&port, &threads_num, &queue_max_size, schedalg , argc, argv);

    //queues create
    wait_queue = queue_create(queue_max_size);
    worker_queue = queue_create(threads_num);

    //threads create
    pthread_t* threads = malloc(sizeof(*threads)*threads_num);
    for(int i = 0; i< threads_num; i++){
        int* thread_args = {i};
        pthread_create(&threads[i], NULL, thread_main, (void *)thread_args);
    }
    thread_dynamic = malloc(sizeof(int)*threads_num);
    thread_static = malloc(sizeof(int)*threads_num);
    thread_total = malloc(sizeof(int)*threads_num);
    for(int i =0; i < threads_num; i++){
        thread_dynamic[i] = 0;
        thread_static[i] = 0;
    }

    //mutex create
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&block_cond, NULL);

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);



    pthread_mutex_lock(&m);
    if(queue_size(worker_queue) + queue_size(wait_queue) == queue_max_size){
        if(strcmp(schedalg, "block") == 0){
            while(queue_size(worker_queue) + queue_size(wait_queue) == queue_max_size){
                pthread_cond_wait(&block_cond, &m);
            }
        } else if(strcmp(schedalg, "dh") == 0){
            if(queue_empty(wait_queue)){
                Close(connfd);
                pthread_mutex_unlock(&m);
                continue;
            }
             else {
                 int fd = dequeue(wait_queue);
                 Close(fd);
            }
        } else if(strcmp(schedalg, "random") == 0){
            if(queue_empty(wait_queue)){
                Close(connfd);
                pthread_mutex_unlock(&m);
                continue;
            }
            else {
                int num_to_drop = queue_size(wait_queue) / 2;
                for(int i = 0; i < num_to_drop; i++) {
                    int random_index = rand() % queue_size(wait_queue);
                    int fd = dequeue_index(wait_queue, random_index);
                    Close(fd);
                }
            }
        } else if(strcmp(schedalg, "dt") == 0){
            Close(connfd);
            pthread_mutex_unlock(&m);
            continue;
        }
    }
    struct timeval arrival;
    gettimeofday(&arrival,NULL);

    enqueue(wait_queue, connfd, arrival);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&m);
    }

}


    


 
