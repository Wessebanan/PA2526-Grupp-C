#include "webConnection.h"


WebConnection::WebConnection()
{
	this->msgToUsers = (char*)"start";
	this->msgToClient = (char*)"No msg yet";

	//this->nrOfPlayers = 0;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}


	FD_ZERO(&master);
	FD_SET(ListenSocket, &master);

	for (size_t i = 0; i < mMaxUserSockets; i++)
	{
		userSockets[i] = INVALID_SOCKET;
	}

	this->initThread();
}

WebConnection::~WebConnection()
{
	this->shutDown();
}

bool WebConnection::shutDown()
{
	this->shutDownThread();

	FD_CLR(ListenSocket, &master);
	closesocket(ListenSocket);
	//if (this->ListenSocket != INVALID_SOCKET)
	//{
	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);

		shutdown(sock, SD_SEND);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();


	//TerminateThread(t_update,0);
	WaitForSingleObject(t_update, INFINITE);
	CloseHandle(t_update);
	//}

	return true;
}

webMsgData WebConnection::parseMsg(char* userMsg)
{
	webMsgData retVal;

	string str = userMsg;

	retVal.player = (str[0] - 48);

	int hun = ((int)str[1] - 48) * 100;
	int ten = ((int)str[2] - 48) * 10;
	int one = (int)str[3] - 48;

	retVal.action = hun + ten + one;

	retVal.data = str.substr(4, str.size());

	return retVal;
}

bool WebConnection::executeUserAction(webMsgData wmd)
{
	if (wmd.player < 4)
	{
		switch (wmd.action)
		{
		case ActionType::NAME:
			this->setName(wmd);
			break;
		case ActionType::TILE:
			this->setTile(wmd);
			break;
		case ActionType::BUTTON:
			this->setButton(wmd);
			break;
		case ActionType::COMMAND:
			this->setCommand(wmd);
			break;
		default:
			cout << "-Parsing error: Not a action" << endl;
			return false;
			break;
		}
	}
	else
	{
		cout << "-Parsing error: User is beyond amount of player slots" << endl;
		return false;
	}


}

void WebConnection::setName(webMsgData wmd)
{
	mUsers[wmd.player].name = wmd.data;

	int res = 0;
	this->sendMsg(this->userSockets[wmd.player],
		(char*)string(
			"2. Your name is now " + 
			wmd.data
		).c_str()
		, res);
}

void WebConnection::setTile(webMsgData wmd)
{

	int hun = ((int)wmd.data[0] - 48) * 100;
	hun -= 100;
	int ten = ((int)wmd.data[1] - 48) * 10;
	int one = (int)wmd.data[2] - 48;
	mUsers[wmd.player].tile[0] = hun + ten + one;

	hun = ((int)wmd.data[3] - 48) * 100;
	hun -= 100;
	ten = ((int)wmd.data[4] - 48) * 10;
	one = (int)wmd.data[5] - 48;
	mUsers[wmd.player].tile[1] = hun + ten + one;

	int res = 0;
	this->sendMsg(this->userSockets[wmd.player], 
		(char*)string("3. Your tile is now " + 
			to_string(mUsers[wmd.player].tile[0]) + 
			"," +
			to_string(mUsers[wmd.player].tile[1])
		).c_str()
		, res);
}

void WebConnection::setButton(webMsgData wmd)
{

	int hun = ((int)wmd.data[0] - 48) * 100;
	hun -= 100;
	int ten = ((int)wmd.data[1] - 48) * 10;
	int one = (int)wmd.data[2] - 48;

	mUsers[wmd.player].button = hun + ten + one;
}

void WebConnection::setCommand(webMsgData wmd)
{
	mUsers[wmd.player].command = wmd.data;
}

void WebConnection::playersJoin()
{
	this->runPlayerJoin = true;

		while (this->runThread && this->runPlayerJoin)
		{
			fd_set copy = master;

			//cout << this->msgToClient << endl;

			int socketCount = select(0, &copy, 0, 0, 0);

			for (size_t i = 0; i < socketCount; i++)
			{
			//	cout << "-i " << i << endl;
				SOCKET sock = copy.fd_array[i];

				// if the socket is the listener in the array
				if (sock == ListenSocket)
				{
					this->addUserSocket(sock);
				}
				else // its the cliet socket
				{
					iResult = recv(sock, recvbuf, recvbuflen, 0);

					if (iResult <= 0 || iResult == 8)
					{
						this->removeUserSocket(sock, iResult);
					}
					else
					{
						recvbuf[iResult] = 0;

						char sendbuf[1024];
						size_t sendbuf_size = 0;

						// Connect if it hasnt
						if (this->checkForKey(sock, recvbuf, iSendResult))
						{
							cout << "-A key was sent in joining phase" << endl;
						}
						// if the socket is the listener in the array
						else
						{
							//cout << endl << "___We just got a message from " << this->idUserSocket(sock) << endl;

							char* userMsg = reciveMsg(sock, recvbuf, iSendResult);

							string str1 = userMsg;

							if (!str1.compare(string("start")))
							{
								this->gameLoop();
							}
						}
					}
				}
			}
		}


		printf("Closing playerJoin ...\n");
}

void WebConnection::gameLoop()
{
	this->runGameLoop = true;
	int startCount = master.fd_count;

	cout << "--------------------------------" << endl;
	cout << "---------STARTING GAME----------" << endl;
	cout << "--------------------------------" << endl;

	int p = 0;
	while (p < 4)
	{
		// Broadcast channel in the futore
		string ss;
		ss += "3. PLAYER " + to_string(p);
		if (this->userSockets[p] != INVALID_SOCKET)
			this->sendMsg(this->userSockets[p], (char*)ss.c_str(), iSendResult);
		p++;
	}

	int nrMsg = 0;
	while (this->runThread && this->runGameLoop)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, 0, 0, 0);

		for (size_t i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			// if the socket is the listener in the array
			if (sock == ListenSocket)
			{
				// If the listener gets set in this we have a new player mid-game
				cout << "-LISTENER got a player mid game" << i << endl;

				this->addUserSocket(sock);
			}
			else // its the cliet socket
			{
				iResult = recv(sock, recvbuf, recvbuflen, 0);

				if (iResult <= 0 || iResult == 8)
				{
					this->removeUserSocket(sock, iResult);
				}
				else
				{
					recvbuf[iResult] = 0;

					char sendbuf[1024];
					size_t sendbuf_size = 0;

					// Connect if it hasnt
					if (this->checkForKey(sock, recvbuf, iSendResult))
					{
						string ss;
						ss += "3. PLAYER " + to_string(idUserSocket(sock));
						this->sendMsg(sock, (char*)ss.c_str(), iSendResult);
					}
					// if the socket is the listener in the array
					else
					{
						//cout << endl << "___We just got a message from " << this->idUserSocket(sock) << endl;

						char* userMsg = reciveMsg(sock, recvbuf, iSendResult);

						broadcastMsg("Recived mesages: " + to_string(nrMsg++));

						webMsgData wmd = parseMsg(userMsg);
						if (wmd.player == -1)
						{
							cout << "-Couldnt parse msg form user" << endl;
						}
						else
						{
							wmd.player = idUserSocket(sock);

							cout << "-From player " << wmd.player << endl
								<< userMsg << endl;

							executeUserAction(wmd);
						}
					}
				}
				
			}
		}
	}

	printf("Closing gameloop\n");
}

void WebConnection::broadcastMsg(string msg)
{
	int p = 0;
	// Broadcast channel in the futore
	string ss;
	ss += "1. " + msg;
	while (p < 4)
	{

		if (this->userSockets[p] != INVALID_SOCKET)
		{
			this->sendMsg(this->userSockets[p], (char*)msg.c_str(), iSendResult);
		}
		//this->sendMsg(sock, (char*)string(to_string(p)).c_str(), iSendResult);
		
		p++;
	}
}

int WebConnection::getUserButton(int player)
{
	return mUsers[player].button;
}

std::string WebConnection::getUserName(int player)
{
	return mUsers[player].name;
}

int WebConnection::getUserTile(int player, int axis)
{
	return mUsers[player].tile[axis];
}

string WebConnection::getUserCommand(int player)
{
	return mUsers[player].command;
}

bool WebConnection::setGamestate(int gamestate)
{
	return true;
}

void WebConnection::initThread(void)
{

	this->runThread = true;

	t_update = CreateThread(
		nullptr,
		0,
		staticThreadStart,
		(void*) this,
		0,
		&id_t_update);
}

DWORD WINAPI WebConnection::staticThreadStart(LPVOID lpParam)
{
	WebConnection* This = (WebConnection*)lpParam;

	This->playersJoin();


	printf("Closing thread\n");
	return 0;
}

int WebConnection::idUserSocket(SOCKET sock)
{
	int player = -1;
	//cout << "-Nr of Players getID: " << nrOfPlayers << endl;
	if (sock == INVALID_SOCKET)
	{
		cout << "-Asked for ID of invlaid_socket" << endl;
	}
	else if (sock == 0)
	{
		cout << "-Asked for ID of 0 socket" << endl;
	}
	else
		for (size_t p = 0; p < this->mMaxUserSockets; p++)
		{
			if (sock == this->userSockets[p])
			{
				player = p;
			}
		}
	return player;
}

bool WebConnection::checkForKey(SOCKET sock, char* recBuff, int& Res)
{
	char sendbuf[1024];
	size_t sendbuf_size = 0;

	// see if it's requesting a key
	char* pKey = strstr(recBuff, "Sec-WebSocket-Key:");

	if (pKey)
	{
		// parse just the key part
		pKey = strchr(pKey, ' ') + 1;
		char* pEnd = strchr(pKey, '\r');
		*pEnd = 0;

		char key[256];
		_snprintf_s(key, _countof(key), "%s%s", pKey, WEBSOCKET_KEY);

		unsigned char result[20];
		const unsigned char* pSha1Key = sha1(key);

		// endian swap each of the 5 ints
		for (int i = 0; i < 5; i++)
		{
			for (int c = 0; c < 4; c++)
				result[i * 4 + c] = pSha1Key[i * 4 + (4 - c - 1)];
		}

		pKey = base64_encode(result, 20);

		const char* pTemplateResponse = "HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Accept: %s\r\n\r\n";

		_snprintf_s(sendbuf, _countof(sendbuf), pTemplateResponse, pKey);
		sendbuf_size = strlen(sendbuf);
		//cout << pKey << endl;

		Res = send(sock, sendbuf, (int)sendbuf_size, 0);
		//cout << sendbuf << endl;

		return true;
	}
	else
	{
		return false;
	}
}

char* WebConnection::reciveMsg(SOCKET sock, char* recvbuf, int& Res)
{
	
	// else read, print the response, and echo it back to the server
	_websocket_header* h = (_websocket_header*)recvbuf;

	_mask_key* mask_key;

	unsigned long long length;

	if (h->len < 126)
	{
		length = h->len;
		mask_key = (_mask_key*)(recvbuf + sizeof(_websocket_header));
	}
	else if (h->len == 126)
	{
		_extended_16* extended = (_extended_16*)(recvbuf + sizeof(_websocket_header));

		length = (extended->value[0] << 8) | extended->value[1];
		mask_key = (_mask_key*)(recvbuf + sizeof(_websocket_header) + sizeof(_extended_16));
	}
	else
	{
		_extended_64* extended = (_extended_64*)(recvbuf + sizeof(_websocket_header));

		length = (((unsigned long long) extended->value[0]) << 56) | (((unsigned long long) extended->value[1]) << 48) | (((unsigned long long) extended->value[2]) << 40) |
			(((unsigned long long) extended->value[3]) << 32) | (((unsigned long long) extended->value[4]) << 24) | (((unsigned long long) extended->value[5]) << 16) |
			(((unsigned long long) extended->value[6]) << 8) | (((unsigned long long) extended->value[7]) << 0);

		mask_key = (_mask_key*)(recvbuf + sizeof(_websocket_header) + sizeof(_extended_64));
	}

	char* client_msg = ((char*)mask_key) + sizeof(_mask_key);

	if (h->mask)
	{
		for (int i = 0; i < length; i++)
			client_msg[i] = client_msg[i] ^ mask_key->value[i % 4];
	}
	return client_msg;
}

void WebConnection::sendMsg(SOCKET sock, char* client_msg, int& Res)
{
	char* pData;
	char sendbuf[1024];
	size_t sendbuf_size = 0;

	_websocket_header* h = (_websocket_header*)sendbuf;
	*h = _websocket_header{};

	h->opcode = 0x1; //0x1 = text, 0x2 = blob
	h->fin = 1;

	char text[256];
	_snprintf_s(text, _countof(text), "%s", client_msg);

	unsigned long long msg_length = strlen(text);

	sendbuf_size = sizeof(_websocket_header);

	if (msg_length <= 125)
	{
		pData = sendbuf + sizeof(_websocket_header);
		h->len = msg_length;
	}
	else if (msg_length <= 0xffff)
	{
		h->len = 126;

		_extended_16* extended = (_extended_16*)(sendbuf + sendbuf_size);
		sendbuf_size += sizeof(_extended_16);

		extended->value[0] = (msg_length >> 8) & 0xff;
		extended->value[1] = msg_length & 0xff;
	}
	else
	{
		h->len = 127;

		_extended_64* extended = (_extended_64*)(sendbuf + sendbuf_size);
		sendbuf_size += sizeof(_extended_64);

		extended->value[0] = ((msg_length >> 56) & 0xff);
		extended->value[1] = ((msg_length >> 48) & 0xff);
		extended->value[2] = ((msg_length >> 40) & 0xff);
		extended->value[3] = ((msg_length >> 32) & 0xff);
		extended->value[4] = ((msg_length >> 24) & 0xff);
		extended->value[5] = ((msg_length >> 16) & 0xff);
		extended->value[6] = ((msg_length >> 8) & 0xff);
		extended->value[7] = ((msg_length >> 0) & 0xff);
	}

	pData = sendbuf + sendbuf_size;

	memcpy(pData, text, (size_t)msg_length);
	sendbuf_size += (size_t)msg_length;

	Res = send(sock, sendbuf, (int)sendbuf_size, 0);
	//cout << sendbuf << endl;
}

bool WebConnection::removeUserSocket(SOCKET sock, int error)
{
	if (sock == INVALID_SOCKET || sock == 0)
	{
		cout << "-ERROR: Tried to remove closed socket" << endl;
	}
	else
	{
		if (error == 0)
		{
			printf("Connection closing...\n");
			// Drop the client
			closesocket(sock);
			FD_CLR(sock, &master);
			return true;
		}
		else if (error < 0)
		{
			int id = idUserSocket(sock);
			printf("recv failed with error < 0 : %d\n", WSAGetLastError());
			
			// Failed with handshake

			closesocket(sock);
			FD_CLR(sock, &master);
			this->userSockets[id] = INVALID_SOCKET;
			for (size_t i = 1; i < mMaxUserSockets; i++)
			{
				if (master.fd_array[i] == 0)
				{
					this->master.fd_array[i - 1] = 0;
					break;
				}
			}
			cout << "--The user closed--" << endl;
			return true;
		}
		else if (error == 8)
		{
			// User Closed site
			printf("recv failed with error 8: %d\n", WSAGetLastError());
			int id = idUserSocket(sock);
			closesocket(sock);
			FD_CLR(sock, &master);
			this->userSockets[id] = INVALID_SOCKET;
			for (size_t i = 1; i < mMaxUserSockets; i++)
			{
				if (master.fd_array[i] == 0)
				{
					this->master.fd_array[i-1] = 0;
					break;
				}
			}
			cout << "--The user closed--" << endl;
			return true;
		}
	}
	
	return false;
}

bool WebConnection::addUserSocket(SOCKET sock)
{
	int firstEmpty = 0;
	for (size_t i = 0; i < mMaxUserSockets; i++)
	{
		if (userSockets[i] == INVALID_SOCKET)
		{
			firstEmpty = i;
			break;
		}
	}

	cout << "-Assigning user to slot: " << firstEmpty << endl;

	// Add the connection to one of the new clients
	this->userSockets[firstEmpty] = accept(ListenSocket, nullptr, nullptr);

	// Adds to the master
	FD_SET(this->userSockets[firstEmpty], &master);

	//this->nrOfPlayers++;
	return false;
}

void WebConnection::shutDownSocket(SOCKET sock)
{
	cout << "Closing dow a socket" << endl;
	closesocket(sock);
	FD_CLR(sock, &master);
	//this->nrOfPlayers--;

	WSACleanup();

	this->runGameLoop = false;

	this->shutDownThread();
}

void WebConnection::shutDownThread()
{
	this->runThread = false;
	this->connectionOK = false;
	//CloseHandle(this->t_update);
}
