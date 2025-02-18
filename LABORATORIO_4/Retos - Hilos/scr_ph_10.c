#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LINES 10  // Número de líneas de texto aleatorio a generar
#define MAX_LINE_LENGTH 80  // Longitud máxima de cada línea de texto

void generateRandomText(FILE *file) {
    for (int i = 0; i < NUM_LINES; i++) {
        for (int j = 0; j < MAX_LINE_LENGTH - 1; j++) {
            // Generar caracteres aleatorios desde 'a' hasta 'z'
            char randomChar = 'a' + (rand() % 26);
            fputc(randomChar, file);
        }
        fputc('\n', file);  // Nueva línea al final de cada línea de texto
    }
}

int main() {
    FILE *file, *copy;
    char line[MAX_LINE_LENGTH];

    FILE *archivo = fopen("config.properties", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración de propiedaddes");
        return 1;
    }

    char linea[NUM_LINES];
    char *variables[MAX_LINE_LENGTH];
    int contador = 0;

    while (fgets(linea, sizeof(linea), file)) {
        if (strchr(linea, '=')) { // Solo procesar líneas con '='
            char *valor = strchr(linea, '=') + 1; // Obtener lo que está después de '='

            // Borra el salto de línea final si existe
            size_t len = strlen(valor);
            if (valor[len - 1] == '\n') {
                valor[len - 1] = '\0';
            }

            variables[contador++] = strdup(valor); // Guardar
        }
    }
    fclose(archivo);

    // Crear y escribir texto aleatorio en "archivo.txt"
    char *path= strcat(variables[1], variables[0]);

    file = fopen(variables[0], "w");
    if (!file) {
        perror("No se pudo abrir archivo.txt para escritura");
        return 1;
    }

    // Generar y escribir texto aleatorio
    generateRandomText(file);

    // Cerrar el fichero original
    fclose(file);

    // Abrir el fichero original y el fichero de destino para la copia
    file = fopen(variables[0], "r");
    copy = fopen(variables[2], "w");  // Ajusta esta ruta
    if (!file || !copy) {
        perror("No se pudo abrir uno de los archivos");
        return 1;
    }

    // Copiar el contenido de "archivo.txt" a "copia_archivo.txt"
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        fputs(line, copy);
    }

    // Cerrar ambos ficheros
    fclose(file);
    fclose(copy);

    printf("El archivo ha sido copiado exitosamente.\n");
    return 0;
}
