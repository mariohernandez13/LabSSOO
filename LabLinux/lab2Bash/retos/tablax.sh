#!/bin/bash

#dado un número hay que mostrar su tabla de multiplicar de 1 a 12
#validar que el input es un número

echo -n "Introduzca un numero: "
read input

if [[ "$input" =~ ^[0-9]+$ ]]; then
	echo "Tabla de multiplicar del $input:"
	for i in {1..12}; do
	echo "$input x $i = $((input*i))"
	done
else
	echo "Introduzca un numero Valido"

fi
