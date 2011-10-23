#include "loader.h"

// void print_nivel(Nivel nivel) {
// 	int i, j;
// 	
// 	printf("vidas: %d\n", nivel.vidas);
// 	printf("tiempo de enfriamiento: %d\n", nivel.tiempo_enfriamiento);
// 	printf("tiempo de salto: %d\n", nivel.tiempo_de_salto);
// 	printf("numero de niveles: %d\n", nivel.numero_de_niveles);
// 	printf("nivel: %d\n", nivel.nivel);
// 	printf("numero impactos: %d\n", nivel.numero_impactos);
// 	printf("incremento velocidad: %d\n", nivel.incremento_velocidad);
// 	printf("numero de bloques: %d\n", nivel.numero_de_bloques);
// 	
// 	for(i = 0; i < nivel.numero_de_bloques*3; i+=3) {
// 		printf("%d %d %d\n", nivel.bloques[i], nivel.bloques[i+1], nivel.bloques[i+2]);
// 	}
// 
// }
// 
Nivel cargar_nivel (char * filename) {
	FILE *file;
	int i = 0;
	int *bloques;
	Nivel nivel;
	
	// open file
	if(!(file = fopen(filename,"r"))) {
		perror("Error abriendo el archivo");
		exit(-1);
	}
	printf("Archivo abierto\n");
	
	// load nivel
	fscanf(file,"%d\n",&nivel.vidas);
	fscanf(file,"%d\n",&nivel.tiempo_enfriamiento);
	fscanf(file,"%d\n",&nivel.tiempo_de_salto);
	fscanf(file,"%d\n",&nivel.numero_de_niveles);
	fscanf(file,"%d\n",&nivel.nivel);
	fscanf(file,"%d %d\n",&nivel.numero_impactos,&nivel.incremento_velocidad);
	fscanf(file,"%d\n",&nivel.numero_de_bloques);
	
	nivel.bloques = malloc(sizeof(bloque)*nivel.numero_de_bloques);
	for( i = 0; i <= nivel.numero_de_bloques; i++) {
		fscanf(file,"%d %d %c\n",&nivel.bloques[i].fila,&nivel.bloques[i].columna,&nivel.bloques[i].color);
	}

	// close file
	if(fclose(file)) {
		printf("Error al cerrar el archivo\n");
		exit(-1);
	}
	printf("Archivo Cerrado\n");
	return nivel;
}