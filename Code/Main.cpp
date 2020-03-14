#include <SFML\Graphics\RenderWindow.hpp>

#include "GameObject.hpp"
#include "Character.hpp"
#include "CharacterMovement.hpp"
#include "Enemy.hpp"
#include "Wall.hpp"

#include <zmq.hpp>
#include <fstream>
#include <iostream>
#include <duktape.c>
#include <duk_config.h>
#include <dukglue/dukglue.h>
#include "Collider.hpp"
#include "Missile.hpp"
#include "Projectile.hpp"
#include "ProjectileCollider.hpp"

#define sleep(n)    Sleep(n)

using namespace std;
using namespace sf;
priority_queue<GameEvent, vector<GameEvent>, Compare> eventQueue;
vector<GameObject*> scene;
Vector2f projectedMove(0,0);
Character player(Vector2f(50, 50), Vector2f(200, 650), "Textures/Spaceship.jpg");
Missile blast(Vector2f(10, 20), Vector2f(0, 900), Vector2f(0,0));
int iterationClock = 0;
bool shot = false;
bool play = true;
float loop;
int id;

/*
 * Uses the load script function from Noah's example
 */
static void load_script_from_file(duk_context* ctx, const char* filename)  
{																		  
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	duk_push_lstring(ctx, buffer.str().c_str(), (duk_size_t)(buffer.str().length()));
}

struct CharacterUpdate {

	int charID;
	sf::Vector2f position;

} typedef CharacterUpdate;

void sceneOne(vector<GameObject*>* scene) {
	
	scene->push_back(new Wall(Vector2f(25, 750), Vector2f(0, 0), "Textures/SciFi2.jpg"));
	scene->push_back(new Wall(Vector2f(25, 750), Vector2f(775, 0), "Textures/SciFi2.jpg"));

	// Row 1
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(50, 25), Vector2f(200, 25), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(150, 25), Vector2f(300, 25), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(250, 25), Vector2f(400, 25), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(350, 25), Vector2f(500, 25), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(450, 25), Vector2f(600, 25), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(550, 25), Vector2f(700, 25), "Textures/Alien.jpg"));

	// Row 2
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(50, 100), Vector2f(200, 100), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(150, 100), Vector2f(300, 100), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(250, 100), Vector2f(400, 100), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(350, 100), Vector2f(500, 100), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(450, 100), Vector2f(600, 100), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(550, 100), Vector2f(700, 100), "Textures/Alien.jpg"));

	// Row 3
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(50, 175), Vector2f(200, 175), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(150, 175), Vector2f(300, 175), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(250, 175), Vector2f(400, 175), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(350, 175), Vector2f(500, 175), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(450, 175), Vector2f(600, 175), "Textures/Alien.jpg"));
	scene->push_back(new Enemy(Vector2f(50, 50), Vector2f(550, 175), Vector2f(700, 175), "Textures/Alien.jpg"));

}

void onEvent(GameEvent e);

void raise(GameEvent e) {
	if (iterationClock >= e.time) {
		onEvent(e);
	}
	else {
		eventQueue.push(e);
	}
}

void onEvent(GameEvent e) {

	// Create a heap and initial context
	duk_context* ctx = NULL;

	ctx = duk_create_heap_default();
	if (!ctx) {
		printf("Failed to create a Duktape heap.\n");
		exit(1);
	}

	switch (e.type) {
	case(Death):

		load_script_from_file(ctx, "Scripts/death.js");
		if (duk_peval(ctx) != 0) {
			printf("Error: %s\n", duk_safe_to_string(ctx, -1));
			duk_destroy_heap(ctx);
			exit(1);
		}
		duk_pop(ctx);

		duk_push_global_object(ctx);
		duk_get_prop_string(ctx, -1, "death");

		dukglue_push(ctx, &player);

		dukglue_register_method(ctx, &GameObject::setRender, "toggleRender");
		dukglue_set_base_class<GameObject, Character>(ctx);

		if (duk_pcall(ctx, 1) != 0)
			printf("Error: %s\n", duk_safe_to_string(ctx, -1));

		duk_pop(ctx);
		
		raise(GameEvent{ Spawn, 0, iterationClock + 1, player.getCheckpoint() });
		break;
	case(Spawn):
		play = false;
		break;
	case(Input):
		projectedMove = e.data;
		for (int i = 0; i < scene.size(); i++) {
			if (scene[i]->isCollidable()) {
				int j = scene[i]->hasComponent("Collider");
				if (j >= 0) {
					Collider* collide = (Collider*)scene[i]->getComponent(j);
					GameEvent e = collide->update(&player, &projectedMove);
					e.time = iterationClock;
					raise(e);
				}
			}
		}
		break;
	case(Collision):
		projectedMove = e.data;
		break;
	case(Impact):
		shot = false;
		Projectile* missileBlast = (Projectile*)blast.getComponent(blast.hasComponent("Projectile"));
		missileBlast->reset(Vector2f(0, 900));
		break;
	}

	duk_destroy_heap(ctx);
}

int main() 
{

	// character position message
	CharacterUpdate message = { 0, sf::Vector2f(0.f,0.f) };

	// SUB socket connection
	zmq::context_t context1(1);
	zmq::socket_t socket1(context1, ZMQ_SUB);

	socket1.connect("tcp://localhost:5556");

	// REQ socket connection
	zmq::context_t context2(1);
	zmq::socket_t socket2(context2, ZMQ_REQ);

	std::cout << "Requesting connection" << std::endl;
	socket2.connect("tcp://localhost:5555");

	// Create message
	CharacterUpdate* update = &message;

	// Ask permission to connect
	zmq_send(socket2, update, sizeof(CharacterUpdate), 0);

	zmq_recv(socket2, &message, sizeof(CharacterUpdate), 0);

	if (message.charID == 0) {

		std::cout << "Connection failed" << std::endl;

		return 0;
	}
	else {

		id = message.charID;
	
	}

	zmq_setsockopt(socket1, ZMQ_SUBSCRIBE, NULL, 0);


	// Create window
	sf::RenderWindow window(sf::VideoMode(800, 750), "My window", sf::Style::Default);

	// Load scene
	
	sceneOne(&scene);
	
	// Set locations
	vector<Vector2f> locations;
	for (int i = 0; i < scene.size(); i++) {
		locations.push_back(Vector2f(0,0));
	}

	// Create objects_en_scene count
	int objects_en_scene = scene.size();

	// Character

	Vector2f unaltered_Char_location = player.getShape()->getPosition();

	// Send starting location
	message.charID = id;
	message.position = player.getShape()->getPosition();

	zmq_send(socket2, &message, sizeof(CharacterUpdate), 0);

	cout << "starting location sent" << endl;

	// Recieve amount of objects in scene (including characters)

	zmq_recv(socket2, &message, sizeof(CharacterUpdate), 0);

	objects_en_scene = message.charID;

	// Make objects to represent characters also connected
	for (int i = scene.size(); i < objects_en_scene; i++) {
		if (i != id) {
			scene.push_back(new Character(Vector2f(25, 50), Vector2f(200, 400), "Textures/SciFi2.jpg"));
			locations.push_back(Vector2f(0, 0));
		}
		else {
			scene.push_back(new Character(Vector2f(25, 50), Vector2f(200, 400), "Textures/SciFi2.jpg"));
			locations.push_back(player.getShape()->getPosition());
		}
	}

	bool takingInput = true;

	// Game loop
	// Game loop
	while (window.isOpen() && play)
	{

		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::LostFocus)
				takingInput = false;

			if (event.type == sf::Event::GainedFocus)
				takingInput = true;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			shot = true;
			Projectile* missileBlast = (Projectile*)blast.getComponent(blast.hasComponent("Projectile"));
			missileBlast->reset(player.getShape()->getPosition());
		}

		//recieve locations

		for (int i = scene.size(); i < objects_en_scene; i++) {
			scene.push_back(new Character(Vector2f(25, 50), Vector2f(0, 0), "Textures/SciFi2.jpg"));
			locations.push_back(Vector2f(0, 0));
		}

		zmq_recv(socket1, locations.data(), sizeof(Vector2f) * objects_en_scene, 0);


		// Client - Load character movement

		int compLoc = player.hasComponent("CharacterMovement");
		CharacterMovement* moveControl = (CharacterMovement*)player.getComponent(compLoc);

		GameEvent e = moveControl->movement(1, takingInput);
		e.time = iterationClock;
		raise(e);

		for (int i = 0; i < objects_en_scene; i++) {

			scene[i]->getShape()->setPosition(locations[i]);
			if (shot && i != id) {
				int m = blast.hasComponent("Collider");
				ProjectileCollider* blastCollide = (ProjectileCollider*)blast.getComponent(m);

				raise(blastCollide->update(scene[i], &projectedMove));
			}

		}

		if (shot) {
			Projectile* missileBlast = (Projectile*)blast.getComponent(blast.hasComponent("Projectile"));
			missileBlast->update(1);
		}

		// Move character and scene (sideboundary)

		moveControl->update(projectedMove);
		unaltered_Char_location += projectedMove;

		// send character location
		message.charID = id;
		message.position = unaltered_Char_location;

		locations[id] = unaltered_Char_location;

		zmq_send(socket2, &message, sizeof(CharacterUpdate), 0);

		// recieve amount of objects in next update

		zmq_recv(socket2, &message, sizeof(CharacterUpdate), 0);

		objects_en_scene = message.charID;

		while (!eventQueue.empty()) {
			if (eventQueue.top().time <= iterationClock) {
				onEvent(eventQueue.top());
				eventQueue.pop();
			}
			break;
		}

		// Client - Draw object

		window.clear(sf::Color::Black);

		for (int i = 0; i < scene.size(); i++) {
			if (scene[i]->isRendered()) {
				if (i == id) {
					window.draw(*player.getShape());
				}
				else {
					window.draw(*scene[i]->getShape());
				}
			}
		}

		window.draw(*blast.getShape());

		window.display();

		iterationClock++;

	}

	return 0;
}

