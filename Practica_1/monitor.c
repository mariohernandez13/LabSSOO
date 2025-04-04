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

    fclose(file);

    sem_post(semaforo_transacciones);
    sem_close(semaforo_transacciones);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n", 0);

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
            enviar_alerta(mensajeAlerta); //añadir además el id
        }
    }

    fclose(file);
}

/// @brief Función main de monitor, se encarga de leer el archivo de configuración y el de errores
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char *argv[])
{
    printf("Pulse INTRO para continuar...\n");

    configuracion = leer_configuracion(configuracion);

    while (1)
    {
        // leer_transacciones();
        leer_errores();
        sleep(5); // Pequeña pausa 
    }

    return 0;
}
