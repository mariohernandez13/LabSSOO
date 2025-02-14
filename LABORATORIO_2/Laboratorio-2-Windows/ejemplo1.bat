@echo off
echo Fecha y Hora actuales:
rem la variable %date% sirve para obtener la Fecha del momento de ejecución
rem la variable %time% sirve para obtener el tiempo del momento de ejecución
echo %date% %time%
echo El tiempo actual de la maquina sin mostrar los segundos es: 
rem podemos poner la hora actual del ordenador con el comando TIME, si ponemos TIME /t entonces no pregunta una nueva hora
time /t
pause 