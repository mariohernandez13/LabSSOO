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

    semaforo_config = sem_open("/semaforo_config", O_CREAT, 0644, 1);

    if (semaforo_config == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_config);

    file = fopen("banco.config", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de configuración\n", 0);
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
            else if (strcmp(key, "UMBRAL_RETIROS") == 0)
            {
                configuracion.umbralRetiros = atoi(value);
            }
            else if (strcmp(key, "UMBRAL_TRANSFERENCIAS") == 0)
            {
                configuracion.umbralTransferencias = atoi(value);
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

    sem_post(semaforo_config);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n", 0);

    sem_close(semaforo_config);

    return (state);
}

void recibir_alertas()
{
    int fifo_fd;
    char buffer[256];

    // Verificar si la FIFO ya existe
    if (access(FIFO1, F_OK) != 0)
    {
        if (mkfifo(FIFO1, 0666) == -1)
        {
            escrituraLogGeneral("🟥 Error al crear la tubería FIFO1\n", 0);
            return;
        }
    }

    fifo_fd = open(FIFO1, O_RDONLY);
    if (fifo_fd == -1)
    {
        escrituraLogGeneral("🟥 Error al abrir la tubería en banco", 0);
        return;
    }

    while (read(fifo_fd, buffer, sizeof(buffer)) > 0)
    {
        buffer[strcspn(buffer, "\n")] = 0; // Limpiar \n al final del mensaje
        printf("🚨 ALERTA RECIBIDA: %s\n", buffer);

    }

    close(fifo_fd);
}

/// @brief Limpia los strings de "\n"
/// @param string String que queremos "limpiar" de caracteres indeseados
void limpiezaString(char *string)
{
    for (int i = 0; i < strlen(string); i++)
        if (string[i] == '\n')
            string[i] = '\0';
}

/// 
/// @brief Función que se encarga de registrar uan nueva cuenta en el sistema del banco
/// @param cuenta Parametros de la nueva cuenta
void registroCuenta(Cuenta cuenta)
{

    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";

    // Limpiamos los "\n" de nombre y de saldo porque vienen con dichos caracteres
    limpiezaString(cuenta.titular);
    limpiezaString(cuenta.saldo);

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    file = fopen("data/cuentas.dat", "a+");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas\n", 0);
        return;
    }

    // Concatenamos los strings pasados como valores
    strcpy(linea, "\n");
    strcat(linea, cuenta.numero_cuenta);
    strcat(linea, ",");
    strcat(linea, cuenta.titular);
    strcat(linea, ",");
    strcat(linea, cuenta.saldo);
    strcat(linea, ",");
    strcat(linea, "0");

    fputs(linea, file);

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco\n", 0);

    fclose(file);

    sem_post(semaforo_cuentas);
    sem_close(semaforo_cuentas);
    sem_close(semaforo_banco);
}

/// @brief Función que comprueba si el id pasado como parámetro se encuentra en el archivo "cuentas.dat"
/// @param id Id que queremos comprobar, desde el login o desde el registro
/// @param flag Variable que nos indica si nos encontramos ante un caso de LogIn o de registro
/// @return Devuelve un valor numérico que indica si es válido el id o no: 0 = error en id // 1 = id valido
int existeID(char *id, int flag)
{

    limpiezaString(id);

    int esValido = 1;
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    sem_unlink("/semaforo_cuentas");
    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED)
    {
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_cuentas);

    file = fopen("data/cuentas.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas\n", 0);
        return 0;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;
        key = strtok(linea, ",");
        if (flag == 0)
        {
            if (strcmp(key, id) == 0)
            {
                escrituraLogGeneral("El id ya existe\n", 0);
                esValido = 0;
                break;
            }
        }
        else if (flag == 1)
        {
            if (strcmp(key, id) == 0)
            {
                esValido = 1;
                break;
            }
            else
                esValido = 0;
        }
    }

    fclose(file);

    sem_post(semaforo_cuentas);
    sem_close(semaforo_cuentas);

    return esValido;
}

/// @brief Comprueba en el archivo de cuentas.dat que el id introducido no existe
/// @param id Id introducido por el usuario en el resgistro
/// @param flag Valor que indica si se está llegando a la función desde LogIn o desde registro: 0 = registro || 1 = LogIn
/// @return Valor numérico que indica validez del Id: 0 = error en id // 1 = id valido
int comprobarId(char *id, int flag)
{

    int validez = 1;
    if (atoi(id) < 100)
    {
        validez = 0;
        escrituraLogGeneral("🟥 El id introducido no es válido debido a que es menor a 100\n", 0);
        return validez;
    }

    validez = existeID(id, flag);

    return validez;
}

/// @brief Menú de registro del Banco
void registro()
{

    Cuenta cuenta;

    int comprobacion = 1;

    do
    {
        if (!comprobacion)
            printf("Ha ocurrido un error en tu intento de registro, prueba a volver a intentarlo.\n");

        printf("Bienvenido al registro de SafeBank\n");

        printf("Introduce tu nombre: (no se admiten más de 50 caracteres): \n");
        while (getchar() != '\n')
            ; // Limpieza de buffer de entrada para evitar problemas en lectura de parametros
        fgets(cuenta.titular, sizeof(cuenta.titular), stdin);

        printf("Introduce tu id: (a partir de 100): \n");
        fgets(cuenta.numero_cuenta, sizeof(cuenta.numero_cuenta), stdin);

        printf("Introduce tu saldo: \n");
        fgets(cuenta.saldo, sizeof(cuenta.saldo), stdin);

        comprobacion = comprobarId(cuenta.numero_cuenta, 0);
    } while ((comprobacion != 1) || (cuenta.titular == NULL) || (strlen(cuenta.titular) > MAX_LENGTH_NAME));

    registroCuenta(cuenta);
}

/// @brief Menú de logIn del Banco
void logIn()
{

    char id[MAX_LENGTH_ID];
    int flg_log = 1;
    int comprobacion = 1;
    pid_t pid;

    do
    {
        system("clear");

        printf("=====================================\n");
        printf("🔐 Bienvenido al LogIn de SafeBank        \n");
        printf("=====================================\n");
        printf("💳 Introduce tu ID (debe ser 1000 o mayor): ");

        while (getchar() != '\n')
            ;
        fgets(id, sizeof(id), stdin);
        printf("\n✅ ID ingresado: %s\n", id);
        printf("=====================================\n");

        comprobacion = comprobarId(id, flg_log);
    } while (!comprobacion);

    pid = fork();

    // Comprobamos que fork() no genera un error
    if (pid < 0)
    {
        escrituraLogGeneral("🟥 Error al crear la sesión de LogIn\n", 0);
        return;
    }
    else if (pid == 0)
    { // Proceso hijo
        execlp("gnome-terminal", "gnome-terminal", "--", "./usuario", id, NULL);
        // execlp("./usuario", "./usuario", id, NULL);
        escrituraLogGeneral("🟥 Error al ejecutar ./usuario\n", 0);
    }
}

/// @brief Menú de inicio del Banco SafeBank
void menuBanco()
{
    int opcion = 0;

    do
    {
        system("clear");

        printf("=====================================\n");
        printf("  🏦 Bienvenido a SafeBank 🏦         \n");
        printf("=====================================\n");
        printf("💼 ¿Qué desea hacer?\n");
        printf("🔹 1. Inicio de Sesión\n");
        printf("🔹 2. Registro\n");
        printf("🔹 3. Salir\n");
        printf("=====================================\n");
        printf("👉 Introduce una opción: ");

        scanf("%d", &opcion);

        printf("\n");
        printf("✅ Opción seleccionada: %d\n", opcion);
        printf("=====================================\n");

        switch (opcion)
        {
        case 1:
            logIn();
            break;
        case 2:
            registro(); // Llamamos a funcion registro
            break;
        default:
            break;
        }

    } while (opcion != 3);
}

int main(int argc, char *argv[])
{
    int fd[2];

    char buffer[100];
    char errorMessage[] = "Ha habido un error leyendo el archivo .config";
    char validMessage[] = "Todo correcto maquina";

    int state = 0;

    // Comprobamos que no ocurre problema al generar la pipe
    if (pipe(fd) == -1)
    {
        escrituraLogGeneral("🟥 Error en la generación de la pipe\n", 0);
        return 1;
    }

    leer_configuracion();

    menuBanco();

    system("pkill -f usuario"); // cuando cerramos banco matamos todos los procesos de usuarios
    return (0);
}