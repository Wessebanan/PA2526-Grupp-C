#pragma once

#include "ecsComponentIncludes.h"

namespace ecs
{
	namespace components
	{
		struct UITextComponent : public ECSComponent<UITextComponent>
		{
			char text[30];
			float textColor;
			float backgroundColor;
		};

		struct UITextBoxComponent : public ECSComponent<UITextBoxComponent>
		{
			float backgroundColor;
		};
	}
}