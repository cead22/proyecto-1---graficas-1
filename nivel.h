typedef struct {
	float fila;
	float columna;
	char color;
} bloque;
typedef struct {
	int vidas;
	int tiempo_enfriamiento;
	int tiempo_de_salto;
	int numero_de_niveles;
	int nivel;
	int numero_impactos;
	int incremento_velocidad;
	int numero_de_bloques;
	bloque *bloques;
} Nivel;