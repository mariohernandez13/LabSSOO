#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

void* printVowels(void* arg) {
    time_t end = time(NULL) + 15; // Ejecutar por 15 segundos
    while (time(NULL) < end) {
        printf("A E I O U\n");
        sleep(1); // Pausa de 1 segundo para hacer la salida legible
    }
    return NULL;
}

void* printConsonants(void* arg) {
    time_t end = time(NULL) + 10; // Ejecutar por 10 segundos
    while (time(NULL) < end) {
        printf("B C D F G H J K L M N P Q R S T V W X Y Z\n");
        sleep(1);
    }
    return NULL;
}

void* printEvens(void* arg) {
    int number = 0;
    time_t end = time(NULL) + 20; // Ejecutar por 20 segundos
    while (time(NULL) < end) {
        printf("%d ", number);
        number += 2;
        sleep(1);
    }
    printf("\n");
    return NULL;
}

void* printOdds(void* arg) {
    int number = 1;
    time_t end = time(NULL) + 25; // Ejecutar por 25 segundos
    while (time(NULL) < end) {
        printf("%d ", number);
        number += 2;
        sleep(1);
    }
    printf("\n");
    return NULL;
}

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo
        pthread_t thread1, thread2;

        pthread_create(&thread1, NULL, printVowels, NULL);
        pthread_create(&thread2, NULL, printConsonants, NULL);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    } else if (pid > 0) {
        // Proceso padre
        pthread_t thread3, thread4;

        pthread_create(&thread3, NULL, printEvens, NULL);
        pthread_create(&thread4, NULL, printOdds, NULL);

        pthread_join(thread3, NULL);
        pthread_join(thread4, NULL);
    } else {
        // Error al crear el proceso
        perror("fork");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}