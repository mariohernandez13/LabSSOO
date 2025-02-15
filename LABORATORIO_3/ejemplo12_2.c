#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main (void)
{
    int i, padre;
    padre = 1;

    for (i = 0; i < 3; i++)
    {
        if (padre == 1)
        {
            if (fork() == 0) // Esto indicaría que es el proceso hijo
            {
                fprintf(stdout, "Este es el proceso hijo con Padre: %ld\n", (long) getppid());
                padre = 0;
            }
            else // Este else indicaría que realiza bloque de código del padre
            {
                fprintf(stdout, "Este es el proceso padre con PID: %ld\n", (long) getpid());
                padre = 1;
            }
        }
    }

    return (0);
}