#include "banco.h"
/*
void operacionDevolucion()
{
}

void operacionTransferencia()
{
}

void operacionRetiro()
{
}

void operacionConsultarSaldo()
{
}

void ejecutarOperacion(int opcion)
{

    pthread_t hilo;

    switch (opcion)
    {
    case 1:
        pthread_create(&hilo, NULL, operacionDevolucion, NULL);
        pthread_join(hilo, NULL);
        break;
    case 2:
        pthread_create(&hilo, NULL, operacionTransferencia, NULL);
        pthread_join(hilo, NULL);
        break;
    case 3:
        pthread_create(&hilo, NULL, operacionRetiro, NULL);
        pthread_join(hilo, NULL);
        break;
    case 4:
        pthread_create(&hilo, NULL, operacionConsultarSaldo, NULL);
        pthread_join(hilo, NULL);
        break;
    default:
        break;
    }
}
*/

/// @brief Función que realiza el menu de Usuario
void menuUsuario(char *id)
{

    int opcion = 0;

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
        //ejecutarOperacion(opcion);
    } while (opcion != 5);
}

int main(int argc, char *argv[])
{
    if (argc == 2)
        menuUsuario(argv[1]);
    else if(argc == 1)
        escrituraLogGeneral("Error al hacer el inicio de sesión. Se necesita pasar el identificador a usuario\n");
}