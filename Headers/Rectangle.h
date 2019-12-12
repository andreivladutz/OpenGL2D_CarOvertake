#pragma once
#include "DrawableObject.h"

class Rectangle : public DrawableObject {
public:
	Rectangle(glm::vec4 color, float width, float height);
	Rectangle(std::vector<glm::vec4> colors, float width, float height);

	std::vector<glm::vec4> initPoints(float width, float height);
	void overwriteVertices(float width, float height);

	glm::vec4 getBottomLeftPt();
	glm::vec4 getTopLeftPt();
	glm::vec4 getBottomRightPt();
	glm::vec4 getTopRightPt();

	friend class CarObject;
protected:
	float width, height;
	glm::vec4 bottomLeftPt, topLeftPt,
		bottomRightPt, topRightPt;
};