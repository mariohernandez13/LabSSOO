#include "banco.h"

CONFIG configuracion;

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
int leer_configuracion() {

    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");
    
    if (file == NULL) {
        escrituraLogGeneral("Error al abrir el archivo de configuración\n");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        // Mirar una posible conversión de esto a un switch case en el futuro
        if (key && value) {
            if (strcmp(key, "LIMITE_RETIRO") == 0) {
                configuracion.limiteRetiros = atoi(value);
            } else if (strcmp(key, "LIMITE_TRANSFERENCIA") == 0){
                configuracion.limiteTransferencia = atoi(value);
            } else if (strcmp(key, "UMBRAL_RETIROS") == 0){
                configuracion.umbralRetiros = atoi(value);
            } else if (strcmp(key, "UMBRAL_TRANSFERENCIAS") == 0){
                configuracion.umbralTransferencias = atoi(value);
            } else if (strcmp(key, "NUM_HILOS") == 0){
                configuracion.numHilos = atoi(value);
            } else if (strcmp(key, "ARCHIVO_CUENTAS") == 0){
                strncpy(configuracion.archivoCuentas, value, strlen(value));
            } else if (strcmp(key, "ARCHIVO_TRANSACCIONES") == 0){
                strncpy(configuracion.archivoTransacciones, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "ARCHIVO_LOG") == 0){
                strncpy(configuracion.archivoLog, value, MAX_LINE_LENGTH);
            }
        }
    }

    fclose(file);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n");

    return (state);
}

int leer_transacciones(){

    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("transacciones.log", "r");
    
    if (file == NULL) {
        escrituraLogGeneral("Error al abrir el archivo de transacciones\n");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        // Mirar una posible conversión de esto a un switch case en el futuro
        if (key && value) {
            
        }
    }

    fclose(file);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n");

    return (state);

}

int main(int argc, char *argv[]) {

    leer_configuracion();

}
