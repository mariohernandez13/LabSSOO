#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

#define TEMPORIZADOR 4

void reciboAlarma(int signal)
{
    printf("Banco ha recibido alarma\n");
} 

int main(){

    int fd[2];
    char buffer[100];
    char *mensaje = "Hola papá";

    pid_t pid = fork();

    signal(SIGALRM, reciboAlarma);

    if (pipe(fd) == -1)
    {
        perror("Error al generar la pipe\n");
        return(1);
    }

    if (pid == 0){
        alarm(TEMPORIZADOR - 1);
        pause();
    } else {
        sleep(TEMPORIZADOR);
        printf("El padre recibió: %s\n", mensaje);
    }

    return(0);

}