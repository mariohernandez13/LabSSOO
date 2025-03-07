#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE_LENGTH 255

int readFile(FILE *file){

    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        if (key && value){
            if (strcmp(key, "patata"))
                strncpy(username, value, MAX_LINE_LENGTH);
            else
                state = 1;
        }
    }

    fclose(file);

    return(state);
}


int main(){

    FILE *file;
    pid_t pid = fork();
    int fd[2];

    char buffer[100];
    char errorMessage[] = "Ha habido un error leyendo el archivo .config";
    char validMessage[] = "Todo correcto maquina";

    int state = 0;

    // Comprobamos un posible error 
    if (pipe(fd) == -1){
        perror("Error en la generación de la pipe\n");
        return(1);
    }

    if (pid != 0){ 
        printf("El padre empieza...");
        close(fd[0]);
        state = readFile(file);
        printf("%d", state);
        if (state)
            write(fd[1], errorMessage, strlen(errorMessage) + 1);
        else
            write(fd[1], validMessage, strlen(validMessage) + 1);
        printf("El padre muere");
        close(fd[1]);
    } 
    else {
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        printf("El hijo recibió: %s\n", buffer);
        close(fd[0]);
    }

    return(0);

}