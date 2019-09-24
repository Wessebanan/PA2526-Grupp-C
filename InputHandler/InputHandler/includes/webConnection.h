#pragma once
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

// List of the diffrent actions the user wants to do from the web
enum ActionType
{
	NAME,
	TILE,
	BUTTON,
	COMMAND
};
// struct that holds the parsed info
struct webMsgData
{
	// What index the player has, 
	int player = -1;
	// a defined action, 999 posible actions, use ENUM
	int action = -1;
	// the The rest of the recived data to be used, ea. name of item or command
	string data = "";
};

// Collected information to be requested by frontend
struct playerInfo
{
	// What plaer this struct is
	int playerIndex = -1;
	// Name that the player can change
	string name = "No name yet";
	// crrent selected tile
	int tile[2] = { -1,-1 };
	// current selected button
	int button = -1;
	// current selected command
	string command = "No command yet";
};

// Handles getting info from the website
class WebConnection
{
public:
	WebConnection();
	~WebConnection();
	// retuns true when the thread was closed
	bool shutDown();

	// Checks if the connection was successful
	bool isConnected() { return this->connectionOK; };

	// Returns the button index the player has selected
	int getUserButton(int player);
	// Returns the name of the set player (playres cant change this yet)
	std::string getUserName(int player);

	// Returns the tile index of the axis (0 = X or 1 = Y)
	int getUserTile(int player, int axis);

	// Returns the command of the player
	string getUserCommand(int player);
	
	// returns hte number of players that have connected since the client started up
	int getNrOfPlayers() { return this->nrOfPlayers; };
	
	// Changes the gamestate for the users
	bool setGamestate(int gamestate);


private:
	// Array of information to be sent to frontend
	playerInfo mUsers[4];


	//// THREAD VARIBLES
	// starts the thread to run sockets on the side
	void initThread(void);
	static DWORD WINAPI staticThreadStart(LPVOID lpParam);
	// Handles to hold the thread
	HANDLE t_update;
	// id for thread,m isnt used
	DWORD id_t_update;


	//// EXECUTE AFTER GETTING MSG FROM USER
	// parses the message inte a struct
	webMsgData parseMsg(char* userMsg);
	// Switchcase for the mesage
	bool executeUserAction(webMsgData wmd);
	// Saves and sends out a new name
	void setName(webMsgData wmd);
	// Saves and sends out new tile
	void setTile(webMsgData wmd);
	// Saves new button
	void setButton(webMsgData wmd);
	// Saves new button
	void setCommand(webMsgData wmd);


	//// BACKEND STATES
	// Players joining the game
	void playersJoin();
	// Gameloop for getting controll information from web
	void gameLoop();


	// FUNCTIONS FOR INTERACTIONS
	//Send out msg to all players
	void broadcastMsg(string msg);
	// Identifies what player the current socket is
	int idPlayerSocket(SOCKET sock);
	// takes in the a new message
	char* reciveMsg(SOCKET sock, char* recvbuf, int& Res);
	// sends out a message to the user, default will print to 0.
	void sendMsg(SOCKET sock, char* client_msg, int& Res);

	// Varible to see if system is still connected
	bool connectionOK = true;

	// %%%%		SOCKETS		%%%%
	// socket that looks for new connections
	SOCKET ListenSocket;
	// interpets the key and preformes handshake
	bool checkForKey(SOCKET sock, char* recBuff, int& Res);

	int nrOfPlayers;
	SOCKET playerSockets[30];
	fd_set master; 

	// BACKEND DEFENITIONS
	WSADATA wsaData;
	int iResult;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	// HOLDS INFORMATION INCOMING AND OUTGOING INFORMATION
	char* msgToUsers;
	char* msgToClient;


	// thread varibles and functions to handle sockets 
	void shutDownSocket(SOCKET sock);
	void shutDownThread();
	bool runThread = false;
	bool runPlayerJoin = false;
	bool runGameLoop = false;
};