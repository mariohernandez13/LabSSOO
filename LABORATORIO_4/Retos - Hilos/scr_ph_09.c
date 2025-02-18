#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// ./base/
// └── sub1/
//     └── sub2/

#define MAX_LINE 1024
#define MAX_DIRS 100

void crearDirectorio(char *ruta){
    if (mkdir(ruta, 0777) == -1) {
        perror("Error al crear directorio");
    }
    else {
        printf("Directorio creado: %s\n", ruta);
    }
}


int main() {
    FILE *file = fopen("config.properties", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración de propiedaddes");
        return 1;
    }

    char linea[MAX_LINE];
    char *rutas[MAX_DIRS];
    int contador = 0;

    while (fgets(linea, sizeof(linea), file)) {
        if (strncmp(linea, "DIRECTORIOS=", 12) == 0) {
            char *valor = linea + 12; // Saltar "DIRECTORIOS="
            char *token = strtok(valor, ",\n");

            while (token && contador < MAX_DIRS) {
                rutas[contador++] = strdup(token); // Guardar cada directorio
                token = strtok(NULL, ",\n");
            }
        }
    }
    fclose(file);

    // Crear los directorios
    for (int i = 0; i < contador; i++) {
        crearDirectorio(rutas[i]);
        free(rutas[i]); // Liberar memoria
    }

    printf("Estructura de directorios creada con éxito.\n");
    return 0;

    // Crear el directorio base
    // if (mkdir("./base", 0777) == -1) {
    //     perror("Error al crear el directorio base");
    //     return 1;
    // }

    // // Crear el primer subdirectorio dentro del directorio base
    // if (mkdir("./base/sub1", 0777) == -1) {
    //     perror("Error al crear el primer subdirectorio");
    //     return 1;
    // }

    // // Crear un segundo subdirectorio dentro del primer subdirectorio
    // if (mkdir("./base/sub1/sub2", 0777) == -1) {
    //     perror("Error al crear el segundo subdirectorio");
    //     return 1;
    // }

    // printf("Estructura de directorios creada con éxito.\n");

    // return 0;
}
