#include "../Headers/Context2D.h"
#include <iostream>

Context2D* Context2D::instance = nullptr;

Context2D::Context2D(const char* windowTitle) noexcept(false) {
	try {
		windowHandler.initialize(windowTitle);
	}
	catch (std::exception &e) {
		throw e;
	}

	glfwSetFramebufferSizeCallback(windowHandler.getWindow(), Context2D::handleFramebufferResize);
	
	setResizeMatrix();
}

Context2D::~Context2D() {
	delete progrLoader;

	if (positionAttribIndex != -1 && colorAttributeIndex != -1) {
		glDisableVertexArrayAttrib(vaoId, positionAttribIndex);
		glDisableVertexArrayAttrib(vaoId, colorAttributeIndex);
	}

	glDeleteVertexArrays(1, &vaoId);

	glDeleteBuffers(1, &uniformBlockBufferName);
}


void Context2D::setResizeMatrix(int width, int height) {
	if (width == 0 || height == 0) {
		glfwGetFramebufferSize(windowHandler.getWindow(), &width, &height);
	}

	resizeMatrix = glm::scale(glm::mat4(1), glm::vec3(1. / (width / 2), 1. / (height / 2), 1));

	framebufWidth = width;
	framebufHeight = height;
}

void Context2D::handleFramebufferResize(GLFWwindow* window, int width, int height) {
	Context2D& ctx = Context2D::getInstance();

	glViewport(0, 0, width, height);
	ctx.setResizeMatrix(width, height);
}

Context2D& Context2D::compileProgram(std::map<std::string, GLenum> shadersDictionary) noexcept(false) {
	progrLoader = new ProgramLoader();

	try {
		for (auto& shader : shadersDictionary) {
			// first is the path to the shader, second is the type of the shader
			progrLoader->attachShader(shader.first.c_str(), shader.second);
		}

		programId = progrLoader
			->linkProgram()
			.deleteShaders()
			.getProgramId();
	}
	catch (std::exception &e) {
		throw e;
	}

	glUseProgram(programId);

	return *this;
}

Context2D& Context2D::createVao(const char* positionAttributeName, const char* colorAttributeName) noexcept(false) {
	// create the vao
	glCreateVertexArrays(1, &vaoId);
	glBindVertexArray(vaoId);

	// get the locations of the attributes
	positionAttribIndex = glGetAttribLocation(programId, positionAttributeName);
	colorAttributeIndex = glGetAttribLocation(programId, colorAttributeName);

	if (positionAttribIndex == -1) {
		throw new std::exception("Position attribute's location not found");
	}

	if (colorAttributeIndex == -1) {
		throw new std::exception("Color attribute's location not found");
	}

	// the format of the position and color attribute is the format of a glm::vec4
	glVertexArrayAttribFormat(vaoId, positionAttribIndex, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(vaoId, colorAttributeIndex, 4, GL_FLOAT, GL_FALSE, 0);

	// enable the attributes' data fetching
	glEnableVertexArrayAttrib(vaoId, positionAttribIndex);
	glEnableVertexArrayAttrib(vaoId, colorAttributeIndex);

	return *this;
}

Context2D& Context2D::bindDrawableObject(DrawableObject &obj) noexcept(false) {
	// get the names of the buffers
	GLuint vertexBufferId = obj.getVertexBufferId(), colorBufferId = obj.getColorBufferId();

	// make sure we do not exceed the maximum allowed vao bound buffers
	GLint maxAttributeBindings = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &maxAttributeBindings);

	if (currBindingIndex + 2 > (GLuint)maxAttributeBindings) {
		throw std::exception("Maximum buffer attribute bindings reached. Cannot bind any more buffers");
	}

	// bind the vertex buffer
	glVertexArrayVertexBuffer(vaoId, currBindingIndex, vertexBufferId, 0, sizeof(glm::vec4));
	// let the object know where the vertex buffer is bound
	obj.setVertexBufferBindingId(currBindingIndex++);

	// bind the color buffer
	glVertexArrayVertexBuffer(vaoId, currBindingIndex, colorBufferId, 0, sizeof(glm::vec4));
	// let the object know where the color buffer is bound
	obj.setColorBufferBindingId(currBindingIndex++);

	return *this;
}

Context2D& Context2D::drawObject(DrawableObject &obj) {
	GLuint vertexBufBinding = obj.getVertexBufferBindingId(),
		colorBufBinding = obj.getColorBufferBindingId();

	// switch the attribute bindings to this object's buffers
	glVertexArrayAttribBinding(vaoId, positionAttribIndex, vertexBufBinding);
	glVertexArrayAttribBinding(vaoId, colorAttributeIndex, colorBufBinding);

	// enable the attributes' data fetching
	// glEnableVertexArrayAttrib(vaoId, positionAttribIndex);
	// glEnableVertexArrayAttrib(vaoId, colorAttributeIndex);

	
	/*
	DrawableObject::TransformationMatrices modelTransformationMatrices = obj.getTransformationMatrices();

	// before drawing apply the camera and model transformations
	glm::mat4 transformationMatrix = resizeMatrix * cameraTranslation 
		* modelTransformationMatrices.translationMatrix
		* modelTransformationMatrices.rotationMatrix
		* modelTransformationMatrices.scaleMatrix;

	glUniformMatrix4fv(transfMatrixLocation, 1, GL_FALSE, &transformationMatrix[0][0]);
	*/
	

	fillShaderUniformMatrices(obj.getTransformationMatrices());

	obj.draw();

	return *this;
}


Context2D& Context2D::setTransformationMatrixAttributeName(const GLchar* name) noexcept(false) {
	transfMatrixLocation = glGetUniformLocation(programId, name);

	if (transfMatrixLocation == -1) {
		throw std::exception("Matrix location not found");
	}

	return *this;
}

Context2D& Context2D::setTransformationMatricesUniformBlock(const GLchar** uniformBlockNames, GLuint uniformBlockBinding) noexcept(false) {
	this->uniformBlockBinding = uniformBlockBinding;

	// get the indices of the 5 transformation matrices
	GLuint uniformIndices[uniformBlockMembersCount];

	glGetUniformIndices(programId, uniformBlockMembersCount, uniformBlockNames, uniformIndices);

	// get the offsets and strides of the matrix members
	glGetActiveUniformsiv(programId, uniformBlockMembersCount, uniformIndices, GL_UNIFORM_OFFSET, uniformMembersOffsets);
	glGetActiveUniformsiv(programId, uniformBlockMembersCount, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matricesStrides);

	/*
	std::cout << "the offsets of each member:\n";
	for (int i = 0; i < 5; i++) {
		std::cout << uniformMembersOffsets[i] << std::endl;
	}

	std::cout << "the matrixStride of each member:\n";
	for (int i = 0; i < 5; i++) {
		std::cout << matricesStrides[i] << std::endl;
	}
	*/

	/*
	GLuint blockIndex = glGetUniformBlockIndex(programId, "TransformBlock");

	std::cout << "block index = " << blockIndex << "\n";

	glUniformBlockBinding(programId, blockIndex, uniformBlockBinding);
	*/

	// also create the buffer which will keep the data of the matrices
	glCreateBuffers(1, &uniformBlockBufferName);
	// the size of the underlying buffer is -> the stride of each column in bytes * 4 columns for a matrix * number of members 
	uniformBlockBufferSize = matricesStrides[0] * 4 * uniformBlockMembersCount;
	glNamedBufferStorage(uniformBlockBufferName, uniformBlockBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

	// bind the buffer source for the TransformBlock
	glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, uniformBlockBufferName);

	return *this;
}

// function to help filling a local buffer with a mat4 at different offsets with the given stride
void fillLocalBuffer(unsigned char* buffer, GLint offset, GLint stride, const glm::mat4 &matrix) {
	const int matrixSize = 4, elemSize = sizeof(GLfloat);

	/*
	std::cout << "\nMatrix\n";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << "\n";
	}
	*/

	int columnOffset, elementOffset;
	// each column
	for (int i = 0; i < matrixSize; i++) {
		columnOffset = offset + i * stride;
		// each row
		for (int j = 0; j < matrixSize; j++) {
			elementOffset = columnOffset + j * elemSize;

			*((float*)(buffer + elementOffset)) = matrix[i][j];
		}
	}
}

// fill the underlying buffer's memory
void Context2D::fillShaderUniformMatrices(const DrawableObject::TransformationMatrices &modelTransformations) {
	unsigned char* localBuffer = new unsigned char[uniformBlockBufferSize];

	fillLocalBuffer(localBuffer, uniformMembersOffsets[resizeMatrixIdx], matricesStrides[resizeMatrixIdx], resizeMatrix);
	fillLocalBuffer(localBuffer, uniformMembersOffsets[cameraTranslationIdx], matricesStrides[cameraTranslationIdx], cameraTranslation);
	fillLocalBuffer(localBuffer, uniformMembersOffsets[modelTranslationIdx], matricesStrides[modelTranslationIdx], modelTransformations.translationMatrix);
	fillLocalBuffer(localBuffer, uniformMembersOffsets[modelRotationIdx], matricesStrides[modelRotationIdx], modelTransformations.rotationMatrix);
	fillLocalBuffer(localBuffer, uniformMembersOffsets[modelScaleIdx], matricesStrides[modelScaleIdx], modelTransformations.scaleMatrix);
	
	/*
	for (int k = 0; k < 5; k++) {
		int offset = k * 64;

		std::cout << "\nMatrix " << k << " =\n";
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				std::cout << ((GLfloat*)(localBuffer + offset + i * 16))[j] << " ";
			}
			std::cout << "\n";
		}
	}
	*/

	// copy the contents from the localBuffer to the uniform block's buffer
	glNamedBufferSubData(uniformBlockBufferName, 0, uniformBlockBufferSize, localBuffer);

	delete [] localBuffer;
}

Context2D& Context2D::translateCamera(double x, double y) {
	cameraTranslation = glm::translate(cameraTranslation, glm::vec3(x, y, 0));
	
	// what actually happens is objects are translated with negative dX and dY
	// but we imagine that the camera is translated with positive dX and dY and objects remain fixed
	translatedX -= x;
	translatedY -= y;

	return *this;
}

GLuint Context2D::getProgramId() {
	return programId;
}

GLuint Context2D::getVaoId() {
	return vaoId;
}

void Context2D::renderLoop() {
	windowHandler.setLoopCallback(Context2D::render).loop();
}


std::pair<glm::vec2, glm::vec2> Context2D::getViewportCoords() {
	float halfWidth = framebufWidth / 2, halfHeight = framebufHeight / 2;

	return {
		// topLeft
		glm::vec2(-halfWidth + translatedX, halfHeight + translatedY),
		// bottomRight
		glm::vec2(halfWidth + translatedX, -halfHeight + translatedY)
	};
}