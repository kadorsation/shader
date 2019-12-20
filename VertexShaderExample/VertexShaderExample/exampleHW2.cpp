#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <cmath>
#define PI 3.14159265358

using namespace std;

int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;
GLuint vboName;

//Initialize the shaders 
void shaderInit();

//Number of vertices that will be sent to shader
int verticeNumber = 20 * 10 * 2;



//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	void setPositionE(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	GLfloat texcoord[2];
	void setPositionM(float x, float y) {
		texcoord[0] = x;
		texcoord[1] = y;
	};
};

//Collecting vertex datas
VertexAttribute *draw();

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);

	//Generate a new buffer object
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = draw();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display() {   
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.6f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	glUseProgram(program);

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, verticeNumber);
	glUseProgram(0);
	glPopMatrix();

	// Light ball
	glTranslatef(1.1, 1.0, 1.3);
	GLfloat diffuse2[] = { 0.4, 0.5, 0.5, 0.5 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse2);
	GLUquadricObj* quadric;
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.5f, 0.0f);
	gluSphere(quadric, 0.05f, 20, 10);



	glutSwapBuffers();
}

VertexAttribute* draw() {
	int k = 0;
	GLfloat x, y, z;
	GLfloat slice = 20, stack = 10;
	GLfloat slice_step = 2 * PI / slice, stack_step = PI / stack;
	VertexAttribute* vertices = new VertexAttribute[20 * 11 * 2];
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			x = sin(j * stack_step) * cos(i * slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin(i * slice_step);
			vertices[k].setPositionE(x, y, z);
			k++;

			x = sin(j * stack_step) * cos((i + 1) * slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1) * slice_step);
			vertices[k].setPositionE(x, y, z);
			k++;
		}
	}
	return vertices;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {

}

void idle() {
	glutPostRedisplay();
}