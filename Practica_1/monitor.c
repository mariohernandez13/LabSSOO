#include "banco.h"

CONFIG configuracion;

void enviar_alerta(char *mensaje)
{
    int fifo_fd;

    // Verificar si la FIFO ya existe
    // if (access(FIFO1, F_OK) == 0)
    // {
    //     escrituraLogGeneral("La FIFO ya existe\n", 0);
    // }

    // Crear la tubería si no existe
    // if (mkfifo(FIFO1, 0666) == -1)
    // {
    //     escrituraLogGeneral("Error al crear la tubería FIFO1\n", 0);
    //     return;
    // }

    fifo_fd = open(FIFO1, O_WRONLY);

    if (fifo_fd == -1)
    {
        escrituraLogGeneral("🟥 Error al abrir la tubuería en monitor\n", 0);
        return;
    }

    write(fifo_fd, mensaje, strlen(mensaje) + 1);
    close(fifo_fd);
}

/// @brief Actualiza el archivo errores.dat incrementando el tipo de error correspondiente
/// @param id ID del usuario (como string)
/// @param tipoError Tipo de error: 0=retiro, 1=ingreso, 2=transferencia
void escribir_errores(char *id, int tipoError)
{
    FILE *archivo = fopen("data/errores.dat", "r+");
    if (!archivo)
    {
        escrituraLogGeneral("🟥 Error al abrir errores.dat para escritura\n", 0);
        return;
    }

    char linea[MAX_LINE_LENGTH];
    char nuevaLinea[MAX_LINE_LENGTH];
    char *lineasArchivo[1000];
    int totalLineas = 0;
    int encontrado = 0;

    // Leer todas las líneas del archivo
    while (fgets(linea, sizeof(linea), archivo))
    {
        lineasArchivo[totalLineas] = strdup(linea);
        totalLineas++;
    }

    // Buscar y actualizar la línea correspondiente al ID
    for (int i = 0; i < totalLineas; i++)
    {
        char *temp = strdup(lineasArchivo[i]);
        temp[strcspn(temp, "\n")] = 0;

        char *token = strtok(temp, ",");
        if (token && strcmp(token, id) == 0)
        {
            int errorRetiro = atoi(strtok(NULL, ","));
            int errorIngreso = atoi(strtok(NULL, ","));
            int errorTrans = atoi(strtok(NULL, ","));

            if (tipoError == 0)
                errorRetiro++;
            else if (tipoError == 1)
                errorIngreso++;
            else if (tipoError == 2)
                errorTrans++;

            snprintf(nuevaLinea, sizeof(nuevaLinea), "%s,%d,%d,%d\n", id, errorRetiro, errorIngreso, errorTrans);
            free(lineasArchivo[i]);
            lineasArchivo[i] = strdup(nuevaLinea);
            encontrado = 1;
        }

        free(temp);
    }

    // Si no se encontró el ID, añadir nueva línea
    if (!encontrado)
    {
        int e0 = 0, e1 = 0, e2 = 0;
        if (tipoError == 0)
            e0 = 1;
        if (tipoError == 1)
            e1 = 1;
        if (tipoError == 2)
            e2 = 1;

        snprintf(nuevaLinea, sizeof(nuevaLinea), "%s,%d,%d,%d\n", id, e0, e1, e2);
        lineasArchivo[totalLineas++] = strdup(nuevaLinea);
    }

    // Reescribir el archivo desde cero
    freopen("data/errores.dat", "w", archivo);
    for (int i = 0; i < totalLineas; i++)
    {
        fputs(lineasArchivo[i], archivo);
        free(lineasArchivo[i]);
    }

    fclose(archivo);
    escrituraLogGeneral("🟩 Errores escritos correctamente en errores.dat\n", 0);
}

/// @return
int leer_transacciones()
{
    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    int contadorErroresGenerales = 0;
    int contadorTransaccion = 0;
    char username[MAX_LINE_LENGTH] = "";
    char buffer[100];

    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);

    if (semaforo_transacciones == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_transacciones);

    file = fopen("logs/transacciones.log", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de transacciones\n", 0);
        return 1;
    }

    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0; // Eliminar salto de línea

        // Saltar el timestamp (todo hasta el primer ']')
        char *contenido = strchr(linea, ']');
        if (contenido)
            contenido++; // Mover justo después de ']'
        else
            continue;

        // Procesar la línea desde contenido (no desde linea)
        char *estado = strtok(contenido, ",");
        char *tipo = strtok(NULL, ",");
        char *mensaje = strtok(NULL, ",");
        char *id = strtok(NULL, ",");
        char *cantidadStr = strtok(NULL, ",");

        if (!estado || !tipo || !id)
            continue;

        if (strcmp(estado, "ERROR") == 0)
        {
            int tipoError = -1;
            if (strcmp(tipo, "Retiro") == 0)
                tipoError = 0;
            else if (strcmp(tipo, "Ingreso") == 0)
                tipoError = 1;
            else if (strcmp(tipo, "Transferencia") == 0)
                tipoError = 2;

            if (tipoError != -1)
                escribir_errores(id, tipoError);
        }
    }

    fclose(file);

    sem_post(semaforo_transacciones);
    sem_close(semaforo_transacciones);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo transacciones.log\n", 0);

    return (state);
}

/// @brief Función que se encarga de leer el archivo de errores y enviar una alerta al banco si se supera el umbral
void leer_errores()
{
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    int id;
    int totalErrores, errorRetiro, errorIngreso, errorTransaccion;
    char *key;
    char mensajeAlerta[100];

    file = fopen("data/errores.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de errores\n", 0);
        return;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;

        key = strtok(linea, ",");
        id = atoi(key);

        key = strtok(NULL, ",");
        errorRetiro = atoi(key);

        key = strtok(NULL, ",");
        errorIngreso = atoi(key);

        key = strtok(NULL, ",");
        errorTransaccion = atoi(key);

        // solo se pueden cometer 5 errores generales o 3 específicos de una operación
        totalErrores = errorIngreso + errorRetiro + errorTransaccion;

        if (errorRetiro >= configuracion.umbralRetiros || errorIngreso >= configuracion.umbralIngreso || errorRetiro >= configuracion.umbralTransferencias || totalErrores >= configuracion.umbralTotal)
        {
            escrituraLogGeneral("Se ha superado el umbral de errores, enviando alerta al banco\n", 0);
            enviar_alerta(mensajeAlerta); // añadir además el id
        }
    }

    fclose(file);
}

/// @brief Función que muestra la configración de errores del archivo properties
void mostrar_configuracion()
{
    printf("\n⚙️  CONFIGURACIÓN ACTUAL:\n");
    printf("🔢 Umbral Total: %d\n", configuracion.umbralTotal);
    printf("💸 Umbral Retiros: %d\n", configuracion.umbralRetiros);
    printf("💰 Umbral Ingresos: %d\n", configuracion.umbralIngreso);
    printf("🔁 Umbral Transferencias: %d\n", configuracion.umbralTransferencias);
}

/// @brief Función main de monitor, se encarga de leer el archivo de configuración y el de errores
/// @param argc
/// @param argv
/// @return
int main(int argc, char *argv[])
{
    printf("=============================================\n");
    printf("🛡️  MONITOR DE ANOMALÍAS - SAFEBANK SYSTEM 🛡️\n");
    printf("=============================================\n");

    printf("1️⃣ Ver log de eventos generales\n");
    printf("2️⃣ Ver configuración actual\n");
    printf("3️⃣ Recargar configuración\n");
    printf("4️⃣ Salir\n");
    printf("🡆 Elige una opción: ");

    configuracion = leer_configuracion(configuracion);

    while (1)
    {
        leer_transacciones();
        leer_errores();
        sleep(5); // Pequeña pausa
    }

    return 0;
}
