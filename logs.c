// logs.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h> // Para obtener el timestamp

#define LOG_DIR "logs"

// Función para registrar logs con cliente, acción y archivo
void registrar_log(const char *cliente, const char *accion, const char *nombre_archivo)
{
    // Crear directorio de logs si no existe
    if (CreateDirectory(LOG_DIR, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Obtener el tiempo actual
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        // Abrir el archivo de log para agregar nuevas entradas
        char log_path[256];
        snprintf(log_path, sizeof(log_path), "%s\\registro.log", LOG_DIR);

        FILE *logfile = fopen(log_path, "a");
        if (logfile == NULL)
        {
            printf("Error al abrir el archivo de log.\n");
            return;
        }

        // Escribir el log en el formato: [fecha y hora] Cliente: [cliente] - Acción: [accion], Archivo: [archivo]
        fprintf(logfile, "[%04d-%02d-%02d %02d:%02d:%02d] Cliente: %s - Acción: %s, Archivo: %s\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, cliente, accion, nombre_archivo);

        fclose(logfile); // Cerrar el archivo de log
    }
}
