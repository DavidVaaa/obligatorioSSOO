@echo off
echo Iniciando ejecución de clientes concurrentes...

REM --- Operaciones CRUD en varios archivos simultáneamente ---

REM Operaciones en archivo1
start /b cliente.exe 1 archivo1 "Contenido para archivo1"   REM Crear archivo1
start /b cliente.exe 1 archivo2 "Contenido para archivo2"   REM Crear archivo2
start /b cliente.exe 2 archivo1                             REM Leer archivo1
start /b cliente.exe 4 archivo2                             REM Eliminar archivo2
start /b cliente.exe 3 archivo1 "Nuevo contenido archivo1"  REM Actualizar archivo1
start /b cliente.exe 4 archivo1                             REM Eliminar archivo1

REM Operaciones en archivo2
start /b cliente.exe 1 archivo2 "Contenido para archivo2"   REM Crear archivo2
start /b cliente.exe 2 archivo2                             REM Leer archivo2
start /b cliente.exe 3 archivo2 "Nuevo contenido archivo2"  REM Actualizar archivo2
start /b cliente.exe 4 archivo2                             REM Eliminar archivo2

REM Operaciones en archivo3
start /b cliente.exe 1 archivo3 "Contenido para archivo3"   REM Crear archivo3
start /b cliente.exe 2 archivo3                             REM Leer archivo3
start /b cliente.exe 3 archivo3 "Nuevo contenido archivo3"  REM Actualizar archivo3
start /b cliente.exe 4 archivo3                             REM Eliminar archivo3

REM Operaciones en archivo4
start /b cliente.exe 1 archivo4 "Contenido para archivo4"   REM Crear archivo4
start /b cliente.exe 3 archivo4 "Nuevo contenido archivo4"  REM Actualizar archivo4

REM Operaciones en archivo5
start /b cliente.exe 1 archivo4 "Contenido para archivo5"   REM Crear archivo4
start /b cliente.exe 3 archivo4 "Nuevo contenido archivo5"  REM Actualizar archivo4


echo -------------------------------------------------
echo Todas las operaciones han sido ejecutadas.
echo -------------------------------------------------
pause
