#!/bin/bash
targetDir="/home/ArgentaJC/Ejemplo4Linux"
outputFile="archivos_seleccionados.txt" > "$outputFile"
excludedFile="archivos_excluidos.txt" > "$excludedFile"
read -p "¿Quiere excluir archivos? [y/n]: " confirm
if [ "$confirm" = "y" ]; then
	patron="$1"
fi
for file in "$targetDir"/*.txt; do
	if [[ "$file" =~ "$patron" ]]; then
		echo "$file" >> "$excludedFile"
	else
		echo "$file" >> "$outputFile"
	fi
done
echo "Archivos .txt listados en $outputFile"
echo "Archivos excluidos en $excludedFile"
