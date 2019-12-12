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
		struct UIECSText : public ECSComponent<UIECSText>
		{
			ID unitEntityID;
		};

		struct UIECSLayerText : public ECSComponent<UIECSLayerText>
		{
			int layer;
		};

		struct UITextComponent : public ECSComponent<UITextComponent>
		{
			//char text[30];
			std::wstring mStrText; 

			UITAG tag = UITAG::NOTAG;
			unsigned int text_size = 2;
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
			ID2D1Bitmap1* mpBitmap = nullptr;
			ID2D1Bitmap1* mpTintedBitmap = nullptr;
			std::string mName = "";
			unsigned int mBitmapID;
			bool to_draw = true;
		};

		struct UIThicknessComponent : public ECSComponent<UIThicknessComponent>
		{
			int mThickness; //uses an int to determin the thickness of the draw rect border
		};

		struct UIIWant : public ECSComponent<UIIWant> //a pretty all around component used for extra stuff for special systems
		{
			float elapsedTime = 0.f;
		};

		struct GuideLinkerComponent : public ECSComponent<GuideLinkerComponent>
		{
			ID mPrev;
			//ID mNext;
		};

		struct UIUnitReader : public ECSComponent<UIUnitReader>
		{
			enum UI_UNIT_PLACEMENT
			{
				UI_UNIT_LEFT,
				UI_UNIT_MIDDLE,
				UI_UNIT_RIGHT,
			} unitPlacement;

			ID unitID;
			ID armyID;
			ID playerID;
		};

		struct UIArmyReader : public ECSComponent<UIArmyReader>
		{
			ID playerID;
		};
	}
}