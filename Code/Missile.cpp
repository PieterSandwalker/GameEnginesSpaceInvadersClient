#include "Missile.hpp"
#include "Projectile.hpp"
#include "ProjectileCollider.hpp"
#include <iostream>

Missile::Missile(Vector2f size, Vector2f position, Vector2f destination) : GameObject(NULL, true, true, "MovingPlatform") {

	Shape* shape = new RectangleShape(size);
	shape->setPosition(position);
	setShape(shape);

	shape->setFillColor(sf::Color(100, 200, 200));  //To be replaced by texture

	Projectile* blast = new Projectile(shape, position, destination, 5.f);

	components.push_back(blast);
	components.push_back(new ProjectileCollider(shape));

}