#include "Entity.h"
#include "Monster.h"
#include "Light.h"
#include <iostream>

Entity::Entity(Transform* transform, Mesh* mesh, Texture* texture)
{
	this->transform = transform;
	this->mesh = mesh;
	this->texture = texture;

	if (this->texture != nullptr)
		this->texture->getReferenceCount()++;
	if (this->mesh != nullptr)
		this->mesh->getReferenceCount()++;
	
}
int t = 0, m = 0;
Entity::~Entity()
{
	std::cout << "Entity Deleted." << std::endl;
	if (transform != NULL)
		delete transform;

	if (this->texture != NULL)
	{
		this->texture->getReferenceCount()--;
		std::cout << "ref count: " << this->texture->getReferenceCount() << std::endl;
		if (texture->getReferenceCount() == 0)
		{
			std::cout << "Texture " << t+1 << " was deleted." << std::endl;
			t++;
			delete texture;
		}
			
	}
	
	if (this->mesh != NULL)
	{
		this->mesh->getReferenceCount()--;
		if (mesh->getReferenceCount() == 0)
		{
			delete mesh;
			std::cout << "Mesh " << m + 1 << " was deleted." << std::endl;
			m++;
		}
	}
}

Transform* Entity::getTransform()
{
	return transform;
}

void Entity::render(Shader* shader)
{
	shader->useShader();
	// Set shader data
	shader->setEntity(this);

	// Update shader
	bindTextures();
	shader->update();

	// Render
	mesh->render();
	shader->stopShader();
}

Texture* Entity::getTexture()
{
	return texture;
}

void Entity::setTexture(Texture* texture)
{
	if (this->texture != nullptr) this->texture->getReferenceCount()--;
	this->texture = texture;
	this->texture->getReferenceCount()++;
}

void Entity::setTransform(Transform* transform)
{
	this->transform = transform;
}

void Entity::setMesh(Mesh* mesh)
{
	if (this->mesh != nullptr) this->mesh->getReferenceCount()--;
	this->mesh = mesh;
	this->mesh->getReferenceCount()++;
}

Mesh* Entity::getMesh()
{
	return mesh;
}

void Entity::bindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->textureID);
}