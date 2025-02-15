#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
    printf("PID del proceso antes del exec: %d\n", getpid());
    printf("Ejecutando sleep durante 10 segundos. Usa ps o top para ver este proceso\n");
    
    // Espera 10 segundos antes de reemplazar el proceso
    sleep(10);

    printf("Reemplazando este proceos con '/bin/ls -l'\n");

    // Reemplaza el proceso actual con el comando '/bin/ls -l'
    execl("/bin/ls", "ls", "-l", (char *) NULL);

    //Si llega aquí significa que hubo un error con execl
    perror("Ahora acabó el programa.");

    return(1); // Retorno 1 para indicar un error
}