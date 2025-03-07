#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// este programa llama con un .sh a ./banco.c y ./monitor.c
// además configura el archivo de cuentas, crea archivo de errores (como properties)
// y si lla está llama a exec().

struct Cuenta {
    int numero_cuenta;
    char titular[50];
    float saldo;
    int num_transacciones;
    };


/// @brief 
/// @return 

int main() {
    // Definir la ruta al script .sh
    const char *pathInit = "./init.sh";
    
    // Llamar al script usando system()
    int resultado = system(pathInit);
    
    // Comprobar si se ejecutó correctamente
    if (resultado == 0) {
        printf("¡Script ejecutado correctamente!\n");
    } else {
        printf("Error al ejecutar el script.\n");
    }
    
    return 0;
}




