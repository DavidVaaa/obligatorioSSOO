#ifndef SISTEMA_ARCHIVOS_H
#define SISTEMA_ARCHIVOS_H

#define MAX_ARCHIVOS 100
#define CARPETA_ARCHIVOS "archivos" // Directorio de archivos en disco

typedef struct
{
    char nombre[50];
    char contenido[256];
    int tamanio;
} Archivo;

typedef struct
{
    char nombre[50];
    Archivo archivos[MAX_ARCHIVOS];
    int num_archivos;
} Directorio;

// Declaraciones de funciones
void inicializar_directorio(Directorio *dir, const char *nombre);
void crear_archivo(Directorio *dir, const char *nombre, const char *contenido, const char *cliente);
void listar_archivos(Directorio *dir);
int leer_archivo(Directorio *dir, const char *nombre, const char *cliente);
void actualizar_archivo(Directorio *dir, const char *nombre, const char *nuevo_contenido, const char *cliente);
void eliminar_archivo(Directorio *dir, const char *nombre, const char *cliente);
void crear_carpeta(const char *nombre_carpeta);

#endif
