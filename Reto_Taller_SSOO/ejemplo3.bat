@echo off
rem este if recoge el nombre del posible archivo seleccionado, si no hay ninguno especificado, se ejecuta lo de dentro del if
if "%~1"=="" (
	echo Por favor, especifique el nombre del archivo como parametro
	pause
	exit /b
)
for %%x in 
rem md crea directorio
md MiDirectorio
echo Creando archivo: %1 en MiDirectorio...
rem crea el archivo en la direccion Directorio\ especificando el nombre del archivo y escribiendo en este lo que se le pase en la instruccion echo
> MiDirectorio\%1 echo este es un archivo de prueba
pause