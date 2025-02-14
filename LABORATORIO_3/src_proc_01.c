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
sleep(nivel * 2); // Diferente tiempo de sueño para cada nivel
crearProcesoHijo(nivel - 1);
exit(0);
} else {
wait(NULL);
}
}
}
int main() {
printf("Proceso raíz, PID: %d\n", getpid());
crearProcesoHijo(5);
return 0;
}
