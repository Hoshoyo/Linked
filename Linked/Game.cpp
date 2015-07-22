#include "Game.h"

#include "Display.h"
#include "Time.h"
#include "Input.h"

#include "Map.h"
#include "MapTerrain.h"
#include "MapEntity.h"

#include "Mesh.h"
#include "Grid.h"
#include "EntityMap.h"
#include "MonsterFactory.h"
#include "GameEntityFactory.h"
#include "Projectile.h"

#include "Camera.h"
#include "PrimitiveShader.h"
#include "MapShader.h"
#include "GUIShader.h"
#include "CommonShader.h"
#include "Light.h"

#include "RangeAttack.h"

#include "Player.h"
#include "HPBar.h"

#include "Text.h"

#include <string>
#include <iostream>
#include <cstdlib>

//#define DEBUG

Text* text;
Entity* gui;

Game::Game(int windowsWidth, int windowsHeight)
{	
	// C�mera luz e shaders
	this->camera = new Camera(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), 70.0f, (float)windowsWidth / windowsHeight, 0.1f, 100.0f);
	this->light = new Light(glm::vec3(100, 500, 50), glm::vec3(1, 1, 1));
	this->primitiveShader = new PrimitiveShader("./shaders/normalshader", camera, light);
	this->commonShader = new CommonShader("./shaders/commonshader", camera, light);
	this->mapShader = new MapShader("./shaders/mapshader", camera, light);
	this->fontShader = new GUIShader("./shaders/fontshader");
	
	// Cria��o do Mapa
	std::string mapPath = "./res/Maps/teste.png";
	std::string entitiesMapPath = "./res/Maps/entities.png";
	std::string monsterMapPath = "./res/Maps/monsters.png";

	this->monsterFactory = new MonsterFactory();
	this->gameEntityFactory = new GameEntityFactory();

	this->map = new Map(mapPath, entitiesMapPath, monsterMapPath, 3, monsterFactory, gameEntityFactory);

	// Cria��o do player
	Mesh* playerMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f, 7, 7));
	player = new Player(new Transform(glm::vec3(520, 500, 1.5f), 45, glm::vec3(1, 0, 0), glm::vec3(2, 2, 2)), playerMesh, new Texture("./res/Monsters/Sprites/greenwarrior.png"));
	this->rangeAttack = new RangeAttack(player, &attacks, &monsters, map);
	player->setRangeAttack(this->rangeAttack);
	player->setMaximumHpBasis(100);
	player->setHp(100);
	player->setDefenseBasis(100);
	entities.push_back(player);

	// Temp GUI AND TEXT
	Mesh* guiMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f));
	gui = new Entity(new Transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)), guiMesh, new Texture("./res/GUI/Linked_GUI.png"));
	text = new Text("Hoshoyo", 0.03f);

	Mesh* mapMesh = new Mesh(new Grid(MAP_SIZE, map));
	this->entityMap = new EntityMap(new Transform(), mapMesh,
		new Texture("./res/Maps/stonePath.png"),
		new Texture("./res/Maps/mountain.jpg"),
		new Texture("./res/Maps/water.jpg"),
		new Texture("./res/Maps/grassTex.png"),
		new Texture(mapPath));

	// Cria��o dos Monstros e das Entidades
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			MapCoordinate coordinate = map->getMapCoordinateForMapCreation(glm::vec3(i, j, 0));
			Monster *monster = coordinate.mapMonster.monster;
			GameEntity *gameEntity = coordinate.mapGameEntity.gameEntity;

			if (coordinate.mapMonster.monsterExists)
			{
				if (!map->coordinateHasCollision(glm::vec3(i, j, 0)))
				{
					monster->getTransform()->translate((float)i, (float)j, 1.0f);
					monsters.push_back(monster);
				}
				else
					delete monster;
			}
			if (coordinate.mapGameEntity.gameEntityExists)
			{
				gameEntity->getTransform()->translate((float)i, (float)j, 0);
				gameEntities.push_back(gameEntity);
			}
		}
	}
	/*for (int i = 0; i < monsters.size(); i++)
		std::cout << monsters[i]->getName() << std::endl;*/

	std::cout << "Quantidade de monstros: " << monsters.size() << std::endl;

	lastTime = 0;
}

Game::~Game()
{
	// TODO: fix deletions

	delete primitiveShader;
	delete camera;
	if (monsterFactory != NULL)
		delete monsterFactory;
	if (this->map != NULL)
		delete map;
	for (Monster* monster : monsters)
		delete monster;
}

void Game::render()
{
	// Map
	entityMap->render(mapShader);

	// Player HP Bar
	player->getHPBar()->quad->render(primitiveShader);

	// Generic entities (Player only at the moment)
	for (Entity* e : entities)
	{
		try{
			e->render(primitiveShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
	// Monsters
	for (Entity* e : monsters)
	{
		try{
			e->render(primitiveShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
	// Projectile attacks
	for (Entity* e : attacks)
	{
		try{
			e->render(commonShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
	// Common static entities
	for (Entity* e : gameEntities)
	{
		try{
			e->render(commonShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}

	// Render GUI (Order is important)
	gui->render(fontShader);
	for (Entity* e : text->getEntities())
	{
		try{
			e->render(fontShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}	
	}
}

bool playerDead = false;
unsigned int lastHp = 0;

void Game::update()
{
	// Game input
	input();

	// Camera input & update
	camera->input();
	camera->update(player->getTransform()->getPosition());

	// Light input & update
	light->input();
	light->update(player->getTransform()->getPosition());

	// Player input & update
	player->input(this->map);
	player->update();
	
	// Monsters update
	for (unsigned int i = 0; i < monsters.size(); i++)
		monsters[i]->update(map, player);

	for (unsigned int i = 0; i < monsters.size(); i++)
		if (!monsters[i]->isOnScreen())
		{
			delete monsters[i];
			monsters.erase(monsters.begin() + i);
		}
}


void Game::input()
{
#ifdef DEBUG
	if (Input::keyStates['v'])
		player->setMaximumHpBasis(player->getMaximumHpBasis()+1);		

	if (Input::keyStates['t'])
	{
		double now = Time::getTime();
		double delta = now - lastTime;
		
		if (delta >= 0.3)
		{
			int randomNumberX;
			int randomNumberY;
			glm::vec3 newPos;
			do{
				randomNumberX = std::rand() % 1024;
				randomNumberY = std::rand() % 1024;
				newPos = glm::vec3((float)randomNumberX, (float)randomNumberY, 0);
			} while (map->getMapCoordinateForEntityMovement(newPos).terrain != NORMAL_FLOOR);

			lastTime = Time::getTime();
			entities[0]->getTransform()->translate(newPos.x, newPos.y, entities[0]->getTransform()->getPosition().z);
		}
	}

	if (Input::keyStates['l'])
	{
		double now = Time::getTime();
		if (now - lastTime >= 0.3)
		{
			if (Mesh::drawForm == GL_TRIANGLES)
				Mesh::drawForm = GL_LINES;
			else
				Mesh::drawForm = GL_TRIANGLES;
			lastTime = Time::getTime();
		}
	}
#endif
}