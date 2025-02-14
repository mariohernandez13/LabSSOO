@echo off
rem setea una pregunta de si/no por consola
set /p confirm="¿Esta seguro de que desea borrar el directorio MiDirectorio? [y/n]"
rem si la respuesta no es una y, entonces se acaba el programa
if /i "%confirm%" neq "y" pause
rem si no, se redirige al directorio especificado y lo borra, junto a su contenido
rd /s /q MiDirectorio
echo MiDirectorio y su contenido ha sido borrado
pause