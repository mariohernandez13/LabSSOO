#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

// ./base/
// └── sub1/
//     └── sub2/


int main() {
    // Crear el directorio base
    if (mkdir("./base", 0777) == -1) {
        perror("Error al crear el directorio base");
        return 1;
    }

    // Crear el primer subdirectorio dentro del directorio base
    if (mkdir("./base/sub1", 0777) == -1) {
        perror("Error al crear el primer subdirectorio");
        return 1;
    }

    // Crear un segundo subdirectorio dentro del primer subdirectorio
    if (mkdir("./base/sub1/sub2", 0777) == -1) {
        perror("Error al crear el segundo subdirectorio");
        return 1;
    }

    printf("Estructura de directorios creada con éxito.\n");

    return 0;
}
