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
		};

		struct UIDrawPosComponent : public ECSComponent<UIDrawPosComponent>
		{
			D2D1_RECT_F drawArea;
		};

		struct UIDrawColorComponent : public ECSComponent<UIDrawColorComponent>
		{
			brushColors color;
		};

		struct UIBitmapComponent : public ECSComponent<UIBitmapComponent>
		{
			ID2D1Bitmap* bitmap;
		};

		struct UIThicknessComponent : public ECSComponent<UIThicknessComponent>
		{
			int thickness;
		};

	}
}