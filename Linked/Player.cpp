#include "Player.h"
#include "HPBar.h"
#include "Time.h"
#include "Input.h"
#include "Primitive.h"
#include "Display.h"
#include "RangeAttack.h"
#include "Map.h"
#include "Text.h"

#include <iostream>

Player::Player(Transform* transform, Mesh* mesh, Texture* texture) : Entity(transform, mesh, texture)
{
	setName(PLAYER_DEFAULT_NAME);
	setHp(PLAYER_DEFAULT_HP);
	setMaximumHpBasis(PLAYER_DEFAULT_MAX_HP_BASIS);
	setLives(PLAYER_DEFAULT_LIVES);
	setMaximumLivesBasis(PLAYER_DEFAULT_MAX_LIVES_BASIS);
	setAttackBasis(PLAYER_DEFAULT_ATTACK_BASIS);
	setDefenseBasis(PLAYER_DEFAULT_DEFENSE_BASIS);
	setMagicalPowerBasis(PLAYER_DEFAULT_MAGICAL_POWER_BASIS);
	setSpeedBasis(PLAYER_DEFAULT_SPEED_BASIS);
	setAttackSpeedBasis(PLAYER_DEFAULT_ATTACK_SPEED_BASIS);
	skills = std::vector<Skill*>();
	equipments = std::vector<Equipment*>();
	this->hpBar = new HPBar(this);
	this->ai = new PlayerAI();
	this->isMovingTo = false;
	this->type = LOCAL;
}

Player::Player(Transform* transform, Mesh* mesh, Texture* texture, RangeAttack* rangeAttack) : Player(transform, mesh, texture)
{
	this->rangeAttack = rangeAttack;
}

Player::~Player()
{
	delete hpBar;
	delete ai;
}

void Player::render(Shader* primitiveShader, Shader* fontShader)
{
	Entity::render(primitiveShader);
	this->getHPBar()->quad->render(primitiveShader);

	for (Skill* skill : this->getSkills())
	{
		try{
			if (skill->isActive())
				skill->render(primitiveShader, fontShader);
		}
		catch (...){
			std::cerr << "Error rendering entity" << std::endl;
		}
	}
}

void Player::attack()
{
	this->attacking = true;
	lastAttackTime = Time::getTime();
}

bool Player::isAttacking()
{
	if (this->attacking)
	{
		double now = Time::getTime();
		if ((now - lastAttackTime) > ((1.0f / getTotalAttackSpeed()) * 10.0f))
		{
			this->attacking = false;
			return false;
		}
		else
			return true;
	}
	return false;
}

bool Player::isReceivingDamage()
{
	if (this->receivingDamage)
	{
		double now = Time::getTime();
		if ((now - lastReceivedDamageTime) > PLAYER_RECEIVE_DAMAGE_DELAY)
		{
			this->receivingDamage = false;
			return false;
		}
		else
			return true;
	}
	return false;
}

std::string Player::getName(){
	return name;
}
void Player::setName(std::string name){
	this->name = name;
}

bool Player::isAlive(){
	return getHp() != 0;
}

/* HP */
unsigned int Player::getHp()
{
	return hp;
}

void Player::setHp(unsigned int hp)
{
	this->hp = hp;
}

void Player::doDamage(unsigned int damage)
{
	if (damage > hp)
		hp = 0;
	else
		hp = hp - damage;

	this->receivingDamage = true;
	lastReceivedDamageTime = Time::getTime();
}

void Player::healHp(unsigned int healingAmount){
	unsigned int maximumHp = getTotalMaximumHp();
	if ((healingAmount + hp) > maximumHp)
		hp = maximumHp;
	else
		hp = healingAmount + hp;
}
void Player::restoreHpToMaximum(){
	hp = getTotalMaximumHp();
}
unsigned int Player::getMaximumHpBasis(){
	return maximumHpBasis;
}
void Player::setMaximumHpBasis(unsigned int maximumHpBasis){
	this->maximumHpBasis = maximumHpBasis;
}
unsigned int Player::getTotalMaximumHp(){
	// TO DO
	return getMaximumHpBasis();
}

/* LIVES */
unsigned int Player::getLives(){
	return lives;
}
void Player::setLives(unsigned int lives){
	this->lives = lives;
}
void Player::removeLive(){
	if (lives > 0)
		lives--;
}
void Player::restoreAllLives(){
	lives = getTotalMaximumLives();
}
void Player::restoreLive(){
	if (lives < getTotalMaximumLives())
		lives++;
}
unsigned int Player::getMaximumLivesBasis(){
	return maximumLivesBasis;
}
void Player::setMaximumLivesBasis(unsigned int maximumLivesBasis){
	this->maximumLivesBasis = maximumLivesBasis;
}
unsigned int Player::getTotalMaximumLives(){
	// TO DO
	return getMaximumLivesBasis();
}

/* ATTACK */
unsigned int Player::getAttackBasis(){
	return attackBasis;
}
void Player::setAttackBasis(unsigned int attackBasis){
	this->attackBasis = attackBasis;
}
unsigned int Player::getTotalAttack(){
	// TO DO
	return getAttackBasis();
}

/* DEFENSE */
unsigned int Player::getDefenseBasis(){
	return defenseBasis;
}
void Player::setDefenseBasis(unsigned int defenseBasis){
	this->defenseBasis = defenseBasis;
}
unsigned int Player::getTotalDefense(){
	// TO DO
	return getDefenseBasis();
}

/* MAGICAL POWER */
unsigned int Player::getMagicalPowerBasis(){
	return magicalPowerBasis;
}
void Player::setMagicalPowerBasis(unsigned int magicalPowerBasis){
	this->magicalPowerBasis = magicalPowerBasis;
}
unsigned int Player::getTotalMagicalPower(){
	// TO DO
	return getMagicalPowerBasis();
}

/* SPEED */
unsigned int Player::getSpeedBasis(){
	return speedBasis;
}
void Player::setSpeedBasis(unsigned int speedBasis){
	this->speedBasis = speedBasis;
}
unsigned int Player::getTotalSpeed(){
	// TO DO
	return getSpeedBasis();
}

/* ATTACK SPEED */
unsigned int Player::getAttackSpeedBasis(){
	return attackSpeedBasis;
}
void Player::setAttackSpeedBasis(unsigned int attackSpeedBasis){
	this->attackSpeedBasis = attackSpeedBasis;
}
unsigned int Player::getTotalAttackSpeed(){
	// TO DO
	return getAttackSpeedBasis();
}

/* SKILLS */
std::vector<Skill*> Player::getSkills(){
	return skills;
}
Skill* Player::getSkillOfSlot(SkillSlot slot){
	for (unsigned int i = 0; i < skills.size(); i++)
		if (skills[i]->getSlot() == slot)
			return skills[i];
	return NULL;
}
bool Player::addNewSkill(Skill* skill){
	if (this->skills.size() < PLAYER_MAXIMUM_SKILLS)
	{
		skill->setEntity(this);
		this->skills.push_back(skill);
		return true;
	}
	else
		return false;
}
bool Player::isPlayerUsingASkill()
{
	if (this->getSkillOfSlot(SLOT_1) != nullptr && this->getSkillOfSlot(SLOT_1)->isActive()) return true;
	if (this->getSkillOfSlot(SLOT_2) != nullptr && this->getSkillOfSlot(SLOT_2)->isActive()) return true;
	if (this->getSkillOfSlot(SLOT_3) != nullptr && this->getSkillOfSlot(SLOT_3)->isActive()) return true;
	if (this->getSkillOfSlot(SLOT_4) != nullptr && this->getSkillOfSlot(SLOT_4)->isActive()) return true;

	return false;
}
bool Player::isPlayerUsingSkillOfSlot(SkillSlot slot)
{
	if (this->getSkillOfSlot(slot) == NULL)
		return false;
	return this->getSkillOfSlot(slot)->isActive();
}

/* EQUIPMENTS */
std::vector<Equipment*> Player::getEquipments(){
	return equipments;
}
Equipment* Player::getEquipmentOfClass(EquipmentClass equipmentClass){

	for (unsigned int i = 0; i < equipments.size(); i++)
		if (equipments[i]->getEquipmentClass() == equipmentClass)
			return equipments[i];

	return NULL;
}
Equipment* Player::addNewEquipment(Equipment* equipment){
	for (unsigned int i = 0; i < equipments.size(); i++)
	{
		if (equipments[i]->getEquipmentClass() == equipment->getEquipmentClass())
		{
			equipments.erase(equipments.begin() + i);
			equipments.push_back(equipment);
			return equipments[i];
		}
	}

	equipments.push_back(equipment);
	return NULL;
}

/* HP BAR */
HPBar* Player::getHPBar()
{
	return hpBar;
}

/* RANGE ATTACK */
RangeAttack* Player::getRangeAttack()
{
	return this->rangeAttack;
}

void Player::setRangeAttack(RangeAttack* rangeAttack)
{
	this->rangeAttack = rangeAttack;
}

/* INPUT & UPDATE */
void Player::update(Map* map)
{
	if (this->type == NETWORK)
		this->updateMovement(map);
	this->hpBar->update();
	this->refreshTexture();
	if (this->rangeAttack != NULL) this->rangeAttack->update();

	for (Skill* skill : this->skills)
		skill->update();
}

void Player::input(Map* map)
{
	if (this->type == LOCAL)
	{
		this->hpBar->input();
		if (this->rangeAttack != NULL) this->rangeAttack->input();

		glm::vec3 currentPosition = this->getTransform()->getPosition();

		if (Input::keyStates['x'])
			this->healHp(1);

		if (this->isAlive())
		{
			if (Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['s'] && !Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(TOP);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = TOP;
			}
			else if (Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['s'] && Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(TOP_RIGHT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = TOP_RIGHT;
			}
			else if (!Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['s'] && Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(RIGHT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = RIGHT;
			}
			else if (!Input::keyStates['w'] && !Input::keyStates['a'] && Input::keyStates['s'] && Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(BOTTOM_RIGHT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = BOTTOM_RIGHT;
			}
			else if (!Input::keyStates['w'] && !Input::keyStates['a'] && Input::keyStates['s'] && !Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(BOTTOM);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = BOTTOM;
			}
			else if (!Input::keyStates['w'] && Input::keyStates['a'] && Input::keyStates['s'] && !Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(BOTTOM_LEFT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = BOTTOM_LEFT;
			}
			else if (!Input::keyStates['w'] && Input::keyStates['a'] && !Input::keyStates['s'] && !Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(LEFT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = LEFT;
			}
			else if (Input::keyStates['w'] && Input::keyStates['a'] && !Input::keyStates['s'] && !Input::keyStates['d'])
			{
				glm::vec3 deltaVector = this->getDeltaVectorToDirection(TOP_LEFT);
				if (checkIfPlayerIsStillOnTheSameMapPosition(currentPosition, currentPosition + deltaVector) || !map->coordinateHasCollision(currentPosition + deltaVector))
					this->getTransform()->incTranslate(deltaVector.x, deltaVector.y, deltaVector.z);
				this->currentDirection = TOP_LEFT;
			}

			if (Input::keyStates['z'] && this->getSkillOfSlot(SLOT_1) != NULL && !this->isPlayerUsingASkill())
				this->getSkillOfSlot(SLOT_1)->use(this->currentDirection);
			else if (Input::keyStates['x'] && this->getSkillOfSlot(SLOT_2) != NULL && !this->isPlayerUsingASkill())
				this->getSkillOfSlot(SLOT_2)->use(this->currentDirection);
			else if (Input::keyStates['c'] && this->getSkillOfSlot(SLOT_3) != NULL && !this->isPlayerUsingASkill())
				this->getSkillOfSlot(SLOT_3)->use(this->currentDirection);
			else if (Input::keyStates['v'] && this->getSkillOfSlot(SLOT_4) != NULL && !this->isPlayerUsingASkill())
				this->getSkillOfSlot(SLOT_4)->use(this->currentDirection);

			if (Input::leftMouseButton)
			{
				if (this->isPlayerUsingSkillOfSlot(SLOT_1))
					this->getSkillOfSlot(SLOT_1)->cancelIfPossible();
				else if (this->isPlayerUsingSkillOfSlot(SLOT_2))
					this->getSkillOfSlot(SLOT_2)->cancelIfPossible();
				else if (this->isPlayerUsingSkillOfSlot(SLOT_3))
					this->getSkillOfSlot(SLOT_3)->cancelIfPossible();
				else if (this->isPlayerUsingSkillOfSlot(SLOT_4))
					this->getSkillOfSlot(SLOT_4)->cancelIfPossible();

				Input::leftMouseButton = false;
			}
		}
	}
}

PlayerType Player::getType()
{
	return this->type;
}

void Player::setType(PlayerType type)
{
	this->type = type;
}

void Player::updateMovement(Map* map)
{
	if (isMovingTo)
	{
		glm::vec3 pPos = this->getTransform()->getPosition();
		float frameTime = (float)Display::frameTime;
		float range = frameTime * this->getTotalSpeed();
		MovementDefinition newPos = this->ai->movePerfectlyTo(map, pPos, this->destination, range);

		if (newPos.doMove)
		{
			this->getTransform()->translate(newPos.movement.x, newPos.movement.y, newPos.movement.z);
			this->currentDirection = newPos.direction;
			switch (this->currentDirection)
			{
			case TOP: std::cout << "top" << std::endl; break;
			case TOP_RIGHT: std::cout << "topright" << std::endl; break;
			case TOP_LEFT: std::cout << "topleft" << std::endl; break;
			case LEFT: std::cout << "left" << std::endl; break;
			case RIGHT: std::cout << "right" << std::endl; break;
			case BOTTOM_LEFT: std::cout << "bottomleft" << std::endl; break;
			case BOTTOM_RIGHT: std::cout << "bottomright" << std::endl; break;
			}
		}
		else
			this->isMovingTo = false;
	}
}

void Player::refreshTexture()
{
	double now = Time::getTime();
	bool shouldChangeTexture = (now - textureChangeTime) > PLAYER_TEXTURE_CHANGE_DELAY;

	bool isDead = !this->isAlive();
	bool isAttacking = this->isAttacking();
	bool isReceivingDamage = this->isReceivingDamage();

	if (shouldChangeTexture || this->currentDirection != this->lastDirection || isDead != lastIsDead || isAttacking != lastIsAttacking || isReceivingDamage != lastIsReceivingDamage)
	{
		if (isDead)
			changeTextureBasedOnDirection(this->currentDirection, 48, 48);
		else
		{
			switch (this->currentDirection)
			{
				case TOP:
				case TOP_LEFT:		// TO DO
				case TOP_RIGHT:		// TO DO
					if (!isAttacking && !isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 0, 3);
					else if (isAttacking) changeTextureBasedOnDirection(this->currentDirection, 16, 19);
					else if (isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 32, 35);
					break;
				case RIGHT:
				case BOTTOM_RIGHT:	// TO DO
					if (!isAttacking && !isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 8, 11);
					else if (isAttacking) changeTextureBasedOnDirection(this->currentDirection, 24, 27);
					else if (isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 40, 43);
					break;
				case BOTTOM:
					if (!isAttacking && !isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 4, 7);
					else if (isAttacking) changeTextureBasedOnDirection(this->currentDirection, 20, 23);
					else if (isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 36, 39);
					break;
				case LEFT:
				case BOTTOM_LEFT:	// TO DO
					if (!isAttacking && !isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 12, 15);
					else if (isAttacking) changeTextureBasedOnDirection(this->currentDirection, 28, 31);
					else if (isReceivingDamage) changeTextureBasedOnDirection(this->currentDirection, 44, 47);
					break;
			}
		}

		this->lastDirection = this->currentDirection;
		this->lastIsAttacking = this->isAttacking();
		this->lastIsReceivingDamage = this->isReceivingDamage();
		this->lastIsDead = !this->isAlive();
		this->textureChangeTime = now;
	}
}

void Player::changeTextureBasedOnDirection(MovementDirection direction, unsigned int initialTextureIndex, unsigned int finalTextureIndex)
{
	if (direction != this->lastDirection || this->isAttacking() != lastIsAttacking || this->isReceivingDamage() != lastIsReceivingDamage || !this->isAlive() != lastIsDead)
	{
		this->getMesh()->getQuad()->setIndex(initialTextureIndex);
		this->lastIndexTexture = initialTextureIndex;
	}
	else
	{
		if (this->lastIndexTexture < finalTextureIndex)
		{
			this->getMesh()->getQuad()->setIndex(++this->lastIndexTexture);
		}
		else
		{
			this->getMesh()->getQuad()->setIndex(initialTextureIndex);
			this->lastIndexTexture = initialTextureIndex;
		}
	}
}

bool Player::checkIfPlayerIsStillOnTheSameMapPosition(glm::vec3 currentPosition, glm::vec3 nextPosition)
{
	if (floor(currentPosition.x) == floor(nextPosition.x))
		if (floor(currentPosition.y) == floor(nextPosition.y))
			if (floor(currentPosition.z) == floor(nextPosition.z))
				return true;

	return false;
}

glm::vec3 Player::getDeltaVectorToDirection(MovementDirection direction)
{
	float frameTime = (float)Display::frameTime;
	float range = frameTime * this->getTotalSpeed();

	switch (direction)
	{
	case TOP:
		return glm::vec3(0, range, 0);
	case TOP_RIGHT:
		return glm::vec3(range / SQRT2, range / SQRT2, 0);
	case RIGHT:
		return glm::vec3(range, 0, 0);
	case BOTTOM_RIGHT:
		return glm::vec3(range / SQRT2, -range / SQRT2, 0);
	case BOTTOM:
		return glm::vec3(0, -range, 0);
	case BOTTOM_LEFT:
		return glm::vec3(-range / SQRT2, -range / SQRT2, 0);
	case LEFT:
		return glm::vec3(-range, 0, 0);
	case TOP_LEFT:
		return glm::vec3(-range / SQRT2, range / SQRT2, 0);
	default:
		return glm::vec3(0, 0, 0);
	}
}

void Player::startMovementTo(glm::vec3 destination)
{
	this->destination = destination;
	this->isMovingTo = true;
}