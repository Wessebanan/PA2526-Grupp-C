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
#include "../jsoncpp/json/json.h"


// For future implementation
#define RUNSOCKET false

// Handles getting info from the website
class WebConnection
{
public:
	WebConnection();
	~WebConnection();

	// takes the newest values from the web
	void update();

	bool isConnected() { return this->connectionOK; };

	// Returns the button index the player has selected
	int getPlayerButton(int player);
	// Returns the name of the set player (playres cant change this yet)
	std::string getPlayername(int player);

	// Returns the tile index of the axis (0 = X or 1 = Y)
	int getPlayerTile(int player, int axis);
	
	
	// Changes the gamestate for the users
	bool setGamestate(int gamestate);


private:
	bool connectionOK = true;
	// %%%%		SOCKETS		%%%%

	// To be implemented....

	// %%%%		 JSON		%%%%

	// Path to the file where tha players input is located
	//const char* playerFilePath = "../InputHandler/InputHandler/json/playerMode.json";
	const char* playerFilePath = JSONFILEPLAYER;
	//const char* playerFilePathLib = "../InputHandler/InputHandler/json/playerMode.json";
	// Path to the file where the client writes so the users can read the gamestate
	//const char* webStateFilePath = "C:/gitFolder/InputHandler/InputHandler/json/webState.json";
	const char* webStateFilePath = JSONFILEWEBSTATE;
	//const char* webStateFilePathLib = "../InputHandler/InputHandler/json/webState.json";

	//reads the file from the path
	Json::Value readJson(const char* filePath);
	//returns the playerFile
	Json::Value readJson();

	// tries the libFP if it fails
	Json::Value readJsonPlay();
	// tries the libFP if it fails
	Json::Value readJsonState();

	// writes the json::Value to the filepath
	bool writeJson(const char* filePath, Json::Value outputValue);
	// writes to the gamestate json
	bool writeJson(Json::Value outputValue);

	// Holds the parsed json for the different files
	Json::Value jsonValueWebState;
	Json::Value jsonValuePlayers;
};



#endif // !WEBCONNECTION_H
