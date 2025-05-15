#include "banco.h"

CONFIG configuracion;


/// @brief Función que se encarga de resetear el archivo de errores.dat cuando se ha detectado la necesidad de mandar una alerta al sistema
void resetearErrores()
{
    FILE *archivoErrores;
    char linea[MAX_LINE_LENGTH];
    int lineas = 0;
    char *lineasArchivo[MAX_LINE_LENGTH];

    archivoErrores = fopen("data/errores.dat", "r+");
    if (!archivoErrores)
    {
        escrituraLogGeneral("🟥 Error al abrir errores.dat para escritura en monitor.c, en función: resetearErrores\n", 0);
        return;
    }

    // Este bucle se usa para copiar dentro del array de lineas del archivo todas las cuentas del archivo errores.dat
    while (fgets(linea, MAX_LINE_LENGTH, archivoErrores) && lineas < MAX_LINE_LENGTH)
    {
        lineasArchivo[lineas] = strdup(linea);
        lineas++;
    }

    for (int i = 0; i < lineas; i++)
    {
        char temp[MAX_LINE_LENGTH];
        strncpy(temp, lineasArchivo[i], MAX_LINE_LENGTH);
        temp[MAX_LINE_LENGTH - 1] = '\0';
        char *idArchivo = strtok(temp, ",");

        snprintf(lineasArchivo[i], MAX_LINE_LENGTH, "%s,%d,%d,%d\n",
                 idArchivo, 0, 0, 0); // Reiniciar todos los errores a 0
    }

    // Reescribe todo el archivo errores.dat
    rewind(archivoErrores); // Volver al inicio del archivo para sobrescribirlo
    for (int i = 0; i < lineas; i++)
    {
        fputs(lineasArchivo[i], archivoErrores);
        free(lineasArchivo[i]);
    }
    fclose(archivoErrores);

    escrituraLogGeneral("Errores modificados para indicar que se ha notificado de los errores encontrados en monitor.c, en función: resetearErrores\n", 1);
}

/// @brief Función que se encarga de resetear el archivo de logs de forma que se revisen todas las operaciones a lo largo del archivo.
/// @param id ID de la persona que se ha detectado que ha infrigido las normas del banco
void resetearTransaccionesLog(const char *rutaArchivo, int id)
{
    escrituraLogGeneral("🟥 Reseteando transacciones en monitor.c, función: resetearTransaccionesLog\n", 0);

    FILE *archivoTransacciones;
    char linea[MAX_LINE_LENGTH];  // Buffer para lectura de líneas
    char nuevaLinea[1024];        // Buffer más grande para evitar truncamientos al reescribir líneas
    char *lineasArchivo[1000];    // Array para almacenar las líneas leídas
    int lineas = 0;

    // Abrimos el semáforo si no está abierto
    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);
    if (semaforo_transacciones == SEM_FAILED)
    {
        escrituraLogGeneral("🟥 Error al abrir el semáforo de transacciones en monitor.c, función: resetearTransaccionesLog\n", 0);
        exit(1);
    }

    // Bloqueamos el acceso concurrente
    escrituraLogGeneral("⏳ Esperando acceso al archivo de transacciones para resetear...\n", 0);
    sem_wait(semaforo_transacciones);

    // Abrimos el archivo en modo lectura
    archivoTransacciones = fopen(rutaArchivo, "r");
    if (!archivoTransacciones)
    {
        escrituraLogGeneral("🟥 Error al abrir archivo de transacciones para lectura en resetearTransaccionesLog\n", 0);
        sem_post(semaforo_transacciones);
        return;
    }

    // Leemos todas las líneas y las almacenamos en memoria
    while (fgets(linea, sizeof(linea), archivoTransacciones))
    {
        if (lineas >= 1000)
        {
            escrituraLogGeneral("🟥 Se alcanzó el límite máximo de líneas (1000) en resetearTransaccionesLog\n", 0);
            break;
        }

        lineasArchivo[lineas] = strdup(linea); // Copiamos línea al array dinámicamente

        if (!lineasArchivo[lineas])
        {
            escrituraLogGeneral("🟥 Error de memoria al duplicar línea en resetearTransaccionesLog\n", 0);
            fclose(archivoTransacciones);
            sem_post(semaforo_transacciones);
            return;
        }

        lineas++;
    }
    fclose(archivoTransacciones);

    escrituraLogGeneral("📄 Número total de líneas leídas del archivo de transacciones: ", lineas);

    // Procesamos cada línea para modificar aquellas del ID indicado
    for (int i = 0; i < lineas; i++)
    {
        char contenidoHastaCorchete[MAX_LINE_LENGTH];
        char *contenido = strchr(lineasArchivo[i], ']');

        if (!contenido)
        {
            escrituraLogGeneral("⚠️ Línea sin corchete encontrada, ignorando.\n", 0);
            continue;
        }

        size_t len = contenido - lineasArchivo[i] + 1;
        strncpy(contenidoHastaCorchete, lineasArchivo[i], len);
        contenidoHastaCorchete[len] = '\0';

        contenido++;  // Avanzamos el puntero tras ']'

        // Copia de contenido para usar con strtok
        char contenidoCopia[MAX_LINE_LENGTH];
        strncpy(contenidoCopia, contenido, MAX_LINE_LENGTH - 1);
        contenidoCopia[MAX_LINE_LENGTH - 1] = '\0';

        char *estado = strtok(contenidoCopia, ",");
        char *tipo = strtok(NULL, ",");
        char *mensaje = strtok(NULL, ",");
        char *_id = strtok(NULL, ",");
        char *cantidadStr = strtok(NULL, ",");

        // Verificamos campos válidos
        if (!estado || !tipo || !mensaje || !_id || !cantidadStr)
        {
            escrituraLogGeneral("⚠️ Línea malformada detectada al parsear campos con strtok. Se ignora.\n", 0);
            continue;
        }

        if (atoi(_id) == id)
        {
            if (strcmp(estado, " ERROR") == 0)
            {
                if (snprintf(nuevaLinea, sizeof(nuevaLinea), "%s%s,%s,%s,%s,%s",
                             contenidoHastaCorchete, " ERROR_NOTIFICADO", tipo, mensaje, _id, cantidadStr) >= sizeof(nuevaLinea))
                {
                    escrituraLogGeneral("🟥 Línea truncada al formatear con snprintf (ERROR_NOTIFICADO).\n", 0);
                }

                free(lineasArchivo[i]);
                lineasArchivo[i] = strdup(nuevaLinea);
                if (!lineasArchivo[i])
                {
                    escrituraLogGeneral("🟥 Error de memoria al duplicar línea modificada (ERROR_NOTIFICADO).\n", 0);
                    sem_post(semaforo_transacciones);
                    return;
                }
            }
            else if (strcmp(estado, " OK") == 0)
            {
                if (snprintf(nuevaLinea, sizeof(nuevaLinea), "%s%s,%s,%s,%s,%s",
                             contenidoHastaCorchete, " OK_REVISADO", tipo, mensaje, _id, cantidadStr) >= sizeof(nuevaLinea))
                {
                    escrituraLogGeneral("🟥 Línea truncada al formatear con snprintf (OK_REVISADO).\n", 0);
                }

                free(lineasArchivo[i]);
                lineasArchivo[i] = strdup(nuevaLinea);
                if (!lineasArchivo[i])
                {
                    escrituraLogGeneral("🟥 Error de memoria al duplicar línea modificada (OK_REVISADO).\n", 0);
                    sem_post(semaforo_transacciones);
                    return;
                }
            }
        }
    }

    // Reescribimos el archivo
    archivoTransacciones = fopen(rutaArchivo, "w");
    if (!archivoTransacciones)
    {
        escrituraLogGeneral("🟥 Error al abrir archivo de transacciones para escritura en resetearTransaccionesLog\n", 0);
        for (int i = 0; i < lineas; i++)
            free(lineasArchivo[i]);
        sem_post(semaforo_transacciones);
        return;
    }

    for (int i = 0; i < lineas; i++)
    {
        if (fputs(lineasArchivo[i], archivoTransacciones) == EOF)
        {
            escrituraLogGeneral("🟥 Error al escribir una línea en el archivo de transacciones.\n", 0);
        }
        free(lineasArchivo[i]);
    }

    fclose(archivoTransacciones);
    sem_post(semaforo_transacciones);

    escrituraLogGeneral("🟩 Archivo de transacciones actualizado correctamente en monitor.c, función: resetearTransaccionesLog\n", 0);
}



/// @brief Fucnión que se encarga de mandar una alerta y avisar de los reseteos pertinentes para modificar el sistema del banco.
/// @param mensaje Mensaje que se está mandando a la tubería que comunica Monitor con Banco
/// @param id ID de la persona que ha infrngido las normas del banco
void enviar_alerta(char *mensaje, int id)
{
    // Llamamos a las funciones que se encargan de resetear los archivos
    resetearErrores();

    int fifo_fd;

    fifo_fd = open(FIFO1, O_WRONLY);

    if (fifo_fd == -1)
    {
        escrituraLogGeneral("🟥 Error al abrir la tubuería en monitor en monitor.c, en función: enviar_alerta\n", 0);
        return;
    }

    // Enviamos el mensaje del error desde monitor a Banco a través de la tubería FIFO declarada
    write(fifo_fd, mensaje, strlen(mensaje) + 1);
    close(fifo_fd);
}

/// @brief Actualiza el archivo errores.dat incrementando el tipo de error correspondiente
/// @param id ID del usuario (como string)
/// @param tipoError Tipo de error: 0=retiro, 1=ingreso, 2=transferencia
void escribir_errores(char *id, int tipoError)
{
    FILE *archivo = fopen("data/errores.dat", "r+");
    if (!archivo)
    {
        escrituraLogGeneral("🟥 Error al abrir errores.dat para escritura en monitor.c, en función: enviar_alerta\n", 0);
        return;
    }

    sem_post(semaforo_transacciones);

    char linea[MAX_LINE_LENGTH];
    char nuevaLinea[MAX_LINE_LENGTH];
    char *lineasArchivo[1000];
    int totalLineas = 0;
    int encontrado = 0;

    // Leer todas las líneas del archivo
    while (fgets(linea, sizeof(linea), archivo))
    {
        lineasArchivo[totalLineas] = strdup(linea);
        totalLineas++;
    }

    // Buscar y actualizar la línea correspondiente al ID
    for (int i = 0; i < totalLineas; i++)
    {
        // Copiamos la line actual a una linea temporal para no modificar la original
        char *temp = strdup(lineasArchivo[i]);
        // Limpiamos el salto de linea
        temp[strcspn(temp, "\n")] = 0;

        char *token = strtok(temp, ",");
        if (token && strcmp(token, id) == 0)
        {
            // recogemos los valores de las variables propias de la estructura de errores
            int errorRetiro = atoi(strtok(NULL, ","));
            int errorIngreso = atoi(strtok(NULL, ","));
            int errorTrans = atoi(strtok(NULL, ","));

            if (tipoError == 0)
                errorRetiro++;
            else if (tipoError == 1)
                errorIngreso++;
            else if (tipoError == 2)
                errorTrans++;

            snprintf(nuevaLinea, sizeof(nuevaLinea), "%s,%d,%d,%d\n", id, errorRetiro, errorIngreso, errorTrans);
            free(lineasArchivo[i]);
            lineasArchivo[i] = strdup(nuevaLinea);
            encontrado = 1;
        }

        free(temp);
    }

    // Si no se encontró el ID, añadir nueva línea
    if (!encontrado)
    {
        int e0 = 0, e1 = 0, e2 = 0;
        if (tipoError == 0)
            e0 = 1;
        if (tipoError == 1)
            e1 = 1;
        if (tipoError == 2)
            e2 = 1;

        snprintf(nuevaLinea, sizeof(nuevaLinea), "%s,%d,%d,%d\n", id, e0, e1, e2);
        lineasArchivo[totalLineas++] = strdup(nuevaLinea);
    }

    // Reescribir el archivo desde cero
    freopen("data/errores.dat", "w", archivo);
    for (int i = 0; i < totalLineas; i++)
    {
        fputs(lineasArchivo[i], archivo);
        free(lineasArchivo[i]);
    }

    fclose(archivo);
    escrituraLogGeneral("🟩 Errores escritos correctamente en errores.dat en monitor.c, en función: enviar_alerta\n", 0);
}

/// @brief Funcion que se encarga de revisar el archivo de sesiones.txt y contabiliza las sesiones activas.
/// @param sesiones Array de sesiones activas
/// @return Devuelve sesiones activas
int leer_sesiones(char sesiones[MAX_SESIONES][MAX_LENGTH_ID])
{
    
    FILE *file;
    int totalSesiones = 0;
    char linea[MAX_LINE_LENGTH] = "";

    file = fopen(configuracion.archivoSesiones, "r");

    if (file == NULL) {
        escrituraLogGeneral("🟥 Error: no se pudo abrir el archivo de sesiones en 'leer_sesiones'.\n", 0);
        return -1;
    }

    while (fgets(linea, sizeof(linea), file)) {
        // Elimina el salto de línea si está presente
        linea[strcspn(linea, "\n")] = '\0';

        if (strlen(linea) == 0) {
            escrituraLogGeneral("⚠️ Línea vacía detectada, se ignora.\n", 0);
            continue;
        }

        if (totalSesiones >= MAX_SESIONES) {
            escrituraLogGeneral("🟥 Error: se excedió el número máximo de sesiones permitidas.\n", 0);
            fclose(file);
            return -1;
        }

        // Copiar línea al array
        strncpy(sesiones[totalSesiones], linea, MAX_LENGTH_ID - 1);
        sesiones[totalSesiones][MAX_LENGTH_ID - 1] = '\0';  // Asegurar fin de cadena
        totalSesiones++;
    }

    fclose(file);

    // Registrar cuántas sesiones fueron leídas
    char mensaje[100];
    snprintf(mensaje, sizeof(mensaje), "✅ Sesiones leídas correctamente: %d\n", totalSesiones);
    escrituraLogGeneral(mensaje, 0);

    return totalSesiones;
}

/// @brief Funcion que se encarga de revisar el archivo de transacciones.log por completo y contabiliza cuales son los errores dentro de dicho archivo.
/// @return Devuelve un estado
int leer_transacciones()
{
    FILE *file;
    int state = 0;
    char linea[MAX_LINE_LENGTH] = "";
    int contadorErroresGenerales = 0;
    int contadorTransaccion = 0;
    char username[MAX_LINE_LENGTH] = "";
    char buffer[100];
    char sesiones[MAX_SESIONES][MAX_LENGTH_ID];
    DIR *dir;
    struct dirent *entry;
    int totalSesiones;

    totalSesiones = leer_sesiones(sesiones);
 
    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);

    // Comprobamos que la apertura del semaforo de transacciones no haya sido erronea
    if (semaforo_transacciones == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir el semaforo de transacciones en monitor.c, en función: leer_transacciones", 0);
        exit(1);
    }

    sem_wait(semaforo_transacciones);

    dir = opendir("transacciones");
    if (!dir) {
        escrituraLogGeneral("No se pudo abrir la carpeta 'transacciones'", 0);
        sem_post(semaforo_transacciones);
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Saltar "." y ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Verificar si esta carpeta corresponde a una sesión activa
            int esSesionActiva = 0;
            for (int i = 0; i < totalSesiones; i++) {
                if (strcmp(entry->d_name, sesiones[i]) == 0) {
                    esSesionActiva = 1;
                    break;
                }
            }

            if (!esSesionActiva)
                continue;

            // Construir la ruta al archivo transacciones.log
            char rutaArchivo[512];
            snprintf(rutaArchivo, sizeof(rutaArchivo), "transacciones/%s/transacciones.log", entry->d_name);

            FILE *file = fopen(rutaArchivo, "r");
            if (!file) {
                printf("⚠️ No se pudo abrir %s\n", rutaArchivo);
                continue;
            }

            char linea[MAX_LINE_LENGTH];
            while (fgets(linea, sizeof(linea), file)) {
                linea[strcspn(linea, "\n")] = 0;

                char *contenido = strchr(linea, ']');
                if (contenido)
                    contenido++;
                else
                    continue;

                char *estado = strtok(contenido, ",");
                char *tipo = strtok(NULL, ",");
                char *mensaje = strtok(NULL, ",");
                char *id = strtok(NULL, ",");
                char *cantidadStr = strtok(NULL, ",");

                if (!estado || !tipo || !id)
                    continue;

                if (strcmp(estado, " ERROR") == 0) {
                    int tipoError = -1;
                    if (strcmp(tipo, "Retiro") == 0)
                        tipoError = 0;
                    else if (strcmp(tipo, "Ingreso") == 0)
                        tipoError = 1;
                    else if (strcmp(tipo, "Transferencia") == 0)
                        tipoError = 2;

                    if (tipoError != -1)
                        escribir_errores(id, tipoError);
                    resetearTransaccionesLog(rutaArchivo, atoi(id));
                }
            }

            fclose(file);
        }
    }

    closedir(dir);

    sem_post(semaforo_transacciones);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo transacciones.log en monitor.c, en función: leer_transacciones\n", 0);

    return (state);
}

/// @brief Función que se encarga de leer el archivo de errores y enviar una alerta al banco si se supera el umbral
void leer_errores()
{
    FILE *file;
    char linea[MAX_LINE_LENGTH] = "";
    int id;
    int totalErrores, errorRetiro, errorIngreso, errorTransaccion;
    char *key;
    char mensajeAlerta[100];

    file = fopen("data/errores.dat", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de errores en monitor.c, en función: leer_errores\n", 0);
        return;
    }

    while (fgets(linea, sizeof(linea), file))
    {
        // Eliminamos el salto de linea al final de la cadena
        linea[strcspn(linea, "\n")] = 0;

        // Asignamos valores a las variables propias de la estructura de errores
        key = strtok(linea, ",");
        id = atoi(key);

        key = strtok(NULL, ",");
        errorRetiro = atoi(key);

        key = strtok(NULL, ",");
        errorIngreso = atoi(key);

        key = strtok(NULL, ",");
        errorTransaccion = atoi(key);

        // solo se pueden cometer 5 errores generales o 3 específicos de una operación
        totalErrores = errorIngreso + errorRetiro + errorTransaccion;

        // comprobamos que el usuario registrado en esta linea no infringe nignuna de las limitaciones impuestas por el sistema del banco
        if (errorRetiro >= configuracion.umbralRetiros || errorIngreso >= configuracion.umbralIngreso || errorRetiro >= configuracion.umbralTransferencias || totalErrores >= configuracion.umbralTotal)
        {
            escrituraLogGeneral("Se ha superado el umbral de errores, enviando alerta al banco en monitor.c, en función: leer_errores\n", 0);
            enviar_alerta(mensajeAlerta, id); // añadir además el id
        }
    }
    escrituraLogGeneral("Comprobando errores...\n", 0);
    fclose(file);
}

/// @brief Función main de monitor, se encarga de leer el archivo de configuración y el de errores
/// @param argc
/// @param argv
/// @return
int main(int argc, char *argv[])
{
    printf("=============================================\n");
    printf("🛡️  MONITOR DE ANOMALÍAS - SAFEBANK SYSTEM 🛡️\n");
    printf("=============================================\n");
    printf("⏳ Iniciando el monitor de anomalías...\n");
    mostrarCarga();
    printf("🔍 Detectando anomalías...\n");
    configuracion = leer_configuracion(configuracion); // Leemos la configuracion del sistema

    while (1)
    {
        leer_transacciones(); // Llamamos primero a leer transacciones para comprobar cuantos errores se han detectado hasta el momento
        leer_errores();       // Se llama a leer errores para comprobar que los errores de los usuarios propios del sistema no excede los limites del banco
        sleep(5);
    }

    return 0;
}
