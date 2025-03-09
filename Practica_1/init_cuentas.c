#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// este programa llama con un .sh a ./banco.c y ./monitor.c
// además configura el archivo de cuentas, crea archivo de errores (como properties)
// y si lla está llama a exec().

typedef struct
{
    int numero_cuenta;
    char titular[50];
    float saldo;
    int num_transacciones;
} Cuenta;

/// @brief abre el archivo cuentas.dat y añade usuarios inventados
/// @return 0 si se ejecuta correctamente / 1 si hay error al abrir el fichero
int main()
{
    // abrimos el fichero de log de errores
    FILE *archivoErrores = fopen("errGenerales.log", "a");
    if (archivoErrores == NULL)
    {
        perror("Error al abrir el archivo .log");
        return 1;
    }

    // abrimos el fichero de cuentas
    FILE *archivoCuentas = fopen("cuentas.dat", "a");
    if (archivoCuentas == NULL)
    {
        fprintf(archivoErrores, "Error al crear el archivo de cuentas.\n");
        fclose(archivoErrores);
        return 1;
    }

    // Comprobar si el fichero de cuentas ya tiene datos
    fseek(archivoCuentas, 0, SEEK_END);
    long tamañoArchivo = ftell(archivoCuentas);
    if (tamañoArchivo > 0)
    {
        // El archivo 'cuentas.dat' ya tiene datos, no se sobreescribirá
        fclose(archivoCuentas);
        fclose(archivoErrores);
        return 0;
    }

    Cuenta cuentas[] = {
        {1001, "Alejandro Abajos", 1500},
        {1002, "Ania Gonsales", 2300},
        {1003, "Marito Fernandez", 3200},
        {1004, "Javieh Afrenta", 6000},
        {1005, "Peter Parker", 275.25},
        {1006, "Laura Torres", 1300.60},
        {1007, "Miguel Romero", 800.00},
        {1008, "Sofia Herrera", 920.50},
        {1009, "Luis Vargas", 3100.30},
        {1010, "Carmen Morales", 480.80},
        {1011, "Ricardo Jimenez", 2100.00},
        {1012, "Elena Castro", 3600.40},
        {1013, "Diego Luna", 1550.00},
        {1014, "Isabel Flores", 775.75},
        {1015, "Roberto Mendoza", 2350.20},
        {1016, "Quevedo Estilton", 9990.80},
        {1017, "Hector Cruz", 400.00},
        {1018, "Valeria Suarez", 1980.90},
        {1019, "Adrian Paredes", 300.00}
    };

    int numCuentas = sizeof(cuentas) / sizeof(cuentas[0]); // Número de cuentas

    // Escribir las cuentas en el archivo
    for (int i = 0; i < numCuentas; i++)
    {
        fprintf(archivoCuentas, "%d,%s,%.2f,%d\n",
                cuentas[i].numero_cuenta,
                cuentas[i].titular,
                cuentas[i].saldo,
                cuentas[i].num_transacciones);
    }

    fclose(archivoCuentas);
    fclose(archivoErrores);

    return 0;
}

// Definir la ruta al script .sh
// const char *pathInit = "./init.sh";

// // Llamar al script usando system()
// int resultado = system(pathInit);

// // Comprobar si se ejecutó correctamente
// if (resultado == 0)
// {
//     printf("¡Script ejecutado correctamente!\n");
// }
// else
// {
//     printf("Error al ejecutar el script.\n");
// }