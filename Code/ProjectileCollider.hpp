#pragma once
#include "Collider.hpp"

class ProjectileCollider :
	public Collider
{


public:
	ProjectileCollider(Shape*);
	GameEvent update(GameObject*, Vector2f*);

};
