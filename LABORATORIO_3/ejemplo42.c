#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void LeerYEscribirArchivo(const char *nombreArchivoEntrada, const char *nombreArchivoSalida)
{
    FILE *archivoEntrada = fopen(nombreArchivoEntrada, "r");

    if(archivoEntrada == NULL)
    {
        perror("Error al abrir archivo de entrada\n");
        exit(1);
    }

    FILE *archivoSalida = fopen(nombreArchivoSalida, "w");

    if(archivoSalida == NULL)
    {
        perror("Error al abrir archivo de salida\n");
        exit(1);
    }

    char linea[100]; // Asumiendo que cada línea del archvo tiene menos de 100 caracteres

    while(fgets(linea, sizeof(linea), archivoEntrada) != NULL)
    {
       fprintf(archivoSalida, "PID: %d Leyendo: %s\n", getpid(), linea);
    }

    fclose(archivoEntrada);
    fclose(archivoSalida);
}

int main()
{
    const char *nombreArchivoEntrada = "archivo_entrada.txt";
    const char *nombreArchivoSalida = "archivo_salida.txt";

    for(int i = 0; i < 3; i++)
    {
        pid_t pid = fork();

        if(pid < 0)
        {
            perror("Fork() falló");
            exit(1);
        }
        if(pid == 0)
        {
            // Estamos en el hijo
            LeerYEscribirArchivo(nombreArchivoEntrada, nombreArchivoSalida);
            exit(0);
        }
    }

    // El proceso padre también lee el archivo
    LeerYEscribirArchivo(nombreArchivoEntrada, nombreArchivoSalida);

    // Esperar a que todos los procesos hijos terminen
    for(int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    return(0);
}