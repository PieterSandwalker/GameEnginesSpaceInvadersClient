#include "ProjectileCollider.hpp"
#include "Character.hpp"

ProjectileCollider::ProjectileCollider(Shape* self) : Collider(self)
{

}

GameEvent ProjectileCollider::update(GameObject* enemy, Vector2f* velocity)
{

	bool collision = false;

	sf::FloatRect charBox = enemy->getShape()->getGlobalBounds();
	sf::FloatRect objBox = getSelf()->getGlobalBounds();

	// Horizontal check
	if (charBox.intersects(objBox))
	{
		collision = true;
	}

	if (charBox.intersects(objBox))
	{
		collision = true;
	}

	if (collision) {
		enemy->setCollidable(false);
		enemy->setRender(false);
		return GameEvent{ Impact, 0, 0, *velocity };
	}
	else {

		return GameEvent{ Collision, 0, 0, *velocity };
	}

}
