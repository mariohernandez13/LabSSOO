#define _GNU_SOURCE // Para asignar nombre a los hilos
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 255
#define MAX_LENGTH_NAME 50
#define MAX_LENGTH_SALDO 10
#define MAX_LENGTH_ID 6

#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"

sem_t *semaforo_transacciones;
sem_t *semaforo_config;
sem_t *semaforo_cuentas;
sem_t *semaforo_banco;

/// @brief Estructura de la configuración del sistema del banco
typedef struct
{
    int limiteRetiros;
    int limiteTransferencia;
    int limiteIngreso;
    int umbralRetiros;
    int umbralTransferencias;
    int umbralIngreso;
    int umbralTotal;
    int numHilos;
    char archivoCuentas[MAX_LINE_LENGTH];
    char archivoLog[MAX_LINE_LENGTH];
    char archivoTransacciones[MAX_LINE_LENGTH];
} CONFIG;

/// @brief Estructura de los errores para cada usuario
typedef struct
{
    char *id;
    char *errorRetiro;
    char *errorIngreso;
    char *errorTransaccion;
} Error;

/// @brief Estructura que define una cuenta dentro del sistema
typedef struct
{
    char numero_cuenta[MAX_LENGTH_ID];
    char titular[MAX_LENGTH_NAME];
    char saldo[MAX_LENGTH_SALDO];
    int num_transacciones;
} Cuenta;

typedef struct
{
    char *estado;
    char *tipoTransaccion;
    char *mensaje;
    char *id;
    float cantidad;
} Transaccion;

/// @brief Función que se encarga de imprimir en el archivo "banco.log" las acciones del banco
/// @param log Mensaje a imprimir en el archivo
/// @param flag Variable que indica si la escritura va dirigida hacia el archivo "banco.log" o "transacciones.log" | 0 = banco.log - 1 = transacciones.log
void escrituraLogGeneral(char *log, int flag)
{

    FILE *file;
    char linea[MAX_LINE_LENGTH];
    time_t t; // Para esta función es necesaria la libreria de time.h

    struct tm *tm_info; // esto declara la estructura del tiempo y la fecha actual

    sem_unlink("/semaforo_banco");
    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0644, 1);

    if (semaforo_transacciones == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_banco);
    sem_wait(semaforo_transacciones);

    if (flag == 0)
        file = fopen("logs/banco.log", "a+");
    else if (flag == 1)
        file = fopen("logs/transacciones.log", "a+");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n", 0);
        return;
    }

    time(&t);
    tm_info = localtime(&t);                                      // esta funcion asigna los valores de la fecha y hora actuales al struct declarado arriba
    strftime(linea, sizeof(linea), "%Y-%m-%d %H:%M:%S", tm_info); // esta funcion crear el string con los valores propios de la fecha y hora actuales

    // Escribimos en el archivo de log la acción realizada
    fprintf(file, "[%s] %s", linea, log);

    fclose(file);

    sem_post(semaforo_transacciones);
    sem_post(semaforo_banco);

    sem_close(semaforo_banco);
    sem_close(semaforo_transacciones);
}

/// @brief Función para mostrar un "loading bar"
void mostrarCarga()
{
    char barra[] = "===============================";
    printf("Cargando: ");
    for (int i = 0; i <= 30; i++)
    {
        printf("\rCargando: [%.*s] %d%%", i, barra, (i * 100) / 30);
        fflush(stdout);
        usleep(100000); // 100ms
    }
    printf("\n");
}

/// @brief funcion de carga con puntos suspensivos
void carga_animada()
{
    for (int i = 0; i < 3; i++)
    {
        printf("\rCargando");
        for (int j = 0; j <= i; j++)
        {
            printf(".");
        }
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
CONFIG leer_configuracion(CONFIG configuracion)
{

    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("config/banco.config", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de configuración\n", 0);
        exit(1);
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
            else if (strcmp(key, "UMBRAL_INGRESOS") == 0)
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

    return (configuracion);
}

/// @brief Función para comprobar que solo se introducen números
/// @param str Cadena a comprobar
/// @return 0 = si es es una cadena con texto, 1 = si es válido
int esNumeroValido(char *str)
{
    // Verifica si la cadena contiene solo números (y opcionalmente un punto decimal)
    int puntoDecimalEncontrado = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/// @brief Función para comprobar que una cadena no contiene carácteres especiales
/// @param str Cadena a comprobar
/// @return 0 = si existe un caracter no permitido, 1 = si es válido
int esCadenaValida(const char *str) {
    // Recorre cada carácter de la cadena
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum(str[i])) {
            // Si el carácter no es alfanumérico (ni letra ni número), es inválido
            return 0;  // Retorna 0 (falso) si hay un carácter no permitido
        }
    }
    return 1;  // Retorna 1 (verdadero) si todos los caracteres son válidos
}