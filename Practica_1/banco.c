#include "banco.h"

#define MAX_LINE_LENGTH 255
#define MAX_LENGTH_NAME 50
#define MAX_LENGTH_SALDO 10
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

/// @brief Función que se encarga de imprimir en el archivo "banco.log" las acciones del banco
/// @param log Mensaje a imprimir en el archivo
void escrituraLogGeneral(char *log){

    FILE *file;
    char linea[MAX_LINE_LENGTH];
    time_t t; // Para esta función es necesaria la libreria de time.h

    struct tm *tm_info; // esto declara la estructura del tiempo y la fecha actual

    file = fopen("banco.log", "a+");
    
    if (file == NULL)
    {
        perror("Error al abrir el archivo de cuentas\n");
        return;
    }

    time(&t);
    tm_info = localtime(&t); // esta funcion asigna los valores de la fecha y hora actuales al struct declarado arriba
    strftime(linea, sizeof(linea), "%Y-%m-%d %H:%M:%S", tm_info); // esta funcion crear el string con los valores propios de la fecha y hora actuales

    // Escribimos en el archivo de log la acción realizada
    fprintf(file, "[%s] %s", linea, log);

    fclose(file);
}

/// @brief Limpia los strings de "\n"
/// @param string String que queremos "limpiar" de caracteres indeseados
void limpiezaString(char *string){
    for (int i = 0; i < strlen(string); i++)
        if (string[i]=='\n')
            string[i]='\0'; 
}

/// @brief Función que se encarga de registrar el usuario nuevo introducido
/// @param id Id del usuario nuevo
/// @param nombre Nombre del usuario nuevo
/// @param saldo Saldo del usuario nuevo
void registroCuenta(char *id, char *nombre, char *saldo){

    FILE *file;
    char linea[MAX_LINE_LENGTH];
    
    // Limpiamos los "\n" de nombre y de saldo porque vienen con dichos caracteres
    limpiezaString(nombre); 
    limpiezaString(saldo);

    file = fopen("cuentas.dat", "a+");
    
    if (file == NULL)
    {
        perror("Error al abrir el archivo de cuentas\n");
        return;
    }

    // Concatenamos los strings pasados como valores
    strcpy(linea, id);
    strcat(linea, ",");
    strcat(linea, nombre);
    strcat(linea, ",");
    strcat(linea, saldo);
    strcat(linea, ",");
    strcat(linea, "0");

    fputs(linea, file);

    escrituraLogGeneral("Se ha creado un nuevo usuario en el sistema del banco\n");

    fclose(file);
}

/// @brief Función que comprueba si el id pasado como parámetro se encuentra en el archivo "cuentas.dat"
/// @param id Id que queremos comprobar, desde el login o desde el registro
/// @param flag Variable que nos indica si nos encontramos ante un caso de LogIn o de registro
/// @return Devuelve un valor numérico que indica si es válido el id o no: 0 = error en id // 1 = id valido
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
    char saldo[MAX_LENGTH_SALDO];
    char nombre[MAX_LENGTH_NAME];
    int comprobacion = 1;

    do{
        printf("Bienvenido al registro de SafeBank\n");

        printf("Introduce tu nombre: (no se admiten más de 50 caracteres): \n");
        while(getchar() != '\n'); // Limpieza de buffer de entrada para evitar problemas en lectura de parametros
        fgets(nombre, sizeof(nombre), stdin);

        printf("Introduce tu id: (a partir de 100): \n");
        fgets(id, sizeof(id), stdin);

        printf("Introduce tu saldo: \n");
        fgets(saldo, sizeof(saldo), stdin);

        comprobacion = comprobarId(id, 0);
    }while((comprobacion != 1) || (nombre == NULL) || (strlen(nombre) > MAX_LENGTH_NAME));
    
    registroCuenta(id,nombre,saldo);
}

/// @brief Menú de logIn del Banco
void logIn(){

    char id[MAX_LENGTH_ID];
    int flg_log = 1;

    do{
        printf("\nBienvenido al LogIn de SafeBank\n");
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