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

    FILE *archivo, *copy;
    char line[MAX_LINE_LENGTH];
    char linea[MAX_LINE_LENGTH];
    char *variables[NUM_LINES];
    int contador = 0;
    char *limitador = "=";
    char comillas = '"';
    int i, j = 0;
    size_t len; 

    archivo = fopen("config.properties", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo de configuración de propiedaddes");
        return 1;
    }

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strchr(linea, '=')) { // Solo procesar líneas con '='

            char *valor = strtok(linea, limitador); // Obtener lo que está después de '='
            
            if (strcmp(valor,"file") || strcmp(valor, "input") || strcmp(valor, "output"))
            {
                valor = strtok(NULL, limitador);

                len = strlen(valor);

                if (valor[len - 2] != '\r')
                    strcat(valor, "\r");

                for (i = 0; i <= len; i++){
                    if (valor[i] != comillas)    
                        valor[j++] = valor[i];
                }

                for (i = 0; i <= len; i++)
                {
                    if (valor[i] == comillas)
                        valor[i] = '\0';
                }

                j = 0;

                // Borra el salto de línea final si existe
                if (valor[len - 1] == '\n') 
                    valor[len - 1] = '\0';

                variables[contador++] = strdup(valor); // Guardar
            }
        }
    }

    fclose(archivo);

    archivo = fopen(variables[0], "w");
    if (!archivo) {
        perror("No se pudo abrir archivo.txt para escritura");
        return 1;
    }

    // Generar y escribir texto aleatorio
    generateRandomText(archivo);

    // Cerrar el fichero original
    fclose(archivo);

    // Abrir el fichero original y el fichero de destino para la copia
    archivo = fopen(variables[0], "r");
    copy = fopen(variables[2], "w"); 

    if (!archivo || !copy) {
        perror("No se pudo abrir uno de los archivos");
        return 1;
    }

    // Copiar el contenido de "archivo.txt" a "copia_archivo.txt"
    while (fgets(line, MAX_LINE_LENGTH, archivo) != NULL) {
        fputs(line, copy);
    }

    // Cerrar ambos ficheros
    fclose(archivo);
    fclose(copy);

    printf("El archivo ha sido copiado exitosamente.\n");
    return 0;
}