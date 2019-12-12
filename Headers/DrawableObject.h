#pragma once
#include <vector>
#include "GL/glew.h"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/transform.hpp"
#include "../glm/gtx/string_cast.hpp"

class DrawableObject {
public:
	struct TransformationMatrices {
		glm::mat4 translationMatrix, scaleMatrix, rotationMatrix;
	};

	// gets the vectors of vertices and colors corresponding to each vertex
	// and allocates storage for the underlying glBuffers
	DrawableObject(std::vector<glm::vec4> vertices, std::vector<glm::vec4> colors, GLenum drawMode) noexcept(false);
	~DrawableObject();

	void draw();

	DrawableObject& setPosition(double x, double y);
	DrawableObject& translateX(double deltaX);
	DrawableObject& translateY(double deltaY);

	DrawableObject& resetAngle();
	DrawableObject& rotate(float angle);

	DrawableObject& resetSize();
	DrawableObject& scaleX(double x);
	DrawableObject& scaleY(double y);

	DrawableObject& rotateAround(double x, double y);

	DrawableObject& overwriteVertices(std::vector<glm::vec4> vertices) noexcept(false);
	DrawableObject& overwriteColors(std::vector<glm::vec4> colors) noexcept(false);
	DrawableObject& overwriteColors(glm::vec4 color);
	DrawableObject& overwriteDrawMode(GLenum newDrawMode);

	// get the multiplied matrices
	glm::mat4 getTransformationMatrix();
	// get the separate matrices to multiply them inside the vertex shader
	TransformationMatrices getTransformationMatrices();

	double getAngle();

	// getters and setters
	GLuint getVertexBufferId() { return vertexBufferId; }
	GLuint getColorBufferId() { return colorBufferId; }

	GLuint getVertexBufferBindingId() { return vertexBufferBindingId; }
	GLuint getColorBufferBindingId() { return colorBufferBindingId; }

	void setVertexBufferBindingId(GLuint id) { vertexBufferBindingId = id; }
	void setColorBufferBindingId(GLuint id) { colorBufferBindingId = id; }

	friend class CarObject;
protected:
	// keep count of the number of vertices
	GLsizei vertexCount = 0;
	const GLsizei initialVertexCount = 0, initialColorsCount = 0;

	GLenum drawMode;
	GLuint vertexBufferId = 0, colorBufferId = 0;
	// keep the binding points
	GLuint vertexBufferBindingId = 0, colorBufferBindingId = 0;

	// the translation, scale and rotation of the drawable object
	glm::mat4 translationMatrix = glm::mat4(1),
		scaleMatrix = glm::mat4(1),
		rotationMatrix = glm::mat4(1);
	// option to translate before rotating, this way we don't rotate around the barrycenter of the model
	glm::vec3 rotationPoint = glm::vec3(0, 0, 0);
};