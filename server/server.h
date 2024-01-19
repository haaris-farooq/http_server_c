#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

#define MAX_EVENTS 10
#define THREAD_POOL_SIZE 5
#define QUEUE_SIZE 10

typedef struct SocketQueue {
    int *sockets;
    int size;
    int front;
    int rear;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} SocketQueue;

typedef struct ThreadPool {
    pthread_t *threads;
    int thread_count;
    SocketQueue *socket_queue;
} ThreadPool;

typedef struct ThreadData {
    int client_socket;
} ThreadData;

ThreadPool *create_thread_pool(int thread_count);
void destroy_thread_pool(ThreadPool *pool);
SocketQueue *create_socket_queue(int size);
void destroy_socket_queue(SocketQueue *queue);
void enqueue_socket(SocketQueue *queue, int socket);
int dequeue_socket(SocketQueue *queue);
void *thread_pool_worker(void *arg);
int start_server(int port);

#endif /* SERVER_H */