#include "UISystems.h"

using namespace ecs;
using namespace ecs::systems;

UITextSystem::UITextSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UITextComponent::typeID);
	typeFilter.addRequirement(components::UIDrawColorComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);

}

UITextSystem::~UITextSystem()
{
	//
}

void UITextSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UITextComponent* UITextComp = _entityInfo.getComponent<components::UITextComponent>();
	components::UIDrawPosComponent* UIPosComp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIDrawColorComponent* UIColorComp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	
	mpD2D->PrintText(UITextComp->mStrText, UIPosComp->mDrawArea, UIColorComp->mColor);
}

UIBitmapSystem::UIBitmapSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIBitmapComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
}

UIBitmapSystem::~UIBitmapSystem()
{
	//
}

void UIBitmapSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIDrawPosComponent* UIPosComp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIBitmapComponent* UIBitmapComp = _entityInfo.getComponent<components::UIBitmapComponent>();

	mpD2D->DrawBitmap(UIBitmapComp->mpBitmap, UIPosComp->mDrawArea);
}

ecs::systems::UIRectSystem::UIRectSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIDrawColorComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
	typeFilter.addRequirement(components::UIThicknessComponent::typeID);
}

ecs::systems::UIRectSystem::~UIRectSystem()
{
	//
}

void ecs::systems::UIRectSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIDrawColorComponent* UIColorComp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	components::UIDrawPosComponent* UIPosComp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIThicknessComponent* UIThicknessComp = _entityInfo.getComponent<components::UIThicknessComponent>();

	mpD2D->drawRect(UIPosComp->mDrawArea, UIThicknessComp->mThickness, UIColorComp->mColor);
}

ecs::systems::UISolidRectSystem::UISolidRectSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIDrawColorComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
}

ecs::systems::UISolidRectSystem::~UISolidRectSystem()
{
	//
}

void ecs::systems::UISolidRectSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIDrawColorComponent* UIColorComp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	components::UIDrawPosComponent* UIPosComp = _entityInfo.getComponent<components::UIDrawPosComponent>();

	mpD2D->solidRect(UIPosComp->mDrawArea, UIColorComp->mColor);
}

ecs::systems::UIDebugSystem::UIDebugSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UITextComponent::typeID);
}

ecs::systems::UIDebugSystem::~UIDebugSystem()
{
	//
}

void ecs::systems::UIDebugSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	BaseComponent *pBase = (ECSUser::getComponentsOfType(components::KeyboardComponent::typeID)).next();
	if (pBase)
	{
		components::KeyboardComponent* pKeyboard = static_cast<components::KeyboardComponent*>(pBase);

		if (pKeyboard->E && !this->mPressedLastUpdate)
		{
			toRender = !toRender;
			this->mPressedLastUpdate = true;
		}
		else if (!pKeyboard->E)
		{
			this->mPressedLastUpdate = false;
		}
	}
	
	if (this->toRender)
	{
		components::UITextComponent* UITextComp = _entityInfo.getComponent<components::UITextComponent>();

		mpD2D->PrintDebug(UITextComp->mStrText);
	}
}