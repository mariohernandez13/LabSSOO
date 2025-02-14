#!/bin/bash
targetDir="/ruta/"
outputFile="archivos_seleccionados.txt" > "$outputFile"
for file in "$targetDir" /*.txt; do
	echo "$file" >> "$outputFile"
done

echo "Archivos .txt listados en $outputFile"

