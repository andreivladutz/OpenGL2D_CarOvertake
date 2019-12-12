#pragma once
#include "WindowHandler.h"
#include "ProgramLoader.h"
#include "DrawableObject.h"
#include <string>
#include <map>
#include <exception>

// how many matrices are there in the uniform TransformBlock 
const GLsizei uniformBlockMembersCount = 5;
// the index inside the indices array
const GLint resizeMatrixIdx = 0,
	cameraTranslationIdx = 1,
	modelTranslationIdx = 2,
	modelRotationIdx = 3,
	modelScaleIdx = 4;

// Context2D is a Singleton Class
class Context2D {
public:
	// to be overwritten in the main source file
	static void render(double);

	static Context2D& getInstance(const char* windowTitle) noexcept(false) {
		if (instance == nullptr) {
			try {
				instance = new Context2D(windowTitle);
			}
			catch (std::exception &e) {
				throw e;
			}
		}

		return *instance;
	}

	static Context2D& getInstance() noexcept(false) {
		if (instance == nullptr) {
			throw new std::exception("Context2D not instantiated. Call getInstance(windowWidth, windowHeight) first.");
		}

		return *instance;
	}

	static void handleFramebufferResize(GLFWwindow* window, int width, int height);

	~Context2D();

	// gets a dictionary of key value pairs where
	// key is the path to the shader
	// value is the type of the shader
	Context2D& compileProgram(std::map<std::string, GLenum>) noexcept(false);
	Context2D& createVao(const char *positionAttributeName, const char *colorAttributeName) noexcept(false);
	// bind drawableObject's buffers(vertex and color) to the vao
	Context2D& bindDrawableObject(DrawableObject&);
	Context2D& drawObject(DrawableObject&) noexcept(false);

	Context2D& setTransformationMatrixAttributeName(const GLchar*) noexcept(false);
	// we need the properties' names to get the offset and the stride of the matrices
	// we also need the binding specified in the vertex shader so we cand bind a buffer to that binding point
	Context2D& setTransformationMatricesUniformBlock (const GLchar **uniformBlockNames, GLuint uniformBlockBinding) noexcept(false);

	// fill the underlying buffer's memory
	void fillShaderUniformMatrices(const DrawableObject::TransformationMatrices&);

	Context2D& translateCamera(double x, double y);

	// get the pair (topLeftPoint, bottomRightPoint)
	std::pair<glm::vec2, glm::vec2> getViewportCoords();

	int getScreenWidth() { return framebufWidth; }
	int getScreenHeight() { return framebufHeight; }

	GLuint getProgramId();
	GLuint getVaoId();

	GLFWwindow* getWindow() {
		return windowHandler.getWindow();
	}

	void renderLoop();
private:
	void setResizeMatrix(int = 0, int = 0);

	Context2D(const char* windowTitle) noexcept(false);

	// the pointer to the single instance of this class
	static Context2D* instance;

	// the binding of the TransformBlock
	GLuint uniformBlockBinding = 0;

	// the offsets in the buffer of each member in the TransformBlock uniform block
	GLint uniformMembersOffsets[uniformBlockMembersCount] = { 0 };

	// the strides of the matrices in the buffer of the TransformBlock uniform block
	GLint matricesStrides[uniformBlockMembersCount] = { 0 };
	
	// the name of the allocated buffer
	GLuint uniformBlockBufferName = 0;
	
	// the size in bytes of the underlying buffer
	GLsizei uniformBlockBufferSize = 0;

	WindowHandler windowHandler;
	int framebufWidth, framebufHeight;

	// keep count how much the camera has been translated. this way we can determine the current viewport's coords
	float translatedX = 0, translatedY = 0;

	ProgramLoader *progrLoader = nullptr;
	GLuint programId = 0, vaoId = 0;
	// the locations of the vertex attributes -> position and color
	GLint positionAttribIndex = -1, colorAttributeIndex = -1;

	// the location of the transformation matrix in the vertex shader
	GLint transfMatrixLocation = -1;

	// keep count of the current binding index [0, max binding point]
	GLuint currBindingIndex = 0;

	// camera translation and coordinates resize to the window
	// at first cameraTranslation is the identity matrix i.e. the camera is in the origin
	glm::mat4 cameraTranslation = glm::mat4(1),
		resizeMatrix = glm::mat4(1);
};