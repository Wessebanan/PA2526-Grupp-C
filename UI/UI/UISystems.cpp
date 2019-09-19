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
	
	D2D->PrintText(UITextComp->strText, UIPosComp->drawArea, UIColorComp->color);
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

	D2D->DrawBitmap(UIBitmapComp->bitmap, UIPosComp->drawArea);
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

	D2D->drawRect(UIPosComp->drawArea, UIThicknessComp->thickness, UIColorComp->color);
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

	D2D->solidRect(UIPosComp->drawArea, UIColorComp->color);
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
	if (this->toRender)
	{
	components::UITextComponent* UITextComp = _entityInfo.getComponent<components::UITextComponent>();

	D2D->PrintDebug(UITextComp->strText);
	}
}
