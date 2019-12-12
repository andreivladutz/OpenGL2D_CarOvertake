#pragma once
#include "CarObject.h"
#include <list>

class CarsHandler {
public:
	// the user controllable car and the others
	static CarObject* mainCarObject;
	static std::list<CarObject*> carObjects;

	static const std::vector<glm::vec4> carColors;

	static CarsHandler& initInstance(glm::vec4 mainCarColor);
	static CarsHandler& getInstance() noexcept(false);
	static void removeInstance();

	static void keyCallback(GLFWwindow*, int key, int, int action, int);

	static void translateCar(CarObject *car, double deltaTime);
	static void renderCars(double deltaTime);
protected:
	CarsHandler(glm::vec4 mainCarColor);
	~CarsHandler();

	static CarsHandler* instance;

	static const double maximumOutOfRangeTime;
	static const float overtakeDistance, dangerDistance;

	static bool mainCarCrashed;

	// limit the mainCar acceleration
	const int maximumAcceleration = 200;

	glm::vec4 mainCarColor;
};