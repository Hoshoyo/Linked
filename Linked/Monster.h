#pragma once
#include "Entity.h"
class MonsterAI;
class Map;
enum MovementDirection;

class Monster : public Entity
{
public:
	Monster(Transform* transform, Mesh* mesh, Texture* texture);
	~Monster();
	std::string getName();
	void setName(std::string name);
	unsigned int getHp();
	void setHp(unsigned int hp);
	unsigned int getMaxHp();
	void setMaxHp(unsigned int maxHp);
	void doDamage(unsigned int damage);
	unsigned int getAttack();
	void setAttack(unsigned int attack);
	unsigned int getDefense();
	void setDefense(unsigned int defense);
	unsigned int getSpeed();
	void setSpeed(unsigned int speed);
	unsigned int getRange();
	void setRange(unsigned int range);
	unsigned int getCollisionRange();
	void setCollisionRange(unsigned int collisionRange);
	int getMapColorRed();
	void setMapColorRed(int red);
	int getMapColorGreen();
	void setMapColorGreen(int green);
	int getMapColorBlue();
	void setMapColorBlue(int blue);
	glm::vec3 getMapColor();
	void setMapColor(glm::vec3 mapColor);
	void moveTo(Entity* entity, Map* map);
	void moveAway(Entity* entity, Map* map);
private:
	std::string name;
	unsigned int hp;
	unsigned int maxHp;
	unsigned int attack;
	unsigned int defense;
	unsigned int speed;
	unsigned int range;
	unsigned int collisionRange;
	glm::vec3 mapColor;
	MonsterAI* ai;
	void changeTextureBasedOnMovementDirection(MovementDirection direction);
};