#include "PlayerMovement.h"
#include "Input.h"
#include "Display.h"


PlayerMovement::PlayerMovement(Map* map, Entity* player)
{
	this->map = map;
	this->player = player;
}


PlayerMovement::~PlayerMovement()
{
}

void PlayerMovement::inputPlayerMovement()
{
	glm::vec3 finalPos;
	float frameTime = Display::frameTime;

	if (Input::keyStates['w'])
	{
		player->getTexture()->setIndex(3);
		if (!Input::keyStates['a'] && !Input::keyStates['s'] && !Input::keyStates['d'])
		{
			if (moveTo(NORTH, player->getTransform()->getPosition(), frameTime, true, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (moveTo(NORTH, player->getTransform()->getPosition(), frameTime, false, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['a'])
	{
		player->getTexture()->setIndex(1);
		if (!Input::keyStates['w'] && !Input::keyStates['s'] && !Input::keyStates['d'])
		{
			if (moveTo(WEST, player->getTransform()->getPosition(), frameTime, true, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (moveTo(WEST, player->getTransform()->getPosition(), frameTime, false, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['s'])
	{
		player->getTexture()->setIndex(2);
		if (!Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['d'])
		{
			if (moveTo(SOUTH, player->getTransform()->getPosition(), frameTime, true, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (moveTo(SOUTH, player->getTransform()->getPosition(), frameTime, false, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}

	if (Input::keyStates['d'])
	{
		player->getTexture()->setIndex(0);
		if (!Input::keyStates['w'] && !Input::keyStates['a'] && !Input::keyStates['s'])
		{
			if (moveTo(EAST, player->getTransform()->getPosition(), frameTime, true, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
		else
		{
			if (moveTo(EAST, player->getTransform()->getPosition(), frameTime, false, &finalPos))
				player->getTransform()->translate(finalPos.x, finalPos.y, finalPos.z);
		}
	}
}

bool PlayerMovement::moveTo(MovementDirection direction, glm::vec3 currentPosition, float frameTime, bool isOnlyKeyPressed, glm::vec3 *endPosition)
{
	glm::vec3 nextPosition = currentPosition;
	float range = frameTime * PLAYER_SPEED;

	switch (direction)
	{
	case NORTH:
		nextPosition.y = nextPosition.y + range; break;
	case WEST:
		nextPosition.x = nextPosition.x - range; break;
	case EAST:
		nextPosition.x = nextPosition.x + range; break;
	case SOUTH:
		nextPosition.y = nextPosition.y - range; break;
	}

	if (!MapCoordinate::isOfCollisionType(map->getMapCoordinateForPlayerMovement(nextPosition).terrain))
	{
		endPosition->x = nextPosition.x;
		endPosition->y = nextPosition.y;
		endPosition->z = nextPosition.z;
		return true;
	}
	else
	{
		if (isOnlyKeyPressed)
		{
			for (int i = 1; i <= SLIDE_FACTOR; i++)
			{
				glm::vec3 positionToSlide1 = nextPosition;
				glm::vec3 positionToSlide2 = nextPosition;

				if (direction == NORTH || direction == SOUTH)
				{
					positionToSlide1.x = positionToSlide1.x - i * range;
					positionToSlide2.x = positionToSlide2.x + i * range;
				}
				else if (direction == WEST || direction == EAST)
				{
					positionToSlide1.y = positionToSlide1.y - i * range;
					positionToSlide2.y = positionToSlide2.y + i * range;
				}

				if (!MapCoordinate::isOfCollisionType(map->getMapCoordinateForPlayerMovement(positionToSlide1).terrain))
				{
					if (direction == NORTH || direction == SOUTH)
					{
						endPosition->x = currentPosition.x - SLIDE_SPEED_FACTOR;
						endPosition->y = currentPosition.y;
					}
					else if (direction == EAST || direction == WEST)
					{
						endPosition->x = currentPosition.x;
						endPosition->y = currentPosition.y - SLIDE_SPEED_FACTOR;
					}

					endPosition->z = currentPosition.z;
					return true;
				}
				else if (!MapCoordinate::isOfCollisionType(map->getMapCoordinateForPlayerMovement(positionToSlide2).terrain))
				{
					if (direction == NORTH || direction == SOUTH)
					{
						endPosition->x = currentPosition.x + SLIDE_SPEED_FACTOR;
						endPosition->y = currentPosition.y;
					}
					else if (direction == EAST || direction == WEST)
					{
						endPosition->x = currentPosition.x;
						endPosition->y = currentPosition.y + SLIDE_SPEED_FACTOR;
					}

					endPosition->z = currentPosition.z;
					return true;
				}
			}
		}
	}

	return false;
}