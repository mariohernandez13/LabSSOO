#!/bin/bash

read -p "¿Está seguro de que desea borrar el directorio MiDirectorio? [y/n] " confirm

if [ "$confirm" = "y" ]; then
	rm -rf MiDirectorio
	echo "MiDirectorio y su contenido han sido eliminados."
else
	echo "Operación cancelada."
fi
