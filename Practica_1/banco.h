#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <bits/pthreadtypes.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 255
#define MAX_LENGTH_NAME 50
#define MAX_LENGTH_SALDO 10
#define MAX_LENGTH_ID 6

/// @brief Estructura de la configuración del sistema del banco
typedef struct {
    int limiteRetiros;
    int limiteTransferencia;
    int umbralRetiros;
    int umbralTransferencias;
    int numHilos;
    char archivoCuentas[MAX_LINE_LENGTH];
    char archivoLog[MAX_LINE_LENGTH];
    char archivoTransacciones[MAX_LINE_LENGTH];
} CONFIG;

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
    char* tipoTransaccion[50];
    char* mensaje[255];

} Transaccion;

/// @brief Función que se encarga de imprimir en el archivo "banco.log" las acciones del banco
/// @param log Mensaje a imprimir en el archivo
void escrituraLogGeneral(char *log){

    FILE *file;
    char linea[MAX_LINE_LENGTH];
    time_t t; // Para esta función es necesaria la libreria de time.h

    struct tm *tm_info; // esto declara la estructura del tiempo y la fecha actual

    file = fopen("banco.log", "a+");
    
    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
        return;
    }

    time(&t);
    tm_info = localtime(&t); // esta funcion asigna los valores de la fecha y hora actuales al struct declarado arriba
    strftime(linea, sizeof(linea), "%Y-%m-%d %H:%M:%S", tm_info); // esta funcion crear el string con los valores propios de la fecha y hora actuales

    // Escribimos en el archivo de log la acción realizada
    fprintf(file, "[%s] %s", linea, log);

    fclose(file);
}