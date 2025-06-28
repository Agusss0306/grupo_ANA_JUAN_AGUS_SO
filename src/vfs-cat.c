#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/vfs.h"

#define  MAX_FILE_SIZE (1024 * 1024)
#define FILE_TYPE_MASK 0xF000
#define FILE_TYPE_REGULAR 0x8000

int main (int argc, char *argv[]) {
	if (argc < 3){
		fprintf(stderr, "Uso: %s <imagen> <archivo1> [archivo2...]\n", argv[0]);
		return EXIT_FAILURE; 
	}
	
	const char *image_path = argv[1];
	
	for (int i = 2; i < argc; i++) {
		const char *filename = argv[i];
	
		int inode_number = dir_lookup(image_path, filename);
		if (inode_number < 0){
			fprintf(stderr, "Error: el archivo '%s' no existe. \n", filename);
			continue;
		}
		
		struct inode in;
		if (read_inode(image_path, inode_number, &in) != 0){
			fprintf(stderr, "Error: no se pudo leer el inodo del archivo '%s' .\n", filename);
			continue;
		}
		
		if ((in.mode & FILE_TYPE_MASK) != FILE_TYPE_REGULAR) {
			fprintf(stderr, "Error: '%s' no es un archivo regular .\n", filename);
			continue;		
		}

		if (in.size > MAX_FILE_SIZE) {
			fprintf(stderr, "Error: el archivo '%s' es demasiado grande. \n", filename);
			continue;
		} 
		
		char *buffer = malloc(in.size);
		if (!buffer) {
			fprintf(stderr, "Error: sin memoria. \n");
			return EXIT_FAILURE;
		}

		if (in.size == 0){
			free(buffer);
			continue;
		}
		
		if (inode_read_data(image_path, inode_number, buffer, in.size, 0) != 0) {
			fprintf(stderr, "Error: no se pudo leer el archivo '%s'.\n", filename);
			free(buffer);
			continue;
		}
		
		fwrite(buffer, 1, in.size, stdout);
		free(buffer);

	}
	
	return EXIT_SUCCESS;

}


