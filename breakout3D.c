#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#endif

#include "loader.h"

#define WIDTH_SCALE 5.0
#define HEIGHT_SCALE 2.0
#define DEPTH_SCALE 2.0
#define BALL_RADIUS 0.5
#define BLOCK_SIZE 1.0
#define MIN_X 0.0
#define MIN_Y  0.0
#define Z 2.0
#define MAX_X  50.0
#define MAX_Y  80.0
#define MAX_Z  5.0
#define ESCAPE 27
#define SPACEBAR 32

// timer variables
int star_time;
int previous_time;
int timer = 10;

// game
Game *game;
Nivel *current_level;
int level_index = 0;
int lives = 10;
int score = 0;
int alive = 0;
int status = 0; // lost,won,playing = -1,0,1 resp
int playing;
int ceiling_hits = 0;
int next_jump;

// ball
float ball_y = 0.0;
float ball_x = 25.0;
float ball_direction_y = 1.0;
float ball_direction_x;
float ball_speed_x = 0.0;
float ball_speed_y = 0.0;

// directional bar
float bar_x = 25.0;

// blocks data
Bloque *block_list;
int *remaining_hits;
int *points;
int collision_block = -1;
int blocks_left;

// light
void light_config() { 
	GLfloat light_diffuse[] = {.5, .5, .5, 1.0};
	GLfloat light_ambient[] = {.5, .5, .5, 1.0};
	GLfloat light_specular[] = {.7, .7, .7, 1.0};
	GLfloat light_position[] = {-10.0, 10, 50, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

// draw individual bricks
void draw_brick (Bloque block) {
	float *rgba;
	float yellow[4] = {1.0,1.0,0.0,1.0};
	float orange[4] = {1.0,0.5,0.0,1.0};
	float red[4] = {1.0,0.0,0.0,1.0};
	float green[4] = {0.0,1.0,0.0,1.0};
	float gray[4] = {0.7,0.7,0.7,1.0};
	float black[4] = {0.0,0.0,0.0,1.0};
	// set brick color
	switch (block.color) {
		case 'A':
		rgba = yellow;
		break;
		case 'N':
		rgba = orange;
		break;
		case 'R':
		rgba = red;
		break;
		case 'V':
		rgba = green;
		break;
		case 'G':
		rgba = gray;
		break;
		default: break;
	}
	// draw the brick
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, rgba);
		glMaterialfv(GL_FRONT, GL_AMBIENT, rgba);
		glTranslatef(block.fila,block.columna,0.0);
		glScalef(WIDTH_SCALE,HEIGHT_SCALE,DEPTH_SCALE);
		glutSolidCube(BLOCK_SIZE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
		glMaterialfv(GL_FRONT, GL_AMBIENT, black);
		glutWireCube(BLOCK_SIZE);
	glPopMatrix();
}             

// free memory allocated for game data
void free_memory() {
	free(block_list);
	free(remaining_hits);
	free(points);
}
// push block down 1 row
void lower_blocks () {
	int i;
	if (playing) {
		for(i = 0; i < current_level->numero_de_bloques; ++i) {
			block_list[i].columna -= HEIGHT_SCALE;
			if(block_list[i].columna == -1) {
				// player lost
				status = -1;
				return;
			}
		}
	}
}

// load bricks and store game data
void load_bricks () {
	int i = 0;
	
	// set current level
	if(status == 1) {
		// player has won
		return;
	}
	
	ceiling_hits = 0;
	current_level = game->niveles + level_index;
	
	// allocate memory for game data
	block_list = (Bloque *) malloc(sizeof(Bloque) * current_level->numero_de_bloques);
	remaining_hits = (int *) malloc(sizeof(int) *   current_level->numero_de_bloques);
	points = (int *) malloc(sizeof(int) * current_level->numero_de_bloques);

	// clean memory space
	for(i = 0; i < current_level->numero_de_bloques; i++) {
		block_list[i].fila = 0;
		block_list[i].columna = 0;
		block_list[i].color = 0;
		remaining_hits[i] = 0;
		points[i] = 0;
	}
	
	// store game data
	for(i = 0; i < current_level->numero_de_bloques; i++) {
		block_list[i].fila = (float) current_level->bloques[i].fila * WIDTH_SCALE + 2.5;
		block_list[i].columna = ((float) current_level->bloques[i].columna + 10 + 0.5) * HEIGHT_SCALE;
		block_list[i].color = (float) current_level->bloques[i].color;
		switch (current_level->bloques[i].color) {
			case 'A':
				remaining_hits[i] = 1;
				points[i] = 10;
				blocks_left += 1;
				break;
			case 'N':
				remaining_hits[i] = 3;
				points[i] = 30;
				blocks_left += 3;
				break;
			case 'R':
				remaining_hits[i] = 5;
				points[i] = 50;
				blocks_left += 5;
				break;
			case 'V':
				remaining_hits[i] = 1;
				points[i] = 25;
				blocks_left += 1;
				break;
			case 'G':
				remaining_hits[i] = -1;
				points[i] = 0;
				break;
			default: break;
		}
	}
}

// draw individual bricks
void draw_bricks () {
	int i = 0;
	// brick still in the game
	for(i; i < current_level->numero_de_bloques; i++) {
		if (remaining_hits[i]) {
			draw_brick(block_list[i]);
		}
	}
}

// draw directional bar
void draw_bar () {
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,1.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,1.0,1.0});
		glTranslatef(bar_x,-1.0,0.0);
		glScalef(2*WIDTH_SCALE,HEIGHT_SCALE/2,DEPTH_SCALE);
		glutSolidCube(1);
	glPopMatrix();
	
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.95,0.95,0.95,1.0});
		glTranslatef(bar_x,-1.0,0.0);
		glBegin(GL_QUADS);
		glVertex3f( + 2.0, -0.5,  1.01);
		glVertex3f( + 2.0,  0.5,  1.01);
		glVertex3f( - 2.0,  0.5,  1.01);
		glVertex3f( - 2.0, -0.5,  1.01);
		glEnd();
	glPopMatrix();
}

void draw_ball() {
	if(!alive) {
		ball_x = bar_x;	
		ball_y = 0;
	}
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,1.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,1.0,0.0,1.0});
		glTranslatef(ball_x,ball_y,0.0);
		glutSolidSphere(BALL_RADIUS,20,20);
	glPopMatrix();
}

void draw_bounds() {
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,0.0,1.0});
		glLineWidth(2.0);
		glTranslatef(0.0f,0.0f,-Z);
		glBegin(GL_LINES);
			//ceiling
			glVertex3f(MIN_X,MAX_Y,0.0);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,0.0);
			glVertex3f(MAX_X,MAX_Y,0.0);
			glVertex3f(MIN_X,MAX_Y,0.0);
			// walls
			glVertex3f(MIN_X,MIN_Y,0.0);
			glVertex3f(MIN_X,MIN_Y,2*Z);
			glVertex3f(MIN_X,MIN_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,0.0);			
			glVertex3f(MIN_X,MAX_Y,0.0);
			glVertex3f(MIN_X,MIN_Y,0.0);
			
			glVertex3f(MAX_X,MIN_Y,0.0);
			glVertex3f(MAX_X,MIN_Y,2*Z);
			glVertex3f(MAX_X,MIN_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,0.0);
			glVertex3f(MAX_X,MAX_Y,0.0);
			glVertex3f(MAX_X,MIN_Y,0.0);
		glEnd();
	glPopMatrix();
	glPushMatrix();		
		glTranslatef(0.0f,0.0f,-Z);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.95,0.95,0.95,1.0});
		glBegin(GL_QUADS);
			// ceiling
			glVertex3f(MIN_X,MAX_Y,0.0);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,0.0);

			// walls
			glVertex3f(MIN_X,MIN_Y,0.0);
			glVertex3f(MIN_X,MIN_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,2*Z);
			glVertex3f(MIN_X,MAX_Y,0.0);
			glVertex3f(MAX_X,MIN_Y,0.0);
			glVertex3f(MAX_X,MIN_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,2*Z);
			glVertex3f(MAX_X,MAX_Y,0.0);
		glEnd();
	glPopMatrix();
}

void draw_string(GLuint x, GLuint y, char* s, int *format) {
    glRasterPos2i(x, y);
    for (; *s; s++) {
        glutBitmapCharacter(format, *s);
	}
}

void draw_game_info() {
	char str[60];
	
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float[]){0.0,0.0,0.0,1.0});
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float[]){0.0,0.0,0.0,1.0});
	
	// level name
	if (status == 0) {
		draw_string(25, MAX_Y + 8, current_level->nombre, (int *) GLUT_BITMAP_HELVETICA_18);
	}
	else if (status == -1) {
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float[]){1.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float[]){1.0,0.0,0.0,1.0});
		draw_string(25, MAX_Y + 8, "HA PERDIDO", (int *) GLUT_BITMAP_HELVETICA_18);
		glPopMatrix();
	}
	else {
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float[]){0.0,1.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float[]){0.0,1.0,0.0,1.0});
		draw_string(25, MAX_Y + 8, "HA GANADO", (int *) GLUT_BITMAP_HELVETICA_18);
		glPopMatrix();
	}
	// score
	draw_string(-20, MAX_Y + 8, "Puntuacion: ", (int *)  GLUT_BITMAP_HELVETICA_12);
	sprintf(str, "%d", score);
	draw_string(-19, MAX_Y + 2, str, (int *)  GLUT_BITMAP_HELVETICA_18);

	// lives
	draw_string(-17, MAX_Y - 4, "Vidas: ", (int *) GLUT_BITMAP_HELVETICA_12);
	sprintf(str, "%d", lives);
	draw_string(-16, MAX_Y - 10, str, (int *) GLUT_BITMAP_HELVETICA_18);
	glPopMatrix();
	
}

void display () {
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);				
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw scene elements
	if(status == 0) {
		draw_bounds();
		draw_bricks();
		draw_bar();
		draw_ball();
		draw_game_info();
	}
	else{
		draw_bounds();
		draw_game_info();
	}

	// paint
	glutSwapBuffers();
}

void reshape (int width, int height) {
	// viewport transf.
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

	// project. matrix
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluPerspective(40, (GLfloat)width / (GLfloat)height, 60.0, 130.0); 
	
	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(25.0,-30.0,70.0,25.0,30.0,0.0,0.0,1.0,0.0);
}

// void reset_game() {
// 	alive = 0;
// 	score = 0;
// 	lives = 3;
// 	ball_x = 25;
// 	ball_y = 0;
// 	ball_speed_x = 0;
// 	ball_speed_y = 0;
// 	blocks_left = 0;
// 	level_index = 0;
// }

static void play(int value) {
	int i = 0, j = 0;
	float 	block_left_bound,
	 		block_right_bound,
	 		block_top_bound,
			block_bottom_bound;
	Bloque block;

	
	if (!lives) {
		playing = 0;
		status = -1;
		glutPostRedisplay();
		return;
	}
	
	
	if (!alive) {
		ball_speed_x = 0;
		ball_speed_y = 0;
	}
	
	ball_speed_x = ball_speed_x > 1.0 ? 1.0 : ball_speed_x;
	ball_speed_y = ball_speed_y > 1.0 ? 1.0 : ball_speed_y;
	
	if (!blocks_left) {
		level_index++;
		next_jump = 99999999;
		if(level_index >= game->numero_de_niveles) {
			// player has won
			status = 1;
			playing = 0;
			return;
		}
		ball_x = 25;
		ball_y = 0;

		alive = 0;
		ball_speed_x = 0;
		ball_speed_y = 0;
		free_memory();
		load_bricks();
	}
	if (glutGet(GLUT_ELAPSED_TIME) > next_jump) {
		lower_blocks();
		next_jump += game->tiempo_de_salto;
	}
	
	// Set up the next timer tick (do this first)
    glutTimerFunc(timer, play, 0);

	// Measure the elapsed time
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int time_since_previous_frame = current_time - previous_time;
	int elapsed_time = current_time - star_time;

	ball_y += ball_direction_y * (ball_speed_y);
	ball_x += ball_direction_x * (ball_speed_x);

	// collision w/ bar
	if (	collision_block != -1 && ball_y <= 0.5 && ball_direction_y < 0 && ball_y>-0.2
		&& 	ball_x >= bar_x - WIDTH_SCALE && ball_x <= bar_x + WIDTH_SCALE && playing) {
			
		// collision w/blue part. 0.05555 = 5 degrees aprox.
		if (ball_x <= bar_x - 2.0 || ball_x >= bar_x + 2.0) {
			if (ball_direction_x > 0) {
				// left side
				if (ball_x < bar_x - 2.0) {
					ball_direction_x -= 0.05555;
				}
				// right side
				else {
					ball_direction_x += 0.05555;
				}
			}
			else {
				// left side
				if (ball_x < bar_x - 2.0) {
					ball_direction_x -= 0.05555;
				}
				//right side
				else {
					ball_direction_x += 0.05555;
				}
			}
		}
		ball_direction_y *= -1;
		collision_block = -1;
	}
	
	// collision with blocks
	// if (status != -1 && playing) {
		for(i = 0; i < current_level->numero_de_bloques; ++i) {
			block = block_list[i];
			block_left_bound = block.fila - WIDTH_SCALE/2;
			block_right_bound = block.fila + WIDTH_SCALE/2;
			block_top_bound = block.columna + HEIGHT_SCALE/2;
			block_bottom_bound = block.columna - HEIGHT_SCALE/2;
		
			// collision with top or bottom
			if ( collision_block != i
				&&	ball_x >= block_left_bound - BALL_RADIUS/2
				&&	ball_x <= block_right_bound + BALL_RADIUS/2
				&& playing)
			{
					// top
					if (abs(ball_y - block_top_bound) < BALL_RADIUS/2) {
						ball_direction_y = 1;
						collision_block = i;
						if (block.color == 'V') {
							ball_speed_x += ball_speed_x * 0.1;
							ball_speed_y += ball_speed_y * 0.1;
						}
						if (block.color != 'G') {
							remaining_hits[i]--;
							blocks_left--;
							score += points[i];
						}
						break;
					}
					// bottom
					if (abs(ball_y - block_bottom_bound) < BALL_RADIUS/2) {
						ball_direction_y = -1;
						collision_block = i;
						if (block.color == 'V') {
							ball_speed_x += ball_speed_x * 0.1;
							ball_speed_y += ball_speed_y * 0.1;
						}
						if (block.color != 'G') {
							remaining_hits[i]--;
							blocks_left--;
							score += points[i];
						}
						break;
					}
			}

			// collision with one of the sides
			if ( 	collision_block != i
				&& 	ball_y >= block_bottom_bound - BALL_RADIUS/2
				&&	ball_y <= block_top_bound + BALL_RADIUS/2
				&& playing)
			{
				if( abs(ball_x - block_right_bound) < BALL_RADIUS/2 && ball_direction_x < 0) {
					ball_direction_x *= -1;
					collision_block = i;
					if (block.color == 'V') {
						ball_speed_x += ball_speed_x * 0.1;
						ball_speed_y += ball_speed_y * 0.1;
					}
					if (block.color != 'G') {
						remaining_hits[i]--;
						blocks_left--;
						score += points[i];
					}
					break;
				}
				if( abs(ball_x - block_left_bound) < BALL_RADIUS/2 && ball_direction_x > 0) {
					ball_direction_x *= -1;
					collision_block = i;
					if (block.color == 'V') {
						ball_speed_x += ball_speed_x * 0.1;
						ball_speed_y += ball_speed_y * 0.1;
					}
					if (block.color != 'G') {
						remaining_hits[i]--;
						blocks_left--;
						score += points[i];
					}
					break;
				}
			}
			if (remaining_hits[i] == 0) {
				// brick destroyed
				block_list[i].fila = -5;
				block_list[i].columna = -5;
			}
		
		}
	//}
	
	// collision walls
	if (ball_x - BALL_RADIUS/2 <= MIN_X || ball_x + BALL_RADIUS/2 >= MAX_X) {
		ball_direction_x *= -1.0;
		collision_block = -2;
	}
	
	// collisiom w/ceiling
	if (ball_y + 1 >= MAX_Y) {
		ball_direction_y *= -1.0;
		collision_block = -2;
		ceiling_hits++;
		if(ceiling_hits % 5 == 0) {
			ball_speed_x = ball_speed_x + ball_speed_x * current_level->incremento_velocidad;
			ball_speed_y = ball_speed_y + ball_speed_y * current_level->incremento_velocidad;
		}
	}
	
	// collision w/floor - lose
	if (ball_y < -5.0) {
		alive = 0;
		lives--;
		collision_block = -2;
		playing = 0;
	}
	
	// Force a redisplay to render the new image
	glutPostRedisplay();

	previous_time = current_time;
}
static void key(unsigned char k, int x, int y) {
	unsigned int iseed = (unsigned int) time (NULL);
	srand (iseed);
	switch (k) {
	// escape
	case ESCAPE:
		exit(0);
	case SPACEBAR:
		if (!alive) {
			alive = 1;
			ball_speed_y = 0.3;
			ball_speed_x = 0.3;
			ball_direction_y = 1.0;
			ball_direction_x = (rand() % 2 > 0 ? -1 : 1) * (rand() % 5) * 0.02222;
			next_jump = glutGet(GLUT_ELAPSED_TIME) + game->tiempo_enfriamiento
				+ game->tiempo_de_salto;
			playing = 1;
		}
		break;
	// case 'r':
	// case 'R':
	// 	reset_game();
	// 	free_memory();
	// 	load_bricks();
	// 	break;
	default:
		return;
	}

	// paint
	glutPostRedisplay();
}

void key_special(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:
			bar_x = (bar_x - WIDTH_SCALE - 1 < 0) ? WIDTH_SCALE : bar_x - 1;
			break;
		case GLUT_KEY_RIGHT:
			bar_x = (bar_x + WIDTH_SCALE + 1 > 50) ? 50 - WIDTH_SCALE : bar_x + 1;
		default: 
			return;
	}
	glutPostRedisplay();
}

// move directional bar with mouse
void mouse (int x, int y) {
	GLdouble new_x, new_y, new_z;
	GLdouble model_matrix[16];
    GLdouble projection_matrix[16];
    int viewport[4];
	
	// translate mouse coordinates to world coordinates
    glGetDoublev(GL_MODELVIEW_MATRIX, model_matrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluUnProject(x, viewport[3]-y, 0,
		model_matrix, projection_matrix, viewport,
		&new_x, &new_y, &new_z);
	
	// mouse position valid?
	bar_x = (new_x > WIDTH_SCALE && new_x < MAX_X - WIDTH_SCALE) ? (float) new_x : bar_x;	
	
	// paint
	glutPostRedisplay();
}

int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Play");

	// lights
	light_config();
	
	// hooks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(key_special);
	glutPassiveMotionFunc(mouse);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// load game settings and levels
	game = (Game *) malloc(sizeof(Game));
	load_game(argv[1], game);
	lives = game->vidas;
	
	// load bricks data
	load_bricks();
		
	// Start the timer
    glutTimerFunc(timer, play, 0);

	// initialize the time vars
	star_time = glutGet(GLUT_ELAPSED_TIME);
	previous_time = star_time;


	glutMainLoop();
	return 0;
}

