#include "Game.h"
#include "MapTerrainImageLoader.h"

#include "MapEntityImageLoader.h"
#include <string>
#include <iostream>

#include "Mesh.h"
#include "Grid.h"
#include "Input.h"

#include "Map.h"
#include "Display.h"

/*void Game::printCoordinate(int x, int y)
{
	std::string objectMapPath = "./res/Maps/objectmap.png";
	std::string heightMapPath = "./res/Maps/heightmap.png";
	std::string enumr = "teste";
	Map myOwnMap = Map(objectMapPath, heightMapPath, 3);
	MapCoordinate coord = myOwnMap.getMapCoordinate(glm::vec3(x, y, 0));
	switch (coord.object)
	{
	case NORMAL_FLOOR: enumr = "NORMAL_FLOOR"; break;
	case BLOCKED: enumr = "BLOCKED"; break;
	case HOLE: enumr = "HOLE"; break;
	case SPIKES: enumr = "SPIKES"; break;
	case FIRE: enumr = "FIRE"; break;
	case MUD: enumr = "MUD"; break;
	case SLIPPERY: enumr = "SLIPPERY"; break;
	default:
		enumr = "nenhum"; break;
	}
	std::cout << enumr << std::endl << std:: endl;
}*/

Game::Game(int windowsWidth, int windowsHeight)
{	
	// C�mera luz e shaders
	this->camera = new Camera(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), 70.0f, (float)windowsWidth / windowsHeight, 0.1f, 2500.0f);
	this->light = new Light(glm::vec3(100, 500, 10), glm::vec3(1, 1, 1));
	this->shader = new PrimitiveShader("./shaders/normalshader", camera);
	this->mapShader = new MapShader("./shaders/mapshader", camera);
	
	// Cria��o do player
	Mesh* playerMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 0.28f, 0.5f), new Texture("./res/Textures/clown.png"));
	Entity* player = new Entity(new Transform(glm::vec3(70, 980, 1.1f), 45, glm::vec3(1, 0, 0), glm::vec3(3, 3, 3)), playerMesh);
	entities.push_back(player);

	// Cria��o do Mapa
	std::string mapPath = "./res/Maps/maze.png";
	this->map = new Map(mapPath, mapPath, 3);
	Mesh* mapMesh = new Mesh(new Grid(1024, this->map),
		new Texture("./res/Maps/stonePath.png"),
		new Texture("./res/Maps/mountain.jpg"),
		new Texture("./res/Maps/water.jpg"),
		new Texture("./res/Maps/dirt.png"),
		new Texture(mapPath)
		);
	this->entityMap = new EntityMap(new Transform(), mapMesh);

	// Movimento
	playerMovement = new PlayerMovement(this->map);
}

Game::~Game()
{
	delete shader;
	delete camera;
	delete playerMovement;
}

void Game::render()
{
	((EntityMap*)entityMap)->render(mapShader, light);

	for (Entity* e : entities)
	{
		try{
			e->render(shader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
}

void Game::update()
{
	input();
	glm::vec3 camOri = glm::vec3(entities[0]->getTransform()->getPosition().x, entities[0]->getTransform()->getPosition().y, 0);
	glm::vec3 camPos = glm::vec3(entities[0]->getTransform()->getPosition().x, entities[0]->getTransform()->getPosition().y-10, 30);
	light->lightPosition.x = entities[0]->getTransform()->getPosition().x;
	light->lightPosition.y = entities[0]->getTransform()->getPosition().y;

	camera->setCamPosition(camPos);
	camera->setCamOrientation(camOri);
}

float charRot = 0;

void Game::input()
{
	float frameTime = (float)Display::frameTime;
	glm::vec3 finalPos;

	if (Input::keyStates['w'])
	{
		if (!Input::keyStates['a'] && !Input::keyStates['s'] && !Input::keyStates['d'])
		{
			if (playerMovement->moveTo(NORTH, entities[0]->getTransform()->getPosition(), frameTime, true, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (playerMovement->moveTo(NORTH, entities[0]->getTransform()->getPosition(), frameTime, false, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['a'])
	{
		if (!Input::keyStates['w'] && !Input::keyStates['s'] && !Input::keyStates['d'])
		{
			if (playerMovement->moveTo(WEST, entities[0]->getTransform()->getPosition(), frameTime, true, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (playerMovement->moveTo(WEST, entities[0]->getTransform()->getPosition(), frameTime, false, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['s'])
	{
		if (!Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['d'])
		{
			if (playerMovement->moveTo(SOUTH, entities[0]->getTransform()->getPosition(), frameTime, true, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (playerMovement->moveTo(SOUTH, entities[0]->getTransform()->getPosition(), frameTime, false, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['d'])
	{
		if (!Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['s'])
		{
			if (playerMovement->moveTo(EAST, entities[0]->getTransform()->getPosition(), frameTime, true, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (playerMovement->moveTo(EAST, entities[0]->getTransform()->getPosition(), frameTime, false, &finalPos))
				entities[0]->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['8'])
		light->lightPosition.y += 1.0f;
	if (Input::keyStates['2'])
		light->lightPosition.y -= 1.0f;
	if (Input::keyStates['6'])
		light->lightPosition.x += 1.0f;
	if (Input::keyStates['4'])
		light->lightPosition.x -= 1.0f;
	if (Input::keyStates['1'])
		light->lightPosition.z += 1.0f;
	if (Input::keyStates['9'])
		light->lightPosition.z -= 1.0f;

	if (Input::keyStates['r'])
	{
		charRot += 0.5f;
		entities[0]->getTransform()->rotate(charRot, glm::vec3(1, 0, 0));
	}

	if (Input::keyStates['l'])
	{
	if (Mesh::drawForm == GL_TRIANGLES)
		Mesh::drawForm = GL_LINES;
	else
		Mesh::drawForm = GL_TRIANGLES;
	}
}
