#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE_LENGTH 255
#define MAX_LENGTH_NAME 50

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
int readFile() {
    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");
    
    // 
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        if (key && value) {
            if (strcmp(key, "patata") == 0) {
                strncpy(username, value, MAX_LINE_LENGTH);
            } else {
                state = 1;
            }
        }
    }

    fclose(file);

    return (state);
}

/// @brief Comprueba en el archivo de cuentas.dat que el id introducido no existe
/// @param id Id introducido por el usuario en el resgistro
/// @param flag Valor que indica si se está llegando a la función desde LogIn o desde registro: 0 = registro || 1 = LogIn
/// @return Valor numérico que indica validez del Id: 0 = error en id // 1 = id valido
int comprobarId(int id, int flag){

    int validez = 1;
    if (id < 100)
        validez = 0;

}

/// @brief Menú de registro del Banco
void registro(){

    int id;
    float saldo;
    char nombre[MAX_LENGTH_NAME];
    int comprobacion = 1;

    do{
        printf("Bienvenido al registro de SafeBank\n");
        printf("Introduce tu nombre: (no se admiten más de 50 caracteres)\n");
        fgets(nombre, sizeof(nombre), stdin);
        printf("Introduce tu id: (a partir de 100)\n");
        scanf("%d", id);
        printf("Introduce tu saldo: \n");
        scanf("%.2f", saldo);
    }while(comprobarId(id, 0) || (nombre == NULL) || (strlen(nombre) > MAX_LENGTH_NAME));
    
}

/// @brief Menú de logIn del Banco
void logIn(){

    int id;
    int flg_log = 1;

    do{
        printf("Bienvenido al LogIn de SafeBank\n");
        printf("Introduce tu id: (a partir de 100)\n");
        scanf("%d", id);
    }while(comprobarId(id,flg_log));
    
}

/// @brief Menú de inicio del Banco SafeBank
void menuBanco(){

    int opcion = 0;

    do{
        printf("Bienvenido a SafeBank.\n");
        printf("¿Qué desea hacer?\n");
        printf("1. Inicio Sesión\n");
        printf("2. Registro\n");
        printf("3. Salir\n");
        printf("Introduce una opción: ");
        scanf("%d", opcion);

        switch(opcion)
        {
            case 1: 
                logIn();
                break;
            case 2: 
                registro(); // Llamamos a funcion registro
                break;
            default:
                break; 
        }

    }while(opcion != 3);

}

int main() {
    pid_t pid;
    int fd[2];

    char buffer[100];
    char errorMessage[] = "Ha habido un error leyendo el archivo .config";
    char validMessage[] = "Todo correcto maquina";

    int state = 0;

    // Comprobamos que no ocurre problema al generar la pipe
    if (pipe(fd) == -1) {   
        perror("Error en la generación de la pipe");
        return 1;
    }

    pid = fork();

    // Comprobamos que fork() no genera un error
    if (pid < 0) { 
        perror("Error al crear el proceso hijo");
        return 1;
    }

    if (pid > 0) {  // Proceso padre
        printf("El padre empieza...\n");
        close(fd[0]);  // Cierra el extremo de lectura
        
        state = readFile();
        printf("Estado de readFile(): %d\n", state);
        
        if (state == 1)
            write(fd[1], errorMessage, strlen(errorMessage) + 1);
        else
            write(fd[1], validMessage, strlen(validMessage) + 1);

        close(fd[1]);  // Cierra el extremo de escritura
        printf("El padre muere\n");
        wait(NULL); // Espera a que el hijo termine
    } 
    else {  // Proceso hijo
        close(fd[1]);  // Cierra el extremo de escritura
        read(fd[0], buffer, sizeof(buffer));
        printf("El hijo recibió: %s\n", buffer);
        close(fd[0]);  // Cierra el extremo de lectura
    }

    return (0);
}