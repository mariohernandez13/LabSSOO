#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 255
#define MAX_LENGTH_NAME 50
#define MAX_LENGTH_ID 6

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
            if (strcmp(key, "password") == 0) {
                strncpy(username, value, MAX_LINE_LENGTH);
            } else {
                state = 1;
            }
        }
    }

    fclose(file);

    return (state);
}
/*
/// @brief Lee caracter por caracter hasta un maximo especifico
/// @param buffer Almacena los caracteres escritos
/// @param max_lenght Valor que indica el tamaño maximo de la variable
void leerChar(char *buffer){
    int i = 0;
    char c = ' ';
    int max_lenght = 50;

    while (1) {
        c = getchar();

        // Si el usuario presiona Enter, finaliza la lectura
        if (c == '\n' || c == EOF) {
            break;
        }
        // Si el usuario presiona Backspace y hay caracteres para borrar
        else if ((c == 8 || c == 127) && i > 0) {
            i--;  // Borra el último carácter
            printf("\b \b");  // Borra visualmente en la terminal
        }
        // Si aún hay espacio en el buffer, almacena el carácter
        else if (i < max_lenght - 1) {
            buffer[i++] = c;
            putchar(c);  // Muestra el carácter en pantalla
        }
    }

}
*/
void limpiezaString(char *string){
    for (int i = 0; i < strlen(string); i++)
        if (string[i]=='\n')
            string[i]='\0'; 
}

int existeID(char *id, int flag){
    limpiezaString(id);
    int esValido = 1;
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;
    file = fopen("cuentas.dat", "r");
    if (file == NULL)
    {
        perror("Error al abrir el archivo de cuentas\n");
        return 0;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0;
        key = strtok(linea, ",");
        if (flag == 0)
        {
            if (strcmp(key, id) == 0)
            {
                perror("El id ya existe\n");
                esValido = 0;
                break;
            }
        }else if (flag == 1)
        {
            if (strcmp(key, id) != 0)
                esValido = 0;
        }
    }

    fclose(file);
    return esValido;
}

/// @brief Comprueba en el archivo de cuentas.dat que el id introducido no existe
/// @param id Id introducido por el usuario en el resgistro
/// @param flag Valor que indica si se está llegando a la función desde LogIn o desde registro: 0 = registro || 1 = LogIn
/// @return Valor numérico que indica validez del Id: 0 = error en id // 1 = id valido
int comprobarId(char *id, int flag){

    int validez = 1;
    if (atoi(id) < 100)
    {
        validez = 0;
        return validez;
    }
    
    validez = existeID(id, flag);

    return validez;
    

}

/// @brief Menú de registro del Banco
void registro(){

    char id[MAX_LENGTH_ID];
    float saldo;
    char nombre[MAX_LENGTH_NAME];
    int comprobacion = 1;

    do{
        printf("Bienvenido al registro de SafeBank\n");

        printf("Introduce tu nombre: (no se admiten más de 50 caracteres): \n");
        while(getchar() != '\n');
        fgets(nombre, sizeof(nombre), stdin);

        printf("Introduce tu id: (a partir de 100): \n");
        //while(getchar() != '\n');
        fgets(id, sizeof(id), stdin);

        printf("Introduce tu saldo: \n");
        scanf("%f", &saldo);

        comprobacion = comprobarId(id, 0);
    }while((nombre == NULL) || (strlen(nombre) > MAX_LENGTH_NAME));
    
}

/// @brief Menú de logIn del Banco
void logIn(){

    char id[MAX_LENGTH_ID];
    int flg_log = 1;

    do{
        printf("Bienvenido al LogIn de SafeBank\n");
        printf("Introduce tu id: (a partir de 100)\n");
        //leerChar(id);
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
        scanf("%d", &opcion);

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
    menuBanco();
    // 
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