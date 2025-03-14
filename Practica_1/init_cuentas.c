#include "banco.h"

/// @brief función que compila los ficheros .c del proyecto
/// @return 0 en caso de éxito / 1 en caso de error
int compilarFicheros(char *fichero)
{
    pid_t pid = fork();

    char ficheroc[50];

    strcpy(ficheroc, fichero);
    strcat(ficheroc, ".c");

    if (pid < 0)
    {
        escrituraLogGeneral("Error al hacer fork\n");
        return 1;
    }
    else if (pid == 0) // Proceso hijo
    {
        // utilizamos execlp para ejecutar comandos de shell sin tener que especificar la ruta
        escrituraLogGeneral("Ejecutando gcc para compilación...\n"); // Solo se ejecuta si execlp falla
        execlp("gcc", "gcc", ficheroc, "-o", fichero, (char *)NULL);
        escrituraLogGeneral("Error al ejecutar gcc\n"); // Solo se ejecuta si execlp falla

        exit(1);
    }
    else // Proceso padre
    {
        int estado;
        waitpid(pid, &estado, 0); // Espera a que termine el hijo

        // verificamos el estado de salida del hijo y si terminó correctamente
        if (WIFEXITED(estado) && WEXITSTATUS(estado) == 0)
        {
            escrituraLogGeneral("Compilación exitosa. Ejecutando banco...\n");

            pid_t pid2 = fork();
            if (pid2 == 0)
            {
                execlp("./banco", "./banco", NULL);
                escrituraLogGeneral("Error al ejecutar ./banco\n");
                exit(1);
            }
            else if (pid2 > 0)
                waitpid(pid2, &estado, 0);
            else
            {
                escrituraLogGeneral("Error al hacer fork para ejecutar ./banco\n");
                return 1;
            }
        }
        else
        {
            printf("Error en la compilación.\n");
            return 1;
        }
    }

    return 0;
}

/// @brief abre el archivo cuentas.dat y añade usuarios inventados
/// @return 0 si se ejecuta correctamente / 1 si hay error al abrir el fichero
int main()
{
    char *banco = "banco";
    char *usuario = "usuario";
    char *monitor = "monitor";

    char *variables[] = {
        banco,
        usuario, 
        monitor
    }; 

    // abrimos el fichero de log de errores
    FILE *archivoErrores = fopen("banco.log", "a");
    if (archivoErrores == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo .log\n");
        return 1;
    }

    // abrimos el fichero de cuentas
    FILE *archivoCuentas = fopen("cuentas.dat", "a");
    if (archivoCuentas == NULL)
    {
        escrituraLogGeneral("Error al crear el archivo de cuentas.\n");
        fclose(archivoErrores);
        return 1;
    }

    // Comprobar si el fichero de cuentas ya tiene datos
    fseek(archivoCuentas, 0, SEEK_END);
    long tamañoArchivo = ftell(archivoCuentas);

    if (tamañoArchivo > 0)
    {
        fclose(archivoCuentas);
        fclose(archivoErrores);
    }
    else
    {
        Cuenta cuentas[] = {
            {"1001", "Alejandro Abajos", "1500"},
            {"1002", "Ania Gonsales", "2300"},
            {"1003", "Marito Fernandez", "3200"},
            {"1004", "Javieh Afrenta", "6000"},
            {"1005", "Peter Parker", "275.25"},
            {"1006", "Laura Torres", "1300.60"},
            {"1007", "Miguel Romero", "800.00"},
            {"1008", "Sofia Herrera", "920.50"},
            {"1009", "Luis Vargas", "3100.30"},
            {"1010", "Carmen Morales", "480.80"},
            {"1011", "Ricardo Jimenez", "2100.00"},
            {"1012", "Elena Castro", "3600.40"},
            {"1013", "Diego Luna", "1550.00"},
            {"1014", "Isabel Flores", "775.75"},
            {"1015", "Roberto Mendoza", "2350.20"},
            {"1016", "Quevedo Estilton", "9990.80"},
            {"1017", "Hector Cruz", "400.00"},
            {"1018", "Valeria Suarez", "1980.90"},
            {"1019", "Adrian Paredes", "300.00"}};

        int numCuentas = sizeof(cuentas) / sizeof(cuentas[0]); // Número de cuentas

        // Escribir las cuentas en el archivo
        for (int i = 0; i < numCuentas; i++)
        {
            fprintf(archivoCuentas, "%s,%s,%s,%d\n",
                    cuentas[i].numero_cuenta,
                    cuentas[i].titular,
                    cuentas[i].saldo,
                    cuentas[i].num_transacciones);
        }

        fclose(archivoCuentas);
        fclose(archivoErrores);
    }

    for (int i = 0; i < 3; i++)
        compilarFicheros(variables[i]);

    sem_t *semaforo_config = sem_open("/semaforo_config", O_CREAT, 0644, 1);

    return 0;
}