#include "UISystems.h"

using namespace ecs;
using namespace ecs::systems;

UITextSystem::UITextSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	componentFilter.addRequirement(components::UITextComponent::typeID);
	componentFilter.addRequirement(components::UIDrawColor::typeID);
	componentFilter.addRequirement(components::UIDrawPos::typeID);

}

UITextSystem::~UITextSystem()
{
	//
}

void UITextSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UITextComponent* UITextComp = _entityInfo.getComponent<components::UITextComponent>();
	components::UIDrawPos* UIPosComp = _entityInfo.getComponent<components::UIDrawPos>();
	components::UIDrawColor* UIColorComp = _entityInfo.getComponent<components::UIDrawColor>();
	
	D2D->PrintText(UITextComp->strText, UIPosComp->drawArea, UIColorComp->color);
}

UIBitmapSystem::UIBitmapSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	componentFilter.addRequirement(components::UIBitmap::typeID);
	componentFilter.addRequirement(components::UIDrawPos::typeID);
}

UIBitmapSystem::~UIBitmapSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	componentFilter.addRequirement(components::UIBitmap::typeID);
	componentFilter.addRequirement(components::UIDrawPos::typeID);
}

void UIBitmapSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIDrawPos* UIPosComp = _entityInfo.getComponent<components::UIDrawPos>();
	components::UIBitmap* UIBitmapComp = _entityInfo.getComponent<components::UIBitmap>();
	D2D->DrawBitmap(UIBitmapComp->bitmap, UIPosComp->drawArea);
}
