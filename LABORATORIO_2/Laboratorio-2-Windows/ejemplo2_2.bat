@echo off
rem md se encarga de crear un directorio
if not exist BatchWorkshop md BatchWorkshop
rem cd se encarga de redirigirte hacia el directorio de la ruta especificada
cd BatchWorkshop
if "type README.txt"==""(
	rem lo que sea > archivo.extension.txt escribe el texto que haya a la izquierda del > en el archivo seleccionado
	echo Bienvenido al taller de Batch > README.txt
	rem lo que sea >> archivo.txt se encarga de escribir al final del archivo seleccionado lo que ponga a la izquierda del > 
	echo Este es un archivo README de ejemplo >> README.txt
) else (
	echo %date% %time% >> README.txt
)
pause