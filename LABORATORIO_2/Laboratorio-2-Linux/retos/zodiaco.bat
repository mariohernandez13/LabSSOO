@echo off
set /p MES=Introduce tu mes de nacimiento: 
set /p DIA=Introduce tu dia de nacimiento: 
if "%MES%"=="Enero" (
	if %DIA% LEQ 20 (echo Tu signo zodiacal es: Capricornio) else (echo Tu signo zodiacal es: Acuario)
)
if "%MES%"=="Febrero" (
	if %DIA% LEQ 19 (echo Tu signo zodiacal es: Acuario) else (echo Tu signo zodiacal es: Piscis)
)
if "%MES%"=="Marzo" (
	if %DIA% LEQ 20 (echo Tu signo zodiacal es: Piscis) else (echo Tu signo zodiacal es: Aries)
)
if "%MES%"=="Abril" (
	if %DIA% LEQ 19 (echo Tu signo zodiacal es: Aries) else (echo Tu signo zodiacal es: Tauro)
)
if "%MES%"=="Mayo" (
	if %DIA% LEQ 20 (echo Tu signo zodiacal es: Tauro) else (echo Tu signo zodiacal es: Geminis)
)
if "%MES%"=="Junio" (
	if %DIA% LEQ 20 (echo Tu signo zodiacal es: Geminis) else (echo Tu signo zodiacal es: Cancer)
)
if "%MES%"=="Julio" (
	if %DIA% LEQ 22 (echo Tu signo zodiacal es: Cancer) else (echo Tu signo zodiacal es: Leo)
)
if "%MES%"=="Agosto" (
	if %DIA% LEQ 22 (echo Tu signo zodiacal es: Leo) else (echo Tu signo zodiacal es: Virgo)
)
if "%MES%"=="Septiembre" (
	if %DIA% LEQ 22 (echo Tu signo zodiacal es: Virgo) else (echo Tu signo zodiacal es: Libra)
)
if "%MES%"=="Octubre" (
	if %DIA% LEQ 22 (echo Tu signo zodiacal es: Libra) else (echo Tu signo zodiacal es: Escorpio)
)
if "%MES%"=="Noviembre" (
	if %DIA% LEQ 21 (echo Tu signo zodiacal es: Escorpio) else (echo Tu signo zodiacal es: Sagitario)
)
if "%MES%"=="Diciembre" (
	if %DIA% LEQ 21 (echo Tu signo zodiacal es: Sagitario) else (echo Tu signo zodiacal es: Capricornio)
)