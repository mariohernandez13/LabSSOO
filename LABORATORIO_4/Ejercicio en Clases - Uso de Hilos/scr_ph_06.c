#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sharedVariable = 0; // Variable compartida

void* addOne(void* arg) {
    sharedVariable += 1; // Sumar 1 a la variable compartida
    printf("Hilo 1 suma 1: %d\n", sharedVariable);
    return NULL;
}

void* subtractOne(void* arg) {
    sharedVariable -= 1; // Restar 1 de la variable compartida
    printf("Hilo 2 resta 1: %d\n", sharedVariable);
    return NULL;
}

void* addTwo(void* arg) {
    sharedVariable += 2; // Sumar 2 a la variable compartida
    printf("Hilo 3 suma 2: %d\n", sharedVariable);
    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread3;

    // Crear los tres hilos
    if (pthread_create(&thread1, NULL, addOne, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread2, NULL, subtractOne, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread3, NULL, addTwo, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Esperar a que los hilos terminen
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    printf("Valor final de la variable compartida: %d\n", sharedVariable);

    return EXIT_SUCCESS;
}
