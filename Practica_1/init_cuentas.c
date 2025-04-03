#include "banco.h"

/// @brief Función que se encarga de compilar todos los archivos .c que albergamos dentro de nuestro repositorio
/// @param fichero Nombre del fichero sin extensión a compilar
/// @param i variable de control de iteraciones
/// @return devuelve un valor que comprueba el estado de la ejecución de la variable. (0 = Ok / 1 = error)
int compilarFicheros(char *fichero, int i)
{
    pid_t pid = fork();

    char ficheroc[50];

    strcpy(ficheroc, fichero);
    strcat(ficheroc, ".c");

    if (pid < 0)
    {
        escrituraLogGeneral("Error al hacer fork\n", 0);
        return 1;
    }
    else if (pid == 0) // Proceso hijo
    {
        // utilizamos execlp para ejecutar comandos de shell sin tener que especificar la ruta
        escrituraLogGeneral("Ejecutando gcc para compilación...\n", 0); // Solo se ejecuta si execlp falla
        execlp("gcc", "gcc", ficheroc, "-o", fichero, (char *)NULL);
        escrituraLogGeneral("Error al ejecutar gcc\n", 0); // Solo se ejecuta si execlp falla

        exit(1);
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
        monitor};

    
    // En caso de no funcionar los semáforos debido a estar bloqueados por otra ejecución anterior usar esto para eliminarlos manualmente
    
    sem_unlink("/semaforo_cuentas");
    sem_unlink("/semaforo_banco");
    sem_unlink("/semaforo_config");
    sem_unlink("/semaforo_transacciones");

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_banco);

    // abrimos el fichero de log de errores
    FILE *archivoErrores = fopen("logs/banco.log", "a");
    if (archivoErrores == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo .log\n", 0);
        return 1;
    }

    sem_wait(semaforo_cuentas);

    // abrimos el fichero de cuentas
    FILE *archivoCuentas = fopen("cuentas.dat", "a");
    if (archivoCuentas == NULL)
    {
        escrituraLogGeneral("Error al crear el archivo de cuentas.\n", 0);
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
    sem_post(semaforo_cuentas);
    sem_post(semaforo_banco);

    for (int i = 0; i < 3; i++)
        compilarFicheros(variables[i], i);

    sem_wait(semaforo_cuentas);
    escrituraLogGeneral("Compilación exitosa. Ejecutando banco...\n", 0);
    sem_post(semaforo_cuentas);

    sem_close(semaforo_banco);
    sem_close(semaforo_cuentas);

    execlp("./banco", "./banco", NULL);

    // sem_wait(semaforo_cuentas);
    escrituraLogGeneral("Error al ejecutar ./banco\n", 0);
    // sem_post(semaforo_cuentas);

    exit(1);

    return 0;
}