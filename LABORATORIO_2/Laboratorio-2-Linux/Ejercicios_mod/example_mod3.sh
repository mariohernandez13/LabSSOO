#!/bin/bash
if [$# -eq 0]; then
	echo "Por favor, especifique el nombre del archivo como parametro."
	exit 1
fi

mkdir -p Midirectorio

if [-z "$2"]; then
	echo "Ingrese contenido del archivo:"
	read contenido
else
	contenido="$2"
fi
	echo "Creando archivo: $1 en Midirectorio"
	echo "$contenido" > Midirectorio/"$1"
