#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/vfs.h"


int main (int argc, char *argv[]) {
	if (argc < 3){
		fprintf(stderr, "Uso: %s imagen archivo1 [archivo2 ...]\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *image_path = argv[1];  // Var con ruta de la imagen
	
	for (int i = 2; i < argc; i++){
		const char *filename = argv[i];

		if (!name_is_valid(filename)) {
			fprintf(stderr, "Error: el nombre '%s' no es válido.\n", filename);
			continue;
		}
		
		if (dir_lookup(image_path, filename) <= 0) {
			fprintf(stderr, "Error: el archivo '%s' ya existe en el directorio.\n", filename);
			continue;
		}

		int inode_number = create_empty_file_in_free_inode(image_path, 0644); // permisos rw-r--r
		if (inode_number < 0){
			fprintf(stderr, "Error: no se pudo crear el archivo '%s' (no hay inodos libres).\n", filename);
			continue;
		}

		if (add_dir_entry (image_path, filename, inode_number) < 0) {
			fprintf(stderr, "Error: no se pudo agregar '%s' al directorio.\n", filename);
			free_inode(image_path, inode_number);  // si no puede crear el archivo libero el inodo
			continue;
		}
		
		printf("Archivo '%s' creado con éxito (inodo %d).\n", filename, inode_number);
	}
	
	return EXIT_SUCCESS; 

	
}

