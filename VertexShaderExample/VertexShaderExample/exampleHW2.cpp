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

GLfloat slice = 20, stack = 10;

//Initialize the shaders 
void shaderInit();

//Number of vertices that will be sent to shader
int verticeNumber = slice * stack * 2;

//ªì©l­È­Ì
GLfloat Ks = 0;
GLfloat Kd = 0;
GLfloat Ka = 0.5;
GLfloat alpha = 3.6;
GLfloat la[3] = { 0.2, 0.2, 0.2 };
GLfloat ld[3] = { 0.5, 0.5, 0.5 };
GLfloat ls[3] = { 0.8, 0.8, 0.8 };


//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat normal[3];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setNormal(float x, float y, float z) {
		normal[0] = x;
		normal[1] = y;
		normal[2] = z;
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
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
	GLfloat lmtx[3] = { 1.1, 1.0 , 1.3 };
	GLfloat vmtx[3] = { 0, 0, 5.6 };
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint lmatLoc = glGetUniformLocation(program, "Light");
	GLint vmatLoc = glGetUniformLocation(program, "Viewer");
	GLint KsLoc = glGetUniformLocation(program, "Ks");
	GLint KdLoc = glGetUniformLocation(program, "Kd");
	GLint KaLoc = glGetUniformLocation(program, "Ka");
	GLint AlLoc = glGetUniformLocation(program, "alpha");
	GLint LaLoc = glGetUniformLocation(program, "la");
	GLint LdLoc = glGetUniformLocation(program, "ld");
	GLint LsLoc = glGetUniformLocation(program, "ls");
	glUseProgram(program);

	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	//input light
	glUniform3fv(lmatLoc, 1, lmtx);
	glUniform3fv(vmatLoc, 1, vmtx);
	glUniform1f(KsLoc, Ks);
	glUniform1f(KdLoc, Kd);
	glUniform1f(KaLoc, Ka);
	glUniform1f(AlLoc, alpha);
	glUniform3fv(LaLoc, 1, la);
	glUniform3fv(LdLoc, 1, ld);
	glUniform3fv(LsLoc, 1, ls);


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
	GLfloat slice_step = 2 * PI / slice, stack_step = PI / stack;
	VertexAttribute* vertices = new VertexAttribute[20 * 10 * 2 * 6];
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			x = sin(j * stack_step) * cos(i * slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin(i * slice_step);
			vertices[k].setPosition(x, y, z);
			vertices[k].setNormal(x, y, z);
			k++;

			x = sin(j * stack_step) * cos((i + 1) * slice_step);
			y = cos(j * stack_step);
			z = sin(j * stack_step) * sin((i + 1) * slice_step);
			vertices[k].setPosition(x, y, z);
			vertices[k].setNormal(x, y, z);
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
	switch (key)
	{
	case '1':
		if (Ks > 0) {
			Ks -= 0.1;
		}
		break;
	case '2':
		if (Ks < 1) {
			Ks += 0.1;
		}
		break;
	case '3':
		if (Kd > 0) {
			Kd -= 0.1;
		}
		break;
	case '4':
		if (Kd < 1) {
			Kd += 0.1;
		}
		break;
	default:
		break;
	}
}

void idle() {
	glutPostRedisplay();
}