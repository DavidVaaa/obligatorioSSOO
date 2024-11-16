// cliente.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h> // Para obtener el timestamp

#define PIPE_NAME "\\\\.\\pipe\\archivo_pipe"

// Función para asegurarse de que el nombre del archivo termine con ".txt"
void asegurar_extension(char *nombre_archivo)
{
    int len = strlen(nombre_archivo);
    if (len < 4 || strcmp(&nombre_archivo[len - 4], ".txt") != 0)
    {
        // Si el nombre del archivo no termina con ".txt", agregarlo
        strcat(nombre_archivo, ".txt");
    }
}

// Función para eliminar la coma al final del nombre del archivo solo si es antes de la extensión ".txt"
void eliminar_coma(char *nombre_archivo)
{
    int len = strlen(nombre_archivo);
    if (len > 4 && nombre_archivo[len - 5] == '.' && nombre_archivo[len - 4] == 't' && nombre_archivo[len - 3] == 'x' && nombre_archivo[len - 2] == 't' && nombre_archivo[len - 1] == ',')
    {
        // Eliminar la coma al final solo si es antes de la extensión ".txt"
        nombre_archivo[len - 1] = '\0';
    }
}

// Función para registrar logs en el cliente
void registrar_log_cliente(int accion, const char *nombre_archivo)
{
    // Crear directorio de logs si no existe
    if (CreateDirectory("logs", NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        // Obtener el tiempo actual
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        // Abrir el archivo de log para agregar nuevas entradas
        char log_path[256];
        snprintf(log_path, sizeof(log_path), "logs/registro_cliente.log");

        FILE *logfile = fopen(log_path, "a");
        if (logfile == NULL)
        {
            printf("Error al abrir el archivo de log.\n");
            return;
        }

        // Escribir el log en el formato: [fecha y hora] Acción: [accion], Archivo: [archivo]
        fprintf(logfile, "[%04d-%02d-%02d %02d:%02d:%02d] Acción: %d, Archivo: %s\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, accion, nombre_archivo);

        fclose(logfile); // Cerrar el archivo de log
    }
}

// Función principal del cliente
int main(int argc, char *argv[])
{
    if (argc < 3)
    { // Solo requiere acción y nombre del archivo para mínimo
        printf("Uso: %s <acción> <nombre_archivo> <contenido>\n", argv[0]);
        return 1;
    }

    int accion = atoi(argv[1]);
    char nombre_archivo[100];
    strcpy(nombre_archivo, argv[2]);

    // Revisar si hay contenido, si no, usar "N/A"
    char contenido[1024];
    if (argc > 3)
    {
        strcpy(contenido, argv[3]);
    }
    else
    {
        strcpy(contenido, "N/A"); // Usar "N/A" para lectura y eliminación
    }

    // Enviar mensaje en el formato estándar
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%d|%s|%s", accion, nombre_archivo, contenido);

    HANDLE pipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        printf("Error al conectar con el servidor.\n");
        return 1;
    }

    // Enviar datos y procesar respuesta igual que antes
    DWORD bytesEscritos;
    if (!WriteFile(pipe, buffer, strlen(buffer), &bytesEscritos, NULL))
    {
        printf("Error al escribir en el pipe.\n");
        return 1;
    }

    char respuesta[512];
    DWORD bytesLeidos;
    if (!ReadFile(pipe, respuesta, sizeof(respuesta) - 1, &bytesLeidos, NULL))
    {
        printf("Error al leer del pipe.\n");
        return 1;
    }
    respuesta[bytesLeidos] = '\0';
    printf("Respuesta del servidor: %s\n", respuesta);

    CloseHandle(pipe);

    return 0;
}
