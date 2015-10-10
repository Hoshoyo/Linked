#include "Game.h"
#include <iostream>
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
#include "TextShader.h"
#include "Light.h"

#include "RangeAttack.h"

#include "Player.h"
#include "HPBar.h"
#include "GUI.h"
#include "ZurikiRageSkill.h"
#include "HoshoyoExplosionSkill.h"

#include "Text.h"

#include "network\Packet.h"
#include "PacketController.h"

#include <string>
#include <iostream>
#include <cstdlib>

Game::Game(int windowsWidth, int windowsHeight)
{
	PacketController::game = this;
	this->createGraphicElements(windowsWidth, windowsHeight);
	this->createMap();
#ifdef SINGLEPLAYER
	this->createOfflinePlayer();
#endif
#ifdef MULTIPLAYER
	this->createUDPConnection();
	this->waitForCreationOfOnlinePlayer();
#endif
	this->createGUI();

#ifdef MULTIPLAYER
	this->loadMonstersAndEntities(false, true);
#endif

#ifdef SINGLEPLAYER
	this->loadMonstersAndEntities(true, true);
#endif

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

#ifdef MULTIPLAYER
	udpClient->virtualDisconnection();
#endif
}

void Game::createGraphicElements(int windowsWidth, int windowsHeight)
{
	// Camera
	this->camera = new Camera(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), 70.0f, (float)windowsWidth / windowsHeight, 0.1f, 1000.0f);
	Input::mouseAttack.setCamera(this->camera);

	// Light
	this->light = new Light(glm::vec3(100, 500, 50), glm::vec3(1, 1, 1));

	// Shaders
	this->primitiveShader = new PrimitiveShader("./shaders/normalshader", camera, light);
	this->commonShader = new CommonShader("./shaders/commonshader", camera, light);
	this->projectileShader = new CommonShader("./shaders/projectile", camera, light);
	this->mapShader = new MapShader("./shaders/mapshader", camera, light);
}

void Game::createMap()
{
	std::string mapPath = TERRAIN_MAP_PATH;
	std::string entitiesMapPath = ENTITIES_MAP_PATH;
	std::string monsterMapPath = MONSTER_MAP_PATH;

	this->monsterFactory = new MonsterFactory();
	this->gameEntityFactory = new GameEntityFactory();

	this->map = new Map(mapPath, entitiesMapPath, monsterMapPath, 3, monsterFactory, gameEntityFactory);
}

void Game::createOfflinePlayer()
{
#ifdef SINGLEPLAYER
	Mesh* playerMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f, 12, 0));
	this->localPlayer = new Player(new Transform(glm::vec3(440, 500, 1.5f), 45, glm::vec3(1, 0, 0), glm::vec3(2, 2, 2)), playerMesh, new Texture("./res/Monsters/Sprites/greenwarrior.png"));
	this->localPlayerRangeAttack = new RangeAttack(localPlayer, &localPlayerAttacks, &monsters, map);
	this->localPlayer->setRangeAttack(this->localPlayerRangeAttack);
	this->localPlayer->setHp(100);
	this->localPlayer->setName("JaOwnes");
	PacketController::localPlayer = localPlayer;

	this->localPlayer->setSpeedBasis(26);
	this->localPlayer->setMaximumHpBasis(100);
	this->localPlayer->setDefenseBasis(100);
	this->localPlayer->setMagicalPowerBasis(20);
	Skill* skill1 = new HoshoyoExplosionSkill(&monsters);
	skill1->setSlot(SLOT_1);
	localPlayer->addNewSkill(skill1);

	Skill* skill2 = new ZurikiRageSkill(&monsters);
	skill2->setSlot(SLOT_2);
	localPlayer->addNewSkill(skill2);

	Skill* skill3 = new HoshoyoExplosionSkill(&monsters);
	skill3->setSlot(SLOT_3);
	localPlayer->addNewSkill(skill3);

	Skill* skill4 = new HoshoyoExplosionSkill(&monsters);
	skill4->setSlot(SLOT_4);
	localPlayer->addNewSkill(skill4);
#endif
}

void Game::waitForCreationOfOnlinePlayer()
{
	do
	{
		this->udpClient->receivePackets();
		Sleep(100);
	}
	while (this->localPlayer == NULL);

	this->localPlayer = PacketController::localPlayer;
}

void Game::createOnlinePlayer(short* data, bool isLocalPlayer)
{
#ifdef MULTIPLAYER
	glm::vec3 localPlayerPosition = glm::vec3(data[8], data[9], data[10]);
	Mesh* playerMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f, 12, 0));
	Player* designedPlayer;

	if (data[0] == 0)
	{
		Transform* pt = new Transform(localPlayerPosition, 45, glm::vec3(1, 0, 0), glm::vec3(2, 2, 2));
		Texture* t = new Texture("./res/Monsters/Sprites/greenwarrior.png");
		designedPlayer = new Player(pt, playerMesh, t);
		designedPlayer->setName("JaOwnes");
	}
	else if (data[0] == 1)
	{
		designedPlayer = new Player(new Transform(localPlayerPosition, 45, glm::vec3(1, 0, 0), glm::vec3(2, 2, 2)), playerMesh, new Texture("./res/Monsters/Sprites/greenwarrior.png"));
		designedPlayer->setName("Hoshoyo");
	}

	designedPlayer->setTotalMaximumHp(data[1]);
	designedPlayer->setHp(data[2]);
	designedPlayer->setTotalAttack(data[3]);
	designedPlayer->setTotalDefense(data[4]);
	designedPlayer->setTotalMagicalPower(data[5]);
	designedPlayer->setTotalSpeed(data[6]);
	designedPlayer->setTotalAttackSpeed(data[7]);

	if (isLocalPlayer)
	{
		this->localPlayer = designedPlayer;
		this->localPlayerRangeAttack = new RangeAttack(localPlayer, &localPlayerAttacks, &monsters, map);
		this->localPlayer->setRangeAttack(this->localPlayerRangeAttack);
		PacketController::localPlayer = this->localPlayer;
	}
	else
	{
		this->secondPlayer = designedPlayer;
		this->secondPlayerRangeAttack = new RangeAttack(secondPlayer, &secondPlayerAttacks, &monsters, map);
		this->secondPlayer->setRangeAttack(this->secondPlayerRangeAttack);
		PacketController::secondPlayer = this->secondPlayer;
	}
#endif
}

void Game::createGUI()
{
	this->gui = new GUI(localPlayer, "./shaders/textshader", "./shaders/fontshader", "./fonts/bluehigh.ttf");
}

void Game::loadMonstersAndEntities(bool loadMonsters, bool loadEntities)
{
	Mesh* mapMesh = new Mesh(new Grid(MAP_SIZE, this->map));
	this->entityMap = new EntityMap(new Transform(), mapMesh,
		new Texture("./res/Maps/stonePath.png"),
		new Texture("./res/Maps/mountain.jpg"),
		new Texture("./res/Maps/water.jpg"),
		new Texture("./res/Maps/grassTex.png"),
		new Texture(TERRAIN_MAP_PATH));

	// Load monsters and entities
	for (int i = 0; i < MAP_SIZE; i++)
	{
		for (int j = 0; j < MAP_SIZE; j++)
		{
			MapCoordinate coordinate = map->getMapCoordinateForMapCreation(glm::vec3(i, j, 0));
			Monster *monster = coordinate.mapMonster.monster;
			GameEntity *gameEntity = coordinate.mapGameEntity.gameEntity;

			if (loadMonsters)
			{
				if (coordinate.mapMonster.monsterExists)
				{
					if (!map->coordinateHasCollision(glm::vec3(i, j, 0)))
					{
						monster->getTransform()->translate((float)i, (float)j, 1.3f);
						monsters.push_back(monster);
					}
					else
						delete monster;
				}
			}
			
			if (loadEntities)
			{
				if (coordinate.mapGameEntity.gameEntityExists)
				{
					gameEntity->getTransform()->translate((float)i, (float)j, 0);
					gameEntities.push_back(gameEntity);
				}
			}
		}
	}
}

void Game::createUDPConnection()
{
	udpClient = new UDPClient(9090, "127.0.0.1");
	PacketController::udpClient = udpClient;
	udpClient->virtualConnection();
}

void Game::render()
{
	// Map
	entityMap->render(mapShader);

	// Player
	localPlayer->render(primitiveShader, gui->getTextRenderer());

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
	for (Entity* e : localPlayerAttacks)
	{
		try{
			projectileShader->activateAlphaBlend();
			e->render(projectileShader);
			projectileShader->deactivateAlphaBlend();
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}

	// Second Player
#ifdef MULTIPLAYER
	if (secondPlayer != NULL)
		secondPlayer->render(primitiveShader, gui->getTextRenderer());

	for (Entity* e : secondPlayerAttacks)
	{
		try{
			e->render(commonShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
#endif MULTIPLAYER

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
	gui->render();
}

void Game::update()
{
#ifdef MULTIPLAYER
	udpClient->receivePackets();
#endif

	// Game input
	input();
	Input::mouseAttack.update();

	// Camera input & update
	camera->input();
	camera->update(localPlayer->getTransform()->getPosition());

	// Light input & update
	light->input();
	light->update(localPlayer->getTransform()->getPosition());

	// Player input & update
	
	localPlayer->input(this->map);
	localPlayer->update(this->map);

#ifdef MULTIPLAYER
	if (secondPlayer != NULL)
		secondPlayer->update(this->map);
#endif
	
	// Monsters update
	for (unsigned int i = 0; i < monsters.size(); i++)
		monsters[i]->update(map, localPlayer);

	for (unsigned int i = 0; i < monsters.size(); i++)
		if (!monsters[i]->isOnScreen())
		{
			delete monsters[i];
			monsters.erase(monsters.begin() + i);
		}

	// GUI update
	gui->update();
}

void Game::input()
{		

#ifdef DEBUG
	if (Input::keyStates['b'])
	{
		glm::vec3 pp = player->getTransform()->getPosition();
		glm::vec3 mi = Input::mouseAttack.getMouseIntersection();


		mi.z = 1.0f;

		Mesh* playerMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f, 7, 7));
		Entity* e = new Entity(new Transform(mi), playerMesh, new Texture("./res/Monsters/Sprites/greenwarrior.png"));
		entities.push_back(e);
	}

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