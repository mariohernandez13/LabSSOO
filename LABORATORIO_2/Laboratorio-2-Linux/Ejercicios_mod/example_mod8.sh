#!/bin/bash

Directorio="MiDirectorio"
Log_File="log_eliminacio.txt"

# Verificamos si el directorio existe
if [[ ! -d $Directorio ]]; then
    echo "El directorio '$Directorio' no existe."
    exit 1
fi

# Generamos el archivo de log con la lista de archivos y directorios que hay que eliminar
echo "listado de archivos y subdirectorios que van a ser eliminados:" > "$Log_File"
ls -R "$Directorio" >> "$Log_File"
echo

read -p "¿Está seguro de que desea borrar el directorio '$Directorio' [y/n]?" confirm
if [ "$confirm" = "y" ]; then
    rm -rf "$Directorio"
    echo "MiDirectorio y su contenido han sido eliminados."
    echo "Se ha guardado un registro en '$Log_File'."
else
    echo "Operacion cancelada"
fi