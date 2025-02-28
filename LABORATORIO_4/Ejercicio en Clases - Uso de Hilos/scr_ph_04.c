#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_LINES 1024  // Máximo número de líneas que esperamos leer

// Esta es la función que el hilo ejecutará
// gcc -o hilo_programa programa.c -lpthread

typedef struct {
    char** lines;  // Array de líneas leídas del fichero
    int num_lines; // Número de líneas leídas
} ThreadData;

void* printLines(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = 0; i < data->num_lines; i++) {
        printf("Hilo %ld: %s", pthread_self(), data->lines[i]);
        // Asumiendo que las líneas terminan con '\n'
    }

    return NULL;
}

int main() {
    FILE* file;
    pthread_t thread1, thread2;
    char* lines[MAX_LINES];
    int count = 0;
    ThreadData data1, data2;

    file = fopen("archivo.txt", "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Leer líneas del fichero
    while (fgets(lines[count], sizeof(lines[count]), file)) {
        count++;
        if (count == MAX_LINES) break;  // Prevenir desbordamiento de buffer
    }
    fclose(file);

    // Dividir las líneas entre los dos hilos
    data1.lines = lines;
    data1.num_lines = count / 2;
    data2.lines = &lines[count / 2];
    data2.num_lines = count - count / 2;

    // Crear los hilos
    pthread_create(&thread1, NULL, printLines, &data1);
    pthread_create(&thread2, NULL, printLines, &data2);

    // Esperar a que los hilos terminen
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return EXIT_SUCCESS;
}

