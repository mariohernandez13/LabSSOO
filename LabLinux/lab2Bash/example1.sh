#!/bin/bash
echo "Fecha y Hora Actual:"
date
fecha=$(date +%w)
diasRestantes=$((5 - fecha))
if [ "$fecha " -ge 5 ]; then
	echo "Ya es fin de semana"
else
	echo "Quedan $diasRestantes dias para el fin de semana"
fi
