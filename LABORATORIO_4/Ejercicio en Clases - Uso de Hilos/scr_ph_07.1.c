#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int num1 = 0, num2 = 0; // Variables para almacenar los números
    char *cadena = NULL; // Variable para almacenar la cadena

    // Definir las opciones largas y cortas
    static struct option long_options[] = {
        {"cadena", required_argument, 0, 'c'},
        {"num1",   required_argument, 0, 'n'},
        {"num2",   required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    int long_index = 0;
    int opt = 0;

    // Procesar las opciones pasadas
    while ((opt = getopt_long(argc, argv, "c:n:m:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'c':
                cadena = optarg;
                break;
            case 'n':
                num1 = atoi(optarg);
                break;
            case 'm':
                num2 = atoi(optarg);
                break;
            default:
                printf("Uso: %s --cadena <cadena> --num1 <numero1> --num2 <numero2>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Verificar que todos los parámetros requeridos fueron proporcionados
    if (cadena == NULL || num1 == 0 || num2 == 0) {
        printf("Todos los parámetros son requeridos\n");
        printf("Uso: %s --cadena <cadena> --num1 <numero1> --num2 <numero2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Imprimir la cadena y el resultado de la multiplicación
    printf("Cadena recibida: %s\n", cadena);
    printf("Resultado de %d * %d = %d\n", num1, num2, num1 * num2);

    return 0;
}
