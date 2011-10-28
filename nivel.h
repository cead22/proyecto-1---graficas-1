typedef struct {
	float fila;
	float columna;
	char color;
	struct Bloque *bloque_siguiente;
} Bloque;

typedef struct {
	char nombre[21];
	int numero_impactos;
	float incremento_velocidad;
	int numero_de_bloques;
	Bloque *bloques;
	struct Nivel *nivel_siguiente;
} Nivel;

typedef struct {
	int vidas;
	int tiempo_enfriamiento;
	int tiempo_de_salto;
	int numero_de_niveles;
	Nivel *niveles;
} Game;