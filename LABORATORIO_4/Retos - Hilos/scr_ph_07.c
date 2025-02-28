#include <stdio.h>
#include <stdlib.h>

void threeParameters(char *argv[]) {
    // Imprimir la cadena recibida
    printf("Cadena recibida: %s\n", argv[1]);

    // Convertir los argumentos numéricos de char* a int (o float si necesitas decimales)
    int num1 = atoi(argv[2]);
    int num2 = atoi(argv[3]);

    // Realizar la multiplicación
    int resultado = num1 * num2;

    // Imprimir el resultado
    printf("Resultado de %d * %d = %d\n", num1, num2, resultado);

    return; 
}

void twoParameters(char *argv[]) {
    // Imprimir la cadena recibida
    printf("Cadena recibida: %s\n", argv[1]);

    // Solo ha habido dos parametros introducidos
    printf("No se puede realizar una operación matemática.\n");
    int num1 = atoi(argv[2]);
    printf("Número introducido es: %d\n", num1);

    return; 
}

void oneParameter(char *argv[]) {
    // Imprimir la cadena recibida
    printf("Cadena recibida: %s\n", argv[1]);

    printf("No ha habido números introducidos luego no se puede hacer ninguna operación.\n");

    return; 
}

// argc --> contador de argumentos que se le pasa al script por terminal
// argv[] --> array de los argumentos que le pasamos por terminal al script
int main(int argc, char *argv[]) {

    // Verificar que se recibieron exactamente 3 argumentos adicionales
    if (argc > 4) {
        printf("Uso: %s <cadena> <numero1> <numero2>\n", argv[0]);
        return 1;
    }

    // Hacemos un switch case para manejar todos los casos de parámetros del reto
    switch (argc)
    {
        case 1: 
            printf("No se ha introducido ningún parámetro.\n");
            printf("Solo podemos mostrar por pantalla esto: %s\n", argv[0]);
            break;
        case 2: 
            oneParameter(argv);
            break;
        case 3: 
            twoParameters(argv);
            break;
        case 4:
            threeParameters(argv);
            break;
    }

    return 0;
}
