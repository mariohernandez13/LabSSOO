schtasks /create /tn "TallerBatch" /tr "C:\Users\javie\OneDrive\Documentos\Reto_Taller_SSOO\script1.bat" /sc daily /sc  /st 10:00
rem este comando se encarga de crear una task programada en el ordenador que ejecuta el .bat especificado
rem /create para especificar que crea una nueva tarea
rem /tn es "task name" 
rem /tr es "task route"
rem /sc es "schedule"
rem /sc onstart para que se ejecute cuando se inicie el ordenador