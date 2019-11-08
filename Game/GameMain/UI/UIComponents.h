#pragma once

#include "ecsComponentIncludes.h"
#include "Direct2D.h"


enum UITAG
{
	STARTTEXT,
	ARMYUI,
	DEBUGUI,
	NOTAG
};

namespace ecs
{
	namespace components
	{
		struct UITextComponent : public ECSComponent<UITextComponent>
		{
			//char text[30];
			std::string mStrText; 

			UITAG tag = UITAG::NOTAG;
		};

		struct UIDrawPosComponent : public ECSComponent<UIDrawPosComponent>
		{
			D2D1_RECT_F mDrawArea; 
		};

		struct UIDrawColorComponent : public ECSComponent<UIDrawColorComponent>
		{
			brushColors mColor; //component need one of 6 colors from the enum brushColor
			//Black, White, Red, Blue, Green, Purple
		};

		struct UITextSizeComponent : public ECSComponent<UITextSizeComponent>
		{
			int mSize = 2;
		};

		struct UIBitmapComponent : public ECSComponent<UIBitmapComponent>
		{
			ID2D1Bitmap1* mpBitmap;
			std::string mName;
		};

		struct UIThicknessComponent : public ECSComponent<UIThicknessComponent>
		{
			int mThickness; //uses an int to determin the thickness of the draw rect border
		};

		struct UIIWant : public ECSComponent<UIIWant> //a pretty all around component used for extra stuff for special systems
		{
			float elapsedTime = 0.f;
		};
	}
}