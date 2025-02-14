#!/bin/bash
gedit &
pid=$!
echo "El editor de texto se ha abierto"
wait "$pid"
echo "telefono apagado o fuera de cobertura"
