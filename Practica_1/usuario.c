#include "banco.h"

float consultarSaldoUsuario(char *id){
    
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
            value = strtok(linea, ",");
            value = strtok(linea, ",");
            escrituraLogGeneral("El id ya existe\n");
            break;
        }
    }

    fclose(file);

    return saldoUsuario;
}

void* operacionDevolucion()
{
}

void* operacionTransferencia()
{
}

void* operacionRetiro()
{
    char *saldoIntroducido;
    float saldoRetiro;

    do{
        printf("Introduce el saldo que quieres retirar de tu cuenta:\n");
        fgets(saldoIntroducido, sizeof(MAX_LENGTH_SALDO), stdin);
        saldoRetiro = strtof(saldoIntroducido, NULL);
    }while(saldoRetiro < 0);
}

void* operacionConsultarSaldo()
{
}

void ejecutarOperacion(int opcion)
{

    pthread_t hilo;

    switch (opcion)
    {
    case 1:
        pthread_create(&hilo, NULL, &operacionDevolucion, NULL);
        pthread_join(hilo, NULL);
        break;
    case 2:
        pthread_create(&hilo, NULL, &operacionTransferencia, NULL);
        pthread_join(hilo, NULL);
        break;
    case 3:
        pthread_create(&hilo, NULL, &operacionRetiro, NULL);
        pthread_join(hilo, NULL);
        break;
    case 4:
        pthread_create(&hilo, NULL, &operacionConsultarSaldo, NULL);
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

    float saldoUsuario = consultarSaldoUsuario(id);

    do
    {
        printf("Bienvenido %s!\n", id);
        printf("1. Devolución\n");
        printf("2. Transferencia\n");
        printf("3. Retiro\n");
        printf("4. Consultar saldo\n");
        printf("5. Salir\n");
        printf("Inserte la acción que quiere ejecutar:\n");

        scanf("%d", &opcion);
        ejecutarOperacion(opcion);
    } while (opcion != 5);
}

int main(int argc, char *argv[])
{
    if (argc == 2)
        menuUsuario(argv[1]);
    else if(argc == 1)
        escrituraLogGeneral("Error al hacer el inicio de sesión. Se necesita pasar el identificador a usuario\n");
}