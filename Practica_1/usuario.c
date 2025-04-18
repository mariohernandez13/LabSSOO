#include "banco.h"

CONFIG configuracion;
TablaCuentas *tabla;

/// @brief Funcion para escribir en el log de transacciones
/// @param flagOperacion 1 = Ingreso, 2 = Retiro, 3 = Transferencia
/// @param flagEstado 1 = OK, 0 = ERROR
/// @param id ID del usuario que realiza la operación
/// @param cantidad Cantidad de dinero de la operación
void escribirLogOperacion(int flagOperacion, int flagEstado, char *id, float cantidad)
{
    char log[255];
    char *estado;
    char *tipo;
    char *mensaje;

    // Determinar estado
    if (flagEstado == 1)
        estado = "OK";
    else
        estado = "ERROR";

    // Determinar tipo de operacion
    switch (flagOperacion)
    {
    case 1:
        tipo = "Ingreso";
        if (flagEstado == 1)
            mensaje = "✅ Ingreso realizado correctamente";
        else
            mensaje = "🟥 Error en el ingreso";

        break;
    case 2:
        tipo = "Retiro";
        if (flagEstado == 1)
            mensaje = "✅ Retiro realizado correctamente";
        else
            mensaje = "🟥 Error en el retiro";

        break;
    case 3:
        tipo = "Transferencia";
        if (flagEstado == 1)
            mensaje = "✅ Transferencia realizada correctamente";
        else
            mensaje = "🟥 Error en la transferencia";

        break;
    default:
        tipo = "Desconocido";
        mensaje = "🟥 Operación desconocida";
    }

    snprintf(log, sizeof(log), "%s,%s,%s,%s,%.2f\n", estado, tipo, mensaje, id, cantidad);
    // ?? snprintf(log, sizeof(log), "%s,%s,%s,%s,%.2f\n", id, tipo, estado, mensaje, cantidad);


    escrituraLogGeneral(log, 1);
}

/// @brief Funcion que trata de conseguir el saldo del usuario en base a su ID
/// @param id Id del usuario
/// @return Devuelve el saldo del usuario
float conseguirSaldoUsuario(char *id)
{
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;
    int encontrado;

    float saldoUsuario;

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir el semáforo de cuentas en usuario.c, en función: conseguirSaldoUsuario\n", 1);
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    file = fopen("data/cuentas.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas en usuario.c, en función: conseguirSaldoUsuario\n", 1);
        return 0;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;
        key = strtok(linea, ",");
        if (strcmp(key, id) == 0)
        {
            key = strtok(NULL, ",");
            key = strtok(NULL, ","); // saltamos los 2 primeros campos para obtener el saldo

            escrituraLogGeneral("Saldo consultado exitosamente en usuario.c, en función: conseguirSaldoUsuario\n", 1);
            saldoUsuario = strtof(key, NULL);
            encontrado = 1;
            break;
        }
    }

    if (encontrado != 1)
    {
        escrituraLogGeneral("Error: ID no encontrado en usuario.c, en función: conseguirSaldoUsuario\n", 1);
        saldoUsuario = -1;
    }

    fclose(file);
    sem_post(semaforo_cuentas);

    return saldoUsuario;
}

/// @brief Función que se encarga de conseguir el saldo del usuario que corresponde al id introducido
/// @param id Id del usuario 
/// @return Devolvemos el saldo actual del usuario
float conseguirSaldoUsuarioEnMemoria(char *id)
{
    // Recorremos el array de cuentas dentro de tabla para encontrar el id del usuario logueado
    float saldoActual;
    
    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);

    sem_wait(semaforo_cuentas); // Esperamos a que el semaforo de cuentas nos permita entrar en la seccion critica de la memoria compartida

    escrituraLogGeneral("🔍 Comprobamos que el id introducido por el usuario existe en la función: conseguirSaldoUsuarioEnMemoria\n", 0);

    for (int i = 0; i < tabla->numCuentas; i++)
    {
        // Si el id es el mismo, asignamos su saldo al saldo a mostrar
        if (strcmp(tabla->cuentas[i].numero_cuenta, id) == 0)
        {
            saldoActual = strtof(tabla->cuentas[i].saldo, NULL);
            break;
        }
        else {
            escrituraLogGeneral("🟥 Error: ID no encontrado en usuario.c, en función: conseguirSaldoUsuarioEnMemoria\n", 0);
        }
    }

    sem_post(semaforo_cuentas); // Habilitamos el semaforo de cuentas de nuevo

    return saldoActual;
}

/// @brief Función que se encarga de actualizar el saldo en el archivo cuentas.dat en función de la operación realizada
/// @param id Número identificador del usuario que realiza la operación
/// @param saldoActualizado  Saldo después de realizar la operación
void actualizarCuentas(char *id, float saldoActualizado)
{

    FILE *archivo;

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir el semáforo de cuentas en usuario.c, en función: actualizarCuentas\n", 0);
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    archivo = fopen("data/cuentas.dat", "r+");
    if (!archivo)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas en usuario.c, en función: actualizarCuentas\n", 1);
        return;
    }

    char linea[MAX_LINE_LENGTH];
    int lineas = 0;
    char *idArchivo, *nombre, *saldo, *numeroTransacciones;

    char *lineasArchivo[MAX_LINE_LENGTH];

    // Este bucle se usa para copiar dentro del array de lineas del archivo todas las cuentas del archivo cuentas.dat
    while (fgets(linea, MAX_LINE_LENGTH, archivo) && lineas < MAX_LINE_LENGTH)
    {
        lineasArchivo[lineas] = strdup(linea);
        lineas++;
    }

    // Usamos strtok para capturar todos los parámetros de las cuentas
    for (int i = 0; i < lineas; i++)
    {
        char *temp = strdup(lineasArchivo[i]); // Variable auxiliar para copiar el contenido de cada linea en cada iteración del bucle
        idArchivo = strtok(temp, ",");
        nombre = strtok(NULL, ",");
        saldo = strtok(NULL, ",");
        numeroTransacciones = strtok(NULL, ",");

        // La línea que coincide con el id del usuario se actualiza con su saldo
        if (idArchivo && strcmp(idArchivo, id) == 0)
        {
            snprintf(lineasArchivo[i], MAX_LINE_LENGTH, "%s,%s,%.2f,%s",
                     idArchivo, nombre, saldoActualizado, numeroTransacciones ? numeroTransacciones : "0");
        }

        free(temp);
    }

    rewind(archivo); // Colocar el puntero al principio del fichero cuentas.dat

    // Reescribe todo el archivo cuentas.dat
    for (int i = 0; i < lineas; i++)
    {
        fputs(lineasArchivo[i], archivo);
        free(lineasArchivo[i]);
    }

    fclose(archivo);
    sem_post(semaforo_cuentas);

    escrituraLogGeneral("Cuentas actualizadas correctamente en usuario.c, en función: actualizarCuentas\n", 1);
}

/// @brief Función que sobreescribe en memoria el valor del saldo del usuario con el id introducido
/// @param id Id del usuario
/// @param saldoActualizado Saldo actualizado del usuario con id introducido
void actualizarCuentasEnMemoria(char *id, float saldoActualizado)
{
    sem_wait(semaforo_cuentas); // Esperamos a que el semaforo de cuentas nos permita entrar en la seccion critica de la memoria compartida

    // Recorremos el array de cuentas dentro de tabla para encontrar el id del usuario logueado
    for (int i = 0; i < tabla->numCuentas; i++)
    {
        // Si el id es el mismo, asignamos su saldo al saldo a mostrar
        if (strcmp(tabla->cuentas[i].numero_cuenta, id) == 0)
        {
            snprintf(tabla->cuentas[i].saldo, sizeof(tabla->cuentas[i].saldo), "%.2f", saldoActualizado);
            break;
        }
    }
    escrituraLogGeneral("Cuentas actualizadas correctamente en usuario.c, en función: actualizarCuentasEnMemoria\n", 1);

    sem_post(semaforo_cuentas); // habilitamos de vuelta el semaforo de cuentas
}

/// @brief Realiza ingreso o retiro en funcion del flag
/// @param SaldoActual Saldo actual del usuario que realiza la operación
/// @param SaldoOperacion Saldo introducido a sumar/restar al saldo actual
/// @param flag Depósito=0 / Retiro=1
/// @param id del usuario que inició sesión
/// @return Saldo actualizado
float realizarOperacion(float saldoActual, float saldoOperacion, int flag, char *id)
{
    switch (flag)
    {
    case 0:
        saldoActual += saldoOperacion;
        escrituraLogGeneral("Operacion ingreso realizada correctamente en usuario.c, en función: realizarOperacion\n", 1);
        break;
        // transferencia = depósito para el receptor y retiro para el emisor
    case 1:
        saldoActual -= saldoOperacion;
        escrituraLogGeneral("Operacion retiro realizada correctamente en usuario.c, en función: realizarOperacion\n", 1);
        break;
    }

    actualizarCuentasEnMemoria(id, saldoActual); // Ahora llamamos a una función que modifica los saldos en memoria en vez de ir al archivo

    mostrarCarga();

    return saldoActual;
}

/// @brief Función que permite al usuario introducir el saldo que va a ingresar
/// @param id Cuenta del usuario logueado
void *operacionDeposito(void *id)
{
    pthread_setname_np(pthread_self(), "operacionDeposito");
    char *_id = (char *)id;
    float saldo = conseguirSaldoUsuarioEnMemoria(_id);
    float saldoDepositar = 0;
    int esValido = 1; // Es valido
    configuracion = leer_configuracion(configuracion);

    do
    {
        if (esValido == 0)
        {
            system("clear");
            printf("=====================================\n");
            printf("❌ ERROR: Saldo introducido no válido\n");
            printf("=====================================\n");
        }

        esValido = 1;
        printf("Introduce cantidad a depositar: \n");
        while (getchar() != '\n')
            ;
        scanf("%f", &saldoDepositar);
        escrituraLogGeneral("✅ Cantidad deposito introducido correctamente en usuario.c, en función: operacionDeposito\n", 1);
        if (saldoDepositar <= 0 || saldoDepositar > configuracion.limiteIngreso)
        {
            esValido = 0;
            escribirLogOperacion(1, 0, _id, saldoDepositar);
        }

    } while (!esValido);

    realizarOperacion(saldo, saldoDepositar, 0, _id);
    escribirLogOperacion(1, 1, _id, saldoDepositar);
    
    printf("\n");
    printf("======================================\n");
    printf("💰 Depósito realizado con éxito\n");
    printf("Pulse INTRO para continuar...\n");
    printf("======================================\n");
    
    while (getchar() != '\n');
    getchar();
}

/// @brief Función que permite al usuario realizar una transferencia a otro usuario
/// @param id Cuenta del usuario logueado
void *operacionTransferencia(void *id)
{
    pthread_setname_np(pthread_self(), "operacionTransferencia");
    char *_id = (char *)id;
    char idDestinatario[255] = "Hola";
    float saldoTransferir = 0;
    float saldo = conseguirSaldoUsuarioEnMemoria(_id);
    float saldoDestinatario = 0;
    char *mensaje;
    char *estado;
    char *tipo;
    char log[255];
    int esValido = 1;
    configuracion = leer_configuracion(configuracion);

    do
    {
        if (esValido == 0)
        {
            system("clear");
            printf("=====================================\n");
            printf("❌ ERROR: ID introducido o saldo a transferir no válido\n");
            printf("=====================================\n");
        }

        esValido = 1;
        printf("👤 Introduce id destinatario: \n");
        while (getchar() != '\n');

        fgets(idDestinatario, sizeof(idDestinatario), stdin);
        idDestinatario[strcspn(idDestinatario, "\n")] = 0; // Eliminar \n para que saldoDestinatario no vuelva vacio
        
        saldoDestinatario = conseguirSaldoUsuario(idDestinatario);
        
        printf("💳 Introduce cantidad a transferir: \n");
        scanf("%f", &saldoTransferir);

        if (saldoTransferir > configuracion.limiteTransferencia || saldoDestinatario < 0
             || saldoTransferir > saldo ||strcmp(_id, idDestinatario) == 0) 
        {
            escribirLogOperacion(3, 0, _id, saldoTransferir);
            esValido = 0;
        }

    } while (!esValido);

    realizarOperacion(saldo, saldoTransferir, 1, _id);
    realizarOperacion(saldoDestinatario, saldoTransferir, 0, idDestinatario);
       
    printf("\n");
    printf("======================================\n");
    printf("💰 Transferencia realizada con éxito\n");
    printf("Pulse INTRO para continuar...\n");
    printf("======================================\n"); 

    escribirLogOperacion(3, 1, _id, saldoTransferir);

    while (getchar() != '\n')
        ;
    getchar();
}

/// @brief Función que permite al usuario introducir el saldo que va a retirar
/// @param id Cuenta del usuario logueado
void *operacionRetiro(void *id)
{
    pthread_setname_np(pthread_self(), "operacionRetiro");
    char *_id = (char *)id;
    float saldo = conseguirSaldoUsuarioEnMemoria(_id);
    float saldoRetirar = 1;
    int esValido = 1;
    configuracion = leer_configuracion(configuracion);
    do
    {
        if (esValido == 0)
        {
            system("clear");
            printf("=====================================\n");
            printf("❌ ERROR: Saldo introducido no válido\n");
            printf("=====================================\n");
        }

        esValido = 1;
        printf("Introduce cantidad a retirar: \n");
        while (getchar() != '\n')
            ;
        scanf("%f", &saldoRetirar);
        if (saldoRetirar <= 0 || saldoRetirar > configuracion.limiteRetiros || saldoRetirar > saldo)
        {
            escrituraLogGeneral("Error: Saldo introducido para transacción en usuario.c, en función: operacionRetiro\n", 1);
            escribirLogOperacion(2, 0, _id, saldoRetirar);
            esValido = 0;
        }
    } while (!esValido);

    realizarOperacion(saldo, saldoRetirar, 1, _id);
    escrituraLogGeneral("✅ Cantidad retiro introducido correctamente en usuario.c, en función: operacionRetiro\n", 1);
    escribirLogOperacion(2, 1, _id, saldoRetirar);
    
    printf("\n");
    printf("======================================\n");
    printf("💰 Retiro realizado con éxito\n");
    printf("Pulse INTRO para continuar...\n");
    printf("======================================\n"); 

    while (getchar() != '\n')
        ;
    getchar();
}

/// @brief Muestra al usuario por pantalla su saldo
/// @param id Cuenta del usuario logueado
void *operacionConsultarSaldo(void *id)
{
    pthread_setname_np(pthread_self(), "operacionConsultarSaldo");

    // Ahora usamos una nueva función que consigue el saldo del usuario accediendo al espacio de memoria compartida
    float saldoActual = conseguirSaldoUsuarioEnMemoria(id);

    printf("\n=====================================\n");
    printf(" 💰 Tu saldo actual es: %.2f 💰\n", saldoActual);
    printf("=====================================\n");
    printf("Pulse INTRO para continuar...\n");
    while (getchar() != '\n')
        ;
    getchar();
}

/// @brief Función que gestiona los hilos de las funciones
/// @param opcion Opción a realizar
/// @param id Cuenta del usuario logueado
void ejecutarOperacion(int opcion, char *id)
{

    pthread_t hilo;

    switch (opcion)
    {
    case 1:
        pthread_create(&hilo, NULL, &operacionDeposito, id);
        pthread_join(hilo, NULL);
        break;
    case 2:
        pthread_create(&hilo, NULL, &operacionTransferencia, id);
        pthread_join(hilo, NULL);
        break;
    case 3:
        pthread_create(&hilo, NULL, &operacionRetiro, id);
        pthread_join(hilo, NULL);
        break;
    case 4:
        pthread_create(&hilo, NULL, &operacionConsultarSaldo, id);
        pthread_join(hilo, NULL);
        break;
    default:
        break;
    }
}

/// @brief Función que realiza el menu de Usuario
/// @param id Cuenta del usuario logueado
void menuUsuario(char *id)
{
    int opcion = 0;
    do
    {
        system("clear");

        printf("=====================================\n");
        printf("        🌟 BIENVENIDO %s 🌟        \n", id);
        printf("=====================================\n");
        printf("🔹 1. Depósito\n");
        printf("🔹 2. Transferencia\n");
        printf("🔹 3. Retiro\n");
        printf("🔹 4. Consultar saldo\n");
        printf("🔹 5. Salir\n");
        printf("=====================================\n");
        printf("👉 Inserte la acción que quiere ejecutar: ");

        scanf("%d", &opcion);

        printf("\n");
        printf("✅ Opción seleccionada: %d\n", opcion);
        printf("=====================================\n");

        ejecutarOperacion(opcion, id);
    } while (opcion != 5);
}

int main(int argc, char *argv[])
{
    configuracion = leer_configuracion(configuracion);

    // Conseguimos la key definida dentro del sistema para acceder a la memoria compartida
    key_t key = ftok(MEM_KEY,1);

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

    if (argc == 2)
        menuUsuario(argv[1]);
    else if (argc == 1)
        escrituraLogGeneral("Error al hacer el inicio de sesión. Se necesita pasar el identificador a usuario en funcíon menuUsuario\n", 0);
}

// TODO crear una función que escriba/aumente el número de errores por usuario, para que lo pueda leer monitor y así pase la alerta con su id al banco
// TODO si hacemos un simulador de ps aux para ver las sesiones podemos coger el PID (guardar en archivos temp) y cerrar solo esa sesión.
// TODO ping al monitor para comprobar que sigue vivo, si no lo sigue, se vuelve a ejecutar desde banco