#pragma once
#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

// Socket includes
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "sha1.h"

#include <string>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN  512
#define DEFAULT_PORT    "80"
#define WEBSOCKET_KEY   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// thread includes
#include <tchar.h>
#include <strsafe.h>


enum ActionType
{
	NAME,
	TILE,
	BUTTON
};

#pragma pack (push, 1)
struct _websocket_header
{
	unsigned char opcode : 4;

	unsigned char rsv3 : 1;
	unsigned char rsv2 : 1;
	unsigned char rsv1 : 1;
	unsigned char fin : 1;

	unsigned char len : 7;
	unsigned char mask : 1;
};

struct _extended_16
{
	unsigned char value[2];
};

struct _extended_64
{
	unsigned char value[8];
};

struct _mask_key
{
	unsigned char value[4];
};

using namespace std;


struct webMsgData
{
	int player = -1;
	int action = -1;
	string data = "";
};

struct playerInfo
{
	int playerIndex = -1;
	string name = "";
	int tile[2] = { -1,-1 };
	int button = -1;
};


// Handles getting info from the website
class WebConnection
{
public:
	WebConnection();
	~WebConnection();


	bool isConnected() { return this->connectionOK; };

	// Returns the button index the player has selected
	int getPlayerButton(int player);
	// Returns the name of the set player (playres cant change this yet)
	std::string getPlayername(int player);

	// Returns the tile index of the axis (0 = X or 1 = Y)
	int getPlayerTile(int player, int axis);
	
	int getNrOfPlayers() { return this->nrOfPlayers; };
	
	// Changes the gamestate for the users
	bool setGamestate(int gamestate);


private:
	void initThread(void);
	static DWORD WINAPI staticThreadStart(LPVOID lpParam);

	HANDLE t_update;
	DWORD id_t_update;


	// takes the newest values from the web
	webMsgData parseMsg(char* userMsg);
	bool executeUserAction(webMsgData wmd);
	void setName(webMsgData wmd);
	void setTile(webMsgData wmd);
	void setButton(webMsgData wmd);
	playerInfo players[4];

	void playersJoin();
	void gameLoop();
	void broadcastMsg(string msg);


	bool connectionOK = true;
	// %%%%		SOCKETS		%%%%

	SOCKET ListenSocket;
	int nrOfPlayers;
	SOCKET playerSockets[30];
	fd_set master; 

	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	char* msgToUsers;
	char* msgToClient;



	int idPlayerSocket(SOCKET sock);

	bool checkForKey(SOCKET sock, char* recBuff, int& Res);
	char* reciveMsg(SOCKET sock, char* recvbuf, int& Res);
	void sendMsg(SOCKET sock, char* client_msg, int& Res);

	void shutDownSocket(SOCKET sock);
	void shutDownThread();
	bool runThread = false;
	bool runPlayerJoin = false;
	bool runGameLoop = false;
};



#endif // !WEBCONNECTION_H
