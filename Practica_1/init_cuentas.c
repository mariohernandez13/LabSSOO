#include "banco.h"

/// @brief Función que se encarga de compilar todos los archivos .c que albergamos dentro de nuestro repositorio
/// @param fichero Nombre del fichero sin extensión a compilar
/// @param i variable de control de iteraciones
/// @return devuelve un valor que comprueba el estado de la ejecución de la variable. (0 = Ok / 1 = error)
int compilarFicheros(char *fichero, int i)
{
    pid_t pid = fork();
    CONFIG configuracion;

    char ficheroc[50];

    strcpy(ficheroc, fichero);
    strcat(ficheroc, ".c");

    configuracion = leer_configuracion(configuracion);

    if (pid < 0)
    {
        escrituraLogGeneral("Error al hacer fork en init_cuentas.c, en función: compilarFicheros\n", 0);
        return 1;
    }
    else if (pid == 0) // Proceso hijo
    {
        // utilizamos execlp para ejecutar comandos de shell sin tener que especificar la ruta
        escrituraLogGeneral("Ejecutando gcc para compilación...\n", 0); // Solo se ejecuta si execlp falla
        execlp("gcc", "gcc", ficheroc, "-o", fichero, (char *)NULL);
        escrituraLogGeneral("Error al ejecutar gcc en init_cuentas.c, en función: compilarFicheros\n", 0); // Solo se ejecuta si execlp falla

        exit(1);
    }

    return 0;
}

void crearDirectorio(Cuenta cuentas[], int numCuentas)
{
    // crea el directorio si no existe y controla errores
    if (mkdir("transacciones", 0777) == -1)
    {
        escrituraLogGeneral("Error al crear el directorio transacciones", 0);
        exit(1);
    }

    for (int i = 0; i < numCuentas; i++)
    {
        char path[100];
        snprintf(path, sizeof(path), "transacciones/%s", cuentas[i].numero_cuenta);

        // Crear el directorio para el usuario
        if (mkdir(path, 0777) == -1)
        {
            escrituraLogGeneral("Error al crear el directorio del usuario", 0);
            exit(1);
        }

        // Crear el archivo transacciones.log dentro del directorio
        char logPath[150];
        snprintf(logPath, sizeof(logPath), "%s/transacciones.log", path);

        FILE *logFile = fopen(logPath, "a");
        if (logFile == NULL)
        {
            char mensajeError[200];
            snprintf(mensajeError, sizeof(mensajeError), "Error al crear el archivo de log para el usuario %s\n", cuentas[i].numero_cuenta);
            escrituraLogGeneral(mensajeError, 0);
            exit(1);
        }

        // Escribir un mensaje inicial en el log
        fprintf(logFile, "Log inicializado para el usuario %s\n", cuentas[i].numero_cuenta);
        fclose(logFile);
    }
}

/// @brief abre el archivo cuentas.dat y añade usuarios inventados
/// @return 0 si se ejecuta correctamente / 1 si hay error al abrir el fichero
int main()
{
    Cuenta cuentas[20]; // Definimos el array de cuentas predeterminado del sistema

    char *banco = "banco";
    char *usuario = "usuario";
    char *monitor = "monitor";
    char *alerta = "alerta";

    char *variables[] = {
        banco,
        usuario,
        monitor,
        alerta};

    long tamañoArchivoError;

    // En caso de no funcionar los semáforos debido a estar bloqueados por otra ejecución anterior usar esto para eliminarlos manualmente

    sem_unlink("/semaforo_cuentas");
    sem_unlink("/semaforo_banco");
    sem_unlink("/semaforo_config");
    sem_unlink("/semaforo_transacciones");

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir los semáforos en init_cuentas en función: main\n", 0);
        exit(1);
    }

    sem_wait(semaforo_banco);

    // abrimos el fichero de log de errores
    FILE *archivoLog = fopen("logs/banco.log", "a");
    if (archivoLog == NULL)
    {
        perror("Error al abrir el archivo .log\n");
        return 1;
    }

    sem_wait(semaforo_cuentas);

    // abrimos el fichero de cuentas
    FILE *archivoCuentas = fopen("data/cuentas.dat", "a");
    if (archivoCuentas == NULL)
    {
        escrituraLogGeneral("Error al crear el archivo de cuentas en init_cuentas en función: main\n", 0);
        fclose(archivoLog);
        return (1);
    }

    // abrimos o creamos el archivo de errores.dat
    FILE *archivoErroresDat = fopen("data/errores.dat", "a");
    if (archivoErroresDat == NULL)
    {
        escrituraLogGeneral("Error al crear el archivo de errrores.\n", 0);
        fclose(archivoErroresDat);
        return (1);
    }

    // Comprobar si el fichero de cuentas ya tiene datos
    fseek(archivoCuentas, 0, SEEK_END);
    long tamañoArchivo = ftell(archivoCuentas);

    if (tamañoArchivo > 0)
    {
        fclose(archivoCuentas);
        fclose(archivoLog);
    }
    else
    {
        Cuenta tempCuentas[] = {
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

        memcpy(cuentas, tempCuentas, sizeof(tempCuentas));

        int numCuentas = sizeof(cuentas) / sizeof(cuentas[0]); // Número de cuentas

        // Escribir las cuentas en el archivo
        for (int i = 0; i < numCuentas; i++)
        {

            if (i == numCuentas - 1)
            {
                fprintf(archivoCuentas, "%s,%s,%s,%d",
                        cuentas[i].numero_cuenta,
                        cuentas[i].titular,
                        cuentas[i].saldo,
                        cuentas[i].num_transacciones);
            }
            else
            {
                fprintf(archivoCuentas, "%s,%s,%s,%d\n",
                        cuentas[i].numero_cuenta,
                        cuentas[i].titular,
                        cuentas[i].saldo,
                        cuentas[i].num_transacciones);
            }
        }

        fclose(archivoCuentas);
        fclose(archivoLog);
    }

    sem_post(semaforo_cuentas);
    sem_post(semaforo_banco);

    // Comprobar si el fichero de cuentas ya tiene datos
    fseek(archivoErroresDat, 0, SEEK_END);
    tamañoArchivoError = ftell(archivoErroresDat);

    if (tamañoArchivoError > 0)
    {
        fclose(archivoErroresDat);
    }
    else
    {
        Error errores[] = {
            {"1001", "0", "0", "0"},
            {"1002", "0", "0", "0"},
            {"1003", "0", "0", "0"},
            {"1004", "0", "0", "0"},
            {"1005", "0", "0", "0"},
            {"1006", "0", "0", "0"},
            {"1007", "0", "0", "0"},
            {"1008", "0", "0", "0"},
            {"1009", "0", "0", "0"},
            {"1010", "0", "0", "0"},
            {"1011", "0", "0", "0"},
            {"1012", "0", "0", "0"},
            {"1013", "0", "0", "0"},
            {"1014", "0", "0", "0"},
            {"1015", "0", "0", "0"},
            {"1016", "0", "0", "0"},
            {"1017", "0", "0", "0"},
            {"1018", "0", "0", "0"},
            {"1019", "0", "0", "0"}};

        int numCuentas = sizeof(errores) / sizeof(errores[0]); // Número de cuentas

        crearDirectorio(cuentas, numCuentas);

        // Escribir las cuentas en el archivo
        for (int i = 0; i < numCuentas; i++)
        {

            if (i == numCuentas - 1)
            {
                fprintf(archivoErroresDat, "%s,%s,%s,%s",
                        errores[i].id,
                        errores[i].errorRetiro,
                        errores[i].errorIngreso,
                        errores[i].errorTransaccion);
            }
            else
            {
                fprintf(archivoErroresDat, "%s,%s,%s,%s\n",
                        errores[i].id,
                        errores[i].errorRetiro,
                        errores[i].errorIngreso,
                        errores[i].errorTransaccion);
            }
        }

        fclose(archivoErroresDat);
    }

    for (int i = 0; i < 4; i++)
        compilarFicheros(variables[i], i);

    sem_wait(semaforo_cuentas);
    escrituraLogGeneral("Compilación exitosa. Ejecutando banco...\n", 0);
    sem_post(semaforo_cuentas);

    sem_close(semaforo_banco);
    sem_close(semaforo_cuentas);

    execlp("./banco", "./banco", NULL);

    // sem_wait(semaforo_cuentas);
    escrituraLogGeneral("Error al ejecutar ./banco en init_cuentas.c, en función:  main\n", 0);
    // sem_post(semaforo_cuentas);

    exit(1);

    return 0;
}