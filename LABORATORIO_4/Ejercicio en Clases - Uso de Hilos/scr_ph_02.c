#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 256

FILE* file;

void* readAndPrintLines(void* arg) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("Hilo %ld: %s", pthread_self(), line);
    }

    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Abrir el fichero
    file = fopen("archivo.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // Crear los hilos
    if (pthread_create(&thread1, NULL, &readAndPrintLines, NULL) != 0 ||
        pthread_create(&thread2, NULL, &readAndPrintLines, NULL) != 0) {
        perror("pthread_create");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Esperar a que los hilos terminen
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Limpieza
    fclose(file);

    return EXIT_SUCCESS;
}
