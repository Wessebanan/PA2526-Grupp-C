#pragma once
#include "ecs.h"
#include "HttpServer.h"
#include "UI/UIComponents.h"
#include <locale>


void InitHttpServer(ecs::EntityComponentSystem& rECS)
{
	HttpServer::RunHttpServer("../Sites/newSite.html", "../Sites/favicon.ico");
	UITextComponent text;
	std::string text_str;
	HttpServer::GetLocalIp4(text_str);

	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> convert;
	std::wstring text_wstr = convert.from_bytes(text_str);

	text_wstr.insert(0, L"Join at adress: ");
	text.mStrText = text_wstr;

	text.tag = UITAG::STARTTEXT;
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