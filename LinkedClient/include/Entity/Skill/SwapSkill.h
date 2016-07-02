#pragma once
#include "Skill.h"

enum class SwapSkillStatus
{
	IDLE,
	WAITING_FOR_SERVER_RESPONSE,
	ANIMATION,
	EXECUTION
};

class SwapSkill : public Skill
{
public:
	SwapSkill(SkillOwner owner);
	~SwapSkill();
	void render(Shader* primitiveShader, Shader* skillShader, TextRenderer* textRenderer);
	void prepareExecution(MovementDirection skillDirection);
	void execute(MovementDirection skillDirection, glm::vec3 skillTargetPosition, int targetCreatureId);
	bool cancelIfPossible();
	void update(std::vector<Monster*> *monsters, std::vector<Player*> *players, Player* localPlayer);
private:
	SwapSkillStatus status;
	bool checkCooldown = false;
};