#include "webConnection.h"


WebConnection::WebConnection()
{
	if (RUNSOCKET)
	{

	}
	else
	{
		// Save the gamestate file to a Json::Value
		this->jsonValueWebState = this->readJson(this->webStateFilePath);
	}
}

WebConnection::~WebConnection()
{
}

void WebConnection::update()
{
	if (RUNSOCKET)
	{

	}
	else
	{
		// Read the file with the buttons, bad to open and read a file each frame
		this->jsonValuePlayers = this->readJson(this->playerFilePath);
	}

}

int WebConnection::getPlayerButton(int player)
{
	int retVal = -1;
	std::string str = jsonValuePlayers[player]["Button"].asString();

	retVal = std::stoi(str);

	return retVal;
}

std::string WebConnection::getPlayername(int player)
{
	return jsonValuePlayers[player]["Name"].asString();
}

int WebConnection::getPlayerTile(int player, int axis)
{
	int retVal = -1;
	if (axis == 0)
	{
		std::string str = jsonValuePlayers[player]["MapTileX"].asString();
		retVal = std::stoi(str);
	}
	else
	{
		std::string str = jsonValuePlayers[player]["MapTileY"].asString();
		retVal = std::stoi(str);
	}

	return retVal;
}

bool WebConnection::setGamestate(int gamestate)
{
	this->jsonValueWebState["gameState"] = std::to_string(gamestate);

	return writeJson(this->jsonValueWebState);
}

Json::Value WebConnection::readJson(const char* filePath)
{
	Json::Value retVal;
	Json::Reader reader;
	std::ifstream players_file;

	players_file.open(filePath, std::ifstream::binary);
	if (!reader.parse(players_file, retVal))
	{
		std::cout << reader.getFormattedErrorMessages();
	}
	players_file.close();

	return retVal;
}
Json::Value WebConnection::readJson()
{
	return readJson(this->playerFilePath);
}

bool WebConnection::writeJson(const char* filePath, Json::Value outputValue)
{
	Json::StyledStreamWriter writer;
	std::ofstream jsonFile;

	jsonFile.open(filePath);
	writer.write(jsonFile, outputValue);
	jsonFile.close();

	return true;
}

bool WebConnection::writeJson(Json::Value outputValue)
{
	return writeJson(this->webStateFilePath, this->jsonValueWebState);
}
