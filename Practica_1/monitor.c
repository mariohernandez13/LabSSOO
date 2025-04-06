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
        escrituraLogGeneral("🟥 Error al abrir errores.dat para escritura\n", 0);
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

    escrituraLogGeneral("Errores modificados para indicar que se ha notificado de los errores encontrados\n", 1);
}

/// @brief Función que se encarga de resetear el archivo de logs de forma que se revisen todas las operaciones a lo largo del archivo. 
/// @param id ID de la persona que se ha detectado que ha infrigido las normas del banco
void resetearTransaccionesLog(int id)
{
    escrituraLogGeneral("🟥 He entrado a resetear transacciones\n", 0);
    FILE *archivoTransacciones;
    char linea[MAX_LINE_LENGTH];
    char *lineasArchivo[1000];
    int lineas = 0;

    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);
    if (semaforo_transacciones == SEM_FAILED)
    {
        escrituraLogGeneral("🟥 Error al abrir el semáforo de transacciones en monitor al intentar resetear las transacciones\n", 0);
        exit(1);
    }

    escrituraLogGeneral("Esperando acceso al archivo transacciones.log para resetear transacciones\n", 0);
    sem_wait(semaforo_transacciones);

    archivoTransacciones = fopen("logs/transacciones.log", "r");
    if (!archivoTransacciones)
    {
        escrituraLogGeneral("🟥 Error al abrir transacciones.log para lectura\n", 0);
        sem_post(semaforo_transacciones);
        return;
    }

    // Leer todas las líneas del archivo
    while (fgets(linea, sizeof(linea), archivoTransacciones))
    {
        lineasArchivo[lineas] = strdup(linea);
        lineas++;
    }
    fclose(archivoTransacciones);

    // Modificar las líneas que correspondan
    for (int i = 0; i < lineas; i++)
    {
        
        char contenidoHastaCorchete[MAX_LINE_LENGTH];
        char *contenido = strchr(lineasArchivo[i], ']');
        if (contenido)
        {
            size_t len = contenido - lineasArchivo[i] + 1; // recogemos la longitud de la cadena hasta el corchete (el +1 es para incluir el corchete)
            strncpy(contenidoHastaCorchete, lineasArchivo[i], len); // copiamos el valor de la cadena hasta el corchete, incluido
            contenidoHastaCorchete[len] = '\0'; // Aseguramos la terminación de cadena
        }

        if (contenido)
            contenido++; // Mover justo después de ']'
        else
            continue;

        char contenidoCopia[MAX_LINE_LENGTH];
        strncpy(contenidoCopia, contenido, MAX_LINE_LENGTH);
        contenidoCopia[MAX_LINE_LENGTH - 1] = '\0'; 

        char *estado = strtok(contenidoCopia, ",");
        char *tipo = strtok(NULL, ",");
        char *mensaje = strtok(NULL, ",");
        char *_id = strtok(NULL, ",");
        char *cantidadStr = strtok(NULL, ",");

        if ((strcmp(estado, " ERROR") == 0) && (id == atoi(_id)))
        {
            snprintf(linea, 500, "%s%s,%s,%s,%s,%s",
                     contenidoHastaCorchete, " ERROR_NOTIFICADO", tipo, mensaje, _id, cantidadStr);
            free(lineasArchivo[i]);
            lineasArchivo[i] = strdup(linea);
        }
        else if ((strcmp(estado, " OK") == 0) && (id == atoi(_id)))
        {
            snprintf(linea, 500, "%s%s,%s,%s,%s,%s",
                     contenidoHastaCorchete, " OK_REVISADO", tipo, mensaje, _id, cantidadStr);
            free(lineasArchivo[i]);
            lineasArchivo[i] = strdup(linea);
        }
    }

    // Reescribir el archivo desde cero
    archivoTransacciones = fopen("logs/transacciones.log", "w");
    if (!archivoTransacciones)
    {
        escrituraLogGeneral("🟥 Error al abrir transacciones.log para escritura\n", 0);
        for (int i = 0; i < lineas; i++)
            free(lineasArchivo[i]);
        sem_post(semaforo_transacciones);
        return;
    }

    for (int i = 0; i < lineas; i++)
    {
        fputs(lineasArchivo[i], archivoTransacciones);
        free(lineasArchivo[i]);
    }

    fclose(archivoTransacciones);
    sem_post(semaforo_transacciones);

    escrituraLogGeneral("🟩 Transacciones modificadas correctamente para indicar que se ha notificado de los errores encontrados\n", 0);
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
        escrituraLogGeneral("🟥 Error al abrir la tubuería en monitor\n", 0);
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
        escrituraLogGeneral("🟥 Error al abrir errores.dat para escritura\n", 0);
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
    escrituraLogGeneral("🟩 Errores escritos correctamente en errores.dat\n", 0);
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

    semaforo_transacciones = sem_open("/semaforo_transacciones", O_CREAT, 0644, 1);

    // Comprobamos que la apertura del semaforo de transacciones no haya sido erronea
    if (semaforo_transacciones == SEM_FAILED)
    {
        escrituraLogGeneral("Error al abrir el semaforo de transacciones", 0);
        exit(1);
    }

    sem_wait(semaforo_transacciones);

    file = fopen("logs/transacciones.log", "r");

    if (file == NULL)
    {
        escrituraLogGeneral("🟥 Error al abrir el archivo de transacciones\n", 0);
        return 1;
    }

    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), file))
    {
        linea[strcspn(linea, "\n")] = 0; // Eliminar salto de línea

        // Saltar el timestamp (todo hasta el primer ']')
        char *contenido = strchr(linea, ']');
        if (contenido)
            contenido++; // Mover justo después de ']'
        else
            continue;

        // Procesar la línea desde contenido (no desde linea)
        char *estado = strtok(contenido, ",");
        char *tipo = strtok(NULL, ",");
        char *mensaje = strtok(NULL, ",");
        char *id = strtok(NULL, ",");
        char *cantidadStr = strtok(NULL, ",");

        if (!estado || !tipo || !id)
            continue;

        if (strcmp(estado, " ERROR") == 0)
        {
            int tipoError = -1;
            if (strcmp(tipo, "Retiro") == 0)
                tipoError = 0;
            else if (strcmp(tipo, "Ingreso") == 0)
                tipoError = 1;
            else if (strcmp(tipo, "Transferencia") == 0)
                tipoError = 2;

            if (tipoError != -1)
                escribir_errores(id, tipoError);
            resetearTransaccionesLog(atoi(id));
        }
    }

    fclose(file);

    sem_post(semaforo_transacciones);

    escrituraLogGeneral("Se ha leído correctamente el contenido del archivo transacciones.log\n", 0);

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
        escrituraLogGeneral("🟥 Error al abrir el archivo de errores\n", 0);
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
            escrituraLogGeneral("Se ha superado el umbral de errores, enviando alerta al banco\n", 0);
            enviar_alerta(mensajeAlerta, id); // añadir además el id
        }
    }

    fclose(file);
}

/// @brief Función que muestra la configración de errores del archivo properties
void mostrar_configuracion()
{
    printf("\n⚙️  CONFIGURACIÓN ACTUAL:\n");
    printf("🔢 Umbral Total: %d\n", configuracion.umbralTotal);
    printf("💸 Umbral Retiros: %d\n", configuracion.umbralRetiros);
    printf("💰 Umbral Ingresos: %d\n", configuracion.umbralIngreso);
    printf("🔁 Umbral Transferencias: %d\n", configuracion.umbralTransferencias);
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

    printf("1️⃣ Ver log de eventos generales\n");
    printf("2️⃣ Ver configuración actual\n");
    printf("3️⃣ Recargar configuración\n");
    printf("4️⃣ Salir\n");
    printf("🡆 Elige una opción: ");

    configuracion = leer_configuracion(configuracion); // Leemos la configuracion del sistema

    while (1)
    {
        leer_transacciones(); // Llamamos primero a leer transacciones para comprobar cuantos errores se han detectado hasta el momento 
        leer_errores(); // Se llama a leer errores para comprobar que los errores de los usuarios propios del sistema no excede los limites del banco
        sleep(5); 
    }

    return 0;
}
