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

using namespace sf;
using namespace std;

class Player {
public:
	int ID;
	string nickName;
	bool isAlive = true;
	int lifes = 1;
	int level = 1;//level 1 = 1 dmg, level 2 = treus 2 vides
	Vector2i position;
	int steps = 0;
	bool haveTurn = false;
	bool hasShot = false;
	bool hasBomb = true;
	sf::TcpSocket *playerSocket;
	int kills = 0;//com les kills nomes sinformen al player que guanya la partida en el moment donat no fa falta que el client les tingui, simplement li pasarem un int que despres 
			//imprimira per console
	Player(int _ID, sf::TcpSocket* _playerSocket, Vector2i _position) {
		ID = _ID;
		position = _position;
		playerSocket = _playerSocket;
	};
	Player() {};
};