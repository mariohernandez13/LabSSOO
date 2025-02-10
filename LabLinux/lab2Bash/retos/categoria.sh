#!/bin/bash

echo -n "Introduzca un valor numerico: "
read input
if ! [[ $input =~ ^[0-9]+$ ]]; then
	echo "ERROR - Debe introducir un numero."
	exit -1
fi

if [[ $input -le 15 ]]; then
	echo "1"
elif [[ $input -ge 16 && $input -le 55 ]]; then
	echo "2"
elif [[ $input -ge 56 && $input -le 100 ]]; then
	echo "3"
elif [[ $input -ge 101 && $input -le 200 ]]; then
	echo "4"
else
	echo "5"
fi
