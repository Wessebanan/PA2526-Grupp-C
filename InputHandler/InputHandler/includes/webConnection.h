/*
Documentation syntax: 
Client is the c++ application
User is the website in browser/phone
*/

#pragma once
#ifndef WEBCONNECTION_H
#define WEBCONNECTION_H

#include <iostream>
#include <fstream>
#include "jsoncpp/json/json.h"


#define RUNSOCKET false

class WebConnection
{
public:
	WebConnection();
	~WebConnection();

	void update();

	// Returns the button index the player has selected
	int getPlayerButton(int player);
	// Returns the name of the set player (playres cant change this yet)
	std::string getPlayername(int player);

	// Returns the tile index of the axis (0 = X or 1 = Y)
	int getPlayerTile(int player, int axis);
	
	
	// Changes the gamestate for the users
	bool setGamestate(int gamestate);


private:
	// %%%%		SOCKETS		%%%%

	// To be implemented....

	// %%%%		 JSON		%%%%

	// Path to the file where tha players input is located
	const char* playerFilePath = "C:/gitFolder/InputHandler/InputHandler/jn son/playerMode.json";
	// Path to the file where the client writes so the users can read the gamestate
	const char* webStateFilePath = "C:/gitFolder/InputHandler/InputHandler/json/webState.json";


	//reads the file from the path
	Json::Value readJson(const char* filePath);
	//returns the playerFile
	Json::Value readJson();

	// writes the json::Value to the filepath
	bool writeJson(const char* filePath, Json::Value outputValue);
	// writes to the gamestate json
	bool writeJson(Json::Value outputValue);

	// Holds the parsed json for the different files
	Json::Value jsonValueWebState;
	Json::Value jsonValuePlayers;
};



#endif // !WEBCONNECTION_H
