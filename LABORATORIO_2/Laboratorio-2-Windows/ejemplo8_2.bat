@echo off
rem variable del directorio a borrar
set /p directorio="Introduce el directorio que quieres borrar: " 
rem setea una pregunta de si/no por consola
set /p confirm="¿Desea borrar el directorio %directorio%? [y/n]: "
rem si la respuesta no es una y, entonces se acaba el programa
if /i "%confirm%" neq "y" goto FINAL
rem Encuentra el archivo que coincida con el directorio especificado
for %%f in (%directorio%) do (
	echo El directorio: %%f será borrado > archivo_borrado.txt
	echo. >> archivo_borrado.txt
)
rem Si ponemos el listado del directorio especificado dentro del archivo podemos ver toda su informacíon en el log
dir "%directorio%" >> archivo_borrado.txt
type archivo_borrado.txt
set /p confirm_2="¿Esta seguro de que desea borrar el directorio %directorio%? [y/n]: "
rem si no, se redirige al directorio especificado y lo borra, junto a su contenido
if /i "%confirm_2%" neq "y" goto FINAL
rd /s /q %directorio%
echo %directorio% y su contenido ha sido borrado
:FINAL
pause