#include <iostream>
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <list>
#include <thread>
#include "../PracticaXarxes/Player.h"
using namespace std;
using namespace sf;

class PlayerClient : public Player{
public:
	PlayerClient(int _ID, string _nickName, Vector2i _position)
	{
		ID = _ID;
		nickName = _nickName;
		position = _position;
		
	};

};


enum MessageIDs
{
	CreateNewLobby,
	JoinLobby,
	LobbyName,
	LobbyInfoDump,
	WantToJoinLobby,
	LobbyPlayerJoin,
	LobbyStartGame,
	Connect,
	AcceptConnect,
	PlayerInfoBegin,
	TurnId,
	NewLife,
	NewPosition,
	Move,
	Shoot,
	NewShot,
	Bomb,
	NewBombID,
	PassTurn,
	PlayerLeft,
	EndMatch,
	Level,
	TextID
};


enum Direction
{
	Up,
	Down,
	Right,
	Left,
};

struct LobbyInfo {
	string LobbyName;
	int LobbyID;
	int NumPlayers;

};

#define MAX_MENSAJES 30
sf::TcpListener listener;
//sf::IpAddress ip = sf::IpAddress::getLocalAddress();
sf::TcpSocket socket;
vector<string> aMensajes;
bool endGame;
PlayerClient *Player;
vector<PlayerClient*> playersVector;
string myName;
bool readyToStart = false;
bool createLobby;
int MyLobbyId;

int windowWidth = 700;
int windowHeight = 500;
int verticalCells = 10;
int horizontalCells = 10;
int cellSize = windowHeight / horizontalCells;

void receiveFunction() {
	
	while (true) {
		int idReceived;
		sf::Packet packet;
		sf::String mensajeRecibido;
		sf::Socket::Status statusReceive = socket.receive(packet);
		packet >> idReceived;

		if (statusReceive == sf::Socket::Disconnected) {
			cout << "error al recibir datos" << endl;
			socket.disconnect();
			endGame = true;
			break;
		}
		else {
			switch (idReceived) {
			case LobbyPlayerJoin:
			{
				int numPlayers;
				

			}
			case AcceptConnect://quan accepta la conexio el servido es crea una nou player en client
			{
				int iD;
				Vector2i playerPos;
				packet >> iD >> playerPos.x >> playerPos.y;
				Player = new PlayerClient(iD, myName, playerPos);
				//rivalsInGame.push_back(Player);
				//guardar 
				break;
			}
			case PlayerInfoBegin:
			{
				//rivalsInGame = new vector<PlayerClient*>();
				cout << "Match Begins" << endl;
				int id0, id1, id2, id3;
				string rivalName0, rivalName1, rivalName2, rivalName3;
				Vector2i rival0Pos, rival1Pos, rival2Pos, rival3Pos;

				packet >> id0 >> rivalName0 >> rival0Pos.x >> rival0Pos.y 
						>> id1 >> rivalName1 >> rival1Pos.x >> rival1Pos.y
						>> id2 >> rivalName2 >> rival2Pos.x >> rival2Pos.y
						>> id3 >> rivalName3 >> rival3Pos.x >> rival3Pos.y;

				PlayerClient *rival0 = new PlayerClient(id0, rivalName0, rival0Pos);
				playersVector.push_back(rival0);
				PlayerClient *rival1 = new PlayerClient(id1, rivalName1, rival1Pos);
				playersVector.push_back(rival1);
				PlayerClient *rival2 = new PlayerClient(id2, rivalName2, rival2Pos);
				playersVector.push_back(rival2);
				PlayerClient *rival3 = new PlayerClient(id3, rivalName3, rival3Pos);
				playersVector.push_back(rival3);
				readyToStart = true;
				break;
			}
			case TurnId:
			{	
				int anId;
				packet >> anId;
				if (anId == Player->ID) {
					Player->haveTurn = true;
				}
				for (int i = 0; i < playersVector.size(); i++) {
					if (anId == playersVector[i]->ID) {
						playersVector[i]->haveTurn = true;
					}
					else {
						playersVector[i]->haveTurn = false;
					}
				}			
				break;
			}
			case NewPosition:
			{
				int anId;
				Vector2i newPos;
				packet >> anId >> newPos.x >> newPos.y;
				if (anId == Player->ID) {
					Player->position = newPos;
				}
				for (int i = 0; i < (int)playersVector.size(); i++) {
					if (anId == playersVector[i]->ID) {
						playersVector[i]->position = newPos;
						cout << playersVector[i]->nickName << " is in a new position" << endl;
					}
				}
				break;
			}
			case NewShot: //<< bool << playerId << direction(string) << diditHit << idHited << lifesHited
			{
				bool _haveShoot, didItHit;
				int _id, direction, _idHitted, _lifes;
				packet >> _haveShoot;
				if (_haveShoot) {
					cout << "You have already shot this turn, wait to your next turn to do some action" << endl;
				}
				else {
					packet >> _id >> direction >> didItHit >> _idHitted >> _lifes;
					//cout << "id " << _id << " direction " << (int)direction << " didItHit " << didItHit << " idHitted " << _idHitted << "_lifes new " << _lifes << endl;
					if (_id == Player->ID)
					{
						cout << "You have shot" << endl;
					}
					else {
						for (int i = 0; i < (int)playersVector.size(); i++) {
							if (_id == playersVector[i]->ID) {
								cout << playersVector[i]->nickName << " has shot" << endl;
							}
						}
					}
					if (didItHit == true) {
						if (_idHitted == Player->ID) {
							Player->lifes = _lifes;
							if (Player->lifes <= 0) {
								cout << "YOU LOSE" << endl;
							}
						}
						for (int i = 0; i < (int)playersVector.size(); i++) {
							if (_idHitted == playersVector[i]->ID) {
								playersVector[i]->lifes = _lifes;
								cout << "Player " << playersVector[i]->nickName << " have been shot";
								if (playersVector[i]->lifes <= 0) {
									cout << "Player " << playersVector[i] << " has died" << endl;
								}
							}
						}
					}
				}
				break;
			}
			case Level://<< playersGaming[i]->ID << playersGaming[i]->level;
			{
				int _id, _level;
				packet >> _id >> _level;
				if (_id == Player->ID) {
					Player->level = _level;
					cout << "Level up: " << _level << endl;
				}
				else {
					for (int i = 0; i < (int)playersVector.size(); i++) {
						if (playersVector[i]->ID == _id) {
							playersVector[i]->level = _level;
							cout << "Rival " << playersVector[i]->nickName << " has Leveled Up to " << _level << endl;
						}
					}
				}
				break;
			}
			case NewBombID://<< NewBombID << playersGaming[i]->hasBom << playersGaming[i]->ID b << didItHit 
			{			//<< numAfectedplayers << idAffected << lifes 
				int _id, numAffected, _idAffected, _lifes;
				bool _haveBomb, _isHit;
				packet >> _haveBomb >> _id  >> _isHit;
				if (_haveBomb) {
					//el jugador no te perque saber si els rivals tenen bomba o no, aixi que en el cas que el id sigui dun rival no lactualitzarem, notifiquem quan la llencen i prou
					for (int i = 0; i < (int)playersVector.size(); i++) {
						if (_id == playersVector[i]->ID && _isHit) {
							cout << "Player " << playersVector[i]->nickName << " droped the bomb" << endl;
							packet >> numAffected;
							cout << "Number of players affected: " << numAffected << endl;
							for (int j = 1; j <= numAffected; j++) {
								packet >> _idAffected >> _lifes;
								if (_idAffected == Player->ID) {
									Player->lifes = _lifes;
									if (Player->lifes <= 0) {
										cout << "YOU LOSE" << endl;
									}
								}
								for (int k = 0; k < (int)playersVector.size(); k++) {
									if (_idAffected == playersVector[k]->ID)
									{
										playersVector[k]->lifes = _lifes;
										if (playersVector[k]->lifes <= 0) 
										{
										cout << "Player " << playersVector[k]->nickName << " has died" << endl;
										}
									}
								}
								
							}
						}
					}
				}
				else
				{
					cout << "You have already droped the only bomb in this game" << endl;
				}
				
				break;
			}
			case EndMatch://<< playersGaming[i]->ID << playersGaming[i]->kills;
			{
				int _idWinner, _kills;
				packet >> _idWinner >> _kills;
				if (_idWinner == Player->ID) {
					Player->level = _idWinner;
					cout << "Congratulations, you have won. Kills : " << _kills << endl;
				}
				else {
					for (int i = 0; i < (int)playersVector.size(); i++) {
						if (playersVector[i]->ID == _idWinner) {
							playersVector[i]->level = _idWinner;
							cout << "Rival " << playersVector[i]->nickName << " has won " << endl;
						}
					}
				}
				break;
			}
			case PlayerLeft:
			{
				int anId;
				packet >> anId;

				for (int i = 0; i < (int)playersVector.size(); i++) {
					if (anId == playersVector[i]->ID) {
						cout << playersVector[i]->nickName << " left the game" << endl;
					}
				}
				break;
			}
			case TextID:
			{
				string newMessage;
				packet >> newMessage;
				aMensajes.push_back(newMessage);
				break;
			}
			default:
				break;
			}
			

		}
		
	}
}

void sendInfo(Socket _socket, Packet _packToSend) {
}

void DrawFunction(sf::RenderWindow* window) {
	sf::Font font;
	if (!font.loadFromFile("cour.ttf")) { cout << "Can't load the font file" << endl; }


	for (int i = 0; i < verticalCells; i++) {
		for (int j = 0; j < horizontalCells; j++) {
			sf::RectangleShape whiteCell(sf::Vector2f(cellSize, cellSize));
			whiteCell.setFillColor(sf::Color::White);

			sf::RectangleShape cyanCell(sf::Vector2f(cellSize, cellSize));
			cyanCell.setFillColor(sf::Color::Cyan);


			if (i % 2 == 0) {
				if (j % 2 == 0) {
					whiteCell.setPosition(sf::Vector2f(i*cellSize, j*cellSize));
					window->draw(whiteCell);
				}
				else {
					cyanCell.setPosition(sf::Vector2f(i*cellSize, j*cellSize));
					window->draw(cyanCell);
				}
			}
			else {
				if (j % 2 == 1) {
					whiteCell.setPosition(sf::Vector2f(i*cellSize, j*cellSize));
					window->draw(whiteCell);
				}
				else {
					cyanCell.setPosition(sf::Vector2f(i*cellSize, j*cellSize));
					window->draw(cyanCell);
				}
			}
		}
	}

#pragma region HUD
	sf::Text Player1Name(playersVector[0]->nickName, font, 16);
	Player1Name.setStyle(sf::Text::Bold);
	Player1Name.setPosition(sf::Vector2f(555, 55));

	sf::Text Player2Name(playersVector[1]->nickName, font, 16);
	Player2Name.setStyle(sf::Text::Bold);
	Player2Name.setPosition(sf::Vector2f(555, 130));

	sf::Text Player3Name(playersVector[2]->nickName, font, 16);
	Player3Name.setStyle(sf::Text::Bold);
	Player3Name.setPosition(sf::Vector2f(555, 205));

	sf::Text Player4Name(playersVector[3]->nickName, font, 16);
	Player4Name.setStyle(sf::Text::Bold);
	Player4Name.setPosition(sf::Vector2f(555, 270));

	sf::Text Player1LifesText("Lifes: ", font, 14);
	Player1LifesText.setPosition(sf::Vector2f(550, 75));

	sf::Text Player2LifesText("Lifes: ", font, 14);
	Player2LifesText.setPosition(sf::Vector2f(550, 150));

	sf::Text Player3LifesText("Lifes: ", font, 14);
	Player3LifesText.setPosition(sf::Vector2f(550, 225));

	sf::Text Player4LifesText("Lifes: ", font, 14);
	Player4LifesText.setPosition(sf::Vector2f(550, 290));

	sf::Text Player1Lifes(to_string(playersVector[0]->lifes), font, 16);
	Player1Lifes.setStyle(sf::Text::Bold);
	Player1Lifes.setPosition(sf::Vector2f(600, 74));

	sf::Text Player2Lifes(to_string(playersVector[1]->lifes), font, 16);
	Player2Lifes.setStyle(sf::Text::Bold);
	Player2Lifes.setPosition(sf::Vector2f(600, 149));

	sf::Text Player3Lifes(to_string(playersVector[2]->lifes), font, 16);
	Player3Lifes.setStyle(sf::Text::Bold);
	Player3Lifes.setPosition(sf::Vector2f(600, 224));

	sf::Text Player4Lifes(to_string(playersVector[3]->lifes), font, 16);
	Player4Lifes.setStyle(sf::Text::Bold);
	Player4Lifes.setPosition(sf::Vector2f(600, 289));

	sf::Text Player1LevelText("Level: ", font, 14);
	Player1LevelText.setPosition(sf::Vector2f(550, 95));

	sf::Text Player2LevelText("Level: ", font, 14);
	Player2LevelText.setPosition(sf::Vector2f(550, 170));

	sf::Text Player3LevelText("Level: ", font, 14);
	Player3LevelText.setPosition(sf::Vector2f(550, 245));

	sf::Text Player4LevelText("Level: ", font, 14);
	Player4LevelText.setPosition(sf::Vector2f(550, 310));

	sf::Text Player1Level(to_string(playersVector[0]->level), font, 16);
	Player1Level.setStyle(sf::Text::Bold);
	Player1Level.setPosition(sf::Vector2f(600, 94));

	sf::Text Player2Level(to_string(playersVector[1]->level), font, 16);
	Player2Level.setStyle(sf::Text::Bold);
	Player2Level.setPosition(sf::Vector2f(600, 169));

	sf::Text Player3Level(to_string(playersVector[2]->level), font, 16);
	Player3Level.setStyle(sf::Text::Bold);
	Player3Level.setPosition(sf::Vector2f(600, 244));

	sf::Text Player4Level(to_string(playersVector[3]->level), font, 16);
	Player4Level.setStyle(sf::Text::Bold);
	Player4Level.setPosition(sf::Vector2f(600, 309));

	sf::Text TurnText("Turn: ", font, 16);
	TurnText.setStyle(sf::Text::Bold);
	TurnText.setPosition(sf::Vector2f(530, 20));

	sf::Text EventTextMove("WASD to move the tank", font, 12);
	EventTextMove.setStyle(sf::Text::Bold);
	EventTextMove.setPosition(sf::Vector2f(510, 360));

	sf::Text ShootTextMove("ARROWS to shoot", font, 12);
	ShootTextMove.setStyle(sf::Text::Bold);
	ShootTextMove.setPosition(sf::Vector2f(510, 375));

	sf::Text BombTextMove("R to throw bomb", font, 12);
	BombTextMove.setStyle(sf::Text::Bold);
	BombTextMove.setPosition(sf::Vector2f(510, 390));

	sf::Text EndTurnTextMove("ENTER to end your turn", font, 12);
	EndTurnTextMove.setStyle(sf::Text::Bold);
	EndTurnTextMove.setPosition(sf::Vector2f(510, 405));


#pragma endregion

	sf::Text TurnName;
	TurnName.setStyle(sf::Text::Bold);
	TurnName.setPosition(sf::Vector2f(590, 20));
	for (int j = 0; j < playersVector.size(); j++) {
		if (playersVector[j]->haveTurn == true) {
			string temp = playersVector[j]->nickName;
			TurnName.setString(temp);
			TurnName.setFont(font);
			TurnName.setCharacterSize(16);
		}
	}

	sf::CircleShape circle1, circle2, circle3, circle4;
	circle1.setRadius(25.f);
	circle2.setRadius(25.f);
	circle3.setRadius(25.f);
	circle4.setRadius(25.f);
	circle1.setPosition(playersVector[0]->position.x * cellSize, playersVector[0]->position.y * cellSize);
	circle2.setPosition(playersVector[1]->position.x * cellSize, playersVector[1]->position.y * cellSize);
	circle3.setPosition(playersVector[2]->position.x * cellSize, playersVector[2]->position.y * cellSize);
	circle4.setPosition(playersVector[3]->position.x * cellSize, playersVector[3]->position.y * cellSize);
	circle1.setFillColor(Color::Blue);//1,1
	circle2.setFillColor(Color::Green);//1,10
	circle3.setFillColor(Color::Yellow);//10, 1
	circle4.setFillColor(Color::Red);//10,10

	window->draw(Player1Name);
	window->draw(Player2Name);
	window->draw(Player3Name);
	window->draw(Player4Name);
	window->draw(Player1LifesText);
	window->draw(Player2LifesText);
	window->draw(Player3LifesText);
	window->draw(Player4LifesText);
	window->draw(Player1Lifes);
	window->draw(Player2Lifes);
	window->draw(Player3Lifes);
	window->draw(Player4Lifes);
	window->draw(Player1LevelText);
	window->draw(Player2LevelText);
	window->draw(Player3LevelText);
	window->draw(Player4LevelText);
	window->draw(Player1Level);
	window->draw(Player2Level);
	window->draw(Player3Level);
	window->draw(Player4Level);
	window->draw(TurnText);
	window->draw(TurnName);
	window->draw(EventTextMove);
	window->draw(ShootTextMove);
	window->draw(BombTextMove);
	window->draw(EndTurnTextMove);

	if (playersVector[0]->lifes > 0)
	{
		window->draw(circle1);
	}
	if (playersVector[1]->lifes > 0)
	{
		window->draw(circle2);
	}
	if (playersVector[2]->lifes > 0)
	{
		window->draw(circle3);
	}
	if (playersVector[3]->lifes > 0)
	{
		window->draw(circle4);
	}
}

char askLobby() {
	char playerCreateLobby;
	cout << "What do you want to do?" << endl << "1. Create a lobby" << endl << "2. Join a lobby" << endl;
	cin >> playerCreateLobby;

	return playerCreateLobby;
}

string writeLobbyName() {
	string lobbyNameString;
	cout << "Give the lobby a name:" << endl;
	cin >> lobbyNameString;

	return lobbyNameString;
}

int selectLobbyLifes() {
	int lobbyLifes;
	cout << "Select the ammount of lifes that players will start with (1, 2 or 3):" << endl;
	cin >> lobbyLifes;

	return lobbyLifes;
}

void DrawLobbyFunction(sf::RenderWindow* window) {

}
int main()
{
	
	cout << "Introduce your name" << endl;
	cin >> myName;

	//1.ESTABLECER CONEXION
	sf::Socket::Status status = socket.connect(IpAddress::getLocalAddress(), 50000, sf::seconds(5.f));//sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	if (status != sf::Socket::Done)
	{
		cout << " Could not connect " << endl;
	}
	else
	{
		cout << "Connected " << endl;
	}

	sf::Packet connectPacket;
	connectPacket << Connect << myName;
	socket.send(connectPacket);


	//Preguntar si vol crear una nova partida o si vol unir-se a una
	char playerSelected = askLobby();

	if(playerSelected == '1' )
	{
		cout << "You will create a new lobby, name your Lobby:" << endl;
		
		//Dona nom a la lobby
		Packet newLobbyCreation;
		newLobbyCreation << CreateNewLobby << writeLobbyName(); //Comprovar al servidor si esta repetit
		//sf::Socket::Status status = socket.send(newLobbyCreation);
		cout << "How many lives do you want to play at?" << endl;
		//Selecciona les vides
		newLobbyCreation << selectLobbyLifes();

		socket.send(newLobbyCreation);

		newLobbyCreation.clear();

		cout << "You created a new lobby" << endl;



		int lobbyIdTemp;
		Packet lobbyIdPacket;
		socket.receive(lobbyIdPacket);
		lobbyIdPacket >> lobbyIdTemp;
		MyLobbyId = lobbyIdTemp;
		cout << "my Lobby id is: " << lobbyIdTemp << endl;
		//S'obre el chat i espera a que es conectin 4 jugadors

		//El que ha creat la partida escriu START

		//Comença la partida

	
	}
	else if (playerSelected == '2') 
	{
		cout << "How many lives do you want to play with?" << endl;
		//S'uneix lobby
		int lifesToPlay;
		cin >> lifesToPlay;
		Packet joinLobby;
		joinLobby << WantToJoinLobby << lifesToPlay;

		socket.send(joinLobby);
		joinLobby.clear();
		
		vector<LobbyInfo> LobbyVector;
		Packet lobbyInfoPacket;
		sf::Socket::Status statusReceive = socket.receive(lobbyInfoPacket);
		int idPacket;
		lobbyInfoPacket >> idPacket;
		cout << idPacket << endl;
	
		int counter;
		lobbyInfoPacket >> counter;
		cout << "There are " << counter << " server with your settings: " << endl;
		for (int k = 0; k < counter; k++) {
			lobbyInfoPacket >> LobbyVector[k].LobbyName >> LobbyVector[k].NumPlayers >> LobbyVector[k].LobbyID;
			cout << "1. " << LobbyVector[k].LobbyName << " Players: " << LobbyVector[k].NumPlayers << endl;
		}
		cout << "wich server do you want to join? (enter the number behind the server name)" << endl;
		int serverID;
		cin >> serverID;
		MyLobbyId = LobbyVector[serverID].LobbyID;
		Packet LobbyToJoin;
		LobbyToJoin << JoinLobby << LobbyVector[serverID].LobbyID;
		system("pause");
		
		//Apareixen les lobbys

		//Pregunta si vol que nomes li apareguin les que tenen X vides o totes.

		//Selecciona la partida i s'uneix

		//S'obri el chat i espera a que el creador escrigui START

		//Comença la partida

	
	}



	sf::Vector2i screenDimensions(windowWidth, windowHeight);

	sf::RenderWindow window;
	const string windowName = "BattleArena: " + myName + "\n";
	cout << windowName;
	window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), windowName);
	window.setFramerateLimit(60);
	sf::RenderWindow* _window = &window;
	sf::Font font;
	if (!font.loadFromFile("cour.ttf")) { std::cout << "Can't load the font file" << std::endl; }

	sf::String mensaje = myName + ">";

	sf::Text chattingText(mensaje, font, 14);
	chattingText.setFillColor(sf::Color(0, 160, 0));
	chattingText.setStyle(sf::Text::Bold);
	sf::Text text(mensaje, font, 14);
	text.setFillColor(sf::Color(0, 160, 0));
	text.setStyle(sf::Text::Bold);
	text.setPosition(0, 260);

	sf::RectangleShape separator(sf::Vector2f(800, 5));
	separator.setFillColor(sf::Color(200, 200, 200, 255));
	separator.setPosition(0, 550);

	std::thread recieveThread(receiveFunction);
	recieveThread.detach();

	while (window.isOpen())
	{
		
		if (readyToStart == true) { // draw game 
			DrawFunction(_window);
		}

		sf::Event evento;
		while (window.pollEvent(evento))
		{
			switch (evento.type)
			{
			
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evento.key.code == sf::Keyboard::Escape) {
					socket.disconnect();
					window.close();
				}
				else if (evento.key.code == sf::Keyboard::Return) // send message here, receive en un thread
				{
					sf::Packet sendPacket;
					sendPacket << TextID << MyLobbyId << mensaje;
					sf::Socket::Status status = socket.send(sendPacket);
					if (status != sf::Socket::Done) {
						std::cout << "error al enviar misatge " << std::endl;
						break;
					}

					mensaje = myName + ">";
					sendPacket.clear();
				}
				else if (evento.key.code == sf::Keyboard::Space && readyToStart == true)
				{
					if (Player->haveTurn == true) {
						sf::Packet passTurn;
						passTurn << PassTurn;
						sf::Socket::Status status = socket.send(passTurn);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						passTurn.clear();
						Player->haveTurn = false;
						cout << "You passed turn" << endl;
					}

				}
				else if (evento.key.code == sf::Keyboard::W && readyToStart == true) {
					if (Player->haveTurn == true) {
						Packet clientPos;
						clientPos << Move << Up;
						sf::Socket::Status status = socket.send(clientPos);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientPos.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::S && readyToStart == true) {
					if (Player->haveTurn == true) {
						Packet clientPos;
						clientPos << Move << Down;
						sf::Socket::Status status = socket.send(clientPos);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientPos.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::D && readyToStart == true) {
					if (Player->haveTurn == true) {
						Packet clientPos;
						clientPos << Move << Right;
						sf::Socket::Status status = socket.send(clientPos);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientPos.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::A && readyToStart == true) {
					if (Player->haveTurn == true) {
						Packet clientPos;
						clientPos << Move << Left;
						sf::Socket::Status status = socket.send(clientPos);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientPos.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::Up) {
					if (Player->haveTurn == true) {
						Packet clientShot;
						clientShot << Shoot << Up;
						sf::Socket::Status status = socket.send(clientShot);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientShot.clear();
					}
					else {
						cout << "Is not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::Down) {
					if (Player->haveTurn == true) {
						Packet clientShot;
						clientShot << Shoot << Down;
						sf::Socket::Status status = socket.send(clientShot);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientShot.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::Right) {
					if (Player->haveTurn == true) {
						Packet clientShot;
						clientShot << Shoot << Right;
						sf::Socket::Status status = socket.send(clientShot);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientShot.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::Left) {
					if (Player->haveTurn == true) {
						Packet clientShot;
						clientShot << Shoot << Left;
						sf::Socket::Status status = socket.send(clientShot);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientShot.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				else if (evento.key.code == sf::Keyboard::R && readyToStart == true) {
					if (Player->haveTurn == true) {
						Packet clientBomb;
						clientBomb << Bomb;
						sf::Socket::Status status = socket.send(clientBomb);
						if (status != sf::Socket::Done) {
							std::cout << "error al enviar misatge " << std::endl;
							break;
						}
						clientBomb.clear();
					}
					else {
						cout << "It's not your turn" << endl;
					}
				}
				break;
			case sf::Event::TextEntered:
				if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
					mensaje += (char)evento.text.unicode;
				else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
					mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
				break;
			}
		}
		if (readyToStart == false) {
			window.draw(separator);
			for (size_t i = 0; i < aMensajes.size(); i++)
			{
				std::string chatting = aMensajes[i];
				chattingText.setPosition(sf::Vector2f(0, 20 * i));
				chattingText.setString(chatting);
				window.draw(chattingText);
			}
			std::string mensaje_ = mensaje + "_";
			text.setString(mensaje_);
			window.draw(text);
		}
		/*
		if (status == sf::Socket::Status::Disconnected) {
			Packet playerLeft;
			playerLeft << PlayerLeft;
			sf::Socket::Status status = socket.send(playerLeft);
			if (status != sf::Socket::Done) {
				std::cout << "error al enviar misatge " << std::endl;
				break;
			}
			playerLeft.clear();
		}
		*/

		window.display();
		window.clear();

	}
	
	
}