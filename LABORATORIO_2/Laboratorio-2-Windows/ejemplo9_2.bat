@echo off
setlocal

rem Solicitar la palabra a excluir al usuario
set /p excludeWord=Ingresa la palabra a excluir: 

rem Definir la carpeta de origen y los archivos de salida
set "sourceDir=C:\Users\mario\OneDrive\Documentos\GitHub\LabSSOO\LABORATORIO_2\Laboratorio-2-Windows"
set "outputFile=combined.txt"
set "tempFile=temp.txt"

rem Borrar archivos de salida si ya existen
if exist "%outputFile%" del "%outputFile%"
if exist "%tempFile%" del "%tempFile%"

rem Procesar cada archivo .txt en la carpeta
for %%F in ("%sourceDir%\*.txt") do (
    echo Procesando: %%F
    for /f "usebackq delims=" %%L in (`type "%%F" ^| findstr /V /C:"%excludeWord%"`) do (
        if not "%%L"=="" echo %%L>>"%tempFile%"
    )
    echo.>>"%tempFile%"  rem Agregar una línea en blanco entre archivos
)

rem Mover el archivo temporal al archivo de salida final
move /y "%tempFile%" "%outputFile%"
echo Todos los archivos .txt han sido combinados en %outputFile%, excluyendo líneas con "%excludeWord%".
pause
