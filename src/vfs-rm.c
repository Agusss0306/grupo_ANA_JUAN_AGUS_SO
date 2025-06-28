#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Uso: %s <imagen> <archivo>\n", argv[0]);
		return 1;
	}

	const char *image_path = argv[1];
	const char *filename = argv[2];

	// Se busca en la entrada del directorio correspondiente al archivo a eliminar.
	int inode_number = dir_lookup(image_path, filename);
	if (inode_number < 0) {
		fprintf(stderr, "Archivo '%s' no encontrado en la imagen. \n", filename);
		return 1;
	}

	// Se lee el inodo correspondiente a ese numero
	struct inode in;
	read_inode(image_path, inode_number, &in);

	// Se truncan los datos del archivo, liberando los bloques
	inode_trunc_data(image_path, &in);

	// Se escribe el inodo actualizado (vacio) en la tabla de inodos
	write_inode(image_path, inode_number, &in);

	// Se libera el numero de inodo para que quede disponible
	free_inode(image_path, inode_number);

	// Se elimina la entrada del directorio
	if (remove_dir_entry(image_path, filename) < 0) {
		fprintf(stderr, "Error al eliminar la entrada del directorio. \n");
		return 1;
	}

	printf("Archivo '%s' eliminado correctamente. \n", filename);
	return 0;
}
