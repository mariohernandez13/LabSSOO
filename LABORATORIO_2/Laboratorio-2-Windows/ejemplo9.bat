@ECHO OFF
SET "sourceDir=C:\ruta\del\directorio"
SET "outputFile=combined.txt"
REM Verifica si el archivo de salida ya existe y lo elimina
IF EXIST "%outputFile%" DEL "%outputFile%"
REM Combina el contenido de todos los archivos .txt en uno solo
FOR %%f IN ("%sourceDir%\*.txt") DO (
 ECHO Combinando: %%~nxf
 TYPE "%%f" >> "%outputFile%"
 ECHO. >> "%outputFile%"
 REM Añade una línea en blanco entre los contenidos de los archivos
)
ECHO Todos los archivos .txt han sido combinados en %outputFile%.
PAUSE