#pragma once

#include "Rectangle.h"
#include "Circle.h"
#include "Context2D.h"
#include "DecorationsHandler.h"

const glm::vec4 blackColor = glm::vec4(0, 0, 0, 1);

struct CollisionRectangle;

class CarObject
{
public:
	static float topYRoad, bottomYRoad;

	enum class direction {LEFT, RIGHT};
	const float SIGNALING_TIME = 2, BLINK_TIME = 0.3f;

	float carWidth, carHeight;

	// initial acceleration is 100
	int accelerate = 100;

	// keep count how long the other cars have been out of camera view
	double outOfViewRangeTime = 0;
	bool wasOutOfRange = false;

	CarObject(
		glm::vec4 color,
		float width = DecorationsHandler::getInstance().roadWidth / 2.3,
		float height = DecorationsHandler::getInstance().roadHeight / 4
	);

	~CarObject();

	CarObject& drawCar();
	CarObject& positionObjectAndDraw(DrawableObject& obj, glm::vec4 &position);

	CarObject& scaleX(double) = delete;
	CarObject& scaleY(double) = delete;

	CarObject& setPosition(double x, double y);
	CarObject& translateX(double deltaX);
	CarObject& translateY(double deltaY);

	CarObject& resetAngle();
	CarObject& rotate(float angle);

	bool isInViewRange();

	double getAngle();
	glm::vec2 getCarCenter();

	// get the distance from this car to another car
	float getDistance(CarObject &otherCar);

	void signal(direction dir);

	bool shouldBeOvertaken() {
		return translatedY - carHeight / 2 <= DecorationsHandler::getInstance().getFirstLaneY()
			&& translatedY + carHeight / 2 >= - DecorationsHandler::getInstance().roadHeight / 2;
	}

	bool isInFirstLane() {
		return translatedY <= DecorationsHandler::getInstance().getFirstLaneY();
	}

	bool isInSecondLane() {
		return translatedY >= DecorationsHandler::getInstance().getSecondLaneY();
	}

	bool isBehindCar(CarObject& otherCar) {
		return translatedX <= otherCar.translatedX;
	}

	bool isLaneFree(float bottomY, float topY);

	CollisionRectangle getCollisionRect();
	// the colide result will be stored in the collisionRectangle returned
	CollisionRectangle isCollidingInto(CarObject& otherCar);
	
	// returns true if the overtaking has finished
	bool overtakeCar(CarObject& car);

	bool overtakingCar = false;
	bool returningToFirstLane = false;

	CarObject* carBeingOvertaken = nullptr;
private:
	// the delta between car xes before the begining of the overtaking
	float deltaXBefore = 0;

	void resetSignalTime();
	void updateBlinkingStatus();

	// apply the transformations of this car to the global drawable objects
	void applyTransformations();

	bool signalingLeft = false, signalingRight = false, blinkOn = false;
	float lastTimeBlinked = 0, signalStartTime = 0;

	const int noCirclePoints = 100;
	const glm::vec4 headlightColor = glm::vec4(1, 0.5, 0.3, 1),
		headlightOnColor = glm::vec4(1, 0, 0, 1);

	float wheelWidth, wheelHeight, headlightRadius;

	// keep count of the angle and translation of the car
	double angle = 0, translatedX = 0, translatedY = 0;

	// these are shared resources
	static Rectangle *carBody, *wheel;
	static Circle *headlight;
	
	static int carInstances;

	Context2D &ctx;

	std::vector<glm::vec4> wheelPositions;
	glm::vec4 carColor, lftHeadlightPos, rightHeadlightPos;
};

struct CollisionRectangle {
	glm::vec2 btLft, btRght, topRght, topLft;

	bool hasCollision = true;
	// the minimum translation vector to stop the rectangles from intersecting
	glm::vec2 minTranslationVector;

	CollisionRectangle(glm::vec2 btLft, glm::vec2 btRght, glm::vec2 topRght, glm::vec2 topLft);
	// it returns itself
	CollisionRectangle isColliding(CollisionRectangle& other);

	glm::vec2 getCenter();
	float intervalDistance(float minA, float maxA, float minB, float maxB);
};