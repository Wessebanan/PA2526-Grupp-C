#pragma once

#include "ecs.h"

#include "../../AI/AI/includes/GridProp.h"
#include "..//gameAI/GridEcsFunctions.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameUtility/UtilityComponents.h"
#include <DirectXMath.h>


void InitGrid(ecs::EntityComponentSystem& rECS)
{
	GridEcsFunctions::CreateGrid(rECS, 12, 12, 1.0f);

}