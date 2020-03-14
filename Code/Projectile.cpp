#include "Projectile.hpp"
#include <math.h>
#include <iostream>

Projectile::Projectile(Shape* self, Vector2f start, Vector2f dest, float velocity) : Component("Projectile")
{
	setStartLocation(start);
	setDestination(dest);
	setShape(self);

	Vector2f distance = dest - start;
	float dist = pow(distance.x, 2) + pow(distance.y, 2);
	dist = sqrt(dist);
	dist = dist / velocity;
	steps = (int)dist;
	current_step = 1;
	direction = true;

	setVelocity(distance / dist);

}

void Projectile::update(int time)
{
	if (current_step != steps) {
		getSelf()->move(final_velocity);
		current_step++;
	}

}

void Projectile::reset(Vector2f start) {

	setStartLocation(start);
	setDestination(start + Vector2f(0.f, 700.f));
	self->setPosition(start);
	current_step = 0;

}

void Projectile::setStartLocation(Vector2f start)
{
	start_location = start;
}

Vector2f Projectile::getStartLocation()
{
	return start_location;
}

void Projectile::setDestination(Vector2f dest)
{
	destination = dest;
}

Vector2f Projectile::getDestination()
{
	return destination;
}

void Projectile::setVelocity(Vector2f velocity)
{
	final_velocity = velocity;
}

Vector2f Projectile::getVelocity()
{
	return final_velocity;
}

void Projectile::setShape(Shape* newSelf)
{
	self = newSelf;
}

Shape* Projectile::getSelf()
{
	return self;
}
