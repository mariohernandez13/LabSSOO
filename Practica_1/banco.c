#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE_LENGTH 255

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
int readFile() {
    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");
    
    // 
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        if (key && value) {
            if (strcmp(key, "patata") == 0) {
                strncpy(username, value, MAX_LINE_LENGTH);
            } else {
                state = 1;
            }
        }
    }

    fclose(file);
    return state;
}

int main() {
    pid_t pid;
    int fd[2];

    char buffer[100];
    char errorMessage[] = "Ha habido un error leyendo el archivo .config";
    char validMessage[] = "Todo correcto maquina";

    int state = 0;

    // Comprobamos que no ocurre problema al generar la pipe
    if (pipe(fd) == -1) {   
        perror("Error en la generación de la pipe");
        return 1;
    }

    pid = fork();

    // Comprobamos que fork() no genera un error
    if (pid < 0) { 
        perror("Error al crear el proceso hijo");
        return 1;
    }

    if (pid > 0) {  // Proceso padre
        printf("El padre empieza...\n");
        close(fd[0]);  // Cierra el extremo de lectura
        
        state = readFile();
        printf("Estado de readFile(): %d\n", state);
        
        if (state == 1)
            write(fd[1], errorMessage, strlen(errorMessage) + 1);
        else
            write(fd[1], validMessage, strlen(validMessage) + 1);

        close(fd[1]);  // Cierra el extremo de escritura
        printf("El padre muere\n");
        wait(NULL); // Espera a que el hijo termine
    } 
    else {  // Proceso hijo
        close(fd[1]);  // Cierra el extremo de escritura
        read(fd[0], buffer, sizeof(buffer));
        printf("El hijo recibió: %s\n", buffer);
        close(fd[0]);  // Cierra el extremo de lectura
    }

    return 0;
}

