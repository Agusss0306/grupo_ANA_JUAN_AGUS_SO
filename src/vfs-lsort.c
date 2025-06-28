#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

#define MAX_ENTRIES (BLOCK_SIZE / sizeof(struct dir_entry))

// Comparador usado por qsort para ordenar alfabeticamente los nombres
int compare_entries(const void *a, const void *b) {
	const struct dir_entry *e1 = (const struct dir_entry *)a;
	const struct dir_entry *e2 = (const struct dir_entry *)b;
	return strcmp(e1->name, e2->name); //de forma que el orden es ascendente
}

int main(int argc, char *argv[]) {
	// se verifica que se haya pasado el argumento de la imagen
	if (argc != 2) {
		fprintf(stderr, "Uso: %s <imagen>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *image_path = argv[1];

	// se lee el i-node del directorio raiz
	struct inode root_inode;
	if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
		fprintf(stderr, "Error al leer el inodo raiz\n");
		return EXIT_FAILURE;
	}

	// se lee el bloque de datos del directorio
	uint8_t buffer[BLOCK_SIZE] = {0};
	int dir_block = root_inode.direct[0];
	if (read_block(image_path, dir_block, buffer) != 0) {
		fprintf(stderr, "Error al leer el bloque de datos del directorio\n");
		return EXIT_FAILURE;
	}

	// se interpreta ese bloque como una serie de entradas de directorio
	struct dir_entry *entries = (struct dir_entry *)buffer;
	//se copian las entradas validas a este array auxiliar para ordenarlas
	struct dir_entry valid_entries[MAX_ENTRIES];
	int count = 0;

	// se filtran entradas validas (inode != 0 y no son "." o "..")
	for (int i=0; i < MAX_ENTRIES; ++i) {
		if (entries[i].inode != 0 && strcmp(entries[i].name, ".") != 0 && strcmp(entries[i].name, "..") != 0) {
			// se copia la entrada valida al array
			valid_entries[count++] = entries[i];
		}
	}

	// se ordenan las entradas alfabeticamente por nombre
	qsort(valid_entries, count, sizeof(struct dir_entry), compare_entries);

	// se muestra la info de cada entrada ordenada

	for (int i = 0; i < count; ++i) {
		struct inode in;
		// se lee el inodo correspondiente a la entrada
		if (read_inode(image_path, valid_entries[i].inode, &in) == 0) {
			// se imprime el inodo con nombre formateado 
			print_inode(&in, valid_entries[i].inode, valid_entries[i].name);
		}
	}

	return EXIT_SUCCESS;
}
