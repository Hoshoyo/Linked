#pragma once
#include <iostream>
#include "mapObjectImageLoader.h"
#include "mapHeightImageLoader.h"

class Map
{
public:
	Map(std::string& mapObjectsFilename, std::string& mapHeightsFilename, int nChannels);
	~Map();
	MapCoordinate getMapCoordinate(glm::vec3 coordinate);
private:
	MapObjectImageLoader* mapObjectLoader;
	MapHeightImageLoader* mapHeightLoader;
};
