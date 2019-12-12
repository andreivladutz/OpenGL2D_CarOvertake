#include "../Headers/CarObject.h"
#include "../Headers/CarsHandler.h"
#include "../glm/gtx/norm.hpp"
#include <iostream>
#include <limits>
#include <list>

constexpr float negInf = std::numeric_limits<float>::lowest(),
	posInf = std::numeric_limits<float>::max();

float CarObject::topYRoad = 0,
	CarObject::bottomYRoad = 0;

Rectangle* CarObject::carBody = nullptr, * CarObject::wheel = nullptr;
Circle* CarObject::headlight = nullptr;

int CarObject::carInstances = 0;

CarObject::CarObject(glm::vec4 color, float width, float height)
: carWidth(width), carHeight(height), wheelWidth(width / 4),
wheelHeight(height / 3), headlightRadius(width / 20), ctx(Context2D::getInstance()) {
	// the first instance of CarObject also inits the static resources (the drawable entities)
	if (carInstances == 0) {
		carBody = new Rectangle(color, width, height);
		wheel = new Rectangle(blackColor, wheelWidth, wheelHeight);
		headlight = new Circle(headlightRadius, noCirclePoints, headlightColor);
	}

	carInstances++;

	glm::vec4 wheelOffset = glm::vec4(wheelWidth / 2 + wheelWidth / 10, 0, 0, 0), 
		headlightOffset = glm::vec4(0, height / 5, 0, 0);

	wheelPositions = {
		carBody->bottomLeftPt + wheelOffset,
		carBody->bottomRightPt - wheelOffset,
		carBody->topRightPt - wheelOffset,
		carBody->topLeftPt + wheelOffset
	};

	lftHeadlightPos = carBody->topRightPt - headlightOffset;
	rightHeadlightPos = carBody->bottomRightPt + headlightOffset;

	carColor = color;

	if (topYRoad == 0) {
		topYRoad = DecorationsHandler::getInstance().roadHeight / 2;
		bottomYRoad = - DecorationsHandler::getInstance().roadHeight / 2;
	}
}

CarObject::~CarObject() {
	carInstances--;

	// the last instance of the carObjects also deletes the shared resources
	if (carInstances == 0) {
		delete carBody;
		delete wheel;
		delete headlight;
	}
}

CarObject& CarObject::positionObjectAndDraw(DrawableObject &obj, glm::vec4& position) {
	glm::vec4 rotatedPosition = obj.rotationMatrix * position;

	obj.translateX(rotatedPosition[0]);
	obj.translateY(rotatedPosition[1]);

	ctx.drawObject(obj);

	obj.translateX(-rotatedPosition[0]);
	obj.translateY(-rotatedPosition[1]);

	return *this;
}

CarObject& CarObject::drawCar() {
	applyTransformations();
	updateBlinkingStatus();

	carBody->overwriteColors(carColor);
	ctx.drawObject(*carBody);

	// Draw the car window
	glm::vec4 windowColor = carColor - glm::vec4(0.2, 0.2, 0.2, 0.2);

	for (int i = 0; i < 4; i++) {
		windowColor[i] = glm::max(0.f, windowColor[i]);
	}

	constexpr float perpendicularAngle = glm::pi<float>() / 2;

	wheel->overwriteColors(windowColor)
		.scaleX(2)
		.rotate(perpendicularAngle);

	glm::vec4 windowPosition = glm::vec4(0, carBody->bottomLeftPt.x + wheelHeight / 2 + wheelHeight / 10, 0, 1);

	positionObjectAndDraw(*wheel, windowPosition);

	wheel->rotate(-perpendicularAngle)
		.overwriteColors(blackColor)
		.resetSize();

	// draw the headlights
	if (blinkOn && signalingLeft) {
		headlight->overwriteColors(headlightOnColor);
	}
	else {
		headlight->overwriteColors(headlightColor);
	}
	positionObjectAndDraw(*headlight, lftHeadlightPos);


	if (blinkOn && signalingRight) {
		headlight->overwriteColors(headlightOnColor);
	}
	else {

		headlight->overwriteColors(headlightColor);
	}
	positionObjectAndDraw(*headlight, rightHeadlightPos);

	// draw the wheels
	for (glm::vec4& pos: wheelPositions) {
		positionObjectAndDraw(*wheel, pos);
	}

	return *this;
}

void CarObject::applyTransformations() {
	std::vector<DrawableObject*> objects{ carBody, wheel, headlight };

	for (DrawableObject* obj : objects) {
		obj->setPosition(0, 0)
			.translateX(translatedX)
			.translateY(translatedY)
			.resetAngle()
			.rotate(angle);
	}
}

CarObject& CarObject::setPosition(double x, double y) {
	translatedX = x;
	translatedY = y;
	/*carBody.setPosition(x, y);
	wheel.setPosition(x, y);
	headlight.setPosition(x, y);*/

	return *this;
}

CarObject& CarObject::translateX(double deltaX) {
	translatedX += deltaX;
	/*carBody.translateX(deltaX);
	wheel.translateX(deltaX);
	headlight.translateX(deltaX);*/

	return *this;
}

CarObject& CarObject::translateY(double deltaY) {
	translatedY += deltaY;
	/*carBody.translateY(deltaY);
	wheel.translateY(deltaY);
	headlight.translateY(deltaY);*/

	return *this;
}


CarObject& CarObject::resetAngle() {
	angle = 0;
	/*carBody.resetAngle();
	wheel.resetAngle();
	headlight.resetAngle();*/

	return *this;
}

CarObject& CarObject::rotate(float angle) {
	this->angle += angle;
	/*carBody.rotate(angle);
	wheel.rotate(angle);
	headlight.rotate(angle);*/

	if (angle > 0) {
		signal(CarObject::direction::LEFT);
	}
	else if (angle < 0) {
		signal(CarObject::direction::RIGHT);
	}

	return *this;
}

double CarObject::getAngle() {
	return angle;
}

void CarObject::resetSignalTime() {
	signalStartTime = lastTimeBlinked = (float)glfwGetTime();
}


void CarObject::updateBlinkingStatus() {
	// not signaling => not blinking
	if (!signalingLeft && !signalingRight) {
		return;
	}

	float currTime = (float)glfwGetTime();

	if (currTime - signalStartTime >= SIGNALING_TIME) {
		blinkOn = signalingLeft = signalingRight = false;
			
		return;
	}

	if (currTime - lastTimeBlinked >= BLINK_TIME) {
		if (blinkOn == true) {
			blinkOn = false;
		}
		else if (blinkOn == false) {
			blinkOn = true;
		}
		lastTimeBlinked = currTime;
	}
}

void CarObject::signal(CarObject::direction dir) {
	CarObject::direction oppositeDir = (dir == CarObject::direction::LEFT) ? 
		CarObject::direction::RIGHT : CarObject::direction::LEFT;

	if (oppositeDir == CarObject::direction::RIGHT && signalingRight) {
		signalingRight = false;

		resetSignalTime();
	}

	if (oppositeDir == CarObject::direction::LEFT && signalingLeft) {
		signalingLeft = false;

		resetSignalTime();
	}

	if (!signalingLeft && !signalingRight) {
		resetSignalTime();
	}

	if (dir == CarObject::direction::LEFT) {
		signalingLeft = true;
	}
	else {
		signalingRight = true;
	}
}

bool CarObject::isInViewRange() {
	return DecorationsHandler::RectVertices(
		carBody->width, carBody->height, glm::vec2(translatedX, translatedY)
	).isInViewRange(Context2D::getInstance());
}

glm::vec2 CarObject::getCarCenter() {
	return {
		translatedX,
		translatedY
	};
}

float CarObject::getDistance(CarObject& otherCar) {
	glm::vec3 deltaVec = glm::vec3(getCarCenter() - otherCar.getCarCenter(), 0);

	return glm::l2Norm(deltaVec);
}

bool CarObject::isLaneFree(float bottomY, float topY) {
	std::list<CarObject*>& carObjects = CarsHandler::carObjects;

	float leftX = translatedX - carWidth - carWidth / 2,
		rightX = translatedX + carWidth + carWidth / 2;

	for (CarObject* car : carObjects) {
		if (car == this || car == nullptr) {
			continue;
		}

		// if another car is in the lane in the range of our translatedX then the lane is not free
		if (car->translatedY >= bottomY && car->translatedY <= topY
			&& car->translatedX >= leftX && car->translatedX <= rightX) {
			return false;
		}
	}

	return true;
}

bool CarObject::overtakeCar(CarObject& car) {
	// can we overtake?
	if (!overtakingCar && !isLaneFree(0, topYRoad)) {
		return false;
	}

	// the overtaking just started now
	if (!overtakingCar) {
		overtakingCar = true;
		carBeingOvertaken = &car;

		deltaXBefore = translatedX - car.translatedX;

		rotate(PI / 4);
	}

	// got to the second lane so we can go straight now
	if (isInSecondLane()) {
		resetAngle();
	}

	float newDeltaX = translatedX - car.translatedX;

	// the car can return to the first lane now
	if (abs(newDeltaX) > abs(deltaXBefore) && glm::sign(newDeltaX) * glm::sign(deltaXBefore) == -1 
		&& isLaneFree(bottomYRoad, 0)) {
		// turn right
		if (angle == 0) {
			rotate(-PI / 4);

			returningToFirstLane = true;
		}
	}

	if (returningToFirstLane) {
		// if the car already turned right and
		// it got to the first lane so it can go straight now
		if (isInFirstLane()) {
			resetAngle();

			overtakingCar = false;
			returningToFirstLane = false;
			carBeingOvertaken = nullptr;

			// finished overtaking the car
		}
	}

	return true;
}

CollisionRectangle CarObject::getCollisionRect() {
	applyTransformations();

	glm::vec4 btLft = carBody->getBottomLeftPt(),
		btRght = carBody->getBottomRightPt(),
		topRght = carBody->getTopRightPt(),
		topLft = carBody->getTopLeftPt();

	return CollisionRectangle(
		{ btLft.x, btLft.y }, { btRght.x, btRght.y }, { topRght.x, topRght.y }, { topLft.x, topLft.y }
	);
}

CollisionRectangle CarObject::isCollidingInto(CarObject& otherCar) {
	// apply transformations to get the transformed coords of the points
	// for this car
	CollisionRectangle thisCarRect = getCollisionRect(),
		// for the other car
		otherCarRect = otherCar.getCollisionRect();

	return thisCarRect.isColliding(otherCarRect);
}

CollisionRectangle::CollisionRectangle(glm::vec2 btLft, glm::vec2 btRght, glm::vec2 topRght, glm::vec2 topLft)
: btLft(btLft), btRght(btRght), topRght(topRght), topLft(topLft), minTranslationVector() {}

CollisionRectangle CollisionRectangle::isColliding(CollisionRectangle& other) {
	// only 2 edges are sufficient as the other two are parallel to these two
	std::vector<glm::vec2> edges {
		// this edges
		btRght - btLft, 
		topRght - btRght,
		// the other rectangle's edges
		other.btRght - other.btLft, 
		other.topRght - other.btRght
	}, thisPts{
		btLft, btRght, topRght, topLft
	}, otherPts{
		other.btLft, other.btRght, other.topRght, other.topLft
	};

	hasCollision = true;

	glm::vec2 otherToThisVector = getCenter() - other.getCenter();
	float minimumIntervalDistance = posInf;

	glm::vec2 translationAxis;

	// we don't have to compute the perpendicular axis to an edge of the rect,
	// as it is represented by the other edge(they are perpendicular)
	for (glm::vec2 normal : edges) {
		normal = glm::normalize(normal);

		// the heads of the intervals
		float minA = posInf, maxA = negInf, minB = posInf, maxB = negInf;

		// project all the points to the normal
		for (glm::vec2 pt : thisPts) {
			float projection = glm::dot(pt, normal);

			if (projection < minA) {
				minA = projection;
			}
			
			if (projection > maxA) {
				maxA = projection;
			}
		}

		for (glm::vec2 pt : otherPts) {
			float projection = glm::dot(pt, normal);

			if (projection < minB) {
				minB = projection;
			}
			
			if (projection > maxB) {
				maxB = projection;
			}
		}

		// if the intervals don't intersect =>(Separating Axis Th) that the rectangles don't intersect
		if (minB > maxA || maxB < minA) {
			hasCollision = false;
			break;
		}

		// they intersect
		float distBetweenInt = glm::abs(intervalDistance(minA, maxA, minB, maxB));

		if (distBetweenInt < minimumIntervalDistance) {
			minimumIntervalDistance = distBetweenInt;

			// if the normal has an opposite direction take the negated normal
			if (glm::dot(otherToThisVector, normal) < 0) {
				translationAxis = -normal;
			}
			else {
				translationAxis = normal;
			}
		}
	}

	if (hasCollision) {
		minTranslationVector = translationAxis * minimumIntervalDistance;
	}

	return *this;
}

float CollisionRectangle::intervalDistance(float minA, float maxA, float minB, float maxB) {
	// interval A is to the left of interval B
	if (minA < minB) {
		return minB - maxA;
	}
	else {
		return minA - maxB;
	}
}

glm::vec2 CollisionRectangle::getCenter() {
	return (btLft + btRght + topRght + topLft) / 4.f;
}