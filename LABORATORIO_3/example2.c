#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

pid_t pid;
pid = fork();

if (pid == 0) {
// Código del proceso hijo
printf("Soy el hijo. Mi PID es: %d\n", getpid());
printf("El PID de mi padre es: %d\n", getppid());
sleep(2); // Simular que el hijo está haciendo algo
printf("El hijo ha terminado.\n");
} else if (pid > 0) {
// Código del proceso padre
printf("Soy el padre. Mi PID es: %d\n", getpid());
printf("El PID de mi padre es: %d\n", getppid());
wait(NULL); // Esperar a que el hijo termine
printf("El proceso hijo ha terminado. Continúo en el padre.\n");
} else {
perror("Error al crear el proceso");
}
return 0;
}
