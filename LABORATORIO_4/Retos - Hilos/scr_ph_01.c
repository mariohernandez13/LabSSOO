#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Esta es la función que el hilo ejecutará
// gcc -o hilo_programa programa.c -lpthread
void* printMessages(void* arg) {
    char* message = (char*)arg;
    printf("Hilo en ejecucion ID del hilo: %lu, ID del proceso: %d\n", pthread_self(), getpid());
    printf("%s\n", message);
    sleep(100);
    return NULL;
}

int main() {
    pthread_t thread1;

    // Crear el hilo, pasando una cadena de texto como argumento para que la imprima
    if (pthread_create(&thread1, NULL, printMessages, "Hola, soy un hilo ejecutándome.") != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    printf("Proceso principal en ejecucion ID del hilo principal: %lu\n", pthread_self());
    // Esperar a que el hilo termine
    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
