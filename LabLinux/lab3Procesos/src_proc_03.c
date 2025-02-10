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
crearProcesoHijo(nivel - 1);
sleep(10); // Todos los procesos duermen después de que se haya creado toda la jerarquía
exit(0);
} else {
wait(NULL);
}
}
}
int main() {
printf("Proceso raíz, PID: %d\n", getpid());
crearProcesoHijo(5);
sleep(10); // El proceso raíz también duerme
return 0;
}
