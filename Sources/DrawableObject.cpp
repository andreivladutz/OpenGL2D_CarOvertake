#include "../Headers/DrawableObject.h"
#include "../Headers/Context2D.h"
// #include <iostream>

DrawableObject::DrawableObject(
	std::vector<glm::vec4> vertices,
	std::vector<glm::vec4> colors,
	GLenum drawMode
) noexcept(false) : drawMode(drawMode),
vertexCount(vertices.size()), initialVertexCount(vertices.size()), initialColorsCount(colors.size()) {
	// create the buffers for the object
	glCreateBuffers(1, &vertexBufferId);
	glCreateBuffers(1, &colorBufferId);

	// allocate the buffers and copy data
	glNamedBufferStorage(vertexBufferId, vertices.size() * sizeof(glm::vec4), &vertices[0][0], GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(colorBufferId, colors.size() * sizeof(glm::vec4), &colors[0][0], GL_DYNAMIC_STORAGE_BIT);

	// bind the buffers. if the maximum number of buffer bindings has been reached, bindDrawableObject will throw
	try {
		Context2D::getInstance().bindDrawableObject(*this);
	}
	catch (std::exception & err) {
		// just throw it outside
		throw err;
	}
	/*
	std::cout << "Vertices\n";
	for (auto& vertex : vertices) {
		std::cout << glm::to_string(vertex) << std::endl;
	}
	std::cout << "Colors\n";
	for (auto& color : colors) {
		std::cout << glm::to_string(color) << std::endl;
	}
	*/
}

DrawableObject::~DrawableObject() {
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &colorBufferId);
}

DrawableObject& DrawableObject::setPosition(double x, double y) {
	translationMatrix = glm::translate(glm::mat4(1), glm::vec3(x, y, 0));

	return *this;
}

DrawableObject& DrawableObject::translateX(double deltaX) {
	translationMatrix = glm::translate(translationMatrix, glm::vec3(deltaX, 0, 0));

	return *this;
}

DrawableObject& DrawableObject::translateY(double deltaY) {
	translationMatrix = glm::translate(translationMatrix, glm::vec3(0, deltaY, 0));

	return *this;
}

DrawableObject& DrawableObject::resetAngle() {
	rotationMatrix = glm::mat4(1);

	return *this;
}

DrawableObject& DrawableObject::rotate(float angle) {
	rotationMatrix = glm::rotate(rotationMatrix, angle, glm::vec3(0, 0, 1));

	return *this;
}

DrawableObject& DrawableObject::resetSize() {
	scaleMatrix = glm::mat4(1);

	return *this;
}

DrawableObject& DrawableObject::scaleX(double x) {
	scaleMatrix = glm::scale(scaleMatrix, glm::vec3(x, 1, 1));

	return *this;
}

DrawableObject& DrawableObject::scaleY(double y) {
	scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, y, 1));

	return *this;
}

DrawableObject& DrawableObject::rotateAround(double x, double y) {
	rotationPoint = glm::vec3(x, y, 0);

	return *this;
}

DrawableObject::TransformationMatrices DrawableObject::getTransformationMatrices() {
	// scale, rotate around specified point and then translate the model to the world space
	glm::mat4 rotationPointTranslation = glm::translate(-rotationPoint),
		rotationPointTranslateBack = glm::translate(rotationPoint);

	return {
		translationMatrix = translationMatrix,
		scaleMatrix = scaleMatrix,
		rotationMatrix = rotationPointTranslateBack * rotationMatrix * rotationPointTranslation
	};
}

glm::mat4 DrawableObject::getTransformationMatrix() {
	// scale, rotate around specified point and then translate the model to the world space
	glm::mat4 rotationPointTranslation = glm::translate(-rotationPoint),
		rotationPointTranslateBack = glm::translate(rotationPoint);

	return translationMatrix * rotationPointTranslateBack * rotationMatrix * rotationPointTranslation * scaleMatrix;
}

double DrawableObject::getAngle() {
	double sign = glm::sign(glm::asin(rotationMatrix[0][1]));

	// set the sign to 1 for 180 degrees
	if (sign == 0) {
		sign = 1;
	}

	return ((double)glm::acos(rotationMatrix[0][0])) * sign;
}

void DrawableObject::draw() {
	glDrawArrays(drawMode, 0, vertexCount);
}

DrawableObject& DrawableObject::overwriteVertices(std::vector<glm::vec4> vertices) noexcept(false) {
	if (initialVertexCount < (GLsizei) vertices.size()) {
		throw new std::exception("Cannot update smaller buffer's storage with more vertices than allocated for");
	}

	vertexCount = vertices.size();
	glNamedBufferSubData(vertexBufferId, 0, vertices.size() * sizeof(glm::vec4), &vertices[0]);

	return *this;
}

DrawableObject& DrawableObject::overwriteColors(std::vector<glm::vec4> colors) noexcept(false) {
	if (initialColorsCount < (GLsizei) colors.size()) {
		throw new std::exception("Cannot update smaller buffer's storage with more colors than allocated for");
	}

	glNamedBufferSubData(colorBufferId, 0, colors.size() * sizeof(glm::vec4), &colors[0]);

	return *this;
}

DrawableObject& DrawableObject::overwriteColors(glm::vec4 color) {
	std::vector <glm::vec4> colors(initialColorsCount, color);

	glNamedBufferSubData(colorBufferId, 0, initialColorsCount * sizeof(glm::vec4), &colors[0]);

	return *this;
}

DrawableObject& DrawableObject::overwriteDrawMode(GLenum newDrawMode) {
	drawMode = newDrawMode;

	return *this;
}