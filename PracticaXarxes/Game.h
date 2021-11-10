#pragma once
#include<SFML\Network.hpp>
#include<iostream>
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <list>
#include "Player.h"

class Game
{
public:
	std::vector<Player> clientsInLobby;
	int lifesToStart;
	string lobbyName;
	bool gameStarted = false;
	int lobbyID;
	Game();
	~Game();
};

