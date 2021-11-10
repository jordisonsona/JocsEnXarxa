#include<iostream>
#include <SFML\Graphics.hpp>
#include <vector>
#include <cstring>
#include <list>
#include "Player.h"
#include "Game.h"


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
void SendToAll(Packet _packet, vector<Player> _players) {
	for (int i = 0; i < _players.size(); i++) {
		_players[i].playerSocket->send(_packet);
	}
}

int main() {

	TcpSocket socket;
	vector<Player> playersVector, playersGaming;
	vector<Game> lobbyVector;
	//vector<PlayerServer*> *playersGaming = new vector<PlayerServer*>();//el playersvector sempre conte els 4 players, el playersGaming conte els jugadors vius
	TcpListener listener;
	int IDtogive = 0;
	int lobbyIDtogive = 0;
	bool gameStarted = false;
	int numMoves = 0;
	vector<Vector2i> startingPositions;
	
	startingPositions.push_back(sf::Vector2i(0,0)); 
	startingPositions.push_back(sf::Vector2i(0, 9));
	startingPositions.push_back(sf::Vector2i(9, 0));
	startingPositions.push_back(sf::Vector2i(9, 9));

	//5000 es el puerto por el que escucha el servidor.
	//El cliente debe conocer la ip del servidor y el puerto por el que escucha.
	Socket::Status status = listener.listen(50000);
	if (status != sf::Socket::Done)
	{
		//No se puede vincular al puerto 50000
		std::cout << "cant link port 50000" << std::endl;
	}

	SocketSelector selector;
	selector.add(listener);
	
	while (true)
	{
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				TcpSocket* client = new TcpSocket;
				if (listener.accept(*client) == Socket::Done)
				{
					cout << "nuevo jugador del puerto: " << client->getRemotePort() << endl;
					//aSockets.push_back(client);
					Player player = Player(IDtogive, client, startingPositions[IDtogive]);
					playersVector.push_back(player);
					playersGaming.push_back(player);
					selector.add(*client);
					// fins aqui canvia de lloc 
					if (status != Socket::Disconnected)
					{
						//
					}
				}
				else
				{
					delete client;
				}
			}
			
			else
			{
				for (int i = 0; i<(int)playersVector.size() ; i++)
				{
					TcpSocket* client = playersVector[i].playerSocket;

					if (selector.isReady(*client))
					{
						Packet  receivePacket, clientInfo;
						status = client->receive(receivePacket);
			
						if (status != Socket::Disconnected)
						{
							int messageID;
							string clientName;
							receivePacket >> messageID;
							
							switch (messageID)
							{
								case CreateNewLobby: 
								{
									Game Lobby;
									string lobbyNameTemp;
									int lobbyLifesTemp;
									receivePacket >> lobbyNameTemp >> lobbyLifesTemp;
									Lobby.lobbyName = lobbyNameTemp;
									Lobby.lobbyName = lobbyNameTemp;
									Lobby.lobbyID = lobbyIDtogive;
									for (int j = 0; j < (int)playersVector.size(); j++) {
										if (client->getRemotePort() == playersGaming[j].playerSocket->getRemotePort()) {
											Lobby.clientsInLobby.push_back(playersGaming[j]);
											Packet yourLobby;
											yourLobby << Lobby.lobbyID;
											playersGaming[j].playerSocket->send(yourLobby);
										}
									}
									lobbyVector.push_back(Lobby);
									lobbyIDtogive++;
									break;
								}
								case WantToJoinLobby: 
								{	
									vector<LobbyInfo> LobbiesToGo;
									int lobbyLifesIneed;
									int counter = 0;
									receivePacket >> lobbyLifesIneed;
									Packet lobbyInfoPack;
									lobbyInfoPack << LobbyInfoDump;
									for (int i = 0; i < (int)lobbyVector.size(); i++) {
										if (lobbyVector[i].lifesToStart == lobbyLifesIneed && (int)lobbyVector[i].clientsInLobby.size() < 4) {
											LobbyInfo Lobby;
											Lobby.LobbyName = lobbyVector[i].lobbyName;
											Lobby.LobbyID = lobbyVector[i].lobbyID;
											Lobby.NumPlayers = (int)lobbyVector[i].clientsInLobby.size();
											LobbiesToGo.push_back(Lobby);
											// nom del loby, vides amb que es juga, numero de jugadors en el lobby
											counter += 1;
										}
										
									}
									lobbyInfoPack << counter;
									for (int k = 0; k < counter; k++) {
										lobbyInfoPack << LobbiesToGo[k].LobbyName << LobbiesToGo[k].NumPlayers << LobbiesToGo[k].LobbyID;
									}
									client->send(lobbyInfoPack);
									break;
								}
								case JoinLobby: 
								{
									int lobbyIdToJoin;
									receivePacket >> lobbyIdToJoin;
									for (int j = 0; j < (int)lobbyVector.size(); j++) {
										if (lobbyIdToJoin == lobbyVector[j].lobbyID && (int)lobbyVector[j].clientsInLobby.size() < 4) {
											lobbyVector[j].clientsInLobby.push_back(playersGaming[i]);
											for (int k = 0; k < (int)lobbyVector[j].clientsInLobby.size(); k++) {
												Packet PlayersInLobby;
												PlayersInLobby << LobbyPlayerJoin <<(int)lobbyVector[j].clientsInLobby.size();
												for (int i = 0; i < (int)lobbyVector[j].clientsInLobby.size(); i++) {
													PlayersInLobby << lobbyVector[j].clientsInLobby[i].ID << lobbyVector[j].clientsInLobby[i].nickName << lobbyVector[j].clientsInLobby[i].position.x << lobbyVector[j].clientsInLobby[i].position.y;

												}
												client->send(PlayersInLobby);
											}
										}
									}
									
									break;
								}
								case Connect:
								{
									receivePacket >> clientName;
									cout << "got a connection from: " << clientName << endl;
									playersGaming[i].nickName = clientName;
									playersVector[i].nickName = clientName;
				
									clientInfo << AcceptConnect << IDtogive << playersGaming[IDtogive].position.x << playersGaming[IDtogive].position.y;
								
									client->send(clientInfo);
									clientInfo.clear();

									IDtogive++;

									if ((int)playersGaming.size()==4)
									{
										std::cout << "sendingMatchBegin" << std::endl;
										Packet infoAndBegin;
									  
										infoAndBegin << PlayerInfoBegin;
										for (int i = 0; i < (int)playersGaming.size(); i++) {
											infoAndBegin << playersGaming[i].ID << playersGaming[i].nickName <<playersGaming[i].position.x << playersGaming[i].position.y;
										
										}
										//SENDALL INFO AND BEGIN
										SendToAll(infoAndBegin, playersVector);
									
										infoAndBegin.clear();

										//podem fer un wait de 3 segons
										Packet firstTurn;
										firstTurn << TurnId << playersGaming[0].ID;//LA PARTIDA EL COMENÇA EL 1R JUGADOREN QUE S0HA CONCETAT;
										SendToAll(firstTurn, playersVector);
										firstTurn.clear();

									}
									break;
								}
								case Move:
								{
									int NewPosX, NewPosY;
									int lobbyID;
									int playerID;
									int direction;
									bool badMove = false;
									receivePacket  >> lobbyID >> direction;
									Packet NewMovePacket;
									for (int m = 0; m < (int)lobbyVector[lobbyID].clientsInLobby.size(); m++) {
										if (client->getRemotePort() == lobbyVector[lobbyID].clientsInLobby[m].playerSocket->getRemotePort()) {
											playerID = m;
										}
									}
									if (direction == Up)
									{
										NewPosX = lobbyVector[lobbyID].clientsInLobby[playerID].position.x;
										NewPosY = lobbyVector[lobbyID].clientsInLobby[playerID].position.y - 1;
										if (NewPosY < 0) {
											badMove = true;
										}
										for (int j = 0; j < (int)lobbyVector[lobbyID].clientsInLobby.size(); j++) {
											if (NewPosX == lobbyVector[lobbyID].clientsInLobby[j].position.x && NewPosY == lobbyVector[lobbyID].clientsInLobby[j].position.y) {
												badMove = true;
											}

										}
									
									}
									else if (direction == Down) {
										NewPosX = lobbyVector[lobbyID].clientsInLobby[playerID].position.x;
										NewPosY =  lobbyVector[lobbyID].clientsInLobby[playerID].position.y + 1;
										if (NewPosY > 9) {
											badMove = true;
										}
										for (int j = 0; j < (int)playersGaming.size(); j++) {
											if (NewPosX == lobbyVector[lobbyID].clientsInLobby[j].position.x && NewPosY == lobbyVector[lobbyID].clientsInLobby[j].position.y) {
												badMove = true;
											}

										}
									}
									else if (direction == Right) {
										NewPosX = lobbyVector[lobbyID].clientsInLobby[playerID].position.x + 1;
										NewPosY = lobbyVector[lobbyID].clientsInLobby[playerID].position.y;
										if (NewPosX > 9) {
											badMove = true;
										}
										for (int j = 0; j < (int)playersGaming.size(); j++) {
											if (NewPosX == lobbyVector[lobbyID].clientsInLobby[j].position.x && NewPosY == lobbyVector[lobbyID].clientsInLobby[j].position.y) {
												badMove = true;
											}

										}
									}
									else if (direction == Left) {
										NewPosX = lobbyVector[lobbyID].clientsInLobby[playerID].position.x-1;
										NewPosY = lobbyVector[lobbyID].clientsInLobby[playerID].position.y;
										if (NewPosX < 0) {
											badMove = true;
										}
										for (int j = 0; j < (int)playersGaming.size(); j++) {
											if (NewPosX == lobbyVector[lobbyID].clientsInLobby[j].position.x && NewPosY == lobbyVector[lobbyID].clientsInLobby[j].position.y) {
												badMove = true;
											}

										}
									}
									if (!badMove && numMoves<3) {
										lobbyVector[lobbyID].clientsInLobby[playerID].position.x = NewPosX;
										lobbyVector[lobbyID].clientsInLobby[playerID].position.y = NewPosY;
										NewMovePacket << NewPosition << lobbyVector[lobbyID].clientsInLobby[playerID].ID << lobbyVector[lobbyID].clientsInLobby[playerID].position.x << lobbyVector[lobbyID].clientsInLobby[playerID].position.y;
										numMoves++;
										SendToAll(NewMovePacket, lobbyVector[lobbyID].clientsInLobby);
									}

									NewMovePacket.clear();
									break;
								}			
								case Shoot:
								{
									std::cout << "Got a shoot from: " << playersGaming[i].nickName << std::endl;
									int direction;
									bool badShoot = false;
									receivePacket >> direction;
									//funcio en el servido ha de calcular el dispar i veure si colisiona amb algu
									Packet newShootPacket;
									Vector2i shootPos = playersGaming[i].position;
									bool didItHit = false;
									int IdPlayerHited = -1;//si el numero es -1 voldra dir que no li ha donat a ningu
									int lifesToUpdate = 0;//numero de vidas que tindra un jguador si rep hit
									int counter;
									newShootPacket << NewShot;
									if (!playersGaming[i].hasShot)
									{
										newShootPacket << playersGaming[i].hasShot << playersGaming[i].ID;
										playersGaming[i].hasShot = true;
										if (direction == Up)
										{
											for (int j = 1; j <= 3; j++)
											{
												if (shootPos.y == 0)
												{
													j = 3;
												}
												else
												{
													shootPos.y = playersGaming[i].position.y - j;
													for (int k = 0; k < (int)playersGaming.size(); k++) {
														if (shootPos == playersGaming[k].position) {
															didItHit = true;
															IdPlayerHited = playersGaming[k].ID;
															lifesToUpdate = playersGaming[k].lifes - playersGaming[i].level;//level = dmg, level 1 = treus 1 vida, lvl 2 = treus 2 vides
															playersGaming[k].lifes = lifesToUpdate;
															if (lifesToUpdate <= 0) {
																cout << "Player " << playersGaming[k].nickName << " have died" << endl;
																playersGaming[i].kills += 1;
																counter = k;
																//playersGaming.erase(playersGaming.begin() + k);
															}
															j = 3;//un cop hi ha un Hit el proyectil no continua la trayectoria
														}
													}
												}
											}
										}
										else if (direction == Down)
										{
											for (int j = 1; j <= 3; j++)
											{
												if (shootPos.y == 9)
												{
													j = 3;
												}
												else
												{
													shootPos.y = playersGaming[i].position.y + j;
													for (int k = 0; k < (int)playersGaming.size(); k++) {
														if (shootPos == playersGaming[k].position) {
															didItHit = true;
															IdPlayerHited = playersGaming[k].ID;
															lifesToUpdate = playersGaming[k].lifes - playersGaming[i].level;//level = dmg, level 1 = treus 1 vida, lvl 2 = treus 2 vides
															playersGaming[k].lifes = lifesToUpdate;
															if (lifesToUpdate <= 0) {
																cout << "Player " << playersGaming[k].nickName << " have died" << endl;
																playersGaming[i].kills += 1;
																counter = k;
															}
															j = 3;//un cop hi ha un Hit el proyectil no continua la trayectoria
														}
													}
												}
											}
										}
										else if (direction == Right)
										{
											for (int j = 1; j <= 3; j++)
											{
												if (shootPos.x == 9)
												{
													j = 3;
												}
												else
												{
													shootPos.x = playersGaming[i].position.x + j;
													for (int k = 0; k < (int)playersGaming.size(); k++) {
														if (shootPos == playersGaming[k].position) {
															didItHit = true;
															IdPlayerHited = playersGaming[k].ID;
															lifesToUpdate = playersGaming[k].lifes - playersGaming[i].level;//level = dmg, level 1 = treus 1 vida, lvl 2 = treus 2 vides
															playersGaming[k].lifes = lifesToUpdate;
															if (lifesToUpdate <= 0) {
																cout << "Player " << playersGaming[k].nickName << " have died" << endl;
																playersGaming[i].kills += 1;
																//playersGaming.erase(playersGaming.begin() + k);
																counter = k;
															}
															j = 3;//un cop hi ha un Hit el proyectil no continua la trayectoria
														}
													}
												}
											}
										}
										else if (direction == Left)
										{
											for (int j = 1; j <= 3; j++)
											{
												if (shootPos.x == 0)
												{
													j = 3;
												}
												else
												{
													shootPos.x = playersGaming[i].position.x - j;
													for (int k = 0; k < (int)playersGaming.size(); k++) {
														if (shootPos == playersGaming[k].position) {
															didItHit = true;
															IdPlayerHited = playersGaming[k].ID;
															lifesToUpdate = playersGaming[k].lifes - playersGaming[i].level;//level = dmg, level 1 = treus 1 vida, lvl 2 = treus 2 vides
															playersGaming[k].lifes = lifesToUpdate;
															if (lifesToUpdate <= 0) {
																cout << "Player " << playersGaming[k].nickName << " have died" << endl;
																playersGaming[i].kills += 1;
																//playersGaming.erase(playersGaming.begin() + k);
																counter = k;
															}
															j = 3;//un cop hi ha un Hit el proyectil no continua la trayectoria
														}
													}
												}
											}
										}
										//enviem en el seguen orde: 
											//1.NewShoot: posicio del dispara i si li dona algu, li resta vida
											//2.LevelUp: si el player ha matat algu pujarlu de nivel
											//3.EndMarc: si despres de matar algu nomes queda 1 player, ha guanyat
										//cout << "Send New Shoot:" << endl;
										//cout << "idSniper: " << playersGaming[i].ID << " direction: " << (int)direction
										//	<< " didItHIt: " << didItHit << " idHit: " << IdPlayerHited << "lifesUpdated: " << lifesToUpdate << endl;
										newShootPacket << direction << didItHit << IdPlayerHited << lifesToUpdate;
										SendToAll(newShootPacket, playersVector);
										if (didItHit == true && lifesToUpdate <= 0)//significa que el player ha matat algu perque te <= 0 un player despres del dispar, per tant puja nivell
										{
											//pujar nivell, comprovar condicio de victoria

											Packet levelUp;
											playersGaming[i].level += 1;
											cout << "Player " << playersGaming[i].nickName << " have level up to " << playersGaming[i].level << endl;
											levelUp << Level << playersGaming[i].ID << playersGaming[i].level;
											SendToAll(levelUp, playersVector);
											if ((int)playersGaming.size() - 1 == 1)//es a dir, despres de matar algu nomes queda 1 player, winn
											{
												cout << "WinCondition" << endl;
												Packet winner;
												winner << EndMatch << playersGaming[i].ID << playersGaming[i].kills;
												SendToAll(winner, playersVector);
												winner.clear();
											}


											playersGaming.erase(playersGaming.begin() + counter);
											levelUp.clear();
										}									
									}
									else 
									{
										cout << "This player already have shoot and can't shoot again this turn" << endl;
										newShootPacket << playersGaming[i].hasShot;
										playersGaming[i].playerSocket->send(newShootPacket);
									}
									newShootPacket.clear();
									break;   
								}
								case Bomb:
								{
									int bombXPos, bombYPos;
									bool didItHit = false;
									Packet NewBomId;
									int IdPlayerHited;
									int lifesToUpdate;//numero de vidas que tindra un jguador si rep hit
									vector<pair<int, int>> vectorAffectedPlayers;//pair <iDplayerHited, lifesUpdaed>. Temporal vector that saves the playersHited by the bomb
									NewBomId << NewBombID;
									if (playersGaming[i].hasBomb == true) {
										Vector2i tempPos;
										bombXPos = playersGaming[i].position.x;
										bombYPos = playersGaming[i].position.y;
										NewBomId << playersGaming[i].hasBomb;//li envio true, tens bomba al client
										NewBomId << playersGaming[i].ID;
										playersGaming[i].hasBomb = false;
										//la bomba te impacta en creu
										for (int j = 0; j <= 3; ++j) {//bucle de 4 direccions {0 = up, 1 = down, 2 = right, 3 = left}
											for (int k = 1; k <= 3; k++) {//bucle de 3 casellas en cada respectiva direcio
												if (j == 0)//up
												{
													tempPos.x = bombXPos;
													tempPos.y = bombYPos;
													tempPos.y -= k;
													for (int n = 0; n < (int)playersGaming.size(); n++) {//recorro vector players
														if (tempPos == playersGaming[n].position) {
															if (didItHit == false) {
																didItHit = true;
																NewBomId << didItHit;
															}
															IdPlayerHited = playersGaming[n].ID;
															lifesToUpdate = playersGaming[n].lifes - (playersGaming[i].level+ 1);
															playersGaming[n].lifes = lifesToUpdate;
															vectorAffectedPlayers.push_back(make_pair(IdPlayerHited, lifesToUpdate));
														}
													}									
												}
												else if(j == 1)//down
												{
													tempPos.x = bombXPos;
													tempPos.y = bombYPos;
													tempPos.y += k;
													for (int n = 0; n < (int)playersGaming.size(); n++) {
														if (tempPos == playersGaming[n].position) {
															if (didItHit == false) {
																didItHit = true;
																NewBomId << didItHit;
															}
															IdPlayerHited = playersGaming[n].ID;
															lifesToUpdate = playersGaming[n].lifes - (playersGaming[i].level + 1);
															playersGaming[n].lifes = lifesToUpdate;
															vectorAffectedPlayers.push_back(make_pair(IdPlayerHited, lifesToUpdate));
														}
													}
												}
												else if (j == 2)//right
												{
													tempPos.x = bombXPos;
													tempPos.y = bombYPos;
													tempPos.x += k;
													for (int n = 0; n < (int)playersGaming.size(); n++) {
														if (tempPos == playersGaming[n].position) {
															if (didItHit == false) {
																didItHit = true;
																NewBomId << didItHit;
															}
															IdPlayerHited = playersGaming[n].ID;
															lifesToUpdate = playersGaming[n].lifes - (playersGaming[i].level + 1);
															playersGaming[n].lifes = lifesToUpdate;
															vectorAffectedPlayers.push_back(make_pair(IdPlayerHited, lifesToUpdate));
														}
													}																									
												}
												else if(j == 3) //left
												{
													tempPos.x = bombXPos;
													tempPos.y = bombYPos;
													tempPos.x -= k;
													for (int n = 0; n < (int)playersGaming.size(); n++) {
														if (tempPos == playersGaming[n].position) {
															if (didItHit == false) {
																didItHit = true;
																NewBomId << didItHit;
															}
															IdPlayerHited = playersGaming[n].ID;
															lifesToUpdate = playersGaming[n].lifes - (playersGaming[i].level + 1);
															playersGaming[n].lifes = lifesToUpdate;
															vectorAffectedPlayers.push_back(make_pair(IdPlayerHited, lifesToUpdate));
														}
													}
												}
											}
										}
										if (didItHit) {
											NewBomId << (int)vectorAffectedPlayers.size();//int numberAffectedplayers
											for (int j = 0; (int)j < vectorAffectedPlayers.size(); j++)
											{
												NewBomId << vectorAffectedPlayers[j].first << vectorAffectedPlayers[j].second;//first = idHitted, second = lifesupdated
												if (vectorAffectedPlayers[j].second <= 0)
												{
													for (int k = 0; k < (int)playersGaming.size(); k++) {//comparo la llista de afectats amb la llista de jugadors jugan
														if (vectorAffectedPlayers[j].first == playersGaming[k].ID) {
															cout << "Player " << playersGaming[k].nickName << " have died" << endl;
															Packet levelUp;
															playersGaming[i].level += 1;
															cout << "Player " << playersGaming[i].nickName << " have level up" << endl;
															levelUp << Level << playersGaming[i].ID << playersGaming[i].level;
															SendToAll(levelUp, playersVector);
															if ((int)playersGaming.size() - 1 == 1)//es a dir, despres de matar algu nomes queda 1 player, winn
															{
																Packet winner;
																winner << EndMatch << playersGaming[i].ID << playersGaming[i].kills;
																SendToAll(winner, playersVector);
																winner.clear();
															}				

															playersGaming.erase(playersGaming.begin() + k);
															levelUp.clear();
														}
													}												
												}
											}
										}

										else {
											NewBomId << 0;//si no ha donat a ningu numberaffectedplayers = 0
										}
										cout << "el paket a enviar te: ";
										cout << "hadBomToDrop " << playersGaming[i].hasBomb << " IdWhoDropBomb " << playersGaming[i].ID
											<< " SomeHitted " << didItHit << " numAffected " << (int)vectorAffectedPlayers.size()
											<< " idHitted " << endl;
										SendToAll(NewBomId, playersVector);
										vectorAffectedPlayers.clear();
										break;
									}
									else {
										//enviar missatge de que no te bomba? 
										cout << "Player " << playersGaming[i].nickName << " have already drop the bomb in this game" << endl;
										NewBomId << false;
										playersGaming[i].playerSocket->send(NewBomId);
									}

									clientInfo.clear();
									NewBomId.clear();
									break;
								}
								case PassTurn:
								{
							
									cout << " PassTurn of " << playersVector[i].nickName << endl;							
									Packet nextTurnID;
									for (int counter = 0; counter < (int)playersGaming.size(); counter++)
									{
										playersGaming[counter].hasShot = false;
									}
									int nextIdturn = playersGaming[(i + 1) % (int)playersGaming.size()].ID;
									cout << "Player: " << playersGaming[(i + 1) % (int)playersGaming.size()].nickName << " have turn" << endl;
									nextTurnID << TurnId << nextIdturn;
									SendToAll(nextTurnID, playersVector);
									nextTurnID.clear();
									numMoves = 0;
									break;
								}
								case TextID:
								{
									int lobbyId;
									string mensajeToSend;
									receivePacket >> lobbyId >> mensajeToSend;
									Packet mensajePacket;
									mensajePacket << TextID << mensajeToSend;
									SendToAll(mensajePacket, lobbyVector[lobbyId].clientsInLobby);
									break;
								}

							}
						}
						else if (status == Socket::Disconnected)
						{
							Packet sendDisconnect;
							sendDisconnect << PlayerLeft << playersVector[i].ID;
							cout << "Player " << playersGaming[i].nickName << " disconnected" << endl;
							SendToAll(sendDisconnect, playersVector);
							selector.remove(*client);
							sendDisconnect.clear();
						}
					}
				}
			}
		}

		
	}

	return 0;
}


	
