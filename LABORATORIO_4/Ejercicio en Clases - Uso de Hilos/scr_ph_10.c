#include <stdio.h>
#include <stdlib.h>

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

    // Crear y escribir texto aleatorio en "archivo.txt"
    file = fopen("archivo.txt", "w");
    if (!file) {
        perror("No se pudo abrir archivo.txt para escritura");
        return 1;
    }

    // Generar y escribir texto aleatorio
    generateRandomText(file);

    // Cerrar el fichero original
    fclose(file);

    // Abrir el fichero original y el fichero de destino para la copia
    file = fopen("archivo.txt", "r");
    copy = fopen("/ruta/especifica/copia_archivo.txt", "w");  // Ajusta esta ruta
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
