#!/bin/bash

sourceDir="pruebaTXT"
outputFile="combined.txt" 
tempFile="temp_combined.txt" 

> "$outputFile"
if [ -e "$sourceDir"/*.txt ]; then


for file in "$sourceDir"/*.txt; do
    echo "Combinando: $file"
    cat "$file" >> "$outputFile"
    echo "" >> "$outputFile" # Añade una línea en blanco entre los contenidos de los archivos
done

awk '!seen[$0]++' "$outputFile" > "$tempFile"
mv "$tempFile" "$outputFile"
echo "Todos los archivos .txt han sido combinados en $outputFile."

else 
    echo "No hay archivos .txt en $sourceDir."
fi