#include "../Headers/DecorationsHandler.h"

#include<iostream>

DecorationsHandler* DecorationsHandler::instance = nullptr;

DecorationsHandler::RectVertices::RectVertices(float width, float height, glm::vec2 center)
: center(center), width(width), height(height) {
	topLeft = glm::vec2(center.x - width / 2, center.y + height / 2);
	bottomRight = glm::vec2(center.x + width / 2, center.y - height / 2);
}

bool DecorationsHandler::RectVertices::isInViewRange(Context2D &ctx) {
	std::pair<glm::vec2, glm::vec2> viewportCoords = ctx.getViewportCoords();
	glm::vec2& topLeftView = viewportCoords.first, & bottomRightView = viewportCoords.second;

	return topLeft.x <= bottomRightView.x && bottomRight.x >= topLeftView.x &&
		topLeft.y >= bottomRightView.y && bottomRight.y <= topLeftView.y;
}

DecorationsHandler::DecorationsHandler(float roadWidth, float roadHeight)
: roadWidth(glm::floor(roadWidth)), roadHeight(glm::floor(roadHeight)),
stripWidth(glm::floor(roadWidth / 3)), stripHeight(glm::floor(roadHeight / 10)), stripDistance(glm::floor(stripWidth / 2)),
decorationObject(roadColor, roadWidth, roadHeight), horizontalRoads(), horizontalStrips() {
	generateRoads();
	generateStrips();
}

DecorationsHandler::~DecorationsHandler() {
	while (!horizontalRoads.empty()) {
		RectVertices* road = horizontalRoads.front();
		horizontalRoads.pop_front();

		delete road;
	}
}

DecorationsHandler& DecorationsHandler::getInstance() {
	if (DecorationsHandler::instance == nullptr) {
		instance = new DecorationsHandler();
	}

	return *instance;
}

DecorationsHandler& DecorationsHandler::drawDecorations() {
	generateRoads();
	generateStrips();

	// std::cout << "ROADS LENGTH = " << horizontalRoads.size() << "\n";
	// std::cout << "STRIPS LENGTH = " << horizontalStrips.size() << "\n";

	for (RectVertices *vertices : horizontalRoads) {
		if (!lastDrewRoads) {
			decorationObject.overwriteVertices(roadWidth, roadHeight);
			decorationObject.overwriteColors(roadColor);

			lastDrewRoads = true;
		}

		// translate the road to the center of the vertices Rect
		decorationObject.setPosition(vertices->center.x, vertices->center.y);


		ctx.drawObject(decorationObject);
	}

	for (RectVertices* vertices : horizontalStrips) {
		if (lastDrewRoads) {
			decorationObject.overwriteVertices(stripWidth, stripHeight);
			decorationObject.overwriteColors(stripColor);

			lastDrewRoads = false;
		}

		decorationObject.setPosition(vertices->center.x, vertices->center.y);
		
		ctx.drawObject(decorationObject);
	}

	return *this;
}

void DecorationsHandler::generateRoads() {
	addFrontHorizontalRoad();
	addBackHorizontalRoad();

	// check if leftmost and rightmost road rects are in view range
	while (!horizontalRoads.empty() && !horizontalRoads.front()->isInViewRange(ctx)) {
		RectVertices* leftRoad = horizontalRoads.front();

		horizontalRoads.pop_front();

		delete leftRoad;
	}

	while (!horizontalRoads.empty() && !horizontalRoads.back()->isInViewRange(ctx)) {
		RectVertices* rightRoad = horizontalRoads.back();

		horizontalRoads.pop_back();

		delete rightRoad;
	}
}

void DecorationsHandler::generateStrips() {
	addFrontHorizontalStrips();
	addBackHorizontalStrips();

	// check if leftmost and rightmost road strips are in view range
	while (!horizontalStrips.empty() && !horizontalStrips.front()->isInViewRange(ctx)) {
		RectVertices* leftStrip = horizontalStrips.front();

		horizontalStrips.pop_front();

		delete leftStrip;
	}

	while (!horizontalStrips.empty() && !horizontalStrips.back()->isInViewRange(ctx)) {
		RectVertices* rightStrip = horizontalStrips.back();

		horizontalStrips.pop_back();

		delete rightStrip;
	}
}

void DecorationsHandler::addFrontHorizontalStrips() {
	resetCenteredStrip();

	RectVertices* leftmostStrip = horizontalStrips.front();

	do {
		horizontalStrips.push_front(
			new RectVertices(stripWidth, stripHeight, leftmostStrip->center - glm::vec2(stripDistance + stripWidth, 0))
		);

		leftmostStrip = horizontalStrips.front();
	} while (leftmostStrip->isInViewRange(ctx));

}

void DecorationsHandler::addBackHorizontalStrips() {
	resetCenteredStrip();

	RectVertices* rightmostStrip = horizontalStrips.back();

	rightmostStrip = new RectVertices(stripWidth, stripHeight, rightmostStrip->center + glm::vec2(stripDistance + stripWidth, 0));
	while (rightmostStrip->isInViewRange(ctx)) {
		horizontalStrips.push_back(rightmostStrip);

		rightmostStrip = new RectVertices(stripWidth, stripHeight, rightmostStrip->center + glm::vec2(stripDistance + stripWidth, 0));
	}

	// the last rightmost strip is not pushed (out of view range)
	delete rightmostStrip;
}

void DecorationsHandler::addFrontHorizontalRoad() {
	resetCenteredRoad();

	RectVertices* leftmostRoad = horizontalRoads.front();
	 
	horizontalRoads.push_front(new RectVertices(roadWidth, roadHeight, leftmostRoad->center - glm::vec2(roadWidth - 5, 0)));
}

void DecorationsHandler::addBackHorizontalRoad() {
	resetCenteredRoad();

	RectVertices* rightmostRoad = horizontalRoads.back();

	horizontalRoads.push_back(new RectVertices(roadWidth, roadHeight, rightmostRoad->center + glm::vec2(roadWidth - 5, 0)));
}

void DecorationsHandler::resetCenteredRoad() {
	if (!horizontalRoads.empty()) {
		return;
	}

	std::pair<glm::vec2, glm::vec2> viewportCoords = ctx.getViewportCoords();

	float centerX = (viewportCoords.first.x + viewportCoords.second.x) / 2,
		centerY = (viewportCoords.first.y + viewportCoords.second.y) / 2;

	horizontalRoads.push_back(new RectVertices(roadWidth, roadHeight, glm::vec2(centerX, 0)));
}

void DecorationsHandler::resetCenteredStrip() {
	if (!horizontalStrips.empty()) {
		return;
	}

	std::pair<glm::vec2, glm::vec2> viewportCoords = ctx.getViewportCoords();

	float centerX = (viewportCoords.first.x + viewportCoords.second.x) / 2,
		centerY = (viewportCoords.first.y + viewportCoords.second.y) / 2;

	horizontalStrips.push_back(new RectVertices(stripWidth, stripHeight, glm::vec2(centerX, 0)));
}

void DecorationsHandler::instanceDestroy() {
	delete DecorationsHandler::instance;
}