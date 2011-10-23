
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGl/glu.h>

#define WIDTH 5
#define HEIGHT 2
#define DEPTH 2

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

void draw_brick (GLint x, GLint y, GLfloat r, GLfloat g, GLfloat b) {
	glPushMatrix();
	glTranslatef(1,0,0);
		glColor3f(r, g, b);
		glBegin(GL_QUADS);
			// front
			glVertex2f(x, y);
		   	glVertex2f(x + WIDTH, y);        
			glVertex2f(x + WIDTH, y + HEIGHT);
			glVertex2f(x, y + HEIGHT);
		glEnd();
	glPopMatrix();
}             

void draw_axes (GLfloat size) {
	glLineWidth(1.0f);
	
	// x axis
	// glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	GLfloat blue[] = {0,0,1,1};
	GLfloat green[] = {0,1,0,.1};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_AMBIENT, green);
	
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(size, 0.0f, 0.0f);
	glEnd();
	
	// y axis
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, size, 0.0f);
	glEnd();

	// z axis
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, size);
	glEnd();
	
	glPopMatrix();
}

void draw_grid () {
	float x = 0.0, y = 0.0;
	int i, j;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();
	glLineWidth(1.0f);
	// glTranslatef(0,0,-10);
	// glTranslatef(0.0f,0.0f,-75.0f);
	// glScalef(0.1f,0.1f,1.0f);
	// horizontal
	for(i = 0; i <= 40; ++i, y += 2) {
		glBegin(GL_LINES);
		glVertex2f(0.0f,y);
		glVertex2f(125.0f,y);
		glEnd();
	}
	// vertical
	for(j = 0; j <= 25; ++j, x += 5) {
		glBegin(GL_LINES);
		glVertex2f(x,0.0f);
		glVertex2f(x,80.0f);
		glEnd();
	}
	glPopMatrix();
}

void display () {

	glMatrixMode(GL_MODELVIEW);

	glClearColor(1.0f, 1.0f, 1.0f ,1.0f);				
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gluLookAt(0.0,0.0,10.0,0.0,0.0,0.0,0.0,1.0,0.0);

	// draw_grid();
	draw_axes(100);
	// glPushMatrix();
	// glTranslatef(0,0,-1);
	// glScalef(.1,.1,.1);
	// glPushMatrix();
	// glLoadIdentity();
	glColor3f(1.0,0.0,0.0);
	glutSolidIcosahedron();
	// glPopMatrix();
	glutSwapBuffers();
}

void reshape (int width, int height) {
	// if (height == 0) height = 1;
   // GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	
	gluPerspective(50, (GLfloat)width / (GLfloat)height, 10.0, 50.0); 
	
	// if (width <= height) {
	//       glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);  // aspect <= 1
	//    } else {
	//       glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -1.0, 1.0);  // aspect > 1
	// }
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,10.0,0.0,0.0,0.0,0.0,1.0,0.0);
}

int main (int argc, char *argv[]) {
	// window settings
	glutInit(&argc, argv);
	glutInitWindowSize(640,640);
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