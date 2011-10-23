#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>

#include "loader.h"

#define WIDTH_SCALE 5
#define HEIGHT_SCALE 2
#define DEPTH_SCALE 2
#define TIMERMSECS 20
#define MIN_X 0
#define MIN_Y  0
#define MIN_Z  0
#define MAX_X  50
#define MAX_Y  80
#define MAX_Z  5

// Global variables for measuring time (in milli-seconds)
int startTime;
int prevTime;

Nivel nivel;
float ball_direction = -1.0;
float ball_y = 65.0;

void light_config() { 
	GLfloat light_ambient[] = {.1, .1, .1, 1.0};
	GLfloat light_diffuse[] = {.8, .8, .8, 1.0};
	GLfloat light_specular[] = {.7, .7, .7, 1.0};
	GLfloat light_position[] = {-10.0, 10, 20, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void draw_brick (GLint x, GLint y, char color) {
	float *rgba;
	float yellow[4] = {1.0,1.0,0.0,1.0};
	float orange[4] = {1.0,0.5,0.0,1.0};
	float red[4] = {1.0,0.0,0.0,1.0};
	float green[4] = {0.0,1.0,0.0,1.0};
	float gray[4] = {0.7,0.7,0.7,1.0};
	switch (color) {
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
	glMaterialfv(GL_FRONT, GL_DIFFUSE, rgba);
	glMaterialfv(GL_FRONT, GL_AMBIENT, rgba);
	
	glPushMatrix();
		glTranslatef(WIDTH_SCALE*x+2.5,HEIGHT_SCALE*y+1 + DEPTH_SCALE*10,-2.0);
		glScalef(5.0,2.0,2.0);
		glutSolidCube(1);
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
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){.1,.1,.1,.1});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){.1,.1,.1,.1});

		glLineWidth(1.0f);
		glTranslatef(0,0,-1.0f);
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
		
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,0.0,1.0});
		// game area
		glLineWidth(2.0f);
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

void draw_bricks () {
	int i;
	for(i = 0; i < nivel.numero_de_bloques; i++) {
		draw_brick(nivel.bloques[i].fila,nivel.bloques[i].columna,nivel.bloques[i].color);
		printf("%d %d %c\n",nivel.bloques[i].fila,nivel.bloques[i].columna,nivel.bloques[i].color);
	}
}

void display () {
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);				
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_grid();
	draw_axes(30);
	draw_bricks();
	glPushMatrix();
	glTranslatef(25.0,ball_y,-2.0);
		glutSolidSphere(1,20,20);
	glPopMatrix();
	
	// floor
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,0.0,1.0});
		glBegin(GL_QUADS);
			glVertex3f(0.0,0.0,5.0);
			glVertex3f(0.0,0.0,-10.0);
			glVertex3f(50.0,0.0,-10.0);
			glVertex3f(50.0,0.0,5.0);
		glEnd();
	glPopMatrix();
	
	// ceiling
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,0.0,0.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,0.0,0.0,1.0});
		glBegin(GL_QUADS);
			glVertex3f(0.0,MAX_Y,5.0);
			glVertex3f(0.0,MAX_Y,-10.0);
			glVertex3f(50.0,MAX_Y,-10.0);
			glVertex3f(50.0,MAX_Y,5.0);
		glEnd();
	glPopMatrix();
	
	
	// fondo
	{
		// glPushMatrix();
		// 	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){0.0,1.0,0.0,0.2});
		// 	// glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){0.0,1.0,0.0,0.2});
		// 	glTranslatef(0.0,0.0,-1.0);
		// 	glBegin(GL_QUADS);
		// 		glVertex3f(0.0,0.0,0.0);
		// 		glVertex3f(0.0,60.0,0.0);
		// 		glVertex3f(50.0,60.0,0.0);
		// 		glVertex3f(50.0,0.0,0.0);
		// 	glEnd();
		// glPopMatrix();
	}
	
	glutSwapBuffers();
}

void reshape (int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	
	gluPerspective(50, (GLfloat)width / (GLfloat)height, 100.0, 120.0); 
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(25.0,40.0,100.0,25.0,40.0,0.0,0.0,1.0,0.0);
}

static void animate(int value)
{
	// Set up the next timer tick (do this first)
    glutTimerFunc(TIMERMSECS, animate, 0);

	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;

	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####

	// Rotate the triangle
	ball_y += ball_direction*0.5;

	// test collision with floor
	if (ball_y - 1 <= MIN_Y || ball_y + 1 >= MAX_Y) ball_direction *= -1.0;

	// Force a redisplay to render the new image
	glutPostRedisplay();

	prevTime = currTime;
}

int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Play")	;


	light_config();
	
	glutDisplayFunc(display);
	// glutIdleFunc(display);
	glutReshapeFunc(reshape);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	nivel = cargar_nivel("nivel1.txt");
	
	// Start the timer
    glutTimerFunc(TIMERMSECS, animate, 0);

	// initialize the time vars
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;


	glutMainLoop();
	return 0;
}