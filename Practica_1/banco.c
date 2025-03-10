#include "banco.h"

CONFIG configuracion;

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
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
        return;
    }

    time(&t);
    tm_info = localtime(&t); // esta funcion asigna los valores de la fecha y hora actuales al struct declarado arriba
    strftime(linea, sizeof(linea), "%Y-%m-%d %H:%M:%S", tm_info); // esta funcion crear el string con los valores propios de la fecha y hora actuales

    // Escribimos en el archivo de log la acción realizada
    fprintf(file, "[%s] %s", linea, log);

    fclose(file);
}

/// @brief Función que se llama para leer el archivo de configuración
/// @return valor numérico que indica la validez de la lectura
int leer_configuracion() {

    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    char *key, *value;

    char username[MAX_LINE_LENGTH] = "";

    file = fopen("banco.config", "r");
    
    if (file == NULL) {
        escrituraLogGeneral("Error al abrir el archivo de configuración\n");
        return 1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;
        
        key = strtok(linea, "=");
        value = strtok(NULL, "=");

        // Mirar una posible conversión de esto a un switch case en el futuro
        if (key && value) {
            if (strcmp(key, "LIMITE_RETIRO") == 0) {
                configuracion.limiteRetiros = atoi(value);
            } else if (strcmp(key, "LIMITE_TRANSFERENCIA") == 0){
                configuracion.limiteTransferencia = atoi(value);
            } else if (strcmp(key, "UMBRAL_RETIROS") == 0){
                configuracion.umbralRetiros = atoi(value);
            } else if (strcmp(key, "UMBRAL_TRANSFERENCIAS") == 0){
                configuracion.umbralTransferencias = atoi(value);
            } else if (strcmp(key, "NUM_HILOS") == 0){
                configuracion.numHilos = atoi(value);
            } else if (strcmp(key, "ARCHIVO_CUENTAS") == 0){
                strncpy(configuracion.archivoCuentas, value, strlen(value));
            } else if (strcmp(key, "ARCHIVO_TRANSACCIONES") == 0){
                strncpy(configuracion.archivoTransacciones, value, MAX_LINE_LENGTH);
            } else if (strcmp(key, "ARCHIVO_LOG") == 0){
                strncpy(configuracion.archivoLog, value, MAX_LINE_LENGTH);
            }
        }
    }

    fclose(file);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo banco.config\n");

    return (state);
}

/// @brief Limpia los strings de "\n"
/// @param string String que queremos "limpiar" de caracteres indeseados
void limpiezaString(char *string){
    for (int i = 0; i < strlen(string); i++)
        if (string[i]=='\n')
            string[i]='\0'; 
}

/// @brief Función que se encarga de registrar uan nueva cuenta en el sistema del banco
/// @param cuenta Parametros de la nueva cuenta
void registroCuenta(Cuenta cuenta){

    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    
    // Limpiamos los "\n" de nombre y de saldo porque vienen con dichos caracteres
    limpiezaString(cuenta.titular); 
    limpiezaString(cuenta.saldo);

    file = fopen("cuentas.dat", "a+");
    
    if (file == NULL)
    {
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
        return;
    }

    // Concatenamos los strings pasados como valores
    strcpy(linea, "\n");
    strcat(linea, cuenta.numero_cuenta);
    strcat(linea, ",");
    strcat(linea, cuenta.titular);
    strcat(linea, ",");
    strcat(linea, cuenta.saldo);
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
        escrituraLogGeneral("Error al abrir el archivo de cuentas\n");
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
                escrituraLogGeneral("El id ya existe\n");
                esValido = 0;
                break;
            }
        }else if (flag == 1)
        {
            if (strcmp(key, id) == 0)
            {
                esValido = 1;
                break;
            }
            else
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
        escrituraLogGeneral("El id introducido no es válido debido a que es menor a 100\n");
        return validez;
    }
    
    validez = existeID(id, flag);

    return validez;
}

/// @brief Menú de registro del Banco
void registro(){

    Cuenta cuenta;

    int comprobacion = 1;

    do{
        if (!comprobacion)
            printf("Ha ocurrido un error en tu intento de registro, prueba a volver a intentarlo.\n");

        printf("Bienvenido al registro de SafeBank\n");

        printf("Introduce tu nombre: (no se admiten más de 50 caracteres): \n");
        while(getchar() != '\n'); // Limpieza de buffer de entrada para evitar problemas en lectura de parametros
        fgets(cuenta.titular, sizeof(cuenta.titular), stdin);

        printf("Introduce tu id: (a partir de 100): \n");
        fgets(cuenta.numero_cuenta, sizeof(cuenta.numero_cuenta), stdin);

        printf("Introduce tu saldo: \n");
        fgets(cuenta.saldo, sizeof(cuenta.saldo), stdin);

        comprobacion = comprobarId(cuenta.numero_cuenta, 0);
    }while((comprobacion != 1) || (cuenta.titular == NULL) || (strlen(cuenta.titular) > MAX_LENGTH_NAME));
    
    registroCuenta(cuenta);
}

/// @brief Menú de logIn del Banco
void logIn(){

    char id[MAX_LENGTH_ID];
    int flg_log = 1;
    int comprobacion = 1;
    pid_t pid;
    char *path = "usuarios.c";

    do{
        printf("\nBienvenido al LogIn de SafeBank\n");

        printf("Introduce tu id: (a partir de 100)\n");
        while(getchar() != '\n');
        fgets(id, sizeof(id), stdin);

        comprobacion = comprobarId(id, flg_log);
    }while(!comprobacion);
    
    pid = fork();

    // Comprobamos que fork() no genera un error
    if (pid < 0) { 
        escrituraLogGeneral("Error al crear la sesión de LogIn\n");
        return;
    }
    else if (pid == 0){  // Proceso hijo
        execl("usuario.c", id, configuracion, (char *) NULL);
    }
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
    
    // Comprobamos que no ocurre problema al generar la pipe
    if (pipe(fd) == -1) {   
        escrituraLogGeneral("Error en la generación de la pipe\n");
        return 1;
    }
    
    leer_configuracion();

    //pid = fork();

    /*if (pid < 0)
    {
        escrituraLogGeneral("Error al generar un hijo de banco (usuario)\n");
    }
    else if (pid == 0)
    {
        menuBanco();
    }
    else 
    {
        wait(NULL);
    }*/

    menuBanco();

    return (0);
}