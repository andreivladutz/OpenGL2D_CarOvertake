#include "../Headers/Rectangle.h"

Rectangle::Rectangle(glm::vec4 color, float width, float height)
	: Rectangle(std::vector< glm::vec4 >(4, color), width, height) {}

Rectangle::Rectangle(
	std::vector< glm::vec4 > colors, float width, float height
) : DrawableObject(initPoints(width, height), colors, GL_QUADS),
	width(width), height(height) {}

std::vector<glm::vec4> Rectangle::initPoints(float width, float height) {
	return {
		bottomLeftPt = glm::vec4(glm::floor(-width / 2.), glm::floor(-height / 2.), 0, 1),
		bottomRightPt = glm::vec4(glm::round(width / 2.), glm::floor(-height / 2.), 0, 1),
		topRightPt = glm::vec4(glm::round(width / 2.), glm::round(height / 2.), 0, 1),
		topLeftPt = glm::vec4(glm::floor(-width / 2.), glm::round(height / 2.), 0, 1)
	};
}

void Rectangle::overwriteVertices(float width, float height) {
	DrawableObject::overwriteVertices(initPoints(width, height));
}

glm::vec4 Rectangle::getBottomLeftPt() {
	return getTransformationMatrix() * bottomLeftPt;
}

glm::vec4 Rectangle::getTopLeftPt() {
	return getTransformationMatrix() * topLeftPt;
}

glm::vec4 Rectangle::getBottomRightPt() {
	return getTransformationMatrix() * bottomRightPt;
}

glm::vec4 Rectangle::getTopRightPt() {
	return getTransformationMatrix() * topRightPt;
}