#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

pid_t pid1, pid2;
pid1 = fork();

if (pid1 == 0) {
// Código del proceso hijo
printf("Soy el proceso hijo. Mi PID es: %d\n", getpid());
printf("El PID de mi padre es: %d\n", getppid());
pid2 = fork();
if (pid2 == 0) {
// Código del proceso nieto
printf("Soy el proceso nieto. Mi PID es: %d\n", getpid());
printf("El PID de mi padre es: %d\n", getppid());
} else if (pid2 > 0) {
// El hijo espera al nieto
wait(NULL);
printf("El nieto ha terminado. Proceso hijo continúa.\n");
}
} else if (pid1 > 0) {
// Código del proceso padre
printf("Soy el proceso padre. Mi PID es: %d\n", getpid());
wait(NULL); // El padre espera al hijo
printf("El hijo ha terminado. Proceso padre continúa.\n");
} else {
perror("Error al crear el proceso");
}
return 0;
}
