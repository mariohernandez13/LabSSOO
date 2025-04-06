#include "banco.h"

CONFIG configuracion;
int contadorAlertas;

void menuAdmin()
{
    char contraseña[50] = "";
    int opcion = 0;
    char *comparacion = 0;

    system("clear");
    printf("=====================================\n");
    printf("Introduce la contraseña: \n");
    while(getchar() != '\n');
    fgets(contraseña, sizeof(contraseña), stdin);

    comparacion = strstr(contraseña, "admin");

    if (!comparacion)
    {
        escrituraLogGeneral("🟥 Contraseña incorrecta\n", 0);
        return;
    }
    else
    {
        escrituraLogGeneral("Contraseña correcta, accediendo al menú de administrador\n", 0);
        do{

            system("clear");
            printf("=====================================\n");
            printf("       🌟 BIENVENIDO ADMIN 🌟        \n");
            printf("=====================================\n");
            printf("1. Mostrar configuracion actual\n");
            printf("2. Mostrar arbol de procesos de la aplicacion\n");
            printf("3. Mostrar semaforos activos en el sistema\n");
            printf("4. Mostrar pid de banco con el pid de los usuarios hijos\n");
            printf("5. Mostrar hilos activos actualmente\n");
            printf("6. Salir\n");
            printf("=====================================\n");
            printf("👉 Introduce una opción: ");
            scanf("%d", &opcion);

            switch (opcion)
            {
                case 3:
                    printf("\n");
                    printf("=====================================\n");
                    printf("🚦 Los semáforos activos en el sistema son: \n");
                    system("ls /dev/shm/");
                    printf("=====================================\n");
                    sleep(5);
                    break;
                case 6:
                    break;
            }
        }while (opcion != 6);
    }
    return;
}

/// @brief Función que se encarga de recibir alertas de la tubería FIFO creada entre Monitor y Banco
/// @return NULL
/// @note Se ejecuta en un hilo por separado para no interrumpir el flujo principal del programa
void *recibirAlertas()
{
    int fifo_fd;
    char buffer[256];

    // Verificar si la FIFO ya existe

    if (mkfifo(FIFO1, 0666) == -1)
    {
        escrituraLogGeneral("🟥 Error al crear la tubería FIFO1 en banco\n", 0);
    }

    fifo_fd = open(FIFO1, O_RDONLY);

    if (fifo_fd == -1)
    {
        escrituraLogGeneral("🟥 Error al abrir la tubería en banco", 0);
    }

    while (1)
    {
        ssize_t bytesRead = read(fifo_fd, buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            buffer[strcspn(buffer, "\n")] = 0; // Limpiar \n al final del mensaje
            escrituraLogGeneral("🚨 ALERTA RECIBIDA\n", 0);
            contadorAlertas++;
        }
        else if (bytesRead == -1)
        {
            escrituraLogGeneral("🟥 Error al leer de la tubería FIFO1\n", 0);
            break;
        }
        sleep(1);
    }

    close(fifo_fd);
    return NULL;
}

/// @brief Limpia los strings de "\n"
/// @param string String que queremos "limpiar" de caracteres indeseados
void limpiezaString(char *string)
{
    for (int i = 0; i < strlen(string); i++)
        if (string[i] == '\n')
            string[i] = '\0';
}


/// @brief Función que se encarga de registrar uan nueva cuenta en el sistema del banco
/// @param cuenta Parametros de la nueva cuenta
void registroCuenta(Cuenta cuenta)
{
    FILE *fileCuenta;
    FILE *fileError;
    char linea[MAX_LINE_LENGTH] = "";
    char lineaError[MAX_LINE_LENGTH] = "";

    // Limpiamos los "\n" de nombre y de saldo porque vienen con dichos caracteres
    limpiezaString(cuenta.titular);
    limpiezaString(cuenta.saldo);

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0644, 1);

    // Si la apertura de cualquiera de los semaforos es erronea, se desestima la funcion y mandamos un log
    if (semaforo_cuentas == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir uno de los semaforos a la hora de hacer un resgistro", 0);
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    fileCuenta = fopen("data/cuentas.dat", "a+");

    if (fileCuenta == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas\n", 0);
        fclose(fileCuenta);
        return;
    }

    fileError = fopen("data/errores.dat", "a+");

    // Si el archivo falla paramos la funcion y mandamos un log a banco.log
    if (fileError == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de errores\n", 0);
        fclose(fileError);
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

    fputs(linea, fileCuenta);

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco\n", 0);

    fclose(fileCuenta);

    // Concatenamos strings para formar la linea a añadir en el archivo de errores.dat
    strcpy(lineaError, "\n");
    strcat(lineaError, cuenta.numero_cuenta);
    strcat(lineaError, ",");
    strcat(lineaError, "0");
    strcat(lineaError, ",");
    strcat(lineaError, "0");
    strcat(lineaError, ",");
    strcat(lineaError, "0");
    
    // Ponemos la linea de error en el archivo de errores.dat
    fputs(lineaError, fileError);

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco. Se ha creado una nueva fila en errores.dat\n", 0);

    fclose(fileError);

    // Manejamos los semaforos para evitar problemas de concurrencia
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
    if (atoi(id) < 1000)
    {
        validez = 0;
        escrituraLogGeneral("🟥 El id introducido no es válido debido a que es menor a 1000\n", 0);
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
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        system("clear");

        printf("=====================================\n");
        printf("🔐 Bienvenido al LogIn de SafeBank        \n");
        printf("=====================================\n");
        printf("💳 Introduce tu ID (debe ser 1000 o mayor): ");

        fflush(stdout);

        if (fgets(id, sizeof(id), stdin) == NULL) {
            printf("❌ Error al leer el ID. Intenta nuevamente.\n");
            continue;
        }

        printf("\n✅ ID ingresado: %s\n", id);
        printf("=====================================\n");

        comprobacion = comprobarId(id, flg_log);

        if (!comprobacion)
        {
            printf("❌ ID inválido o no registrado. Por favor, intenta nuevamente.\n");
        }

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
        escrituraLogGeneral("🟥 Error al ejecutar ./usuario\n", 0);
        exit(1);
    }
}

/// @brief función que crea el hilopara leer la tubería constantemente
void iniciarHiloAlerta()
{
    pthread_t hiloAlerta;

    // Crear el hilo para recibir alertas
    if (pthread_create(&hiloAlerta, NULL, &recibirAlertas, NULL) != 0)
    {
        escrituraLogGeneral("🟥 Error al crear el hilo de alertas\n", 0);
    }
}

/// @brief Menú de inicio del Banco SafeBank
void menuBanco()
{
    int opcion = 0;

    do
    {
        if (contadorAlertas != 0)
        {
            contadorAlertas = 0;
            system("clear");
            printf("===================================\n");
            printf("🚨 ALERTA RECIBIDA\n");
            sleep(1);
            printf("🚨 ALERTA RECIBIDA\n");
            sleep(1);
            printf("🚨 ALERTA RECIBIDA\n");
            sleep(1);
            printf("===================================\n");
            sleep(3);
        }

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
        case 0:
            menuAdmin();
            break;
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
    system("clear");
}

int main(int argc, char *argv[])
{
    unlink(FIFO1);
    unlink(FIFO2);

    pid_t pid = fork();

    if (pid == 0)
    {
        execlp("gnome-terminal", "gnome-terminal", "--", "./monitor", NULL);
        escrituraLogGeneral("Error al ejecutar ./monitor\n", 0);
        exit(1);
    }

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

    configuracion = leer_configuracion(configuracion);

    iniciarHiloAlerta();

    menuBanco();

    system("pkill -f usuario"); // cuando cerramos banco matamos todos los procesos de usuarios
    system("pkill -f './monitor'");

    return (0);
}