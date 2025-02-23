@echo off

rem Definir la carpeta de origen y el archivo de salida
set "sourceDir=C:\Users\mario\OneDrive\Documentos\GitHub\LabSSOO\LABORATORIO_2\Laboratorio-2-Windows"
set "outputFile=combined.txt"

rem Borrar el archivo de salida si ya existe
if exist "%outputFile%" del "%outputFile%"

rem Crear un archivo temporal para almacenar líneas únicas
set "tempFile=temp.txt"
if exist "%tempFile%" del "%tempFile%"

rem Procesar cada archivo .txt en la carpeta
for %%F in ("%sourceDir%\*.txt") do (
    echo Combinando: %%F
    for /f "delims=" %%A in (%%F) do (
        findstr /x /c:"%%A" "%tempFile%" >nul || echo %%A>>"%tempFile%"
    )
    echo.>>"%tempFile%"  rem Agregar una línea en blanco entre archivos
)

rem Mover el archivo temporal al archivo de salida final
move /y "%tempFile%" "%outputFile%"
echo Todos los archivos .txt han sido combinados en %outputFile% sin líneas duplicadas.

