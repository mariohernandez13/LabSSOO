#!/bin/bash

# validamos que las entradas tengan valores esperados
# creamos un script con 2 parámetros (nombre fichero y tipo compresión)
# comprimimos el fichero (zip o tar) y borra el que no está comprimido

echo "Bienvenido a RZIP"
echo "Introduzca el nombre del fichero a comprimir y formato de compresión (zip o tar)" 

# valida que haya 2 parámetros
if [[ "$#" -ne 2 ]]; then
	echo "Debe proporcionar 2 parámetros"
	exit 1
fi

archivoConExtension=$1
tipoCompresion=$2

extensionArchivo="${archivoConExtension##*.}" # conseguimos la extensión del archivo con el punto
archivo=$(basename "$archivoConExtension" "$extensionArchivo") # conseguimos el archivo sin extensión

# valida que exista el fichero
if [[ -f "$archivoConExtension" ]]; then
	
	if [[ "$tipoCompresion" == "zip" ]]; then
			zip "${archivo}zip" "$archivoConExtension"
			
	elif [[ "$tipoCompresion" == "tar" ]]; then
			tar -cvf "${archivo}tar" "$archivoConExtension"
	else
			echo "El formato de compresión no está permitido (solo zip o tar)"
			exit 1
	fi
else
	echo "No se ha encontrado el archivo proporcionado"
	exit 1
fi
