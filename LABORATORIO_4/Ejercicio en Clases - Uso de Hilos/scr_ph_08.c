#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

int main() {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *key, *value;
    char username[MAX_LINE_LENGTH] = "";
    char password[MAX_LINE_LENGTH] = "";

    // Abre el fichero de properties
    file = fopen("config.properties", "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return EXIT_FAILURE;
    }

    // Lee el fichero línea por línea
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        // Elimina el salto de línea al final, si existe
        line[strcspn(line, "\n")] = 0;

        // Divide la línea en clave y valor
        key = strtok(line, "=");
        value = strtok(NULL, "=");

        // Comprueba si la clave y el valor son válidos y guarda los datos
        if (key && value) {
            if (strcmp(key, "username") == 0) {
                strncpy(username, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "password") == 0) {
                strncpy(password, value, MAX_LINE_LENGTH);
            }
        }
    }

    // Cierra el fichero
    fclose(file);

    // Imprime las variables para verificar
    printf("Username: %s\n", username);
    printf("Password: %s\n", password);

    return EXIT_SUCCESS;
}
