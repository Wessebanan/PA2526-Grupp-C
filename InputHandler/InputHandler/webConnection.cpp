#include "webConnection.h"


WebConnection::WebConnection() : mMaxmPlayerSockets(4), mMaxmUserSockets(30)
{
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
	mListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (mListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	// Setup the TCP listening socket
	iResult = bind(mListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(mListenSocket);
		WSACleanup();
	}

	freeaddrinfo(result);

	iResult = listen(mListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(mListenSocket);
		WSACleanup();
	}


	FD_ZERO(&mMaster);
	FD_SET(mListenSocket, &mMaster);

	for (size_t i = 0; i < mMaxmUserSockets; i++)
	{
		mUserSockets[i] = INVALID_SOCKET;
	}
	
	for (size_t i = 0; i < mMaxmPlayerSockets; i++)
	{
		mPlayerSockets[i] = -1;
	}
	for (size_t i = 0; i < mMaxmPlayerSockets; i++)
	{
		mUsers[i].ready = false;
	}

	for (size_t i = 0; i < 4; i++)
	{
		this->mUsers[i].name = "PLAYER";
	}

	this->InitThread();
}

WebConnection::~WebConnection()
{
	this->ShutDown();
}

bool WebConnection::ShutDown()
{
	this->ShutDownThread();

	FD_CLR(mListenSocket, &mMaster);
	closesocket(mListenSocket);
	//if (this->mListenSocket != INVALID_SOCKET)
	//{
	while (mMaster.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = mMaster.fd_array[0];

		// Remove it from the mMaster file list and close the socket
		FD_CLR(sock, &mMaster);

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

webMsgData WebConnection::ParseMsg(char* userMsg)
{
	webMsgData ret_val;

	string str = userMsg;

	ret_val.player = (str[0] - 48);

	int hun = ((int)str[1] - 48) * 100;
	int ten = ((int)str[2] - 48) * 10;
	int one = (int)str[3] - 48;

	ret_val.action = hun + ten + one;

	ret_val.data = str.substr(4, 32);

	return ret_val;
}

bool WebConnection::ExecuteUserAction(webMsgData wmd)
{
	if (wmd.player < 4)
	{
		switch (wmd.action)
		{
		case ActionType::NAME:
			this->SetName(wmd);
			break;
		case ActionType::TILE:
			this->SetTile(wmd);
			break;
		case ActionType::BUTTON:
			this->SetButton(wmd);
			break;
		case ActionType::COMMAND:
			this->SetCommand(wmd);
			break;
		case ActionType::PING:
			this->SetPing(wmd);
			break;
		case ActionType::SENDBUTTON:
			this->ReadyToSend(wmd);
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

void WebConnection::SetName(webMsgData wmd)
{
	mUsers[wmd.player].name = wmd.data;

	int res = 0;
	this->SendMsg(this->mUserSockets[wmd.player],
		(char*)string(
			"2. Your name is now " + 
			wmd.data
		).c_str()
		, res);
}

void WebConnection::SetTile(webMsgData wmd)
{

	// Tile X
	int hun = ((int)wmd.data[0] - 48) * 100;
	hun -= 100;
	int ten = ((int)wmd.data[1] - 48) * 10;
	int one = (int)wmd.data[2] - 48;
	mUsers[wmd.player].tile[0] = hun + ten + one;

	// Tile Y
	hun = ((int)wmd.data[3] - 48) * 100;
	hun -= 100;
	ten = ((int)wmd.data[4] - 48) * 10;
	one = (int)wmd.data[5] - 48;
	mUsers[wmd.player].tile[1] = hun + ten + one;

	// saved if we need to display the selected tile that we just got
	//int res = 0;
	//this->SendMsg(this->mUserSockets[wmd.player], 
	//	(char*)string("4. Your tile is now " + 
	//		to_string(mUsers[wmd.player].tile[0]) + 
	//		"," +
	//		to_string(mUsers[wmd.player].tile[1])
	//	).c_str()
	//	, res);
}

void WebConnection::SetButton(webMsgData wmd)
{

	int hun = ((int)wmd.data[0] - 48) * 100;
	hun -= 100;
	int ten = ((int)wmd.data[1] - 48) * 10;
	int one = (int)wmd.data[2] - 48;

	mUsers[wmd.player].button = hun + ten + one;
}

void WebConnection::ReadyToSend(webMsgData wmd)
{
	mUsers[wmd.player].sendTrap = true;
}

void WebConnection::SetCommand(webMsgData wmd)
{
	mUsers[wmd.player].command = wmd.data;
}

void WebConnection::SetPing(webMsgData wmd)
{
	mUsers[wmd.player].pinged = true;
}

void WebConnection::PlayersJoin()
{
	this->mRunPlayerJoin = true;
	int nrMsg = 0;

	this->mWebGameState = WEBGAMESTATE::WAITING;

	while (this->mRunThread && this->mRunPlayerJoin)
	{
		fd_set copy = mMaster;



		// For debugging the different sockets
		//cout << mUserSockets[0] << "		" << mPlayerSockets[0] << endl;
		//cout << mUserSockets[1] << "		" << mPlayerSockets[1] << endl;
		//cout << mUserSockets[2] << "		" << mPlayerSockets[2] << endl;
		//cout << mUserSockets[3] << "		" << mPlayerSockets[3] << endl;
		//cout << mUserSockets[4] << endl;
		//cout << mUserSockets[5] << endl;

		int socketCount = select(0, &copy, 0, 0, 0);

		for (size_t i = 0; i < socketCount; i++)
		{
		//	cout << "-i " << i << endl;
			SOCKET sock = copy.fd_array[i];

			// if the socket is the listener in the array
			if (sock == mListenSocket)
			{
				cout << "-Got new user" << endl;
				this->AddUserSocket(sock);
			}
			else // its the cliet socket
			{
				iResult = recv(sock, recvbuf, recvbuflen, 0);

				if (iResult <= 0 || iResult == 8)
				{
					this->RemoveUserSocket(sock, iResult);
				}
				else
				{
					recvbuf[iResult] = 0;

					char sendbuf[1024];
					size_t sendbuf_size = 0;

					// Connect if it hasnt
					if (this->CheckForKey(sock, recvbuf, iSendResult))
					{
						//cout << "-A key was sent in joining phase" << endl;

						// What userslot they have
						string ss;
						ss += "2. USER " + to_string(IdUserSocket(sock));
						this->SendMsg(sock, (char*)ss.c_str(), iSendResult);


						BroadcastMsgJoined();
					}
					// if the socket is the listener in the array
					else
					{
						BroadcastMsg("Recived mesages: " + to_string(nrMsg++));
						
						char* user_msg = ReciveMsg(sock, recvbuf, iSendResult);

						cout << "user_msg: "<< user_msg << endl;

						string str1 = user_msg;

						if (!str1.compare(string("join")))
						{
							//cout << "User trying to join as player" << endl;

							//if (nrOfPlayers < mMaxmPlayerSockets)
							if (AddPlayerSocket(sock))
							{
								// Send data to the player joining
								cout << "Player joined as P" << this->IdPlayerSocket(sock) << " from U" << IdUserSocket(sock) << endl;

								// What playerslot they have
								string ss;
								ss += "3. PLAYER " + to_string(IdPlayerSocket(sock) + 1);
								this->SendMsg(sock, (char*)ss.c_str(), iSendResult);

								// send out gamestate to the new user
								char* gamestate_msg = (char*)"perror";

								switch (mWebGameState)
								{
								case PREPPHASE:
									gamestate_msg = (char*)"pprep";
									break;
								case BATTLEPHASE:
									gamestate_msg = (char*)"pbattle";
									break;
								case WAITING:
									gamestate_msg = (char*)"pwait";
									break;
								default:
									gamestate_msg = (char*)"perror";
									break;
								}

								this->SendMsg(sock, gamestate_msg, iSendResult);
							}
							else
								BroadcastMsg("1. User tried to join full game");
						}
						else if (!str1.compare(string("ready"))) // It was a message to parse
						{
							int id = IdPlayerSocket(sock);
							if (id != -1)
							{
								mUsers[id].ready = !mUsers[IdPlayerSocket(sock)].ready;
								//cout << "P" << to_string(IdPlayerSocket(sock)) << " set ready: " << mUsers[IdPlayerSocket(sock)].ready << endl;

								// Send what readystate you are to the user
								string ss;
								ss += "r" + to_string(mUsers[IdPlayerSocket(sock)].ready);
								this->SendMsg(sock, (char*)ss.c_str(), iSendResult);
							}
						}
						else if (user_msg[0] == '0') // It was a message to parse
						{
							webMsgData wmd = ParseMsg(user_msg);
							if (wmd.player == -1)
							{
								cout << "-Couldnt parse msg form user" << endl;
							}
							else
							{
								wmd.player = IdPlayerSocket(sock);

								cout << "-From player " << wmd.player << endl
									<< user_msg << endl;

								ExecuteUserAction(wmd);
							}
						}
						else
						{
							//this->RemoveUserSocket(sock, iResult);
						}

					}
				}
			}
		}
	}


	printf("Closing playerJoin ...\n");
}

void WebConnection::BroadcastMsg(string msg)
{
	int p = 0;
	// Broadcast channel in the futore
	string ss;
	ss += "1. " + msg;
	while (p < 4)
	{

		if (this->mUserSockets[p] != INVALID_SOCKET)
		{
			this->SendMsg(this->mUserSockets[p], (char*)msg.c_str(), iSendResult);
		}
		//this->SendMsg(sock, (char*)string(to_string(p)).c_str(), iSendResult);
		
		p++;
	}
}

void WebConnection::BroadcastMsgJoined()
{
	int p = 0;
	while (p < 4)
	{
		// Broadcast channel in the futore
		string ss = "5.";
		if (this->mPlayerSockets[p] == -1)
		{
			ss += "d";
		}
		else
		{
			ss += "j";
		}
		
		ss += to_string(p);
		BroadcastMsg(ss);
		p++;
	}
}

int WebConnection::GetUserButton(int player)
{
	if (mUsers[player].sendTrap)
		return mUsers[player].button;
	else
		return -1;
}

std::string WebConnection::GetUserName(int player)
{
	return mUsers[player].name;
}

bool WebConnection::IsUserConnected(int player)
{
	return  mUsers[player].connected;
}

int WebConnection::GetUserTile(int player, int axis)
{
	if (mUsers[player].sendTrap)
		return mUsers[player].tile[axis];
	else
		return -1;
}

void WebConnection::resetUserTrap(int player)
{
	mUsers[player].sendTrap = false;

	mUsers[player].button = -1;

	mUsers[player].tile[0] = -1;
	mUsers[player].tile[1] = -1;
}

string WebConnection::GetUserCommand(int player)
{
	string str = mUsers[player].command;
	mUsers[player].command = "";
	return str;
}

void WebConnection::SendUserCommand(int player, int comm)
{
	string ss = "6.";

	switch (comm)
	{
	case 0: // ATTACK
		ss += "0";
		break;
	case 1: // LOOT
		ss += "1";
		break;
	case 2: // REGROUP
		ss += "2";
		break;
	case 3: // FLEE
		ss += "3";
		break;
	default: // ERROR
		ss += "error";
		break;
	}

	this->SendMsg(mUserSockets[mPlayerSockets[player]], (char*)ss.c_str(), iSendResult);
}

bool WebConnection::GetUserPing(int player)
{
	bool ret_val = mUsers[player].pinged;
	mUsers[player].pinged = false;
	return ret_val;
}

bool WebConnection::SetGamestate(WEBGAMESTATE gamestate)
{
	bool ret_val = false;

	if (mWebGameState != gamestate)
	{
		ret_val = true;
		char* gamestate_msg = (char*)"perror";

		switch (gamestate)
		{
		case PREPPHASE:
			gamestate_msg = (char*)"pprep";
			break;
		case BATTLEPHASE:
			gamestate_msg = (char*)"pbattle";
			break;
		case WAITING:
			gamestate_msg = (char*)"pwait";
			break;
		default:
			gamestate_msg = (char*)"perror";
			break;
		}

		for (size_t i = 0; i < 4; i++)
		{
			if (mPlayerSockets[i] >= 0)
			{
				this->SendMsg(mUserSockets[mPlayerSockets[i]], gamestate_msg, iSendResult);

				// Set all users to not ready
				mUsers[i].ready = false;
				this->SendMsg(mUserSockets[mPlayerSockets[i]], (char*)"r0", iSendResult);
				
			}
		}

		this->mWebGameState = gamestate;
	}



	return ret_val;
}

bool WebConnection::ReadyCheck()
{
	bool ret_val = true;
	getNrOfPlayers();
	if (nrOfPlayers == 0)
	{
		ret_val = false;
	}
	for (size_t i = 0; i < 4; i++)
	{
		if(mPlayerSockets[i] != -1) 
			if (mUsers[i].ready == false)
				ret_val = false;
	}

	
	if(ret_val)
		for (size_t i = 0; i < 4; i++)
		{
			// Reset the check
			mUsers[i].ready = false;

			//// Send the reset to the users
			//if (mPlayerSockets[i] != -1)
			//	this->SendMsg(mUserSockets[mPlayerSockets[i]], (char*)"r0", iSendResult);
		}

	return ret_val;
}

void WebConnection::SendVibration(int playerIndex)
{
	this->SendMsg(mUserSockets[mPlayerSockets[playerIndex]], (char*)"vib", iSendResult);

}

void WebConnection::SendVibrationAll()
{
	BroadcastMsg("vib");
}

void WebConnection::InitThread(void)
{

	this->mRunThread = true;

	t_update = CreateThread(
		nullptr,
		0,
		StaticThreadStart,
		(void*) this,
		0,
		&id_t_update);
}

DWORD WINAPI WebConnection::StaticThreadStart(LPVOID lpParam)
{
	WebConnection* This = (WebConnection*)lpParam;

	This->PlayersJoin();


	printf("Closing thread\n");
	return 0;
}

int WebConnection::IdPlayerSocket(SOCKET sock)
{
	int player = -1;
	//cout << "-Nr of Players getID: " << nrOfPlayers << endl;
	if (sock == INVALID_SOCKET)
	{
		cout << "-Asked for ID of invlaid socket" << endl;
	}
	else if (sock == 0)
	{
		cout << "-Asked for ID of 0 socket" << endl;
	}
	else
		for (size_t p = 0; p < 4; p++)
		{
			if (mPlayerSockets[p] >= 0)
			{
				if (sock == mUserSockets[mPlayerSockets[p]])
				{
					player = p;
				}
			}
		}
	return player;
}

int WebConnection::IdUserSocket(SOCKET sock)
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
		for (size_t p = 0; p < this->mMaxmUserSockets; p++)
		{
			if (sock == this->mUserSockets[p])
			{
				player = p;
			}
		}
	return player;
}

bool WebConnection::CheckForKey(SOCKET sock, char* recBuff, int& Res)
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

char* WebConnection::ReciveMsg(SOCKET sock, char* recvbuf, int& Res)
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

void WebConnection::SendMsg(SOCKET sock, char* client_msg, int& Res)
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

bool WebConnection::RemoveUserSocket(SOCKET sock, int error)
{
	if (sock == INVALID_SOCKET || sock == 0)
	{
		cout << "-ERROR: Tried to remove closed socket" << endl;
	}
	else
	{
		int id = IdUserSocket(sock);
		int player_id = IdPlayerSocket(sock);

		//cout << "-remove U: " << id << " P: " << player_id << endl;

		if (mPlayerSockets[player_id] != -1)
			nrOfPlayers--;

		this->mPlayerSockets[player_id] = -1;
		this->mUsers[player_id].connected = false;



		// Send out to all users the new discornected player
		BroadcastMsgJoined();

		if (error == 0)
		{
			printf("Connection closing...\n");
			// Drop the client
			closesocket(sock);
			FD_CLR(sock, &mMaster);
			return true;
		}
		else if (error < 0)
		{
			printf("recv failed with error < 0 : %d\n", WSAGetLastError());

			// Failed with handshake

			closesocket(sock);
			FD_CLR(sock, &mMaster);
			this->mUserSockets[id] = INVALID_SOCKET;
			for (size_t i = 1; i < mMaxmUserSockets; i++)
			{
				if (mMaster.fd_array[i] == 0)
				{
					//this->mMaster.fd_array[id] = this->mMaster.fd_array[i - 1];
					this->mMaster.fd_array[i - 1] = 0;
					break;
				}
			}


			cout << "--The user closed--" << endl;
		}
		else if (error == 8)
		{
			// User Closed site
			printf("recv failed with error 8: %d\n", WSAGetLastError());
			closesocket(sock);
			FD_CLR(sock, &mMaster);
			this->mUserSockets[id] = INVALID_SOCKET;
			for (size_t i = 1; i < mMaxmUserSockets; i++)
			{
				if (mMaster.fd_array[i] == 0)
				{
					//this->mMaster.fd_array[id] = this->mMaster.fd_array[i - 1];
					this->mMaster.fd_array[i - 1] = 0;
					break;
				}
			}

			cout << "--The user closed--" << endl;
		}
		else
			return false;


		return true;
	}
	
	return false;
}

bool WebConnection::AddUserSocket(SOCKET sock)
{
	int first_empty = 0;
	for (size_t i = 0; i < mMaxmUserSockets; i++)
	{
		if (mUserSockets[i] == INVALID_SOCKET)
		{
			first_empty = i;
			break;
		}
	}

	cout << "-Assigning user to slot: " << first_empty << endl;

	// Return false if there was no avalibe socket
	if (first_empty == mMaxmUserSockets)
		return false;

	// Add the connection to one of the new clients
	this->mUserSockets[first_empty] = accept(mListenSocket, nullptr, nullptr);

	// Adds to the mMaster
	FD_SET(this->mUserSockets[first_empty], &mMaster);

	return true;
}
bool WebConnection::AddPlayerSocket(SOCKET sock)
{
	if (getNrOfPlayers() >= 4)
	{
		return false;
	}
	int first_empty = 0;
	for (size_t i = 0; i < mMaxmPlayerSockets; i++)
	{
		if (mPlayerSockets[i] == -1)
		{
			first_empty = i;
			break;
		}
	}

	cout << "-Assigning PLAYER to slot: " << first_empty << endl;

	mPlayerSockets[first_empty] = IdUserSocket(sock);
	mUsers[first_empty].connected = true;
	this->nrOfPlayers++;

	// Send out to all users the new connected player
	BroadcastMsgJoined();

	return true;
}

void WebConnection::ShutDownSocket(SOCKET sock)
{
	cout << "Closing dow a socket" << endl;
	closesocket(sock);
	FD_CLR(sock, &mMaster);

	WSACleanup();

	this->mRunGameLoop = false;

	this->ShutDownThread();
}

void WebConnection::ShutDownThread()
{
	this->mRunThread = false;
	this->mConnectionOK = false;
	//CloseHandle(this->t_update);
}
