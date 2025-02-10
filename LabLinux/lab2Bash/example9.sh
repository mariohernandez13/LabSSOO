#!/bin/bash

sourceDir="/ruta/del/directorio"
outputFile="combined.txt"

> "$outputFile"

for file in "$sourceDir"/*.txt; do
	echo "Combinando: $file"
	cat "$file" >> "$outputFile"
	echo "" >> "$outputFile" # Añade una línea en blanco entre los contenidos de los archivos
done
echo "Todos los archivos .txt han sido combinados en $outputFile."
