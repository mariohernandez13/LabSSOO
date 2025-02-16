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
    pthread_t thread[10];

    // Abrir el fichero
    file = fopen("archivo.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // Crear los hilos
    for(int i = 0; i < 10; i++){
        if(pthread_create(&thread[i], NULL, &readAndPrintLines, NULL) != 0){
            perror("pthread_create");
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    // Esperar a que los hilos terminen
    for(int i = 0; i < 10; i++){
        pthread_join(thread[i], NULL);
    }

    // Limpieza
    fclose(file);

    return EXIT_SUCCESS;
}
