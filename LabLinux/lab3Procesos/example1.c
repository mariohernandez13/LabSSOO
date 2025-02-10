#include <stdio.h>
#include <unistd.h>

int main() {
pid_t pid;
pid = fork(); // Crear un nuevo proceso

if (pid == 0) {
// Este bloque es ejecutado por el proceso hijo
printf("Soy el proceso hijo.\n");
printf("Mi PID es: %d\n", getpid());
printf("El PID de mi proceso padre es: %d\n", getppid());
} else if (pid > 0) {
// Este bloque es ejecutado por el proceso padre
printf("Soy el proceso padre.\n");
printf("Mi PID es: %d\n", getpid());
printf("El PID de mi proceso padre es: %d\n", getppid());
} else {
// fork falló
perror("Error al crear el proceso");
}
return 0;
}
