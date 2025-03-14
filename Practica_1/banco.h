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
