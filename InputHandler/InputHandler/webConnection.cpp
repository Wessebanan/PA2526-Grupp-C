#include "webConnection.h"


WebConnection::WebConnection()
{
	if (RUNSOCKET)
	{
		this->msgToUsers = (char*)"start";
		this->msgToClient = (char*)"No msg yet";

		this->nrOfPlayers = 0;



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


		this->initThread();

	}
	else
	{
		// Save the gamestate file to a Json::Value
		//this->jsonValuePlayers = this->readJson(this->webStateFilePath);
		//this->jsonValueWebState = this->readJsonState();
	}
}

WebConnection::~WebConnection()
{
}

void WebConnection::playersJoin()
{
	if (RUNSOCKET)
	{
		while (true)
		{
			fd_set copy = master;

			cout << this->msgToClient << endl;

			int socketCount = select(0, &copy, 0, 0, 0);
			cout << "-socketcount		" << socketCount << endl;
			cout << "-copy fd_count		" << copy.fd_count << endl;
			cout << "-master fd_count	" << master.fd_count << endl;

			for (size_t i = 0; i < socketCount; i++)
			{
			//	cout << "-i " << i << endl;
				SOCKET sock = copy.fd_array[i];

				// if the socket is the listener in the array
				if (sock == ListenSocket)
				{

					cout << "-Listener is socket " << i << endl;

					// Add the connection to one of the new clients
					this->playerSockets[this->nrOfPlayers] = accept(ListenSocket, nullptr, nullptr);

					// Adds to the master
					FD_SET(this->playerSockets[this->nrOfPlayers], &master);

					this->nrOfPlayers++;

				}
				else // its the cliet socket
				{
					//cout << "-reciving and sending on socket " << i << endl;

					iResult = recv(sock, recvbuf, recvbuflen, 0);

					if (iResult == 0)
					{
						printf("Connection closing...\n");
						// Drop the client
						closesocket(sock);
						FD_CLR(sock, &master);

					}
					else if (iResult < 0)
					{
						printf("recv failed with error: %d\n", WSAGetLastError());
						closesocket(sock);
						WSACleanup();
					}
					else if (iResult == 8)
					{
						printf("recv failed with error: %d\n", WSAGetLastError());
						closesocket(sock);
						FD_CLR(sock, &master);

						cout << "---------------------" << endl;
						cout << "__The client closed__" << endl;
						cout << "---------------------" << endl;
						break;
					}

					recvbuf[iResult] = 0;

					char sendbuf[1024];
					size_t sendbuf_size = 0;

					// Connect if it hasnt
					if (this->checkForKey(sock, recvbuf, iSendResult))
					{

					}
					else // else recive and send a msg back
					{
						cout << "-i " << i << endl;

						for (size_t p = 0; p < nrOfPlayers; p++)
						{
							if (this->playerSockets[p] == sock)
							{
								cout << endl << "___We just got a message from " << p << endl;
							}
						}

						char* client_msg = this->reciveMsg(sock, recvbuf, iSendResult);
						this->msgToClient = client_msg;

						string str1 = client_msg;

						if (!str1.compare(string("8")))
						{
							cout << "--------------------------------" << endl;
							cout << "---------STARTING GAME----------" << endl;
							cout << "--------------------------------" << endl;
							this->gameLoop();
						}

						//this->sendMsg(master.fd_array[1], (char*)this->msgToUsers, iSendResult);
					}
				}
			}


		}// while (iResult > 0);
	}
	else
	{
		// Read the file with the buttons, bad to open and read a file each frame
		//this->jsonValuePlayers = this->readJson(this->playerFilePath);
		//this->jsonValuePlayers = this->readJsonPlay();
	}

}

void WebConnection::gameLoop()
{
	int startCount = master.fd_count;
	while (true)
	{
		fd_set copy = master;


		int socketCount = select(0, &copy, 0, 0, 0);
		cout << socketCount << endl;

		for (size_t i = 0; i < socketCount; i++)
		{
			//	cout << "-i " << i << endl;
			SOCKET sock = copy.fd_array[i];

			// if the socket is the listener in the array
			if (!(sock == ListenSocket))
			{
				int p = 0;
				while (p < 4 && p < this->nrOfPlayers)
				{
					if (sock == this->playerSockets[p])
					{
						this->sendMsg(sock, (char*)"You are player ", iSendResult);
						this->sendMsg(sock, (char*)string(to_string(p)).c_str(), iSendResult);
					}

					p++;
				}
				
			}
		}

	}
}

int WebConnection::getPlayerButton(int player)
{
	int retVal = -1;
	//std::string str = jsonValuePlayers[player]["Button"].asString();

//	retVal = std::stoi(str);

	return retVal;
}

std::string WebConnection::getPlayername(int player)
{

	this->msgToUsers = (char*)"name";
	
	//this->msgToClient

	return this->msgToClient;//jsonValuePlayers[player]["Name"].asString();
}

int WebConnection::getPlayerTile(int player, int axis)
{
	int retVal = -1;
	if (axis == 0)
	{
		//std::string str = jsonValuePlayers[player]["MapTileX"].asString();
	//	retVal = std::stoi(str);
	}
	else
	{
		//std::string str = jsonValuePlayers[player]["MapTileY"].asString();
		//retVal = std::stoi(str);
	}

	return retVal;
}

bool WebConnection::setGamestate(int gamestate)
{
	//this->jsonValueWebState["gameState"] = std::to_string(gamestate);

	return true;// writeJson(this->jsonValueWebState);
}

void WebConnection::initThread(void)
{
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

	return 0;
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
	iResult = recv(sock, recvbuf, recvbuflen, 0);

	if (iResult == 0)
	{
		printf("Connection closing...\n");
		// Drop the client
		closesocket(sock);
		FD_CLR(sock, &master);

	}
	else if (iResult < 0)
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
	}
	else if (iResult == 8)
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(sock);
		FD_CLR(sock, &master);

		cout << "---------------------" << endl;
		cout << "__The client closed__" << endl;
		cout << "---------------------" << endl;
		//break;
	}

	recvbuf[iResult] = 0;

	char sendbuf[1024];
	size_t sendbuf_size = 0;

	// Connect if it hasnt
	if (this->checkForKey(sock, recvbuf, iSendResult))
	{

	}
	else
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
	
	//printf("We recived: %s\r\n", client_msg);

	return (char*)"noMsg";
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