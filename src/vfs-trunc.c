// src/vfs-trunc.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/vfs.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <imagen> <archivo1> [archivo2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];

        int inode_number = dir_lookup(image_path, filename);
        if (inode_number < 0) {
            fprintf(stderr, "Archivo '%s' no encontrado.\n", filename);
            continue;
        }

        struct inode in;
        if (read_inode(image_path, inode_number, &in) != 0) {
            fprintf(stderr, "Error al leer el inodo de '%s'.\n", filename);
            continue;
        }

        // Verificar que sea un archivo regular
        if ((in.mode & FILE_TYPE_MASK) != FILE_TYPE_REGULAR) {
            fprintf(stderr, "'%s' no es un archivo regular, no puede truncarse.\n", filename);
            continue;
        }

        if (inode_trunc_data(image_path, &in) != 0) {
            fprintf(stderr, "Error al truncar los datos de '%s'.\n", filename);
            continue;
        }

        // Reiniciar tamaño y bloques asignados
        in.size = 0;
        in.blocks_assigned = 0;

        if (write_inode(image_path, inode_number, &in) != 0) {
            fprintf(stderr, "Error al escribir el inodo truncado de '%s'.\n", filename);
            continue;
        }

        printf("Archivo '%s' truncado con éxito.\n", filename);
    }

    return EXIT_SUCCESS;
}
