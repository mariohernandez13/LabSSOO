#include "banco.h"

/// @brief Funcion que trata de conseguir el saldo del usuario en base a su ID
/// @param id Id del usuario
/// @return Devuelve el saldo del usuario
float conseguirSaldoUsuario(char *id)
{

    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    float saldoUsuario;

    file = fopen("cuentas.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
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

            escrituraLogGeneral("Saldo consultado exitosamente\n");
            saldoUsuario = strtof(key, NULL);
            break;
        }
    }

    fclose(file);
    printf("%.2f\n", saldoUsuario);
    return saldoUsuario;
}

void actualizarCuentas(char *id, float saldoActualizado)
{
    FILE *archivo;
    archivo = fopen("cuentas.dat", "r+");
    if (!archivo)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
        return;
    }
    char linea[MAX_LINE_LENGTH] = "";
    int lineas;
    char *idArchivo, *nombre, *saldo, *numeroTransacciones;
    while (fgets(linea, MAX_LINE_LENGTH, archivo))
    {
        lineas++;
    }
    char lineasArchivo[lineas][MAX_LINE_LENGTH];
    rewind(archivo);
    while (fgets(lineasArchivo[lineas], MAX_LINE_LENGTH, archivo))
    {
        // copiar cada una de las lineas
        idArchivo = strtok(lineasArchivo[lineas], ",");
        nombre = strtok(NULL, ",");
        saldo = strtok(NULL, ",");
        numeroTransacciones = strtok(NULL, ",");

        if (strcmp(idArchivo, id)== 0) // en caso de que encuentre el usuario actualiza su saldo
        {

            sprintf(lineasArchivo[lineas], "%s,%s,%f,%s\n", idArchivo, nombre, saldoActualizado, numeroTransacciones);
        }
        else
        {
            sprintf(lineasArchivo[lineas], "%s,%s,%s,%s\n", idArchivo, nombre, saldo, numeroTransacciones);
        }
    }

    rewind(archivo);

    for (int i = 0; i < lineas; i++) // reescribe el archivo de cuentas
    {
        fputs(lineasArchivo[i], archivo);
    }

    fclose(archivo);
    escrituraLogGeneral("Cuentas actualizadas correctamente\n");
}

/// @brief
/// @param SaldoActual
/// @param SaldoOperacion
/// @param flag Depósito=0 / Retiro=1
/// @param id del usuario que inició sesión
/// @return
int realizarOperacion(float saldoActual, float saldoOperacion, int flag, char *id)
{
    switch (flag)
    {
    case 0:
        saldoActual += saldoOperacion;
        break;
        // transferencia = depósito para el receptor y retiro para el emisor
    case 1:
        saldoActual -= saldoOperacion;
        break;
    }
    actualizarCuentas(id, saldoActual);
    return saldoActual;
}

void *operacionDeposito()
{
    
}

void *operacionTransferencia()
{
}

void *operacionRetiro(void* id)
{
    char *userId = (char*) id;  // Convertimos el void* a char*
    char *saldoIntroducido;
    float saldoRetiro = 0;
    float saldoActual = conseguirSaldoUsuario(userId);
    do
    {
        printf("Introduce el saldo que quieres retirar de tu cuenta:\n");
        while (getchar() != '\n');
        fgets(saldoIntroducido, sizeof(MAX_LENGTH_SALDO), stdin);
        saldoRetiro = strtof(saldoIntroducido, NULL);
        escrituraLogGeneral("✅ Retiro exitoso.\n");
    } while (saldoRetiro < 0);

    realizarOperacion(saldoActual, saldoRetiro, 1, userId);

    return(NULL);
}

void *operacionConsultarSaldo(void *id)
{
    float saldoActual = conseguirSaldoUsuario(id);
    printf("\n=====================================\n");
    printf(" 💰 Tu saldo actual es: %.2f 💰\n", saldoActual);
    printf("=====================================\n");
}

void ejecutarOperacion(int opcion, char *id)
{

    pthread_t hilo;

    switch (opcion)
    {
    case 1:
        pthread_create(&hilo, NULL, &operacionDeposito, NULL);
        pthread_join(hilo, NULL);
        break;
    case 2:
        pthread_create(&hilo, NULL, &operacionTransferencia, NULL);
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
void menuUsuario(char *id)
{
    int opcion = 0;
    do
    {
        printf("=====================================\n");
        printf("        🌟 BIENVENIDO %s 🌟        \n", id);
        printf("=====================================\n");
        printf("🔹 1. Devolución\n");
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
    if (argc == 2)
        menuUsuario(argv[1]);
    else if (argc == 1)
        escrituraLogGeneral("Error al hacer el inicio de sesión. Se necesita pasar el identificador a usuario\n");
}