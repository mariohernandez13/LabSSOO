@echo off
rem este if recoge el nombre del posible archivo seleccionado, si no hay ninguno especificado, se ejecuta lo de dentro del if
if "%~1"=="" (
	echo Por favor, especifique el nombre del archivo como parametro
	pause
	exit /b
)
rem Establecemos la variable del nombre del directorio a crear
set /p NOMBRE_DIRECTORIO=Introduce el nombre del directorio que quieres crear:
rem md crea directorio si no existe ya
if not exist %NOMBRE_DIRECTORIO% md %NOMBRE_DIRECTORIO%
rem redirige al directorio creado por el usuario
cd %NOMBRE_DIRECTORIO%
for %%x in (%*)do (
	rem crea el archivo en la direccion Directorio\ especificando el nombre del archivo y escribiendo en este lo que se le pase en la instruccion echo
	echo Creando archivo %%x en %NOMBRE_DIRECTORIO%...
	copy con %%x
)
rem Vuelve al directorio original
cd ..
pause