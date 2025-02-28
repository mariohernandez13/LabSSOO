#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// ./base/
// └── sub1/
//     └── sub2/

#define MAX_LINE_LENGTH 255

int main() {

    FILE *file;
    char line[MAX_LINE_LENGTH] = "";
    char *key, *value;
    char path[MAX_LINE_LENGTH] = "";
    char *directory;
    char *limitador = "/";
    char *folder;

    // Abre el fichero de properties
    file = fopen("config.properties", "r");

    if (!file) {
        perror("No se pudo abrir el archivo");
        return EXIT_FAILURE;
    }

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        // Elimina el salto de línea al final, si existe
        line[strcspn(line, "\n")] = 0;

        // Divide la línea en clave y valor
        key = strtok(line, "=");
        value = strtok(NULL, "=");

        // Comprueba si la clave y el valor son válidos y guarda los datos
        if (key && value) {
            if (strcmp(key, "path") == 0) {
                strncpy(path, value, MAX_LINE_LENGTH);
            }
        }
    }

    directory = strtok(path, limitador);
    directory = strtok(NULL, limitador);

    // Crear el directorio base
    if (mkdir(folder, 0777) == -1) {
        perror("Error al crear el directorio base");
        return 1;
    }

    directory = strtok(NULL, limitador);

    // Crear el primer subdirectorio dentro del directorio base
    if (mkdir("./base/sub1", 0777) == -1) {
        perror("Error al crear el primer subdirectorio");
        return 1;
    }

    directory = strtok(NULL, limitador);

    // Crear un segundo subdirectorio dentro del primer subdirectorio
    if (mkdir("./base/sub1/sub2", 0777) == -1) {
        perror("Error al crear el segundo subdirectorio");
        return 1;
    }

    printf("Estructura de directorios creada con éxito.\n");

    return 0;
}
