#include "../Headers/CarsHandler.h"
#include "../glm/gtc/random.hpp"

#include <iostream>

CarsHandler* CarsHandler::instance = nullptr;
CarObject* CarsHandler::mainCarObject = nullptr;
std::list<CarObject*> CarsHandler::carObjects;

const double CarsHandler::maximumOutOfRangeTime = 4;
const float CarsHandler::overtakeDistance = 500;
const float CarsHandler::dangerDistance = 250;

bool CarsHandler::mainCarCrashed = false;

double redSemaphoreT = 3, passedT = 0;


const std::vector<glm::vec4> CarsHandler::carColors {
	glm::vec4(0.6, 0, 0, 1),
	glm::vec4(0, 0.6, 0.4, 1),
	glm::vec4(0.2, 0, 0.7, 1),
	glm::vec4(0.6, 0.6, 0, 1),
	glm::vec4(0.5, 0.2, 0.5, 1),

};

CarsHandler& CarsHandler::initInstance(glm::vec4 mainCarColor) {
	if (instance == nullptr) {
		instance = new CarsHandler(mainCarColor);

		glfwSetKeyCallback(Context2D::getInstance().getWindow(), keyCallback);
	}

	return *instance;
}

CarsHandler& CarsHandler::getInstance() noexcept(false) {
	if (!instance) {
		throw std::exception("The CarsHandler::instance has not been initialised. Call initInstance(carColor) first!");
	}

	return *instance;
}

void CarsHandler::removeInstance() {
	if (instance) {
		delete instance;
	}
}

CarsHandler::CarsHandler(glm::vec4 mainCarColor) : mainCarColor(mainCarColor) {
	mainCarObject = new CarObject(mainCarColor);

	mainCarObject->setPosition(0, DecorationsHandler::getInstance().getFirstLaneY());
}

CarsHandler::~CarsHandler() {
	for (CarObject* carObj : carObjects) {
		delete carObj;
	}

	while (!carObjects.empty()) {
		carObjects.pop_back();
	}

	delete mainCarObject;
}

void CarsHandler::keyCallback(GLFWwindow*, int key, int, int action, int) {
	if (mainCarCrashed) {
		return;
	}

	int& accelerate = mainCarObject->accelerate,
		maximumAcceleration = getInstance().maximumAcceleration;

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT) {
			mainCarObject->rotate(glm::pi<float>() / 18);
		}
		else if (key == GLFW_KEY_RIGHT) {
			mainCarObject->rotate(-glm::pi<float>() / 18);
		}
		else if (key == GLFW_KEY_A) {
			accelerate = glm::min(maximumAcceleration, accelerate + 100);
		}
		else if (key == GLFW_KEY_S) {
			accelerate = glm::max(0, accelerate - 100);
		}
	}
}

void CarsHandler::translateCar(CarObject* car, double deltaTime) {
	double offset = deltaTime * car->accelerate;

	double translateX = offset * glm::cos(car->getAngle()),
		translateY = offset * glm::sin(car->getAngle());

	car->
		translateX(translateX)
		.translateY(translateY);

	// the main car moves the camera
	if (car == mainCarObject) {
		Context2D::getInstance().translateCamera(-translateX, -translateY);
	}
}

void CarsHandler::renderCars(double deltaTime) {
	passedT += deltaTime;

	if (passedT <= redSemaphoreT) {
		mainCarObject->drawCar();

		return;
	}
	
	static double deltaTimeSinceLastSpawn = 0,
		spawnTime = glm::linearRand<double>(1.0, 4.0);

	deltaTimeSinceLastSpawn += deltaTime;

	if (!mainCarCrashed) {
		// handling the mainCar
		translateCar(mainCarObject, deltaTime);
	}

	mainCarObject->drawCar();

	if (deltaTimeSinceLastSpawn >= spawnTime) {
		deltaTimeSinceLastSpawn = 0;
		spawnTime = glm::linearRand<double>(4.0, 10.0);

		// get random color
		int colorsIndex = glm::linearRand<int>(0, carColors.size() - 1);
		CarObject *newCar = new CarObject(carColors[colorsIndex]), 
			*lastCar = carObjects.empty() ? nullptr : carObjects.back();
		carObjects.push_back(newCar);

		// position the new car in the first lane, on the left of the screen
		glm::vec2 mainCarPos = mainCarObject->getCarCenter();
		const DecorationsHandler& decHandler = DecorationsHandler::getInstance();

		float firstLaneY = decHandler.getFirstLaneY();
		newCar->setPosition(mainCarPos.x - (double)Context2D::getInstance().getScreenWidth() / 2, firstLaneY);

		if (lastCar != nullptr && newCar->isCollidingInto(*lastCar).hasCollision) {
			newCar->translateX(-newCar->carWidth);
		}

		// make the car faster than the mainCar
		newCar->accelerate = mainCarObject->accelerate + 100;
	}

	// first translate all cars
	for (CarObject* car : carObjects) {
		if (!mainCarCrashed) {
			translateCar(car, deltaTime);
		}
	}

	for (std::list<CarObject*>::iterator it = carObjects.begin(); it != carObjects.end(); ++it) {
		CarObject* car = *it;

		if (car->isCollidingInto(*mainCarObject).hasCollision) {
			mainCarCrashed = true;
		}

		if (!mainCarCrashed) {
			std::list<CarObject*>::iterator otherIt = it;
			++otherIt;

			// check if this car is going to crash in the other cars
			for (; otherIt != carObjects.end(); ++otherIt) {
				CarObject* otherCar = *otherIt;

				CollisionRectangle collisionResult = car->isCollidingInto(*otherCar);

				// if this car collides with the other cars, translate it back so it doesn't
				if (collisionResult.hasCollision) {
					glm::vec2 translation = collisionResult.minTranslationVector;

					car->translateX(translation.x);
					car->translateY(translation.y);
				}
			}
		}

		if (car->isInViewRange()) {
			// push mainCar in the list just for the sake of this verification. it will pe popped
			carObjects.push_front(mainCarObject);
			
			for (CarObject* otherCar : carObjects) {
				if (otherCar == car) {
					continue;
				}

				if (car->getDistance(*otherCar) <= dangerDistance && car->isBehindCar(*otherCar)) {
					car->accelerate = glm::max(0, car->accelerate - 100);
				}

				// if the car already started overtaking, let it finish
				if (car->overtakingCar) {
					car->overtakeCar(*car->carBeingOvertaken);

				}
				else if (car->getDistance(*otherCar) <= overtakeDistance && car->isBehindCar(*otherCar)) {
					if (otherCar->shouldBeOvertaken() && !car->overtakeCar(*otherCar) || !otherCar->shouldBeOvertaken()) {
						// check if we can overtake the otherCar, otherwise reduce the speed
						car->accelerate = glm::max(car->accelerate - 10, 0);
					}
				}
			}

			// pop the mainCar
			carObjects.pop_front();

			car->drawCar();
			car->wasOutOfRange = false; 
		}
		else {
			// the car has been out of the view range before this loop
			if (car->wasOutOfRange) {
				car->outOfViewRangeTime += deltaTime;
			}
			// first time out of viewRange
			else {
				car->outOfViewRangeTime = 0;
				car->wasOutOfRange = true;
			}

			if (car->outOfViewRangeTime >= maximumOutOfRangeTime) {
				carObjects.erase(it);

				delete car;
			}
		}
	}
}