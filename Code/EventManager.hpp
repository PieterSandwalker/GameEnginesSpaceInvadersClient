#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include <list>
#include "Component.hpp"
#include <zmq.hpp>
#include "GameObject.hpp"
#include <variant>

using namespace std;
using namespace sf;

enum EventType {
	Death,
	Spawn,
	Input,
	Collision,
	Impact
};

struct GameEvent {
	EventType type;
	int objID;
	int time;
	Vector2f data;
};

class Compare {
public:
	bool operator() (GameEvent a , GameEvent b)
	{
		return a.time > b.time;
	}
};

