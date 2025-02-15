#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void LeerArchivo(const char *nombreArchivo)
{
    FILE *file = fopen(nombreArchivo, "r");

    if(file == NULL)
    {
        perror("Error al abrir archivo\n");
        exit(1);
    }

    char linea[100]; // Asumiendo que cada línea del archvo tiene menos de 100 caracteres

    while(fgets(linea, sizeof(linea), file) != NULL)
    {
       printf("PID: %d Leyendo: %s\n", getpid(), linea);
    }

    fclose(file);
}

int main()
{
    const char *nombreArchivo = "archivo.txt";

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
            // Estanos en el hijo
            LeerArchivo(nombreArchivo);
            exit(0);
        }
    }

    // El proceso padre también lee el archivo
    LeerArchivo(nombreArchivo);

    // Esperar a que todos los procesos hijos terminen
    for(int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    return(0);
}