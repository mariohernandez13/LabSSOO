#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Verificar que se recibieron exactamente 3 argumentos adicionales
    if (argc != 4) {
        printf("Uso: %s <cadena> <numero1> <numero2>\n", argv[0]);
        return 1;
    }

    // Imprimir la cadena recibida
    printf("Cadena recibida: %s\n", argv[1]);

    // Convertir los argumentos numéricos de char* a int (o float si necesitas decimales)
    int num1 = atoi(argv[2]);
    int num2 = atoi(argv[3]);

    // Realizar la multiplicación
    int resultado = num1 * num2;

    // Imprimir el resultado
    printf("Resultado de %d * %d = %d\n", num1, num2, resultado);

    return 0;
}
