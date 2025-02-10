#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void crearProcesoHijo(int nivel) {
if (nivel > 0) {
pid_t pid = fork();
if (pid == -1) {
perror("Fork falló");
exit(1);
} else if (pid == 0) { // Proceso hijo
printf("Nivel %d, PID: %d, PPID: %d\n", 5 - nivel, getpid(), getppid());
sleep(10); // Hace que el proceso hijo "duerma" para permitir la visualización del árbol de procesos
crearProcesoHijo(nivel - 1);
exit(0); // Termina el proceso hijo
} else {
wait(NULL); // El proceso padre espera al hijo
}
}
}

int main() {
printf("Proceso raíz, PID: %d\n", getpid());
crearProcesoHijo(5); // Inicia la creación de la estructura de 5 niveles
return 0;
}
