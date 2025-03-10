#include "banco.h"

void operacionDevolucion(){

}

void ejecutarOperacion(int opcion){
 
    pthread_t hilo;

    switch (opcion)
    {
        case 1:
            pthread_create(&hilo, NULL, operacionDevolucion, NULL);
            pthread_join(hilo, NULL);
            break;
        case 2:
            pthread_create(&hilo, NULL, operacionDevolucion, NULL);
            pthread_join(hilo, NULL);
            break;
        case 3:
            pthread_create(&hilo, NULL, operacionDevolucion, NULL);
            pthread_join(hilo, NULL);
            break;
        case 4:
            pthread_create(&hilo, NULL, operacionDevolucion, NULL);
            pthread_join(hilo, NULL);
            break;
        default:
            break;
    }

}

/// @brief Función que realiza el menu de Usuario
void menuUsuario(char *id){

    int opcion = 0;

    while (1)
    {
        printf("Bienvenido %s!\n", id);
        printf("1. Devolución\n");
        printf("2. Transferencia\n");
        printf("3. Retiro\n");
        printf("4. Consultar saldo\n");
        printf("5. Salir\n");
        printf("Inserte la acción que quiere ejecutar:\n");
        
        scanf("%d", &opcion);
        
        switch (opcion){
            case 1:
            case 2: 
            case 3: 
            case 4:
                ejecutarOperacion(opcion);
                break;
            case 5:
                exit(0);
                break;
        }
    }
}

int main(char *id, CONFIG configuracion)
{
    menuUsuario(id);
}