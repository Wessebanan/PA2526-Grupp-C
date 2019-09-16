#pragma once

#include "ecsComponentIncludes.h"
#include "Direct2D.h"

namespace ecs
{
	namespace components
	{
		struct UITextComponent : public ECSComponent<UITextComponent>
		{
			//char text[30];
			std::string strText;
			//D2D1_COLOR_F textColor;
		};

		struct UIDrawPos : public ECSComponent<UIDrawPos>
		{
			D2D1_RECT_F drawArea;
		};

		struct UIDrawColor : public ECSComponent<UIDrawColor>
		{
			brushColors color;
		};

		struct UIBitmap : public ECSComponent<UIBitmap>
		{
			ID2D1Bitmap* bitmap;
			//ID bitmapID;
		};

		//struct BitMapComponent : public ECSComponent<BitMapComponent>
		//{
		//	D2D1_RECT_F drawArea;
		//	ID bitMapID;
		//};
	}
}