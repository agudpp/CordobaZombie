@echo off

echo Generando nuevo reporte
mkdir error_reports

echo Copiando archivos .log
copy *.log error_reports
echo Copiando archivos de configuracion
copy *.cfg error_reports
copy *.xml error_reports
echo Copiando informacion del sistema, si es que hay
copy *.info error_reports

echo Eliminando logs actuales
del *.log

echo Reporte de errores generado correctamente en la carpeta error_reports
echo Por favor, genere un .zip o .rar de esa carpeta e incluyala en el formulario
echo de la pagina testing (www.cordobazombie.com.ar/testing) de nuestro sitio.
echo Desde ya muchas gracias.
pause 

