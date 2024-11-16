// servidor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "sistema_archivos.h"
#include "logs.h"

#define PIPE_NAME "\\\\.\\pipe\\archivo_pipe"
#define MAX_CLIENTES 10
#define LOG_DIR "logs"

// Semáforo global para proteger el acceso a los archivos
HANDLE semaforo;

// Definir la estructura de datos para las solicitudes
typedef struct
{
    HANDLE pipe;
} ClienteInfo;

// Función que maneja las peticiones de los clientes
void manejar_peticion(void *param)
{
    ClienteInfo *info = (ClienteInfo *)param;
    HANDLE pipe = info->pipe;
    char buffer[1024];
    DWORD bytesLeidos;

    if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesLeidos, NULL))
    {
        buffer[bytesLeidos] = '\0'; // Null-terminate el buffer
        printf("Petición recibida: %s\n", buffer);

        // Parseamos la petición usando '|' como delimitador
        char *token = strtok(buffer, "|");
        if (token == NULL)
        {
            printf("Formato de petición incorrecto.\n");
            return;
        }

        int accion = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL)
        {
            printf("Falta el nombre del archivo en la petición.\n");
            return;
        }
        char nombre[100];
        strcpy(nombre, token);

        token = strtok(NULL, "|");
        if (token == NULL)
        {
            printf("Falta el contenido en la petición.\n");
            return;
        }
        char contenido[1024];
        strcpy(contenido, token);

        // Usamos el semáforo para asegurar el acceso exclusivo
        WaitForSingleObject(semaforo, INFINITE);

        // Dependiendo de la acción, llamamos a las funciones correspondientes
        Directorio dir;
        inicializar_directorio(&dir, "Documentos");
        char respuesta[256];
        switch (accion)
        {
        case 1: // Crear archivo
            crear_archivo(&dir, nombre, contenido, "Cliente");
            registrar_log("Cliente", "Crear", nombre); // Registrar log de la acción
            snprintf(respuesta, sizeof(respuesta), "Archivo '%s' creado con éxito.", nombre);
            break;
        case 2: // Leer archivo
            leer_archivo(&dir, nombre, "Cliente");
            registrar_log("Cliente", "Leer", nombre); // Registrar log de la acción
            snprintf(respuesta, sizeof(respuesta), "Archivo '%s' leído con éxito.", nombre);
            break;
        case 3: // Actualizar archivo
            actualizar_archivo(&dir, nombre, contenido, "Cliente");
            registrar_log("Cliente", "Actualizar", nombre); // Registrar log de la acción
            snprintf(respuesta, sizeof(respuesta), "Archivo '%s' actualizado con éxito.", nombre);
            break;
        case 4: // Eliminar archivo
            eliminar_archivo(&dir, nombre, "Cliente");
            registrar_log("Cliente", "Eliminar", nombre); // Registrar log de la acción
            snprintf(respuesta, sizeof(respuesta), "Archivo '%s' eliminado con éxito.", nombre);
            break;
        default:
            snprintf(respuesta, sizeof(respuesta), "Acción no válida: %d", accion);
            printf("%s\n", respuesta);
            ReleaseSemaphore(semaforo, 1, NULL);
            return;
        }

        // Liberamos el semáforo
        ReleaseSemaphore(semaforo, 1, NULL);

        // Enviar respuesta al cliente
        DWORD bytesEscritos;
        WriteFile(pipe, respuesta, strlen(respuesta), &bytesEscritos, NULL);
    }
    // Cerrar el pipe después de procesar la solicitud
    DisconnectNamedPipe(pipe);
    CloseHandle(pipe);
    free(info); // Liberar la memoria del cliente
}

// Función principal del servidor
int main()
{
    // Crear semáforo para controlar el acceso a las operaciones de archivos
    semaforo = CreateSemaphore(NULL, 1, 1, NULL);
    if (semaforo == NULL)
    {
        printf("Error al crear el semáforo.\n");
        return 1;
    }

    // Crear el pipe
    HANDLE pipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                  PIPE_UNLIMITED_INSTANCES, 512, 512, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        printf("Error al crear el pipe.\n");
        return 1;
    }

    printf("Servidor escuchando...\n");

    // Aceptar conexiones de los clientes
    while (1)
    {
        if (ConnectNamedPipe(pipe, NULL) != 0)
        {
            printf("Cliente conectado.\n");

            // Crear un nuevo hilo para manejar la solicitud del cliente
            ClienteInfo *info = (ClienteInfo *)malloc(sizeof(ClienteInfo));
            info->pipe = pipe;

            // Crear el hilo para manejar la petición del cliente
            DWORD threadId;
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)manejar_peticion, info, 0, &threadId);
        }
        else
        {
            printf("Error al conectar al cliente.\n");
        }
    }

    CloseHandle(pipe);
    CloseHandle(semaforo);
    return 0;
}
