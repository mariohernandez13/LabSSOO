@echo off 
rem llama a dos .bats
rem Si el archivo de errores ya existe, lo borra
if exist error.txt ( 
	del error.txt
)
rem Bloque de codigo que ejecuta el primer script
:SCRIPT1
echo Ejecutando script1.bat...
rem Llamamos al primer script
call script1.bat
rem Si ocurre algun error en la ejecucion se registra en el archivo de errores
if errorlevel 1 (
	echo Error al ejecutar script1.bat >> error.txt
	echo script1.bat fallo
) else (
	echo script1.bat ejecutado con exito
)
:SCRIPT2
echo Ejecutando script2.bat...
rem Llamamos al segundo script
call script2.bat
rem Si ocurre algun error en la ejecucion se registra en el archivo de errores
if errorlevel 1 (
	echo Error al ejecutar script1.bat >> error.txt
	echo script2.bat fallo
) else (
	echo script2.bat ejecutado con exito
) 
echo Ambos scripts se han ejecutado
pause