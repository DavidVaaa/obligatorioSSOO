#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>             // Para obtener el timestamp
#include "sistema_archivos.c" // Incluimos el archivo que contiene las funciones del sistema de archivos

// Función para registrar logs con cliente y timestamp
void registrar_log(const char *cliente, const char *accion, const char *nombre_archivo)
{
    time_t t = time(NULL);         // Obtener el tiempo actual
    struct tm tm = *localtime(&t); // Convertir el tiempo a formato local

    // Abrir el archivo de log para agregar nuevas entradas
    FILE *logfile = fopen("logs/registro.log", "a");
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

// Función para mostrar el menú de opciones
void mostrar_menu()
{
    printf("\nOpciones disponibles:\n");
    printf("1. Crear archivo\n");
    printf("2. Leer archivo\n");
    printf("3. Actualizar archivo\n");
    printf("4. Eliminar archivo\n");
    printf("5. Listar archivos\n");
    printf("6. Salir\n");
    printf("Elige una opción: ");
}

// Función para obtener una cadena de texto de forma segura
void obtener_input(char *buffer, int size)
{
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; // Eliminar el salto de línea al final
}

int main()
{
    // Crear la carpeta y el directorio
    crear_carpeta(CARPETA_ARCHIVOS);
    Directorio mi_directorio;
    inicializar_directorio(&mi_directorio, "Documentos");

    int opcion;
    char nombre[100], contenido[1024];
    int permisos;
    char cliente[50]; // Nombre del cliente

    printf("Introduce tu nombre (cliente): ");
    obtener_input(cliente, sizeof(cliente)); // Obtener nombre del cliente

    do
    {
        mostrar_menu();
        scanf("%d", &opcion);
        getchar(); // Limpiar el salto de línea después de leer la opción

        switch (opcion)
        {
        case 1: // Crear archivo
            printf("Introduce el nombre del archivo: ");
            obtener_input(nombre, sizeof(nombre));
            printf("Introduce el contenido del archivo: ");
            obtener_input(contenido, sizeof(contenido));
            printf("Introduce los permisos (0 = solo lectura, 1 = lectura/escritura): ");
            getchar(); // Limpiar el salto de línea
            crear_archivo(&mi_directorio, nombre, contenido);
            registrar_log(cliente, "Crear", nombre); // Registrar log de la acción
            break;

        case 2: // Leer archivo
            printf("Introduce el nombre del archivo a leer: ");
            obtener_input(nombre, sizeof(nombre));
            leer_archivo(&mi_directorio, nombre);
            registrar_log(cliente, "Leer", nombre); // Registrar log de la acción
            break;

        case 3: // Actualizar archivo
            printf("Introduce el nombre del archivo a actualizar: ");
            obtener_input(nombre, sizeof(nombre));
            printf("Introduce el nuevo contenido del archivo: ");
            obtener_input(contenido, sizeof(contenido));
            actualizar_archivo(&mi_directorio, nombre, contenido);
            registrar_log(cliente, "Actualizar", nombre); // Registrar log de la acción
            break;

        case 4: // Eliminar archivo
            printf("Introduce el nombre del archivo a eliminar: ");
            obtener_input(nombre, sizeof(nombre));
            eliminar_archivo(&mi_directorio, nombre);
            registrar_log(cliente, "Eliminar", nombre); // Registrar log de la acción
            break;

        case 5: // Listar archivos
            listar_archivos(&mi_directorio);
            break;

        case 6: // Salir
            printf("Saliendo...\n");
            break;

        default:
            printf("Opción no válida. Intenta nuevamente.\n");
        }
    } while (opcion != 6);

    return 0;
}
