#include "banco.h"

CONFIG configuracion;

/// @brief Funcion que trata de conseguir el saldo del usuario en base a su ID
/// @param id Id del usuario
/// @return Devuelve el saldo del usuario
float conseguirSaldoUsuario(char *id)
{
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    float saldoUsuario;

    semaforo_cuentas = sem_open("/semaforo_cuentas", O_CREAT, 0644, 1);

    if (semaforo_cuentas == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir los semáforos",1);
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    file = fopen("data/cuentas.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n", 1);
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

            escrituraLogGeneral("Saldo consultado exitosamente\n", 1);
            saldoUsuario = strtof(key, NULL);
            break;
        }
    }

    fclose(file);
    sem_post(semaforo_cuentas);

    return saldoUsuario;
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
        perror("Error al abrir los semáforos");
        exit(1);
    }

    sem_wait(semaforo_cuentas);
    archivo = fopen("data/cuentas.dat", "r+");
    if (!archivo)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n", 1);
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

    escrituraLogGeneral("Cuentas actualizadas correctamente\n", 1);
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
        escrituraLogGeneral("Operacion ingreso done\n", 1);
        break;
        // transferencia = depósito para el receptor y retiro para el emisor
    case 1:
        saldoActual -= saldoOperacion;
        escrituraLogGeneral("Operacion retiro done\n", 1);
        break;
    }

    actualizarCuentas(id, saldoActual);
    return saldoActual;
}

/// @brief Función que permite al usuario introducir el saldo que va a ingresar
/// @param id Cuenta del usuario logueado
void *operacionDeposito(void *id)
{
    char *_id = (char *)id;
    float saldo = conseguirSaldoUsuario(_id);
    float saldoDepositar = 0;

    do
    {
        printf("Introduce cantidad a depositar: \n");
        while (getchar() != '\n')
            ;
        scanf("%f", &saldoDepositar);
        escrituraLogGeneral("✅ Cantidad retiro introducido correctamente.\n", 1);
    } while (saldoDepositar <= 0);

    realizarOperacion(saldo, saldoDepositar, 0, _id);
    printf("Depósito realizado con éxito\n");
    printf("Pulse INTRO para continuar...\n");
    while (getchar() != '\n');
    getchar();
}

void escrituraMensajeError(float saldo)
{
    Transaccion transaccion;
    char *mensaje;
    char *estado;
    char *tipo;
    char log[255];

    mensaje = "🟥 Cantidad transferencia introducida es incorrecta";
    tipo = "Transferencia";
    estado = "ERROR";
    snprintf(log, sizeof(log), "%s,%s,%s,%f", estado, tipo, mensaje, saldo);
    escrituraLogGeneral(log, 1);
}

/// @brief Función que permite al usuario realizar una transferencia a otro usuario
/// @param id Cuenta del usuario logueado
void *operacionTransferencia(void *id)
{
    char *_id = (char *)id;
    char idDestinatario[255] = "Hola";
    float saldoTransferir = 0;
    float saldo = conseguirSaldoUsuario(_id);
    float saldoDestinatario = 0;
    char *mensaje;
    char *estado;
    char *tipo;
    char log[255];

    do
    {
        printf("Introduce id destinatario: \n");
        while (getchar() != '\n');
        fgets(idDestinatario, sizeof(idDestinatario), stdin);
        idDestinatario[strcspn(idDestinatario, "\n")] = 0; //Eliminar \n para que saldoDestinatario no vuelva vacio
        saldoDestinatario = conseguirSaldoUsuario(idDestinatario);
        printf("Introduce cantidad a transferir: \n");
        scanf("%f", &saldoTransferir);

        if (saldoTransferir < 0)
            escrituraMensajeError(saldoTransferir);
        if (saldoTransferir > configuracion.limiteTransferencia)
            escrituraMensajeError(saldoTransferir);

    } while (saldoTransferir <= 0);

    realizarOperacion(saldo, saldoTransferir, 1, _id);
    realizarOperacion(saldoDestinatario, saldoTransferir, 0, idDestinatario);
    printf("Transferencia realizada con éxito\n");
    
    mensaje = "✅ Cantidad transferencia introducida correctamente";
    tipo = "Transferencia";
    estado = "OK";
    snprintf(log, sizeof(log), "%s,%s,%s,%s,%.2f", estado, tipo, mensaje, _id, saldoTransferir);
    escrituraLogGeneral(log, 1);

    printf("Pulse INTRO para continuar...\n");
    while (getchar() != '\n');
    getchar();
}

/// @brief Función que permite al usuario introducir el saldo que va a retirar
/// @param id Cuenta del usuario logueado
void *operacionRetiro(void *id)
{
    char *_id = (char *)id;
    float saldo = conseguirSaldoUsuario(_id);
    float saldoRetirar = 1;
    do
    {
        if (saldoRetirar <= 0)
            escrituraLogGeneral("Error: Saldo introducido para transacción \n", 1);
        printf("Introduce cantidad a retirar: \n");
        while (getchar() != '\n')
            ;
        scanf("%f", &saldoRetirar);
        escrituraLogGeneral("✅ Cantidad retiro introducido correctamente.\n", 1);
    } while (saldoRetirar <= 0);

    realizarOperacion(saldo, saldoRetirar, 1, _id);
    printf("Retiro realizado con éxito\n");
    printf("Pulse INTRO para continuar...\n");
    while (getchar() != '\n');
    getchar();
}

/// @brief Muestra al usuario por pantalla su saldo
/// @param id Cuenta del usuario logueado
void *operacionConsultarSaldo(void *id)
{
    float saldoActual = conseguirSaldoUsuario(id);
    printf("\n=====================================\n");
    printf(" 💰 Tu saldo actual es: %.2f 💰\n", saldoActual);
    printf("=====================================\n");
    printf("Pulse INTRO para continuar...\n");
    while (getchar() != '\n');
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

    if (argc == 2)
        menuUsuario(argv[1]);
    else if (argc == 1)
        escrituraLogGeneral("Error al hacer el inicio de sesión. Se necesita pasar el identificador a usuario\n", 0);
}

// TODO crear una función que escriba/aumente el número de errores por usuario, para que lo pueda leer monitor y así pase la alerta con su id al banco
// TODO si hacemos un simulador de ps aux para ver las sesiones podemos coger el PID (guardar en archivos temp) y cerrar solo esa sesión.
// TODO ping al monitor para comprobar que sigue vivo, si no lo sigue, se vuelve a ejecutar desde banco