#include "loader.h"

void load_game (char * filename, Game *game) {
	FILE *file;
	int i,j;
	Nivel *niveles;
	Bloque *bloques;
	
	// open file
	if(!(file = fopen(filename,"r"))) {
		perror("Error abriendo el archivo");
		exit(-1);
	}
	printf("Archivo abierto\n");

	// load game settings
	fscanf(file,"%d\n",&game->vidas);
	fscanf(file,"%d\n",&game->tiempo_enfriamiento);
	fscanf(file,"%d\n",&game->tiempo_de_salto);
	fscanf(file,"%d\n",&game->numero_de_niveles);
	
	niveles = (Nivel *) malloc(sizeof(Nivel) * game->numero_de_niveles);
	for (i = 0; i < game->numero_de_niveles; i++) {
		// load level settings
		fscanf(file,"%s\n",niveles[i].nombre);
		fscanf(file,"%d %f\n",&niveles[i].numero_impactos,&niveles[i].incremento_velocidad);
		fscanf(file,"%d\n",&niveles[i].numero_de_bloques);

		// load level blocks
		bloques = (Bloque *) malloc(sizeof(Bloque) * niveles[i].numero_de_bloques);
		for( j = 0; j <= niveles[i].numero_de_bloques; j++) {
			fscanf(file,"%f %f %c\n",&bloques[j].fila,&bloques[j].columna,&bloques[j].color);
		}
		niveles[i].bloques = bloques;

	}

	game->niveles = niveles;
	
	// close file
	if(fclose(file)) {
		printf("Error al cerrar el archivo\n");
		exit(-1);
	}
	printf("Archivo Cerrado\n");
}