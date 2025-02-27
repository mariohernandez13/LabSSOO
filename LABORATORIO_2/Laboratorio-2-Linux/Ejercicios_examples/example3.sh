#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Por favor, especifique el nombre del archivo como parámetro."
    exit 1
fi
mkdir -p MiDirectorio

echo "Creando archivo: $1 en MiDirectorio"
echo "Este es un archivo de prueba." > MiDirectorio/"$1"