#include "server.h"
#include "connection.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define THREAD_POOL_SIZE 5
#define QUEUE_SIZE 10

volatile sig_atomic_t server_running = 1;
pthread_mutex_t server_running_mutex;

ThreadPool *threadPool;

void handle_signal(int signal)
{
    pthread_mutex_lock(&server_running_mutex);
    server_running = 0;
    pthread_mutex_unlock(&server_running_mutex);
    pthread_cond_broadcast(&threadPool->socket_queue->cond); // Wake up all waiting threads
}

// Function that will be executed in a separate thread
void *handle_connection_thread(void *arg)
{
    struct ThreadData *threadData = (struct ThreadData *)arg;
    handle_connection(threadData->client_socket);
    free(threadData);
    pthread_exit(NULL);
}

ThreadPool *create_thread_pool(int thread_count) {
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    if (pool == NULL) {
        return NULL;
    }

    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }

    pool->thread_count = thread_count;
    pool->socket_queue = NULL; // This will be set later

    return pool;
}

void destroy_thread_pool(ThreadPool *pool) {
    for (int i = 0; i < pool->thread_count; ++i) {
        if (pthread_join(pool->threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread. Continuing...\n");
        }
    }
    free(pool->threads);
    free(pool);
}

SocketQueue *create_socket_queue(int size) {
    SocketQueue *queue = (SocketQueue *)malloc(sizeof(SocketQueue));
    if (queue == NULL) {
        return NULL;
    }

    queue->sockets = (int *)malloc(sizeof(int) * size);
    if (queue->sockets == NULL) {
        free(queue);
        return NULL;
    }

    queue->size = size;
    queue->front = 0;
    queue->rear = 0;
    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        free(queue->sockets);
        free(queue);
        return NULL;
    }
    if (pthread_cond_init(&queue->cond, NULL) != 0) {
        pthread_mutex_destroy(&queue->lock);
        free(queue->sockets);
        free(queue);
        return NULL;
    }

    return queue;
}

void destroy_socket_queue(SocketQueue *queue) {
    free(queue->sockets);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}

void enqueue_socket(SocketQueue *queue, int socket) {
    pthread_mutex_lock(&queue->lock);

    queue->sockets[queue->rear] = socket;
    queue->rear = (queue->rear + 1) % queue->size;

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

int dequeue_socket(SocketQueue *queue) {
    pthread_mutex_lock(&queue->lock);

    while (queue->front == queue->rear) {
        pthread_mutex_lock(&server_running_mutex);
        int running = server_running;
        pthread_mutex_unlock(&server_running_mutex);

        if (!running) {
            pthread_mutex_unlock(&queue->lock);
            return -1;
        }
        pthread_cond_wait(&queue->cond, &queue->lock);
    }

    int socket = queue->sockets[queue->front];
    queue->front = (queue->front + 1) % queue->size;

    pthread_mutex_unlock(&queue->lock);

    return socket;
}

void *thread_pool_worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;

    while (1) {
        pthread_mutex_lock(&server_running_mutex);
        int running = server_running;
        pthread_mutex_unlock(&server_running_mutex);

        if (!running) {
            break;
        }

        int client_socket = dequeue_socket(pool->socket_queue);
        if (client_socket == -1) {
            continue;
        }

        handle_connection(client_socket);
    }

    pthread_exit(NULL);
}

int start_server(int port) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

  

    int server_socket = init_server_socket(port);
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {

        perror("setsockopt");
        return -1;
    }
    if (server_socket == -1) {
        fprintf(stderr, "Error initializing server socket\n");
        return -1;
    }

    printf("Server started and listening on port %d\n", port);

    if (pthread_mutex_init(&server_running_mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing server_running_mutex\n");
        return -1;
    }

    threadPool = create_thread_pool(THREAD_POOL_SIZE);
    threadPool->socket_queue = create_socket_queue(QUEUE_SIZE);

    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        if (pthread_create(&threadPool->threads[i], NULL, thread_pool_worker, (void *)threadPool) != 0) {
            fprintf(stderr, "Error creating thread. Continuing...\n");
        }
    }

    while (server_running) {
        int client_socket = accept_connection(server_socket);
        if (client_socket == -1) {
            continue;
        }

        enqueue_socket(threadPool->socket_queue, client_socket);
    }

    pthread_mutex_destroy(&server_running_mutex);
    destroy_socket_queue(threadPool->socket_queue);
    destroy_thread_pool(threadPool);

    if (close(server_socket) == -1) {
        fprintf(stderr, "Error closing server socket\n");
        return -1;
    }

    return 0;
}