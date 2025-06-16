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
    int block_nbr = get_block_number_at(image_path, &root_inode, 0);
    if (block_nbr < 0 || read_block(image_path, block_nbr, buffer) != 0) {
        fprintf(stderr, "Error al leer el bloque de datos del directorio raíz.\n");
        return EXIT_FAILURE;
    }

    struct dir_entry *entries = (struct dir_entry *)buffer;
    size_t max_entries = BLOCK_SIZE / sizeof(struct dir_entry);

    for (size_t i = 0; i < max_entries; i++) {
        if (entries[i].inode == 0)
            continue;

        struct inode file_inode;
        if (read_inode(image_path, entries[i].inode, &file_inode) != 0) {
            fprintf(stderr, "No se pudo leer el inodo %u\n", entries[i].inode);
            continue;
        }

        print_inode(&file_inode, entries[i].inode, entries[i].name);
    }

    return EXIT_SUCCESS;
}

