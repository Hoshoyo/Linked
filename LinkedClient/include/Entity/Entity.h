#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Primitive.h"

// Remember to delete mesh when extending
class Entity
{
public:
	Entity(Transform* transform, Mesh* mesh, Texture* texture);
	virtual ~Entity();
	Transform* getTransform() const;
	void setTransform(Transform* transform);
	virtual void render(Shader* shader);
	Texture* getTexture();
	void setMesh(Mesh* mesh);

	Mesh* getMesh();
	void setTexture(Texture*);

protected:
	Transform* transform;
	Mesh* mesh;
	Texture* texture;
	virtual void bindTextures();
};

