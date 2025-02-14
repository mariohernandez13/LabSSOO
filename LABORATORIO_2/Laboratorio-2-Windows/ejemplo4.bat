@echo off
rem declaras una variable que es una ruta a un directorio
set "targetDir=C:\Users\javie\OneDrive\Documentos\Reto_Taller_SSOO\carpetaEjemplo4"
rem declaras una variable que es un archivo destino
set "outputFile=archivos_seleccionados.txt"
rem borras el contenido del archivo
del %outputFile% 2>nul
rem para cada archivo que tenga est directorio con la extensión .txt, va a poner su ruta en el archivo destino
for %%i in (%targetDir%\*) do (
	if "%%~xi"==".txt" (
		echo %%i >> %outputFile%
	)
)
echo Archivos.txt listados en %outputFile%
pause