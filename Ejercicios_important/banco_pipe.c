#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int main(){
    
    int fd[2];
    char buffer[100];
    char *mensaje = "Hola papá";

    if (pipe(fd) == -1)
    {
        perror("Error al crear la pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0){
        close(fd[0]);
        write(fd[1], mensaje, strlen(mensaje) + 1);
        close(fd[1]);
    }
    else {
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        printf("El padre recibió: %s\n", buffer);
        close(fd[0]);
    }

    return (0);

}
