#pragma once
#include "Rectangle.h"
#include "Context2D.h"
#include <deque>

// class that draws all the other objects on the screen
class DecorationsHandler {
public:
	struct RectVertices {
		glm::vec2 topLeft, bottomRight, center;
		float width, height;

		RectVertices(float width, float height, glm::vec2 center);

		// check if a rectangle is in the viewport
		bool isInViewRange(Context2D&);
	};

	const glm::vec4 roadColor = glm::vec4(0.4, 0.4, 0.4, 1),
		stripColor = glm::vec4(1, 0.94, 0.83, 1);

	// the details about the road are public
	float roadWidth, roadHeight,
		stripWidth, stripHeight, stripDistance;

	static DecorationsHandler& getInstance();
	static void instanceDestroy();

	DecorationsHandler& drawDecorations();


	void generateRoads();
	void addFrontHorizontalRoad();
	void addBackHorizontalRoad();

	void generateStrips();
	void addFrontHorizontalStrips();
	void addBackHorizontalStrips();

	void resetCenteredRoad();
	void resetCenteredStrip();

	float getSecondLaneY() const {
		return (roadHeight / 2. - stripWidth / 2) / 2 + roadHeight / 8;
	}

	float getFirstLaneY() const {
		return -getSecondLaneY();
	}
protected:
	DecorationsHandler(float roadWidth = Context2D::getInstance().getScreenWidth() / 3, float roadHeight = Context2D::getInstance().getScreenWidth() / 3);
	~DecorationsHandler();

	static DecorationsHandler* instance;
	Context2D& ctx = Context2D::getInstance();

	// the object whose vertices will be ovewritten
	Rectangle decorationObject;
	// know if the vertices should be overwritten
	bool lastDrewRoads = true;
	
	std::deque<RectVertices*> horizontalRoads, horizontalStrips;
};