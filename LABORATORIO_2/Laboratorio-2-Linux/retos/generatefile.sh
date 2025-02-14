#!/bin/bash

while true; do
read -p "Introduzca un Nombre, Tamaño y Tipo[vacio/texto]" nombre tamanio tipo

if [[$# -eq 3]]; then
	break
else
	echo "ERROR - Debe introducir 3 parametros"
fi
done

# se puede crear con fallocate -l tamanio nombre
# esto lo que hace meter contenido en el archivo,
#le da un tamaño determinado y muestra el progreso

dd if="/dev/zer" of="$nombre" bs=1 count="$tamanio" status=progress

echo "$contenido" > "$nombre"

if [[ -f "$nombre" ]]; then
	echo "Archivo creado con $tamanio bytes de tamaño."
else
	echo "ERROR - No se ha creado el archivo."
fi
