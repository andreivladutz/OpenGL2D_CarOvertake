#include "../Headers/CarsHandler.h"
#include "../Headers/DecorationsHandler.h"

#include <iostream>

GLuint programId;

CarsHandler* carsHandler;
DecorationsHandler* decorations;

void errorCallback(int error, const char* errDescription) {
	std::cerr << "Error " + error << "\nDescription: " << errDescription;
}

void colorBackground() {
	static const GLfloat color[] = { 0.88f, 0.88f, 0.88f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, color);
}

void Context2D::render(double deltaTime) {
	colorBackground();

	decorations->drawDecorations();
	carsHandler->renderCars(deltaTime);

	//glFlush();
}

void cleanup() {

}

int main(int argc, char* argv[]) {
	glfwSetErrorCallback(errorCallback);

	// the TransformBlock uniform block details
	static const GLchar* uniformBlockMemberNames[uniformBlockMembersCount] = {
		"TransformBlock.resizeMatrix",
		"TransformBlock.cameraTranslation",
		"TransformBlock.modelTranslation",
		"TransformBlock.modelRotation",
		"TransformBlock.modelScale"
	};
	const GLuint blockBinding = 0;

	try {
		// init window and glew
		programId = Context2D::getInstance("Window Title")
			// compile shaders, link program
			.compileProgram({
				{"./Shaders/homework3VertShader.vert", GL_VERTEX_SHADER},
				{"./Shaders/basicFragShader.frag", GL_FRAGMENT_SHADER},

				})
				.setTransformationMatricesUniformBlock(uniformBlockMemberNames, blockBinding)
			// create and bind vao
			.createVao("in_Position", "in_Color")
			.getProgramId();
	}
	catch (std::exception e) {
		std::cerr << e.what();
		return 0;
	}

	decorations = &DecorationsHandler::getInstance();
	carsHandler = &CarsHandler::initInstance(glm::vec4(0.3, 0.6, 0.3, 1));

	Context2D::getInstance().renderLoop();

	CarsHandler::removeInstance();

	return 0;
}