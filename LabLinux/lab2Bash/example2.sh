#!/bin/bash
if [ -d "Batchworkshop" ]; then 
	if [ -f "Batchworkshop/README.txt" ]; then
		date >> Batchworkshop/README.txt
	else
	echo "Bienvenido al Taller de Bash" > Batchworkshop/README.txt
	echo "Este es un archivo README de ejemplo" >> Batchworkshop/README.txt		

	fi
else
	mkdir -p Batchworkshop
fi
