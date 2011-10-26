#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>

#include "loader.h"

#define WIDTH_SCALE 5.0
#define HEIGHT_SCALE 2.0
#define DEPTH_SCALE 2.0
#define BALL_RADIUS 0.5
#define BLOCK_SIZE 1.0
#define MIN_X 0.0
#define MIN_Y  0.0
#define MIN_Z  0.0
#define MAX_X  50.0
#define MAX_Y  80.0
#define MAX_Z  5.0
#define ESCAPE 27
#define SPACEBAR 32

// timer variables
int star_time;
int previous_time;
int timer = 20;

// levels
Nivel nivel;

// ball
float ball_y = 0.0;
float ball_x = 25.0;
float ball_direction_y = 1.0;
float ball_direction_x = 1.0;
float ball_speed_x = 0.0;
float ball_speed_y = 0.0;

// directional bar
float bar_x = 25.0;

// blocks data
bloque *block_list;
int *remaining_hits;
int collision_block = -1;

int alive = 0;

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

void draw_brick (bloque block) {
	float *rgba;
	float yellow[4] = {1.0,1.0,0.0,1.0};
	float orange[4] = {1.0,0.5,0.0,1.0};
	float red[4] = {1.0,0.0,0.0,1.0};
	float green[4] = {0.0,1.0,0.0,1.0};
	float gray[4] = {0.7,0.7,0.7,1.0};
	float black[4] = {0.0,0.0,0.0,1.0};
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

void draw_axes (GLfloat size) {
	
	glPushMatrix();
		glLineWidth(1.0f);
	
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,1.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,1.0,1.0});
		// x axis
		glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(10*5, 0.0f, 0.0f);
		glEnd();
	
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,0.0,1.0});
		// y axis
		glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 40*2, 0.0f);
		glEnd();

		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,1.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,1.0,0.0,1.0});
		// z axis
		glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, size);
		glEnd();
	
	glPopMatrix();
}

void draw_grid () {
	float x = 0.0, y = 0.0;
	int i, j;
	
	glPushMatrix();
		glPushAttrib(GL_LIGHTING_BIT);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.1,0.8,0.4,1.0});
			glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.1,0.2,0.6,1.0});
		glPopAttrib();
		glLineWidth(1.0f);
		glTranslatef(0,0,-3.0f);
		// free space
		// horizontal
		for(y = 0; y < 20; ++i, y += 2) {
			glBegin(GL_LINES);
			glVertex2f(0.0f,y);
			glVertex2f(50.0f,y);
			glEnd();
		}
		for(y = 70; y <= 80; ++i, y += 2) {
			glBegin(GL_LINES);
			glVertex2f(0.0f,y);
			glVertex2f(50.0f,y);
			glEnd();
		}
		// vertical
		for(x = 0; x <= 50; x += 5) {
			glBegin(GL_LINES);
			glVertex2f(x,0.0f);
			glVertex2f(x,20.0f);
			glVertex2f(x,70.0f);
			glVertex2f(x,80.0f);
			glEnd();
		}
		
		glPushAttrib(GL_LIGHTING_BIT);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,1.0,0.0,1.0});
			glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,1.0,0.0,1.0});
		glPopAttrib();
		// game area
		glLineWidth(1.0f);
		// horizontal
		for(y = 20; y <= 70; y += 2) {
			glBegin(GL_LINES);
			glVertex2f(0.0f,y);
			glVertex2f(50.0f,y);
			glEnd();
		}
		// vertical
		for(x = 0; x <= 50; x += 5) {
			glBegin(GL_LINES);
			glVertex2f(x,20.0f);
			glVertex2f(x,70.0f);
			glEnd();
		}
	glPopMatrix();
}

void load_bricks () {
	int i = 0;
	block_list = (bloque *)malloc(sizeof(bloque)*nivel.numero_de_bloques);
	remaining_hits = (int *)malloc(sizeof(int)*nivel.numero_de_bloques);
	
	for(i; i < nivel.numero_de_bloques; i++) {
		block_list[i].fila = (float)nivel.bloques[i].fila * WIDTH_SCALE + 2.5;
		block_list[i].columna = ((float)nivel.bloques[i].columna + 10 + 0.5) * HEIGHT_SCALE;
		block_list[i].color = (float)nivel.bloques[i].color;
		switch (nivel.bloques[i].color) {
			case 'A':
			remaining_hits[i] = 1;
			break;
			case 'N':
			remaining_hits[i] = 3;
			break;
			case 'R':
			remaining_hits[i] = 5;
			break;
			case 'V':
			remaining_hits[i] = 1;
			break;
			case 'G':
			remaining_hits[i] = -1;
			break;
			default: break;
		}
	}
}

void draw_bricks () {
	int i = 0;
	for(i; i < nivel.numero_de_bloques; i++) {
		if (remaining_hits[i]) {
			draw_brick(block_list[i]);
		}
		else {
			block_list[i].fila = -5;
			block_list[i].columna = -5;
		}
	}
}

void draw_bar () {
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,1.0,1.0});
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,1.0,1.0});
		glTranslatef(bar_x,-1.0,0.0);
		glScalef(2*WIDTH_SCALE,HEIGHT_SCALE/2,DEPTH_SCALE);
	glutSolidCube(1);
	glPopMatrix();
}

void draw_ball() {
	alive ? ball_x : (ball_x = bar_x);
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
		glTranslatef(0.0,0.0,0.0);
		glLineWidth(2.0);
		glBegin(GL_LINES);
			//ceiling
			glVertex3f(0.0,80.0,0.0);
			glVertex3f(0.0,80.0,2.0);
			glVertex3f(0.0,80.0,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,0.0);
			glVertex3f(50.0,80,0.0);
			glVertex3f(0.0,80.0,0.0);
			// walls
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,2.0);
			glVertex3f(0.0,0.0,2.0);
			glVertex3f(0.0,80,2.0);
			glVertex3f(0.0,80,2.0);
			glVertex3f(0.0,80,0.0);			
			glVertex3f(0.0,80,0.0);
			glVertex3f(0.0,0.0,0.0);
			
			glVertex3f(50.0,0.0,0.0);
			glVertex3f(50.0,0.0,2.0);
			glVertex3f(50.0,0.0,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,0.0);
			glVertex3f(50.0,80,0.0);
			glVertex3f(50.0,0.0,0.0);
		glEnd();
	glPopMatrix();
	glPushMatrix();		
		glTranslatef(0.0,0.0,0.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.95,0.95,0.95,1.0});
		glBegin(GL_QUADS);
			// ceiling
			glVertex3f(0.0,80.0,0.0);
			glVertex3f(0.0,80.0,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,0.0);
			// walls
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,2.0);
			glVertex3f(0.0,80,2.0);
			glVertex3f(0.0,80,0.0);
			glVertex3f(50.0,0.0,0.0);
			glVertex3f(50.0,0.0,2.0);
			glVertex3f(50.0,80,2.0);
			glVertex3f(50.0,80,0.0);
		glEnd();
	glPopMatrix();
}

void display () {
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);				
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw scene elements
	// draw_grid();
	draw_bounds();
	draw_bricks();
	draw_bar();
	draw_ball();

	// paint
	glutSwapBuffers();
}

void reshape (int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	
	gluPerspective(40, (GLfloat)width / (GLfloat)height, 60.0, 130.0); 
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(25.0,-30.0,70.0,25.0,30.0,0.0,0.0,1.0,0.0);
}

static void play(int value) {
	int i = 0;
	float 	block_left_bound,
	 		block_right_bound,
	 		block_top_bound,
	 		block_bottom_bound;
	float 	ball_max_x = ball_x + BALL_RADIUS,
	 		ball_min_x = ball_x - BALL_RADIUS,
	 		ball_max_y = ball_y + BALL_RADIUS,
			ball_min_y = ball_y - BALL_RADIUS;
	bloque block;
	
	// Set up the next timer tick (do this first)
    glutTimerFunc(timer, play, 0);

	// Measure the elapsed time
	int current_time = glutGet(GLUT_ELAPSED_TIME);
	int time_since_previous_frame = current_time - previous_time;
	int elapsed_time = current_time - star_time;

	ball_y += ball_direction_y * (ball_speed_y/3);
	ball_x += ball_direction_x * (ball_speed_x/3);

	// collision w/ bar
	if (	collision_block != -1 && ball_y <= 0.5
		&& 	ball_x >= bar_x - WIDTH_SCALE && ball_x <= bar_x + WIDTH_SCALE) {
		ball_direction_y *= -1;
		collision_block = -1;
	}

	// collisiom w/ceiling
	if(ball_y + 1 >= MAX_Y) {
		ball_direction_y *= -1.0;
		collision_block = -2;
	}
	
	// collision with blocks
	for(i = 0; i < nivel.numero_de_bloques; ++i) {
		block = block_list[i];
		block_left_bound = block.fila - WIDTH_SCALE/2;
		block_right_bound = block.fila + WIDTH_SCALE/2;
		block_top_bound = block.columna + HEIGHT_SCALE/2;
		block_bottom_bound = block.columna - HEIGHT_SCALE/2;

		// collision with top or bottom
		if ( 	collision_block != i
			&&	ball_x >= block_left_bound - BALL_RADIUS/2
			&&	ball_x <= block_right_bound + BALL_RADIUS/2
			&& 	(	abs(ball_y - block_top_bound) <= BALL_RADIUS/2 
				|| 	abs(ball_y - block_bottom_bound) <= BALL_RADIUS/2) )
		{
			ball_direction_y *= -1;
			collision_block = i;
			remaining_hits[i]--;
		}
		// collision with one of the sides
		if ( 	collision_block != i
			&& 	ball_y >= block_bottom_bound - BALL_RADIUS/2
			&&	ball_y <= block_top_bound + BALL_RADIUS/2
			&& 	(	abs(ball_x - block_right_bound) <= BALL_RADIUS/2 
				|| 	abs(ball_x - block_left_bound) <= BALL_RADIUS/2))
		{
			ball_direction_x *= -1;
			collision_block = i;
			remaining_hits[i]--;
		}
	}
	
	// collision walls
	if (ball_x - BALL_RADIUS/2 <= MIN_X || ball_x + BALL_RADIUS/2 >= MAX_X) {
		ball_direction_x *= -1.0;
		collision_block = -2;
	}

	// Force a redisplay to render the new image
	glutPostRedisplay();

	previous_time = current_time;
}

static void key(unsigned char k, int x, int y) {
	switch (k) {
	// escape
	case ESCAPE:
		exit(0);
	case SPACEBAR:
		if (!alive) {
			ball_speed_y = 0.5;
			ball_speed_x = 0.5;
			ball_direction_x = -0.2;// + (float) (rand() % 11) + 10.0;
			printf("ball_speed_x %d %f %f \n ",rand(),ball_speed_x, (double)(rand() % 11 + 10.0)/10.0);
			alive = 1;
		}
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
	glutInitWindowSize(600,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Play")	;

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
	
	// load levels from file
	nivel = load_level("nivel1.txt");
	
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