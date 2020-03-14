#include "CharacterMovement.hpp"
#include "Collider.hpp"


CharacterMovement::CharacterMovement(Shape* shape) : Component("CharacterMovement")
{

	setSelf(shape);

}

GameEvent CharacterMovement::movement(int time, bool input)
{
	Vector2f characterMovement(0.f, 0.f);

	// movement speed
	float horizontalMovement = 5.f * time;

	if (input) {
		// character keyboard commands
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			characterMovement.x += horizontalMovement;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			characterMovement.x -= horizontalMovement;
		}
	}

	return GameEvent{ Input, 0, 0, characterMovement };
}

void CharacterMovement::setSelf(Shape* shape)
{
	self = shape;
}

Shape* CharacterMovement::getSelf()
{
	return self;
}

void CharacterMovement::update(Vector2f movement)
{

	getSelf()->move(movement);

}



