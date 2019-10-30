#pragma once
#include "ecs.h"
#include "HttpServer.h"
#include "UI/UIComponents.h"

void InitHttpServer(ecs::EntityComponentSystem& rECS)
{
	HttpServer::RunHttpServer("../Sites/mobileSite.html", "../Sites/favicon.ico");
	UITextComponent text;
	std::string text_str;
	HttpServer::GetLocalIp4(text_str);
	text_str.insert(0, "Join at adress: ");
	text.mStrText = text_str;
	UIDrawPosComponent pos;
	pos.mDrawArea.top = 50.f;
	pos.mDrawArea.left = 700.f;
	pos.mDrawArea.bottom = 300.f;
	pos.mDrawArea.right = 1400.f;
	UIDrawColorComponent color;
	color.mColor = White;
	rECS.createEntity(text, pos, color); // Delete this later pls
}

void StopHttpServer()
{
	HttpServer::StopHttpServer();
}