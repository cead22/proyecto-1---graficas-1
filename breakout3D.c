#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>

#include "loader.h"

#define WIDTH 5
#define HEIGHT 2
#define DEPTH 2

Nivel nivel;

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
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, (float []){1.0,0.0,1.0,1.0});
		glMaterialfv(GL_FRONT, GL_AMBIENT, (float []){1.0,0.0,1.0,1.0});
		glTranslatef(5*x+2.5,2*y+1 + 2*10,-2.0);
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

void load_bricks () {
	int i;
	nivel = cargar_nivel("nivel1.txt");
	for(i = 0; i < nivel.numero_de_bloques*3; i += 3) {
		draw_brick(nivel.bloques[i],nivel.bloques[i+1],nivel.bloques[i+2]);
		// printf("%d,%d,%d\n",nivel.bloques[i],nivel.bloques[i+1],nivel.bloques[i+2]);
	}
}

void display () {

	glMatrixMode(GL_MODELVIEW);

	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);				
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	draw_grid();
	draw_axes(30);
	// draw_brick(5,5,'A');
	load_bricks();
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

int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(20,20);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Play")	;


	light_config();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glutMainLoop();
	return 0;
}