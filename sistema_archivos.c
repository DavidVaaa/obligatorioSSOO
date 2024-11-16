// sistema_archivos.c

#include "sistema_archivos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "logs.h"

#define CARPETA_ARCHIVOS "archivos" // Directorio de archivos en disco

// Inicializa un directorio
void inicializar_directorio(Directorio *dir, const char *nombre)
{
    strcpy(dir->nombre, nombre);     // Copia el nombre del directorio
    dir->num_archivos = 0;           // Inicializa el contador de archivos a 0
    crear_carpeta(CARPETA_ARCHIVOS); // Crear la carpeta si no existe
}

// Crea la carpeta si no existe
void crear_carpeta(const char *nombre_carpeta)
{
    if (CreateDirectory(nombre_carpeta, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        printf("Carpeta '%s' creada o ya existía.\n", nombre_carpeta); // Mensaje de éxito
    }
    else
    {
        printf("Error al crear la carpeta '%s'.\n", nombre_carpeta); // Mensaje de error
    }
}

// Crea un archivo físico en la carpeta
void crear_archivo_fisico(const char *nombre, const char *contenido)
{
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "%s\\%s.txt", CARPETA_ARCHIVOS, nombre); // Ruta completa con extensión .txt

    FILE *archivo = fopen(ruta, "w"); // Abre el archivo en modo escritura
    if (archivo == NULL)
    {
        printf("Error al crear el archivo '%s'.\n", ruta);
        return;
    }
    fprintf(archivo, "%s", contenido); // Escribe el contenido en el archivo
    fclose(archivo);                   // Cierra el archivo
    printf("Archivo físico '%s' creado.\n", ruta);
}

// Crea un archivo en memoria y en disco
void crear_archivo(Directorio *dir, const char *nombre, const char *contenido, const char *cliente)
{
    if (dir->num_archivos < MAX_ARCHIVOS)
    {
        Archivo *archivo = &dir->archivos[dir->num_archivos++]; // Accede al siguiente archivo disponible
        strcpy(archivo->nombre, nombre);
        strcpy(archivo->contenido, contenido);
        archivo->tamanio = strlen(contenido);

        printf("Archivo '%s' creado en memoria.\n", nombre);
        crear_archivo_fisico(nombre, contenido); // Crea el archivo en disco

        // Registrar la creación del archivo en los logs
        registrar_log(cliente, "Crear", nombre);
    }
    else
    {
        printf("Directorio lleno. No se pueden crear más archivos.\n");
    }
}

// Listar los archivos en el directorio
void listar_archivos(Directorio *dir)
{
    printf("Archivos en el directorio '%s':\n", dir->nombre);
    for (int i = 0; i < dir->num_archivos; i++)
    {
        printf("- %s (Tamaño: %d bytes)\n", dir->archivos[i].nombre, dir->archivos[i].tamanio);
    }
}

// Leer el contenido de un archivo
int leer_archivo(Directorio *dir, const char *nombre, const char *cliente)
{
    // Verificar archivo sin la extensión .txt
    FILE *archivo = fopen(nombre, "r");
    if (archivo == NULL)
    {
        // Si no se encontró, probar con la extensión .txt
        char ruta[256];
        snprintf(ruta, sizeof(ruta), "%s\\%s.txt", CARPETA_ARCHIVOS, nombre); // Agregar .txt si no tiene extensión

        archivo = fopen(ruta, "r");
        if (archivo == NULL)
        {
            printf("No se encontró el archivo %s\n", ruta);
            return 1; // Indica error si el archivo no se encuentra
        }
    }

    char contenido[1024];
    printf("Contenido del archivo '%s':\n", nombre);
    while (fgets(contenido, sizeof(contenido), archivo) != NULL)
    {
        printf("%s", contenido); // Muestra el contenido del archivo
    }

    fclose(archivo);

    // Registrar la lectura del archivo en los logs
    registrar_log(cliente, "Leer", nombre);

    return 0; // Indica éxito si la operación fue exitosa
}

// Actualizar el contenido de un archivo
void actualizar_archivo(Directorio *dir, const char *nombre, const char *nuevo_contenido, const char *cliente)
{
    for (int i = 0; i < dir->num_archivos; i++)
    {
        if (strcmp(dir->archivos[i].nombre, nombre) == 0)
        {
            strcpy(dir->archivos[i].contenido, nuevo_contenido); // Actualiza el contenido en memoria
            dir->archivos[i].tamanio = strlen(nuevo_contenido);  // Actualiza el tamaño

            // Actualiza el archivo físico en disco
            char ruta[256];
            snprintf(ruta, sizeof(ruta), "%s\\%s.txt", CARPETA_ARCHIVOS, nombre); // Ruta completa con extensión .txt
            FILE *archivo = fopen(ruta, "w");
            if (archivo != NULL)
            {
                fprintf(archivo, "%s", nuevo_contenido);
                fclose(archivo);
                printf("Archivo '%s' actualizado en disco.\n", ruta);
            }
            else
            {
                printf("Error al actualizar el archivo '%s'.\n", ruta);
            }

            // Registrar la actualización del archivo en los logs
            registrar_log(cliente, "Actualizar", nombre);
            return;
        }
    }
    printf("Archivo '%s' no encontrado.\n", nombre);
}

// Eliminar un archivo (en memoria y en disco)
void eliminar_archivo(Directorio *dir, const char *nombre, const char *cliente)
{
    for (int i = 0; i < dir->num_archivos; i++)
    {
        if (strcmp(dir->archivos[i].nombre, nombre) == 0)
        {
            // Generar la ruta completa del archivo
            char ruta[256];
            snprintf(ruta, sizeof(ruta), "%s\\%s.txt", CARPETA_ARCHIVOS, nombre);

            // Eliminar archivo físico
            if (remove(ruta) == 0)
            {
                printf("Archivo físico '%s' eliminado.\n", ruta);
            }
            else
            {
                printf("Error al eliminar el archivo '%s'. Código de error: %d\n", ruta, GetLastError());
            }

            // Eliminar archivo de la memoria (mover todos los archivos hacia la izquierda)
            for (int j = i; j < dir->num_archivos - 1; j++)
            {
                dir->archivos[j] = dir->archivos[j + 1];
            }
            dir->num_archivos--; // Reducir el contador de archivos

            // Registrar la eliminación del archivo en los logs
            registrar_log(cliente, "Eliminar", nombre);
            return;
        }
    }
    printf("Archivo '%s' no encontrado.\n", nombre);
}
