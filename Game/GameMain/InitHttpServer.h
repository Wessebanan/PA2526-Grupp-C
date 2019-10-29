#pragma once
#include "ecs.h"
#include "HttpServer.h"
#include "UI/UIComponents.h"

void InitHttpServer(ecs::EntityComponentSystem& rECS)
{
	HttpServer::RunHttpServer("../Sites/mobileSite.html", "../Sites/favicon.ico");
	UITextComponent text;
	HttpServer::GetLocalIp4(text.mStrText);
	UIDrawPosComponent pos;
	pos.mDrawArea = { 300.0f, 200.0f, 1000.0f, 300.f};
	UIDrawColorComponent color;
	color.mColor = Purple;
	rECS.createEntity(text, pos, color); // Delete this later pls
}

void StopHttpServer()
{
	HttpServer::StopHttpServer();
}