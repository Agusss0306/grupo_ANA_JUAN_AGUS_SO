// src/vfs-ls.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../include/vfs.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <imagen>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    struct superblock sb;
    if (read_superblock(image_path, &sb) != 0) {
        fprintf(stderr, "Error al leer el superbloque.\n");
        return EXIT_FAILURE;
    }

    struct inode root_inode;
    if (read_inode(image_path, 1, &root_inode) != 0) {
        fprintf(stderr, "Error al leer el inodo del directorio raíz.\n");
        return EXIT_FAILURE;
    }

    uint8_t buffer[BLOCK_SIZE];
    if (read_block(image_path, root_inode.blocks[0], buffer) != 0) {
        fprintf(stderr, "Error al leer el bloque de datos del directorio raíz.\n");
        return EXIT_FAILURE;
    }

    struct dir_entry *entries = (struct dir_entry *)buffer;
    size_t max_entries = BLOCK_SIZE / sizeof(struct dir_entry);

    for (size_t i = 0; i < max_entries; i++) {
        if (entries[i].inode_number == 0)
            continue; // Entrada vacía

        struct inode file_inode;
        if (read_inode(image_path, entries[i].inode_number, &file_inode) != 0) {
            fprintf(stderr, "No se pudo leer el inodo %u\n", entries[i].inode_number);
            continue;
        }

        print_inode(&file_inode, entries[i].inode_number, entries[i].filename);
    }

    return EXIT_SUCCESS;
}
