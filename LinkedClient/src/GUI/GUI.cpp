#include "GUI.h"
#include "Player.h"
#include "Shader.h"
#include "SkillIcon.h"
#include "TextShader.h"
#include "Primitive.h"
#include "GUIShader.h"
#include "Chat.h"

#include <sstream>
#include <iostream>

GUI::GUI(Player* player, std::string textShaderFileName, std::string guiShaderFileName, std::string fontName)
{
	this->player = player;
	this->playerHealth = player->getHp();
	this->playerMaxHealth = player->getTotalMaximumHp();
	this->playerAttack = player->getTotalAttack();
	this->playerDefense = player->getTotalDefense();
	this->playerMagicalPower = player->getTotalMagicalPower();
	this->playerAttackSpeed = player->getTotalAttackSpeed();
	this->playerSpeed = player->getTotalSpeed();

	this->textShader = new TextShader(textShaderFileName);
	this->guiShader = new GUIShader(guiShaderFileName);

	this->color = glm::vec3(LGUI_R, LGUI_G, LGUI_B);
	this->activeText = glm::vec3(0.8f, 0.8f, 0.95f);

	this->skillIconSlot1 = NULL;
	this->skillIconSlot2 = NULL;
	this->skillIconSlot3 = NULL;
	this->skillIconSlot4 = NULL;

	textRenderer = new TextRenderer(textShader, fontName);
	initLeftGUI();
}


GUI::~GUI()
{
	delete textRenderer;
	delete guiShader;
	delete textShader;
	delete leftGUIMesh;
	delete leftGUIChatActiveTexture;
	delete leftGUIChatInactiveTexture;
	delete leftGUIEntity;
}

void GUI::initLeftGUI()
{
	leftGUIMesh = new Mesh(new Quad(glm::vec3(0, 0, 0), 1.0f, 1.0f));
	leftGUIChatInactiveTexture = new Texture(LEFTGUI_PATH_CHATINACTIVE);
	leftGUIChatActiveTexture = new Texture(LEFTGUI_PATH_CHATACTIVE);
	leftGUIEntity = new Entity(new Transform(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)), leftGUIMesh, leftGUIChatInactiveTexture);

	initLeftGUIText(1);
	initLeftGUISkills();
}

void GUI::initLeftGUIText(int attribsHint)
{
	std::stringstream healthStream;
	std::stringstream attackStream;
	std::stringstream defenseStream;
	std::stringstream magicalPowerStream;
	std::stringstream attackSpeedStream;
	std::stringstream speedStream;

	healthStream << "Health: " << playerHealth <<"/"<< playerMaxHealth;
	attackStream << "Attack: " << playerAttack;
	defenseStream << "Defense: " << playerDefense;
	magicalPowerStream << "Magic Power: " << playerMagicalPower;
	attackSpeedStream << "Attack Speed: " << playerAttackSpeed;
	speedStream << "Speed: " << playerSpeed;

	pHealth = healthStream.str();
	pAttack = attackStream.str();
	pDefense = defenseStream.str();
	pMagicalPower = magicalPowerStream.str();
	pAttackSpeed = attackSpeedStream.str();
	pSpeed = speedStream.str();
}

void GUI::initLeftGUISkills()
{

}

void GUI::render()
{
	leftGUIEntity->render(guiShader);
	renderSkillIcons(guiShader);

	// Render Text
	// Player Name
	const float padding = 16.0f;
	textRenderer->renderText(player->getName(), PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN + 5.0f, PLAYER_STATS_FONT_SIZE, color);
	// Attributes
	textRenderer->renderText(pHealth, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 1, PLAYER_STATS_FONT_SIZE, color);
	textRenderer->renderText(pAttack, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 2, PLAYER_STATS_FONT_SIZE, color);
	textRenderer->renderText(pDefense, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 3, PLAYER_STATS_FONT_SIZE, color);
	textRenderer->renderText(pMagicalPower, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 4, PLAYER_STATS_FONT_SIZE, color);
	textRenderer->renderText(pAttackSpeed, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 5, PLAYER_STATS_FONT_SIZE, color);
	textRenderer->renderText(pSpeed, PLAYER_STATS_LEFT_ALIGN, PLAYER_STATS_VERT_ALIGN - padding * 6, PLAYER_STATS_FONT_SIZE, color);

	std::string msg = "Superwaffles eh um cara bem legal, pena que naum pode ver o zeh";
	for (unsigned int i = 0; i < messages.size(); i++)
	{
		textRenderer->renderText(messages[i], 1095.0f, 46.0f + (i*CHAT_SPACING), CHAT_LETTER_SIZE, color);
	}

	textRenderer->renderText(Chat::getStream().str(), 1095.0f, 130.0f - ((CHAT_MAX_MSGS +0.5f) * CHAT_SPACING), CHAT_LETTER_SIZE, activeText);

}

void GUI::setPlayerHealth(unsigned int health, unsigned int maxHealth)
{
	std::stringstream healthStream;
	healthStream << "Health: " << health << "/" << maxHealth;
	playerHealth = health;
	playerMaxHealth = maxHealth;
	pHealth = healthStream.str();
}
void GUI::setPlayerAttack(unsigned int attack)
{
	std::stringstream attackStream;
	attackStream << "Attack: " << attack;
	playerAttack = attack;
}
void GUI::setPlayerDefense(unsigned int defense)
{
	std::stringstream defenseStream;
	defenseStream << "Defense: " << defense;
	playerDefense = defense;
}
void GUI::setPlayerMagicalPower(unsigned int magicalPower)
{
	std::stringstream magicalPowerStream;
	magicalPowerStream << "Magical Power: " << magicalPower;
	playerMagicalPower = magicalPower;
}
void GUI::setPlayerAttackSpeed(unsigned int attackSpeed)
{
	std::stringstream attackSpeedStream;
	attackSpeedStream << "Attack Speed: " << attackSpeed;
	playerAttackSpeed = attackSpeed;
}
void GUI::setPlayerSpeed(unsigned int speed)
{
	std::stringstream speedStream;
	speedStream << "Speed: " << speed;
	playerSpeed = speed;
}

void GUI::update()
{
	if (Chat::isChatActive())
		leftGUIEntity->setTexture(leftGUIChatActiveTexture);
	else
		leftGUIEntity->setTexture(leftGUIChatInactiveTexture);

	if (player->getHp() != playerHealth)
		setPlayerHealth(player->getHp(), player->getTotalMaximumHp());
	
	if (player->getTotalAttack() != playerAttack)
		setPlayerAttack(player->getTotalAttack());

	if (player->getTotalDefense() != playerDefense)
		setPlayerDefense(player->getTotalDefense());

	if (player->getTotalMagicalPower() != playerMagicalPower)
		setPlayerMagicalPower(player->getTotalMagicalPower());

	if (player->getTotalAttackSpeed() != playerAttackSpeed)
		setPlayerAttackSpeed(player->getTotalAttackSpeed());

	if (player->getTotalSpeed() != playerSpeed)
		setPlayerSpeed(player->getTotalSpeed());
}

void GUI::addSkillIcon(SkillIcon* skillIcon)
{
	SkillSlot slot = skillIcon->getSlot();

	switch (slot)
	{
		case SLOT_1: this->skillIconSlot1 = skillIcon; break;
		case SLOT_2: this->skillIconSlot2 = skillIcon; break;
		case SLOT_3: this->skillIconSlot3 = skillIcon; break;
		case SLOT_4: this->skillIconSlot4 = skillIcon; break;
	}
}

void GUI::renderSkillIcons(Shader* shader)
{
	if (this->skillIconSlot1 != NULL) this->skillIconSlot1->render(shader);
	if (this->skillIconSlot2 != NULL) this->skillIconSlot2->render(shader);
	if (this->skillIconSlot3 != NULL) this->skillIconSlot3->render(shader);
	if (this->skillIconSlot4 != NULL) this->skillIconSlot4->render(shader);
}

void GUI::setLeftGUITextColor(glm::vec3& color)
{
	this->color = color;
}

void GUI::setFontSize(int size)
{
	this->fontSize = size;
}

Shader* GUI::getGUIShader()
{
	return this->guiShader;
}

TextRenderer* GUI::getTextRenderer()
{
	return this->textRenderer;
}

void GUI::setNextMessage(std::string& msg)
{
	if (messages.size() < CHAT_MAX_MSGS)
		messages.insert(messages.begin(), msg);
	else
	{
		messages.erase(messages.begin()+messages.size()-1);
		messages.insert(messages.begin(), msg);
	}
}