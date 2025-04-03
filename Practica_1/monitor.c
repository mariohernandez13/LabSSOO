#include "banco.h"

CONFIG configuracion;

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
int leer_configuracion()
{

    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de configuración\n", 0);
        return 1;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;

        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        // Mirar una posible conversión de esto a un switch case en el futuro
        if (key && value)
        {
            if (strcmp(key, "LIMITE_RETIRO") == 0)
            {
                configuracion.limiteRetiros = atoi(value);
            }
            else if (strcmp(key, "LIMITE_TRANSFERENCIA") == 0)
            {
                configuracion.limiteTransferencia = atoi(value);
            }
            else if (strcmp(key, "LIMITE_INGRESO") == 0)
            {
                configuracion.limiteIngreso = atoi(value);
            }
            else if (strcmp(key, "UMBRAL_RETIROS") == 0)
            {
                configuracion.umbralRetiros = atoi(value);
            }
            else if (strcmp(key, "UMBRAL_TRANSFERENCIAS") == 0)
            {
                configuracion.umbralTransferencias = atoi(value);
            }
            else if (strcmp(key, "UMBRAL_INGRESO") == 0)
            {
                configuracion.umbralIngreso = atoi(value);
            }
            else if (strcmp(key, "UMBRAL_TOTAL") == 0)
            {
                configuracion.umbralTotal = atoi(value);
            }
            else if (strcmp(key, "NUM_HILOS") == 0)
            {
                configuracion.numHilos = atoi(value);
            }
            else if (strcmp(key, "ARCHIVO_CUENTAS") == 0)
            {
                strncpy(configuracion.archivoCuentas, value, strlen(value));
            }
            else if (strcmp(key, "ARCHIVO_TRANSACCIONES") == 0)
            {
                strncpy(configuracion.archivoTransacciones, value, MAX_LINE_LENGTH);
            }
            else if (strcmp(key, "ARCHIVO_LOG") == 0)
            {
                strncpy(configuracion.archivoLog, value, MAX_LINE_LENGTH);
            }
        }
    }

    fclose(file);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n", 0);

    return (state);
}

void enviar_alerta(char *mensaje)
{
    int fifo_fd;

    // Crear la tubería si no existe
    if (mkfifo(FIFO1, 0666) == -1)
    {
        escrituraLogGeneral("Error al crear la tubería FIFO1\n", 0);
        return;
    }

    fifo_fd = open(FIFO1, O_WRONLY);

    if (fifo_fd == -1)
    {
        escrituraLogGeneral("Error al abrir la tubuería en monitor", 0);
        return;
    }

    write(fifo_fd, mensaje, strlen(mensaje) + 1);
    close(fifo_fd);
}

int leer_transacciones()
{
    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    int contadorErroresGenerales = 0;
    int contadorTransaccion = 0;
    char username[MAX_LINE_LENGTH] = "";
    char buffer[100];

    //! TENEMOS PENSADO CREAR UNA TUBERÍA AQUÍ ENTRE USUARIOS Y BANCO EN LA QUE SE NOTIFIQUE DE LA OPERACIÓN QUE HA HECHO
    // EL BANCO LA RECIBE Y ENVÍA A MONITOR POR OTRA PIPE UN AVISO DE QUE HA OCURRIDO UNA OPERACIÓN Y QUE PUEDO REVISAR EL ARCHIVO DE TRANSACCIONES
    // DE ESTA FORMA VE SI HAY ALGÚN FALLO / ANOMALÍA

    // DEFINIR ERRORES: ERRORES MÁS DE N VECES, CONTADOR DE ERRORES GENERALES, ESPECÍFICO DE OPERACIONES CONCRETAS -> PETAS
    // CUANDO PASE ESTO, RECIBIMOS EL ERROR = SEÑAL KILL A TODO ?? O CERRAR SESIÓN. LISTAR PROCESOS Y MATARLOS, en ps aux se ve el id y el pid del proceso

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
        escrituraLogGeneral("Error al abrir el archivo de transacciones\n", 0);
        return 1;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;
        // Mirar una posible conversión de esto a un switch case en el futuro
        if (strstr(linea, "Error") != NULL)
        {
            contadorErroresGenerales++;
        }
        else if (strstr(linea, "Operación") != NULL)
        {
            contadorTransaccion++;
            if (configuracion.limiteTransferencia == contadorTransaccion)
            {
                enviar_alerta("🚨 ALERTA\n"); // tubería
            }
        }
    }

    fclose(file);

    sem_post(semaforo_transacciones);
    sem_close(semaforo_transacciones);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n", 0);

    return (state);
}

void leer_errores()
{
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    int id;
    int totalErrores, errorRetiro, errorIngreso, errorTransaccion;
    char *key, *value;
    char mensajeAlerta[100];

    file = fopen("data/errores.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de configuración\n", 0);
        return;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;

        key = strtok(linea, ",");
        int id = atoi(key);

        key = strtok(NULL, ",");
        int errorRetiro = atoi(key);

        key = strtok(NULL, ",");
        int errorIngreso = atoi(key);

        key = strtok(NULL, ",");
        int errorTransaccion = atoi(key);

        totalErrores = errorIngreso + errorRetiro + errorTransaccion;

        if (errorRetiro >= configuracion.umbralRetiros || errorIngreso >= configuracion.umbralIngreso 
            || errorRetiro >= configuracion.umbralTransferencias || totalErrores > configuracion.umbralTotal)
        {
            
            enviar_alerta(mensajeAlerta);
        }
    }
    fclose(file);

}

int main(int argc, char *argv[])
{
    leer_configuracion();

    while (1)
    {
        leer_errores();
        leer_transacciones();
        sleep(5); // Pequeña pausa para evitar uso excesivo de CPU
    }

    return 0;
}
