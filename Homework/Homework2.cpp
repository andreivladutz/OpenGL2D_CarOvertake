#include "../Headers/ProgramLoader.h"
#include <GL/freeglut.h>
#include <iostream>

// helperele din glm necesare
#include "../glm/glm.hpp"  
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

GLuint programId, VboId, VaoId,
	ColorBufferId, transformMatLocation, colorCodeLocation;

const int WIDTH = 800, HEIGHT = 800;
const float PI = 3.1415f;

const GLint BACKGROUND = 0, POLYGON_P1 = 1, POLYGON_P2 = 2, BEFORE_D = 3, AFTER_D = 4, AFTER_POLYGONS = 5;
const GLfloat LEFT = 0, RIGHT = WIDTH, TOP = HEIGHT, BOTTOM = 0;

void init(int* argcPtr, char* argv[]) {
	glutInit(argcPtr, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0); // pozitia initiala a ferestrei
	glutInitWindowSize(WIDTH, HEIGHT); //dimensiunile ferestrei
	glutCreateWindow("Tema 2"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
}

void CreateVBO(void)
{
	// varfurile 
	GLfloat Vertices[] = {
		// cele 4 varfuri din colturi 
		LEFT, TOP, 0.0f, 1.0f,
		LEFT,  BOTTOM, 0.0f, 1.0f,
		RIGHT, BOTTOM, 0.0f, 1.0f,
		RIGHT, TOP, 0.0f, 1.0f,
		// poligonul P1
		400, 500, 0, 1,
		350, 400, 0, 1,
		250, 450, 0, 1,
		200, 600, 0, 1,
		250, 750, 0, 1,
		350, 700, 0, 1,
		400, 500, 0, 1,
		// poligonul P2
		// format din doua poligoane convexe
		//a2
		500, 500, 0, 1,
		//b2
		600, 400, 0, 1,
		//c2
		700, 550, 0, 1,
		//d2
		600, 100, 0, 1,
		//e2
		700, 300, 0, 1,
		//f2
		750, 600, 0, 1,
		//g2
		650, 800, 0, 1,
		//h2
		650, 650, 0, 1,
		//a2
		500, 500, 0, 1,
		// dreptunghiul D
		200, 200, 0, 1,
		200, 100, 0, 1,
		350, 100, 0, 1,
		350, 200, 0, 1,

		// poligonul p2
		//a2
		500, 500, 0, 1,
		//b2
		600, 400, 0, 1,
		//c2
		700, 550, 0, 1,
		//h2
		650, 650, 0, 1,

		//c2
		700, 550, 0, 1,
		//d2
		600, 100, 0, 1,
		//e2
		700, 300, 0, 1,
		//f2
		750, 600, 0, 1,
		//g2
		650, 800, 0, 1,
		//h2
		650, 650, 0, 1,
	};

	GLfloat Colors[] = {
	  // culorile varfurilor fundalului
	  1.0f, 0.0f, 0.0f, 1.0f,
	  0.0f, 1.0f, 0.0f, 1.0f,
	  1.0f, 1.0f, 0.0f, 1.0f,
	  1.0f, 1.0f, 0.0f, 1.0f,
	};


	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// punctele sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	// 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void colorBackground() {
	

	glDrawArrays(GL_QUADS, 0, 4);
}

void renderFunction() {
	transformMatLocation = glGetUniformLocation(programId, "transform_Matrix");
	colorCodeLocation = glGetUniformLocation(programId, "color_Code");
	GLint shouldRotateLocation = glGetUniformLocation(programId, "should_Rotate"),
		translateMLocation = glGetUniformLocation(programId, "translate_M"),
		translateNegMLocation = glGetUniformLocation(programId, "translate_NegM"),
		rotateMatrixLocation = glGetUniformLocation(programId, "rotate_Matrix");

	// nu aplicam inca rotatia
	glUniform1i(shouldRotateLocation, false);
	
	// duc ecranul din [0, 800] x [0, 1000] in [-400, 400] x [-500, 500]
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-WIDTH / 2, -HEIGHT / 2, 0));
	// duc ecranul din [-400, 400] x [-500, 500] in [-1, 1] x [-1, 1]
	glm::mat4 resizeMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / (WIDTH / 2), 1.0f / (HEIGHT / 2), 1));
	
	glm::mat4 transformMatrix = resizeMatrix * translateMatrix;
	glUniformMatrix4fv(transformMatLocation, 1, GL_FALSE, &transformMatrix[0][0]);

	glUniform1i(colorCodeLocation, BACKGROUND);
	colorBackground();

	// desenam poligonul P1
	glUniform1i(colorCodeLocation, POLYGON_P1);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 6);

	// desenam poligonul P2
	glUniform1i(colorCodeLocation, POLYGON_P2);
	// glDrawArrays(GL_TRIANGLE_FAN, 10, 4);
	// glDrawArrays(GL_TRIANGLE_FAN, 14, 6);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 6);
	
	// desenam poligonul P2 doar cu linii
	glPolygonMode(GL_FRONT, GL_LINE);

	glUniform1i(colorCodeLocation, POLYGON_P1);
	// glDrawArrays(GL_TRIANGLE_FAN, 10, 4);
	// glDrawArrays(GL_TRIANGLE_FAN, 14, 6);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 6);

	// desenam poligoanele dupa rotatie
	glUniform1i(colorCodeLocation, AFTER_POLYGONS);
	
	// punct intre cele doua poligoane
	glm::vec3 pointM = glm::vec3(450, 500, 0);

	// aplic o rotatie de centru M
	glm::mat4 translateNegM = glm::translate(glm::mat4(1.0f), -pointM);
	glm::mat4 translateM = glm::translate(glm::mat4(1.0f), pointM);
	glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), PI / 4, glm::vec3(0, 0, 1));

	// rotatia ar trebui sa se aplice in shader
	glUniform1i(shouldRotateLocation, true);
	glUniformMatrix4fv(translateMLocation, 1, GL_FALSE, &translateM[0][0]);
	glUniformMatrix4fv(translateNegMLocation, 1, GL_FALSE, &translateNegM[0][0]);
	glUniformMatrix4fv(rotateMatrixLocation, 1, GL_FALSE, &rotateMatrix[0][0]);

	glLineWidth(3);
	// P1
	glDrawArrays(GL_LINES, 4, 6);
	glDrawArrays(GL_LINES, 5, 6);
	// P2
	glDrawArrays(GL_LINES, 11, 8);
	glDrawArrays(GL_LINES, 12, 8);
	//glDrawArrays(GL_TRIANGLE_FAN, 15, 6);

	// revin la fill ca sa nu faca dreptunghiurile cu line
	glPolygonMode(GL_FRONT, GL_FILL);

	// revenim la matricea de transformare anterioara (cea fara rotatie)
	glUniform1i(shouldRotateLocation, false);

	// desenam dreptunghiul D inainte de transformare
	glUniform1i(colorCodeLocation, BEFORE_D);
	glDrawArrays(GL_QUADS, 20, 4);

	// desenam dreptunghiul dupa transformare
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1), glm::vec3(0.5f, 0.25f, 1));
	transformMatrix = transformMatrix * scaleMatrix;
	glUniformMatrix4fv(transformMatLocation, 1, GL_FALSE, &transformMatrix[0][0]);
	glUniform1i(colorCodeLocation, AFTER_D);
	glDrawArrays(GL_QUADS, 20, 4);

	glFlush();
}

void cleanup() {
	DestroyVBO();
}

int main(int argc, char* argv[]) {
	init(&argc, argv);

	ProgramLoader progrLoader;

	try {
		programId = progrLoader
			.attachShader("./Shaders/homework2VertShader.vert", GL_VERTEX_SHADER)
			.attachShader("./Shaders/homework2FragShader.frag", GL_FRAGMENT_SHADER)
			.linkProgram()
			.deleteShaders()
			.getProgramId();
	}
	catch (std::exception e) {
		std::cout << e.what();

		exit(0);
	}

	glUseProgram(programId);
	CreateVBO();

	renderFunction();
	glutDisplayFunc(renderFunction);
	glutCloseFunc(cleanup);
	glutMainLoop();

	return 0;
}