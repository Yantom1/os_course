#include "queue.h"
#include <stdio.h>

int main(){
    Queue queue  = queue_create(3);
    queue_print(queue);
    if(queue_empty(queue)){
        printf("empty\n");
    } else {
        printf("not empty\n");
    }
    enqueue(queue, 2);
    enqueue(queue, 3);
    queue_print(queue);

    if(queue_empty(queue)){
        printf("empty\n");
    } else {
        printf("not empty\n");
    }

    if(queue_full(queue)){
        printf("full\n");
    } else {
        printf("not full\n");
    }


    dequeue(queue);
    queue_print(queue);
    dequeue(queue);
    dequeue(queue);

    for(int i = 0; i < 6; i++){
        enqueue(queue, 6);
        queue_print(queue);
        if(queue_full(queue)){
            printf("full\n");
        } else {
            printf("not full\n");
        }
    }

    queue_destroy(queue);

    Queue queue2 = queue_create(4);
    enqueue(queue2, 3);
    dequeue_index(queue2, 0);
    queue_print(queue2);

    enqueue(queue2, 4);
    enqueue(queue2, 5);
    enqueue(queue2, 6);
    queue_print(queue2);

    dequeue_index(queue2 ,2);
    dequeue_index(queue2, 2);
    queue_print(queue2);
    dequeue_index(queue2,1);
    queue_print(queue2);
    dequeue_index(queue2, 0);
    queue_print(queue2);
    queue_destroy(queue2);
    return 0;
}