@echo off
rem abre el bloc de notas
if "%~1"=="" (
	echo Archivo no especificado
	pause
) else (
	if exist %1 (
		rem start "" abre el archivo especificado en ""
		start %1
		echo El bloc de notas se ha abierto
	) else (
		rem el comando "copy con" crea el archivo que se le pasa
		rem se puede escribir  el contenido del archivo 
		rem para salir de la edicion del archivo creado hay que darle Ctrl+Z
		copy con %1
		start %1
		echo Nuevo archivo .txt creado
	)
)
pause