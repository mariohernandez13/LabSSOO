#include "banco.h"

CONFIG configuracion;
TablaCuentas *tabla;
int contadorAlertas;

/// @brief Función que se encarga de rellenar la memoria compartida ya creada e inicializada con el contenido de las cuentas
/// @param fichero Nombre del fichero al que va a acceder la función (debería ser el de cuentas)
/// @param tabla Variable de tipo TablaCuentas que se va a rellenar con el contenido del fichero
/// @param size Tamaño definido para la memoria compartida
/// @return Valor entero para indicar validez de la operacion, 0 = OK || 1 = Error
int cargarMemoria(char *fichero, TablaCuentas *tabla, long int size)
{
    // Inicializamos el archivo de cuentas
    FILE *file = fopen(fichero, "r");
    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas en banco.c, en función: cargarMemoria\n", 0);
        return 1;
    }

    // Inicializamos la tabla de cuentas y la cargamos de contenido
    char linea[100]; // Para evitar problemas en la lectura de la linea del archivo lo definimos con una longitud clara
    int i = 0;
    while (fgets(linea, sizeof(linea), file))
    {
        // Separamos todos los valores de cada cuenta por la coma
        char *numero_cuenta = strtok(linea, ",");
        char *titular = strtok(NULL, ",");
        char *saldo = strtok(NULL, ",");
        int numero_transacciones = atoi(strtok(NULL, ","));

        // Copiamos los valores de cada campo en cada cuenta leída del archivo y asignada a Tabla
        snprintf(tabla->cuentas[i].numero_cuenta, MAX_LENGTH_ID,
                 "%s", numero_cuenta);
        snprintf(tabla->cuentas[i].titular, MAX_LENGTH_NAME,
                 "%s", titular);
        snprintf(tabla->cuentas[i].saldo, MAX_LENGTH_SALDO,
                 "%s", saldo);

        // Guardamos el numero de transacciones como integer
        tabla->cuentas[i].num_transacciones = numero_transacciones;

        tabla->cuentas[i].numero_cuenta[MAX_LENGTH_ID - 1] = '\0';
        tabla->cuentas[i].titular[MAX_LENGTH_NAME - 1] = '\0';
        tabla->cuentas[i].saldo[MAX_LENGTH_SALDO - 1] = '\0';

        // Aumentamos el contador de cuentas existentes
        tabla->numCuentas++;
        i++;
    }

    fclose(file); // Cerramos el archivo de cuentas

    return 0;
}

/// @brief Función que se encarga de mostrar el menú del administrador del banco
/// @note Se encarga de mostrar el menú de administrado, que permite ver los hilos, semáforos y demás partes del sistema
void menuAdmin()
{
    char contraseña[50] = "";
    int opcion = 0;
    char *comparacion = 0;

    system("clear");
    printf("=====================================\n");
    printf("Introduce la contraseña: \n");
    while (getchar() != '\n')
        ;
    fgets(contraseña, sizeof(contraseña), stdin);

    comparacion = strstr(contraseña, "magia");

    if (!comparacion)
    {
        escrituraLogGeneral("🟥 Contraseña incorrecta en banco.c, en función: menuAdmin\n", 0);
        return;
    }
    else
    {
        escrituraLogGeneral("Contraseña correcta, accediendo al menú de administrador en banco.c, en función: menuAdmin\n", 0);
        do
        {

            system("clear");
            printf("=====================================\n");
            printf("       🌟 BIENVENIDO ADMIN 🌟        \n");
            printf("=====================================\n");
            printf("1. Mostrar configuración actual\n");
            printf("2. Mostrar árbol de procesos de la aplicación\n");
            printf("3. Mostrar semáforos activos en el sistema\n");
            printf("4. Mostrar pid de banco con el pid de los usuarios hijos\n");
            printf("5. Mostrar hilos activos actualmente\n");
            printf("6. Mostrar estado de memoria compartida\n");
            printf("7. Salir\n");
            printf("=====================================\n");
            printf("👉 Introduce una opción: ");
            scanf("%d", &opcion);

            switch (opcion)
            {
            case 1:
                escrituraLogGeneral("⚙️ Mostrando la configuración actual desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                // Mostramos la configuracion actual de los errores permitidos en el sistema
                printf("=====================================\n");
                printf("⚙️ La configuración de errores actual es:\n");
                printf("🔢 Umbral Total de Errores: %d\n", configuracion.umbralTotal);
                printf("💸 Umbral de Errores en Retiros: %d\n", configuracion.umbralRetiros);
                printf("💰 Umbral de Errores en Ingresos: %d\n", configuracion.umbralIngreso);
                printf("🔁 Umbral de Errores en Transferencias: %d\n", configuracion.umbralTransferencias);
                printf("=====================================\n");
                printf("\n");
                printf("=====================================\n");
                printf("⚙️ La configuración de límites en operaciones es:\n");
                printf("💲 Límite de cantidad permitida en Ingresos: %d\n", configuracion.limiteIngreso);
                printf("💲 Límite de cantidad permitida en Retiros: %d\n", configuracion.limiteRetiros);
                printf("💲 Límite de cantidad permitida en Transacción: %d\n", configuracion.limiteTransferencia);
                printf("=====================================\n");
                sleep(10);
                break;
            case 2:
                escrituraLogGeneral("🌳 Mostrando el arbol de procesos de la aplicacion desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                printf("=====================================\n");
                printf("🌳 Mostrando el árbol de procesos de la aplicación, centrándose en banco\n");
                system("pstree -p | grep banco"); // Este comando se encarga de mostrar todos los hilos activos del sistema que contengan la palabra banco
                printf("=====================================\n");
                printf("\n");
                printf("=====================================\n");
                printf("🌳 Mostrando el árbol de procesos de la aplicación, centrándose en usuario\n");
                system("pstree -p | grep usuario"); // Este comando se encarga de mostrar todos los hilos activos del sistema que contengan la palabra usuario
                printf("=====================================\n");
                sleep(5);
                break;
            case 3:
                escrituraLogGeneral("🚦 Mostrando los semáforos activos en el sistema desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                printf("=====================================\n");
                printf("🚦 Los semáforos activos en el sistema son: \n");
                system("ls /dev/shm/"); // Para poder mostrar los semaforos activos del sistema se hace un acceso a las carpetas puestas y no a ipcs debido a que los semaforos que usamos son de tipo sem_open
                printf("=====================================\n");
                sleep(5);
                break;
            case 4:
                escrituraLogGeneral("💳 Mostramos los PIDs de los procesos activos en el sistema desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                printf("=====================================\n");
                printf("💳 PIDs de los procesos del banco\n");
                system("ps aux | grep banco");
                printf("=====================================\n");
                printf("💳 PIDs de los procesos del usuario\n");
                system("ps aux | grep usuario");
                printf("=====================================\n");
                printf("💳 PIDs de los procesos del monitor\n");
                system("ps aux | grep ./monitor");
                printf("=====================================\n");
                sleep(5);
                break;
            case 5:
                escrituraLogGeneral("🧵 Mostramos los hilos activos en el sistema desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                printf("=====================================\n");
                printf("🧵 Mostramos los hilos activos de los usuarios\n");
                system("ps -e | grep -i usuario | awk '{print $1}' | xargs -I {} ps -T -p {}"); // Este comando se encarga de mostrar todos los hilos activos del sistema que contengan la palabra banco
                printf("=====================================\n");
                printf("🧵 Mostramos los hilos activos del banco\n");
                system("ps -e | grep -i banco | awk '{print $1}' | xargs -I {} ps -T -p {}"); // Este comando se encarga de mostrar todos los hilos activos del sistema que contengan la palabra banco
                printf("=====================================\n");
                sleep(5);
                break;
            case 6:
                char comando[50];
                char comandoLsof[100];
                int pid = getpid();

                sprintf(comando, "pmap -x %d", pid);
                sprintf(comandoLsof, "lsof -p %d | grep /dev/shm", pid);

                escrituraLogGeneral("📊 Mostramos el estado de la memoria compartida desde el menú de administrador en banco.c, en función: menuAdmin\n", 0);
                printf("\n");
                printf("=====================================\n");
                printf("📊 Mostramos el estado de la memoria compartida\n");
                system("ipcs -m"); // Este comando se encarga de mostrar memoria compartida
                printf("\n");
                printf("=====================================\n");
                printf("📊 Mostramos el mapa de memoria del proceso\n");
                system(comando); // Muestra el mapa de memoria del proceso banco
                printf("\n");
                printf("=====================================\n");
                printf("📊 Mostramos los archivos en uso en /dev/shm\n");
                system(comandoLsof); // Archivos en uso en /dev/shm
                printf("=====================================\n");
                sleep(15);

                break;
            case 7:
                break;
            }
        } while (opcion != 7);
    }
    return;
}

/// @brief Limpia los strings de "\n"
/// @param string String que queremos "limpiar" de caracteres indeseados
void limpiezaString(char *string)
{
    for (int i = 0; i < strlen(string); i++)
        if (string[i] == '\n')
            string[i] = '\0';
}

/// @brief Función que se encarga de crear el log de transacciones para un usuario nuevo tras su registro
/// @param numeroCuenta Numero de cuenta del nuevo usuario
void crearLogUsuario(char *numeroCuenta)
{
    FILE *file;
    char directorio[50];
    char ruta[50];

    // Asignamos a la ruta de directorio el numero de cuenta del usuario nuevo
    snprintf(directorio, sizeof(directorio), "%s/%s", "transacciones", numeroCuenta);

    // Creamos el directorio nuevo para el nuevo usuario introducido
    if (mkdir(directorio, 0777) == -1)
    {
        escrituraLogGeneral("Error al crear el directorio del usuario", 0);
        exit(1);
    }

    // Creamos la ruta del archivo de log
    snprintf(ruta, sizeof(ruta), "%s/%s/%s", "transacciones", numeroCuenta, "transacciones.log");

    file = fopen(ruta, "a");
    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de transacciones\n", 0);
        return;
    }

    // Escribimos un mensaje inicial en el log
    fprintf(file, "Log inicializado para el usuario %s\n", numeroCuenta);

    fclose(file);
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

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0666, 1);
    semaforo_banco = sem_open("/semaforo_banco", O_CREAT, 0666, 1);

    // Si la apertura de cualquiera de los semaforos es erronea, se desestima la funcion y mandamos un log
    if (semaforo_cuentas == SEM_FAILED || semaforo_banco == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir uno de los semaforos (semaforo_cuentas, semaforo_banco) a la hora de hacer un resgistro en banco.c, en función: registroCuenta", 0);
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    fileCuenta = fopen("data/cuentas.dat", "a+");

    if (fileCuenta == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas en banco.c, en función: registroCuenta\n", 0);
        fclose(fileCuenta);
        return;
    }

    fileError = fopen("data/errores.dat", "a+");

    // Si el archivo falla paramos la funcion y mandamos un log a banco.log
    if (fileError == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de errores en banco.c, en función: registroCuenta\n", 0);
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

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco en banco.c, en función: registroCuenta\n", 0);

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

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco. Se ha creado una nueva fila en errores.dat en banco.c, en función: registroCuenta\n", 0);

    fclose(fileError);

    // Manejamos los semaforos para evitar problemas de concurrencia
    sem_post(semaforo_cuentas);
    sem_close(semaforo_cuentas);
    sem_close(semaforo_banco);

    printf("\nRegistro realizado de forma correcta\n");
    sleep(2);
    printf("=====================================\n");
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
    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0666, 1);

    if (semaforo_cuentas == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir los semáforos en banco.c, en función: existeID\n", 0);
        exit(1);
    }

    sem_wait(semaforo_cuentas);

    file = fopen("data/cuentas.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de cuentas en banco.c, en función: existeID\n", 0);
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
                escrituraLogGeneral("El id ya existe en banco.c, en función: existeID\n", 0);
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
        escrituraLogGeneral("🟥 El id introducido no es válido debido a que es menor a 1000 en banco.c, en función: comprobarId\n", 0);
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
        system("clear");

        if (!comprobacion)
        {
            printf("Ha ocurrido un error en tu intento de registro, prueba a volver a intentarlo.\n");
            escrituraLogGeneral("🟥 Ha ocurrido un error en tu intento de registro, prueba a volver a intentarlo en banco.c, en función: registro\n", 0);
        }

        printf("\n");
        printf("=====================================\n");
        printf("📋 Bienvenido al registro de SafeBank\n");
        printf("=====================================\n");

        printf("=====================================\n");
        printf("🙋 Introduce tu nombre: (no se admiten más de 50 caracteres): ");
        while (getchar() != '\n')
            ; // Limpieza de buffer de entrada para evitar problemas en lectura de parametros
        fgets(cuenta.titular, sizeof(cuenta.titular), stdin);

        printf("\n");

        printf("🪪 Introduce tu id: (a partir de 100): ");
        fgets(cuenta.numero_cuenta, sizeof(cuenta.numero_cuenta), stdin);

        // TODO: Arreglar ID del usuario Incremental
        // Asignamos el id a la cuenta, que será el número de cuentas + 1000 + 1 para evitar problemas de concurrencia
        // int nuevoNumeroCuenta = 1000 + tabla->numCuentas + 1;
        // snprintf(cuenta.numero_cuenta, sizeof(cuenta.numero_cuenta), "%d", nuevoNumeroCuenta);

        printf("💰 Introduce tu saldo: ");
        fgets(cuenta.saldo, sizeof(cuenta.saldo), stdin);

        comprobacion = comprobarId(cuenta.numero_cuenta, 0);
    } while ((comprobacion != 1) || (cuenta.titular == NULL) || (strlen(cuenta.titular) > MAX_LENGTH_NAME));

    registroCuenta(cuenta);
    semaforo_tabla = sem_open("/semaforo_tabla", O_CREAT, 0666, 1);
    if (semaforo_tabla == SEM_FAILED)
    {
        escrituraLogGeneral("🟥 Error al abrir el semáforo de cuentas en usuario.c, en función: actualizarCuentas\n", 0);
        exit(1);
    }
    sem_wait(semaforo_tabla);
    tabla->cuentas[tabla->numCuentas] = cuenta; // Añadimos la cuenta a la tabla de cuentas
    tabla->numCuentas++;                        // Aumentamos el contador de cuentas existentes
    crearLogUsuario(cuenta.numero_cuenta);
    sem_post(semaforo_tabla); // Habilitamos el semaforo de cuentas de nuevo
}

/// @brief función que se encarga de vaciar el buffer de operaciones y actualizar el archivo cuentas.dat
/// @param arg
/// @return
void *vaciarBuffer(void *arg)
{
    if (buffer == NULL)
    {
        escrituraLogGeneral("🟥 Buffer no inicializado en vaciarBuffer\n", 0);
        return NULL;
    }
    char log[100];
    pthread_setname_np(pthread_self(), "hiloBuffer");

    escrituraLogGeneral("🟦 Hilo de vaciado de buffer iniciado.\n", 0);

    while (1)
    {
        semaforo_buffer = sem_open("/semaforo_buffer", O_CREAT, 0666, 1);
        if (semaforo_buffer == SEM_FAILED)
        {
            escrituraLogGeneral("🟥 Error al abrir el semáforo de cuentas en usuario.c, en función: actualizarCuentas\n", 0);
            exit(1);
        }
        sem_wait(semaforo_buffer);
        snprintf(log, sizeof(log), "✅ banco.c Posición: %d | BufIn: %d\n", buffer->fin, buffer->inicio);
        escrituraLogGeneral(log, 0);
        if (buffer->inicio != buffer->fin)
        {
            escrituraLogGeneral("🟦 Entrando en el bucle de vaciarBuffer...\n", 0);
            Cuenta op = buffer->operaciones[buffer->inicio];

            buffer->inicio = (buffer->inicio + 1) % BUFFER_SIZE;

            FILE *archivo = fopen("data/cuentas.dat", "r+b");
            if (!archivo)
            {
                escrituraLogGeneral("🟥 Error al abrir cuentas.dat\n", 0);
                continue;
            }

            semaforo_tabla = sem_open("/semaforo_tabla", O_CREAT, 0666, 1);
            if (semaforo_tabla == SEM_FAILED)
            {
                escrituraLogGeneral("🟥 Error al abrir el semáforo de cuentas en usuario.c, en función: actualizarCuentas\n", 0);
                exit(1);
            }
            sem_wait(semaforo_tabla);
            for (int i = 0; i < tabla->numCuentas; i++)
            {
                if (strcmp(tabla->cuentas[i].numero_cuenta, op.numero_cuenta) == 0)
                {
                    FILE *archivo = fopen("data/cuentas.dat", "r");
                    FILE *temporal = fopen("data/temp.dat", "w");
                    if (!archivo || !temporal)
                    {
                        escrituraLogGeneral("🟥 Error al abrir archivos de texto\n", 0);
                        return NULL;
                    }

                    char linea[256];
                    char nuevaLinea[256];

                    while (fgets(linea, sizeof(linea), archivo))
                    {
                        char numero[20], nombre[50], saldo[20], bloqueada[5];

                        sscanf(linea, "%[^,],%[^,],%[^,],%s", numero, nombre, saldo, bloqueada);

                        if (strcmp(numero, op.numero_cuenta) == 0)
                        {
                            snprintf(nuevaLinea, sizeof(nuevaLinea), "%s,%s,%s,%s\n",
                                     numero, nombre, op.saldo, bloqueada);
                            fputs(nuevaLinea, temporal);
                            escrituraLogGeneral("✅ Línea actualizada en temporal", 0);
                        }
                        else
                        {
                            fputs(linea, temporal); // Copiar línea original
                        }
                    }

                    fclose(archivo);
                    fclose(temporal);

                    // Reemplazar el archivo original por el actualizado
                    remove("data/cuentas.dat");
                    rename("data/temp.dat", "data/cuentas.dat");

                    escrituraLogGeneral("✅ Cuenta encontrada en cuentas.dat", 0);
                    break;
                }
                snprintf(log, sizeof(log), "🧐 Cuenta que estas buscando es: %s\n", op.numero_cuenta);
                escrituraLogGeneral(log, 0);
            }
            sem_post(semaforo_tabla);

            fclose(archivo);
            escrituraLogGeneral("✅ Cuenta actualizada desde buffer en disco.\n", 0);
        }
        else
        {
            usleep(500000);
        }
        sem_post(semaforo_buffer);
    }

    return NULL;
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
        while ((c = getchar()) != '\n' && c != EOF)
            ;

        system("clear");

        printf("=====================================\n");
        printf("🔐 Bienvenido al LogIn de SafeBank        \n");
        printf("=====================================\n");
        printf("💳 Introduce tu ID (debe ser 1000 o mayor): ");

        fflush(stdout);

        if (fgets(id, sizeof(id), stdin) == NULL)
        {
            printf("❌ Error al leer el ID. Intenta nuevamente.\n");
            escrituraLogGeneral("❌ Error al leer el ID. Intenta nuevamente en banco.c, en función: logIn\n", 0);
            continue;
        }

        printf("\n✅ ID ingresado: %s\n", id);
        printf("=====================================\n");

        comprobacion = comprobarId(id, flg_log);

        if (!comprobacion)
        {
            printf("❌ ID inválido o no registrado. Por favor, intenta nuevamente.\n");
            escrituraLogGeneral("❌ Error ID inválido o no registrado en banco.c, en función: logIn\n", 0);
        }

    } while (!comprobacion);

    pid = fork();

    // Comprobamos que fork() no genera un error
    if (pid < 0)
    {
        escrituraLogGeneral("🟥 Error al crear la sesión de LogIn en banco.c, en función: logIn\n", 0);
        return;
    }
    else if (pid == 0)
    { // Proceso hijo

        // Creamos archivo temporal de sesiones
        FILE *archivoSesiones;

        archivoSesiones = fopen(configuracion.archivoSesiones, "a+");

        fseek(archivoSesiones, 0, SEEK_END);  // ir al final
        fprintf(archivoSesiones, "%s\n", id); // Escribir el usuario que inicia sesión

        fclose(archivoSesiones);

        execlp("gnome-terminal", "gnome-terminal", "--", "./usuario", id, NULL);
        escrituraLogGeneral("🟥 Error al ejecutar ./usuario en banco.c, en función: logIn\n", 0);
        exit(1);
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

/// @brief Función que se encarga de crear el hilo para vaciar el buffer de operaciones
void hiloBuffer()
{
    escrituraLogGeneral("Creando el hilo para vaciar el buffer de operaciones en banco.c ...\n", 0);

    pthread_t hiloBuffer;

    // Crear el hilo para vaciar el buffer que recoge las actualizaciones de cuentas
    if (pthread_create(&hiloBuffer, NULL, &vaciarBuffer, NULL) != 0)
    {
        escrituraLogGeneral("🟥 Error al crear el hilo de buffer en banco.c, en función: hiloBuffer\n", 0);
    }
}

/// @brief Función que se encarga de recibir alertas de la tubería FIFO creada entre Monitor y Banco
/// @return NULL
/// @note Se ejecuta en un hilo por separado para no interrumpir el flujo principal del programa
void *recibirAlertas()
{
    pthread_setname_np(pthread_self(), "recibirAlertas");
    int fifo_fd;
    char buffer[256];

    // Verificar si la FIFO ya existe

    if (mkfifo(FIFO1, 0666) == -1)
    {
        escrituraLogGeneral("🟥 Error al crear la tubería FIFO1 en banco en banco.c, en función: recibirAlertas\n", 0);
    }

    fifo_fd = open(FIFO1, O_RDONLY);

    if (fifo_fd == -1)
    {
        escrituraLogGeneral("🟥 Error al abrir la tubería en banco en banco.c, en función: recibirAlertas\n", 0);
    }

    while (1)
    {
        ssize_t bytesRead = read(fifo_fd, buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            buffer[strcspn(buffer, "\n")] = 0; // Limpiar \n al final del mensaje
            escrituraLogGeneral("🚨 ALERTA RECIBIDA\n", 0);
            contadorAlertas++;
            if (contadorAlertas != 0)
            {
                contadorAlertas = 0;
                pid_t terminalAlerta = fork();

                if (terminalAlerta == 0)
                {
                    execlp("gnome-terminal", "gnome-terminal", "--", "./alerta", NULL);
                }

                contadorAlertas = 0;
            }
        }
        else if (bytesRead == -1)
        {
            escrituraLogGeneral("🟥 Error al leer de la tubería FIFO1 en banco.c, en función: recibirAlertas\n", 0);
            break;
        }

        sleep(1);
    }

    close(fifo_fd);
    return NULL;
}

/// @brief función que crea el hilopara leer la tubería constantemente
void iniciarHiloAlerta()
{
    pthread_t hiloAlerta;

    // Crear el hilo para recibir alertas
    if (pthread_create(&hiloAlerta, NULL, &recibirAlertas, NULL) != 0)
    {
        escrituraLogGeneral("🟥 Error al crear el hilo de alertas en banco.c, en función: iniciarHiloAlerta\n", 0);
    }
}

/// @brief Función que inicia la memoria compartida del sistema
/// @param size
/// @return Error = 1 | OK = 0
int inicializarMemSh(long int size)
{
    // Inicializamos una key para acceder a la memoria compartida
    key_t key = ftok(MEM_KEY, 1);
    if (key == -1)
    {
        escrituraLogGeneral("🟥 Error al generar key para memoria compartida\n", 0);
        escrituraLogGeneral(MEM_KEY, 0);
        return 1;
    }

    // Si la memoria ya estaba creada previamente, la borra para volver a inicializarla
    int shm_id = shmget(key, 0, 0666);
    if (shm_id != -1)
    {
        // Si la memoria ya existe, se elimina
        if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        {
            escrituraLogGeneral("🟥 No se pudo eliminar memoria compartida existente\n", 0);
        }
        else
        {
            escrituraLogGeneral("🟩 Memoria compartida anterior eliminada correctamente\n", 0);
        }
    }

    // Comprobamos el tamaño del archivo de cuentas
    struct stat st;
    if (stat("data/cuentas.dat", &st) == -1)
    {
        escrituraLogGeneral("🟥 Error al obtener el tamaño del archivo de cuentas en banco.c, en función: inicializarMemSh\n", 0);
        return 1;
    }

    size = 0;

    // Mientras que el tamaño del archivo sea mayor al tamaño de la memoria compartida preestablecida, aumentamos un MB su tamaño para evitar problemas
    while (size < st.st_size)
    {
        escrituraLogGeneral("🟥 El tamaño de la memoria compartida es menor que el tamaño del archivo de cuentas en banco.c, en función: inicializarMemSh\n", 0);
        size += MB; // Aumentamos el tamaño de la memoria compartida para evitar problemas de memoria
    }

    // Iniciamos memoria compartida
    shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        escrituraLogGeneral("No se ha podido iniciar memoria compartida", 0);
        return 1;
    }

    // Adjuntar memoria compartida al proceso
    TablaCuentas *tabla = (TablaCuentas *)shmat(shm_id, NULL, 0);
    if (tabla == (void *)-1)
    {
        escrituraLogGeneral("No se pudo adjuntar la memoria compartida al proceso", 0);
        return 1;
    }

    // Cargamos de contenido tabla
    if (cargarMemoria("data/cuentas.dat", tabla, size) != 0)
    {
        escrituraLogGeneral("Error al cargar la memoria compartida en banco.c, en función: inicializarMemSh\n", 0);
        return 1;
    }

    return 0;
}

int inicializarBufferCompartido()
{
    if (access(MEM_KEY, F_OK) == -1)
    {
        FILE *f = fopen(MEM_KEY, "w");
        if (f == NULL)
        {
            escrituraLogGeneral("🟥 No se pudo crear el archivo para ftok\n", 0);
            return 1;
        }
        fclose(f);
    }

    key_t key_buffer = ftok(MEM_KEY, 2);
    if (key_buffer == -1)
    {
        escrituraLogGeneral("🟥 Error al generar key para buffer compartido\n", 0);
        return 1;
    }

    if (key_buffer == -1)
    {
        escrituraLogGeneral("🟥 Error al generar key para buffer compartido\n", 0);
        escrituraLogGeneral(MEM_KEY, 0);
        return 1;
    }

    // Si ya existe memoria para el buffer, la eliminamos
    int shm_id_buffer = shmget(key_buffer, 0, 0666);
    if (shm_id_buffer != -1)
    {
        if (shmctl(shm_id_buffer, IPC_RMID, NULL) == -1)
        {
            escrituraLogGeneral("🟥 No se pudo eliminar la memoria compartida previa del buffer\n", 0);
        }
        else
        {
            escrituraLogGeneral("🟩 Memoria compartida anterior del buffer eliminada correctamente\n", 0);
        }
    }

    // Creamos una nueva memoria compartida para el buffer
    shm_id_buffer = shmget(key_buffer, sizeof(BufferEstructurado), IPC_CREAT | 0666);
    if (shm_id_buffer == -1)
    {
        escrituraLogGeneral("🟥 Error al crear la memoria compartida para el buffer\n", 0);
        return 1;
    }

    // Adjuntamos la memoria compartida al proceso
    buffer = (BufferEstructurado *)shmat(shm_id_buffer, NULL, 0);
    if (buffer == (void *)-1)
    {
        escrituraLogGeneral("🟥 Error al adjuntar la memoria compartida para el buffer\n", 0);
        return 1;
    }

    // Inicializamos los punteros del buffer circular
    buffer->inicio = 0;
    buffer->fin = 0;

    escrituraLogGeneral("🟦 Memoria compartida para buffer inicializada correctamente\n", 0);
    return 0;
}

/// @brief Función que se encarga de manejar las señales SIGINT y SIGHUP dentro de banco
/// @param senal Señal que se le manda al sistema
void manejoSenal(int senal)
{
    char log[256];

    snprintf(log, sizeof(log), "🟥 Se ha recibido la señal: %d, cerrando el programa en banco.c, en función: manejoSenal\n", senal);
    escrituraLogGeneral(log, 0);

    // 🧹 Desvincular y eliminar memoria compartida de cuentas
    if (shmdt(tabla) == -1)
    {
        escrituraLogGeneral("❌ Error al desvincular la memoria compartida de cuentas\n", 0);
    }
    else
    {
        escrituraLogGeneral("✅ Memoria compartida de cuentas desvinculada correctamente\n", 0);
    }

    key_t key_tabla = ftok(MEM_KEY, 1);
    int shmid_tabla = shmget(key_tabla, 0, 0666);
    if (shmid_tabla != -1 && shmctl(shmid_tabla, IPC_RMID, NULL) == 0)
    {
        escrituraLogGeneral("🧹 Memoria compartida de cuentas eliminada correctamente\n", 0);
    }
    else
    {
        escrituraLogGeneral("❌ Error al eliminar la memoria compartida de cuentas\n", 0);
    }

    // 🧹 Desvincular y eliminar memoria compartida del buffer
    if (shmdt(buffer) == -1)
    {
        escrituraLogGeneral("❌ Error al desvincular la memoria compartida del buffer\n", 0);
    }
    else
    {
        escrituraLogGeneral("✅ Memoria compartida del buffer desvinculada correctamente\n", 0);
    }

    key_t key_buffer = ftok(MEM_KEY, 2);
    int shmid_buffer = shmget(key_buffer, 0, 0666);
    if (shmid_buffer != -1 && shmctl(shmid_buffer, IPC_RMID, NULL) == 0)
    {
        escrituraLogGeneral("🧹 Memoria compartida del buffer eliminada correctamente\n", 0);
    }
    else
    {
        escrituraLogGeneral("❌ Error al eliminar la memoria compartida del buffer\n", 0);
    }

    // Cerramos el resto de terminales activas del sistema
    system("pkill -f usuario");
    system("pkill -f './monitor'");

    exit(0);
}

int main(int argc, char *argv[])
{

    // Inicializamos la configuracion establecida desde el archivo .config en el archivo de banco
    configuracion = leer_configuracion(configuracion);
    long int MEMORY_SIZE = MB * atoi(configuracion.maxMemoria); // Convertimos el tamaño de memoria en bytes
    inicializarMemSh(MEMORY_SIZE);                              // Inicializamos la memoria compartida con el dato del .config que indica su capacidad

    unlink(FIFO1);
    unlink(FIFO2);

    inicializarBufferCompartido();

    pid_t pid = fork();

    if (pid == 0)
    {
        execlp("gnome-terminal", "gnome-terminal", "--", "./monitor", NULL);
        escrituraLogGeneral("Error al ejecutar ./monitor en banco.c, en función: main\n", 0);
        exit(1);
    }

    signal(SIGINT, manejoSenal); // Capturamos la señal de interrupción (Ctrl+C)
    signal(SIGHUP, manejoSenal); // Capturamos la señal de cierre de terminal

    int fd[2];

    char buffer[100];
    char errorMessage[] = "Ha habido un error leyendo el archivo .config";
    char validMessage[] = "Todo correcto maquina";

    int state = 0;

    // Conseguimos la key definida dentro del sistema para acceder a la memoria compartida
    key_t key = ftok(MEM_KEY, 1);

    // Definimos en usuario la memoria compartida ya creada en banco.c
    int shm_id = shmget(key, 0, 0666);
    if (shm_id == -1)
    {
        escrituraLogGeneral("🟥 Error al obtener el id de la memoria compartida en usuario.c, en función: main\n", 0);
        return 1;
    }

    // Accedemos a la tabla existente ya en la memoria compartida creada
    tabla = (TablaCuentas *)shmat(shm_id, NULL, 0);
    if (tabla == (void *)-1)
    {
        escrituraLogGeneral("🟥 Error al adjuntar la memoria compartida en usuario.c, en función: main\n", 0);
        return 1;
    }

    // Comprobamos que no ocurre problema al generar la pipe
    if (pipe(fd) == -1)
    {
        escrituraLogGeneral("🟥 Error en la generación de la pipe en banco.c, en función: main\n", 0);
        return 1;
    }

    iniciarHiloAlerta();
    hiloBuffer(); // Llamamos a la función que crea el hilo que vacia el buffer que recoge las actualizaciones de cuentas

    menuBanco();

    system("pkill -f usuario"); // cuando cerramos banco matamos todos los procesos de usuarios
    system("pkill -f './monitor'");
    remove(configuracion.archivoSesiones);
    return (0);
}