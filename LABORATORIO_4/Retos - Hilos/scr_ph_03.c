#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 256

FILE* file;

void* readAndPrintLines(void* arg) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        printf("Hilo %ld: %s", pthread_self(), line);
    }

    // Una posible opción para que cada hilo imprima el fichero de vuelta también sería que se volviese a apuntar
    // al inicio del fichero después de la ejecución de cada hilo
    rewind(file);

    sleep(10);

    return NULL;
}

int main() {
    pthread_t thread[4];

    // Abrir el fichero
    file = fopen("archivo.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // Crear los hilos
    for(int i = 0; i < 4; i++){
        if(pthread_create(&thread[i], NULL, &readAndPrintLines, NULL) != 0){
            perror("pthread_create");
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    // Esperar a que los hilos terminen
    for(int i = 0; i < 4; i++){
        pthread_join(thread[i], NULL);
    }

    // Limpieza
    fclose(file);

    return EXIT_SUCCESS;
}
