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
#define DEFAULT_PORT    "1337"
#define WEBSOCKET_KEY   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// thread includes
#include <tchar.h>
#include <strsafe.h>

enum WEBGAMESTATE
{
	PREPPHASE,
	BATTLEPHASE,
	WAITING
};

//#pragma pack (push, 1)
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
//#pragma pack (pop)

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
	COMMAND,
	PING
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
	// If the player is connected
	bool connected = false;
	// What plaer this struct is
	int playerIndex = -1;
	// Name that the player can change
	string name = "Not joined";
	// crrent selected tile
	int tile[2] = { -1,-1 };
	// current selected button
	int button = -1;
	// current selected command
	string command = "NO STATE";
	// Bool to check if the ping was pressed and picked up by frpntend
	bool pinged = false;
	// Bool to see if they have all pressed the ready button, can be reset from the 
	bool ready = false;
};

// Handles getting info from the website
class WebConnection
{
public:
	WebConnection();
	~WebConnection();
	// retuns true when the thread was closed
	bool ShutDown();

	// Checks if the connection was successful
	bool IsConnected() { return this->mConnectionOK; };

	// Returns the name of the set player (playres cant change this yet)
	std::string GetUserName(int player);
	bool IsUserConnected(int player);

	// Returns the button index the player has selected
	int GetUserButton(int player);
	// Returns the tile index of the axis (0 = X or 1 = Y)
	int GetUserTile(int player, int axis);
	// Reset for when both have been picked up
	void resetUserTrap(int player);

	// Returns the command of the player
	string GetUserCommand(int player);

	// Returns the command of the player
	bool GetUserPing(int player);
	
	// returns hte number of players that have connected since the client started up
	int getNrOfPlayers() 
	{ 
		this->nrOfPlayers = 0;
		for (size_t i = 0; i < 4; i++)
		{
			if (mPlayerSockets[i] != -1)
			{
				nrOfPlayers++;
			}
		}
		return this->nrOfPlayers; 
	};

	// Changes the gamestate for the users
	bool SetGamestate(WEBGAMESTATE gamestate);

	// Loops thourgh and sees if all players are ready, if all are returns true and resets
	bool ReadyCheck();
private:
	// Array of information to be sent to frontend
	playerInfo mUsers[4];

	// Gamestate to be changed and send it out to the users
	WEBGAMESTATE mWebGameState = WEBGAMESTATE::WAITING;

	//// THREAD VARIBLES
	// starts the thread to run sockets on the side
	void InitThread(void);
	static DWORD WINAPI StaticThreadStart(LPVOID lpParam);
	// Handles to hold the thread
	HANDLE t_update;
	// id for thread,m isnt used
	DWORD id_t_update;


	//// EXECUTE AFTER GETTING MSG FROM USER
	// parses the message inte a struct
	webMsgData ParseMsg(char* userMsg);
	// Switchcase for the mesage
	bool ExecuteUserAction(webMsgData wmd);
	// Saves and sends out a new name
	void SetName(webMsgData wmd);
	// Saves and sends out new tile
	void SetTile(webMsgData wmd);
	// Saves new button
	void SetButton(webMsgData wmd);
	// Saves new button
	void SetCommand(webMsgData wmd);
	// Saves the ping
	void SetPing(webMsgData wmd);


	//// BACKEND STATES
	// Players joining the game
	void PlayersJoin();


	// FUNCTIONS FOR INTERACTIONS
	//Send out msg to all players
	void BroadcastMsg(string msg);
	// Identifies what player the current socket is
	int IdPlayerSocket(SOCKET sock);
	int IdUserSocket(SOCKET sock);
	// takes in the a new message
	char* ReciveMsg(SOCKET sock, char* recvbuf, int& Res);
	// sends out a message to the user, default will print to 0.
	void SendMsg(SOCKET sock, char* client_msg, int& Res);
	// closes socket and hadels chages in the playersSockets
	bool RemoveUserSocket(SOCKET sock, int error);
	// adds the new socket to the mUserSockets list
	bool AddUserSocket(SOCKET sock);
	bool AddPlayerSocket(SOCKET sock);



	// Varible to see if system is still connected
	bool mConnectionOK = true;

	// %%%%		SOCKETS		%%%%
	// socket that looks for new connections
	SOCKET mListenSocket;
	// interpets the key and preformes handshake
	bool CheckForKey(SOCKET sock, char* recBuff, int& Res);

	int nrOfPlayers;
	const int mMaxmUserSockets = 30;
	const int mMaxmPlayerSockets = 4;
	int mPlayerSockets[4];
	SOCKET mUserSockets[30];
	fd_set mMaster; 

	// BACKEND DEFENITIONS
	WSADATA wsaData;
	int iResult;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// thread varibles and functions to handle sockets 
	void ShutDownSocket(SOCKET sock);
	void ShutDownThread();
	bool mRunThread = false;
	bool mRunPlayerJoin = false;
	bool mRunGameLoop = false;
};