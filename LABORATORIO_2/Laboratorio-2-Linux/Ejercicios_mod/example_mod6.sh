#!/bin/bash
./script1.sh
if [ $? -eq 0 ]; then
	echo "script1 ejecutado"
else
	echo "error en script1"
	exit
fi
./script2.sh
if [ $? -eq 0 ]; then
	echo "script2 ejecutado"
else
	echo "error en el script2"
	exit
fi
echo "Ambos scripts se han ejecutado."

