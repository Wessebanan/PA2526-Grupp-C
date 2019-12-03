#include "UISystems.h"
#include "..//gameAI/AISystems.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameGameLoop/GameLoopEvents.h"
#include "../gameGameLoop/GameLoopComponents.h"


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
	components::UIDrawPosComponent* UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIDrawColorComponent* UI_color_comp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	
	mpD2D->PrintText(UITextComp->mStrText, UI_pos_comp->mDrawArea, UI_color_comp->mColor);
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
	components::UIDrawPosComponent* UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIBitmapComponent* p_UI_bitmap_comp = _entityInfo.getComponent<components::UIBitmapComponent>();

	//mpD2D->SetBitmapTint(p_UI_bitmap_comp->mpBitmap);

	if (p_UI_bitmap_comp->mpTintedBitmap)
	{
		mpD2D->DrawBitmap(p_UI_bitmap_comp->mpTintedBitmap, UI_pos_comp->mDrawArea);
	}
	else
	{
		mpD2D->DrawBitmap(p_UI_bitmap_comp->mpBitmap, UI_pos_comp->mDrawArea);
	}
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
	components::UIDrawColorComponent* UI_color_comp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	components::UIDrawPosComponent* UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIThicknessComponent* UI_thickness_comp = _entityInfo.getComponent<components::UIThicknessComponent>();

	mpD2D->DrawRect(UI_pos_comp->mDrawArea, UI_thickness_comp->mThickness, UI_color_comp->mColor);
}

ecs::systems::UISolidRectSystem::UISolidRectSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIDrawColorComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
	typeFilter.addRequirement(components::UIIWant::typeID);
}

ecs::systems::UISolidRectSystem::~UISolidRectSystem()
{
	//
}

void ecs::systems::UISolidRectSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIDrawColorComponent* UI_color_comp = _entityInfo.getComponent<components::UIDrawColorComponent>();
	components::UIDrawPosComponent* UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();

	mpD2D->SolidRect(UI_pos_comp->mDrawArea, UI_color_comp->mColor);
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
	BaseComponent *p_base = (ECSUser::getComponentsOfType(components::KeyboardComponent::typeID)).next();
	if (p_base)
	{
		components::KeyboardComponent* p_keyboard = static_cast<components::KeyboardComponent*>(p_base);

		if (p_keyboard->E && !this->mPressedLastUpdate)
		{
			toRender = !toRender;
			this->mPressedLastUpdate = true;
		}
		else if (!p_keyboard->E)
		{
			this->mPressedLastUpdate = false;
		}
	}
	
	if (this->toRender)
	{
		components::UITextComponent* UI_text_comp = _entityInfo.getComponent<components::UITextComponent>();

		mpD2D->PrintDebug(UI_text_comp->mStrText);
	}
}

ecs::systems::UIUpdateSystem::UIUpdateSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::ArmyComponent::typeID);
	typeFilter.addRequirement(components::UITextComponent::typeID);
}

ecs::systems::UIUpdateSystem::~UIUpdateSystem()
{
}

void ecs::systems::UIUpdateSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UITextComponent* p_text = _entityInfo.getComponent<components::UITextComponent>();
	components::ArmyComponent* p_army = _entityInfo.getComponent<components::ArmyComponent>();
	ComponentIterator itt;

	std::wstring ss = L"";

	itt = getComponentsOfType(components::GameLoopComponent::typeID);
	components::GameLoopComponent* p_gl = (components::GameLoopComponent*)itt.next();
	itt = getComponentsOfType(components::UserNameComponent::typeID);
	components::UserNameComponent* p_name_comp = (components::UserNameComponent*)itt.next();


	/*
		Here we update the information in the players corner UI

		We can update the name of all the playesr when the UI has bitmais instead of hwo it is now
	*/
	ss.append(L"Score: ");
	if (p_gl != nullptr)
	{
		ss.append(std::to_wstring(p_gl->mPlayerPoints[(int)p_army->playerID]));
	}
	ss.append(L"\n");
	ss.append(L"Name: ");
	std::string player_name = p_name_comp->names[(int)p_army->playerID];
	ss.append(std::wstring(player_name.begin(),player_name.end()));
	ss.append(L"\n");
	ss.append(L"Health:\n");
	for (size_t i = 0; i < p_army->unitIDs.size(); i++)
	{
		components::HealthComponent* p_unit_hp_comp = getComponentFromKnownEntity<components::HealthComponent>(p_army->unitIDs[i]);
		components::UIBitmapComponent* p_bitmap_comp = getComponentFromKnownEntity<components::UIBitmapComponent>(p_army->unitIDs[i]);
		
		ss.append(std::to_wstring((int)p_unit_hp_comp->mHealth));
		ss.append(L"\n");
	}


	p_text->mStrText = ss;

}
#include "../gameAudio/AudioECSEvents.h"
ecs::systems::UICountDownSystem::UICountDownSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIBitmapComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
	typeFilter.addRequirement(components::UIIWant::typeID);
	subscribeEventCreation(events::CountdownStartEvent::typeID);
}

void ecs::systems::UICountDownSystem::Init()
{
}

ecs::systems::UICountDownSystem::~UICountDownSystem()
{
	//
}
void ecs::systems::UICountDownSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	bool one_sec_has_passed = false;
	components::UIBitmapComponent* p_UI_bitmap_comp = _entityInfo.getComponent<components::UIBitmapComponent>();
	components::UIDrawPosComponent* p_UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::UIIWant* p_UI_time = _entityInfo.getComponent<components::UIIWant>();
	float pre_added_time = p_UI_time->elapsedTime;
	p_UI_time->elapsedTime += _delta;
	if (floor(pre_added_time) != floor(p_UI_time->elapsedTime))
	{
		one_sec_has_passed = true;
	}
		
	

	if (p_UI_time->elapsedTime > 4.f) //remove the entity after 4 second which stops the countdown system to update
	{
		this->mCounter = 3;
		removeEntity(_entityInfo.entity->getID());

		CreateSystem<systems::SwitchStateSystem>(4);

		return;
	}
	if (one_sec_has_passed)
	{
		{
			events::PlaySound m_event;
			if(mCounter == 3)
				m_event.audioName = AudioName::SOUND_two;
			else if(mCounter == 2)
				m_event.audioName = AudioName::SOUND_one;
			else
				m_event.audioName = AudioName::SOUND_go;
			createEvent(m_event);
		}
		this->mCounter--;
		p_UI_pos_comp->mDrawArea.bottom		-= this->mExpand_size;
		p_UI_pos_comp->mDrawArea.left		+= this->mExpand_size;
		p_UI_pos_comp->mDrawArea.right		-= this->mExpand_size;
		p_UI_pos_comp->mDrawArea.top		+= this->mExpand_size;
		p_UI_bitmap_comp->mpBitmap = this->mpD2D->GetBitmap("rob" + std::to_string(this->mCounter)); //load in the new bitmap when a second has passed in a pretty static way
	}

	this->mSize = this->mExpand_size * _delta; //Will make mSize close to expand_size after 1 sec
	p_UI_pos_comp->mDrawArea.bottom		+= this->mSize;
	p_UI_pos_comp->mDrawArea.left		-= this->mSize;
	p_UI_pos_comp->mDrawArea.right		+= this->mSize;
	p_UI_pos_comp->mDrawArea.top		-= this->mSize;
	this->mpD2D->DrawBitmap(p_UI_bitmap_comp->mpBitmap, p_UI_pos_comp->mDrawArea);
}

void ecs::systems::UICountDownSystem::onEvent(TypeID _eventType, BaseEvent* _event)
{
	TypeFilter countdown_filter;
	countdown_filter.addRequirement(ecs::components::UIBitmapComponent::typeID);
	countdown_filter.addRequirement(ecs::components::UIDrawPosComponent::typeID);
	countdown_filter.addRequirement(ecs::components::UIIWant::typeID);
	EntityIterator current_countdowns = getEntitiesByFilter(countdown_filter); 
	
	if(current_countdowns.entities.size() < 1) //check so that 2 countdowns can not exist at the same time
	{
		components::UIBitmapComponent m_bitmap;
		components::UIDrawPosComponent m_pos;
		components::UIIWant m_time;
		int width = this->mpD2D->GetBackbufferBitmap()->GetSize().width;
		int height = this->mpD2D->GetBackbufferBitmap()->GetSize().height;
			
		if (this->mpD2D->GetBitmap("rob3") == nullptr) //check to see if the bitmaps already exist
		{
			this->mpD2D->LoadImageToBitmap("../../UI/Resource/rob0.png", "rob0");
			this->mpD2D->LoadImageToBitmap("../../UI/Resource/rob1.png", "rob1");
			this->mpD2D->LoadImageToBitmap("../../UI/Resource/rob2.png", "rob2");
			m_bitmap.mpBitmap = this->mpD2D->LoadImageToBitmap("../../UI/Resource/rob3.png", "rob3");
		}
		else
			m_bitmap.mpBitmap = this->mpD2D->GetBitmap("rob3");

		m_pos.mDrawArea.bottom = height / 2;
		m_pos.mDrawArea.left   = width / 2;
		m_pos.mDrawArea.right = width / 2;
		m_pos.mDrawArea.top = height / 2;

		createEntity(m_bitmap, m_pos, m_time);
		{
			events::PlaySound m_event;
			m_event.audioName = AudioName::SOUND_three;
			createEvent(m_event);
		}
	}
}

ecs::systems::UIOverlayInitSystem::UIOverlayInitSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(events::ResetUIComponents::typeID);
}

ecs::systems::UIOverlayInitSystem::~UIOverlayInitSystem()
{
	//
}

void ecs::systems::UIOverlayInitSystem::readEvent(BaseEvent& _event, float _delta)
{
	if (_event.getTypeID() != events::ResetUIComponents::typeID) //change 0 to my event id
	{
		return;
	}

	TypeFilter army_filter;
	army_filter.addRequirement(ArmyComponent::typeID);
	EntityIterator armies = getEntitiesByFilter(army_filter);

	TypeFilter ui_filter;
	ui_filter.addRequirement(UIBitmapComponent::typeID);
	ui_filter.addRequirement(UIDrawPosComponent::typeID);
	ui_filter.addRequirement(UIUnitReader::typeID);
	EntityIterator ui_units = getEntitiesByFilter(ui_filter);

	UIUnitReader* p_unit_reader_comp;
	ArmyComponent* p_army_comp;
	for (FilteredEntity ui_unit : ui_units.entities)
	{
		p_army_comp = nullptr;
		p_unit_reader_comp = ui_unit.getComponent<UIUnitReader>();

		// Find correct army
		for (FilteredEntity army : armies.entities)
		{
			ArmyComponent* p_army = army.getComponent<ArmyComponent>();
			if (p_unit_reader_comp->playerID == p_army->playerID)
			{
				p_army_comp = p_army;
				break;
			}
		}

		p_unit_reader_comp->armyID = p_army_comp->getEntityID();
		p_unit_reader_comp->unitID = p_army_comp->unitIDs.at((int)p_unit_reader_comp->unitPlacement);
	}
}

ecs::systems::UIUnitColorUpdateSystem::UIUnitColorUpdateSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(UIUnitReader::typeID);
	typeFilter.addRequirement(UIBitmapComponent::typeID);
}

ecs::systems::UIUnitColorUpdateSystem::~UIUnitColorUpdateSystem()
{
	//
}

void ecs::systems::UIUnitColorUpdateSystem::updateEntity(FilteredEntity& uiUnit, float delta)
{
	UIUnitReader* p_unit_reader_comp = uiUnit.getComponent<UIUnitReader>();
	UIBitmapComponent* p_bitmap_comp = uiUnit.getComponent<UIBitmapComponent>();
	ColorComponent* p_color_comp = getComponentFromKnownEntity<ColorComponent>(p_unit_reader_comp->unitID);
	
	/*
		Check if unit exist, else set default color.
	*/

	if (p_color_comp)
	{
		mpD2D->SetBitmapTint(p_bitmap_comp->mpBitmap, p_bitmap_comp->mpTintedBitmap, p_color_comp->red, p_color_comp->green, p_color_comp->blue);
	}
	else
	{
		mpD2D->SetBitmapTint(p_bitmap_comp->mpBitmap, p_bitmap_comp->mpTintedBitmap, 0, 0, 1);
	}
}

ecs::systems::UIEndOfRoundSystem::UIEndOfRoundSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(events::ResetUIComponents::typeID);
}

ecs::systems::UIEndOfRoundSystem::~UIEndOfRoundSystem()
{
	//
}

void ecs::systems::UIEndOfRoundSystem::readEvent(BaseEvent& _event, float _delta)
{
	if (_event.getTypeID() != events::RoundEndEvent::typeID) //change 0 to my event id
	{
		return;
	}
	int winner = dynamic_cast<ecs::events::RoundEndEvent*>(&_event)->winner;
	ComponentIterator itt = ecs::ECSUser::getComponentsOfType(ecs::components::GameLoopComponent::typeID);
	GameLoopComponent* p_gl = (GameLoopComponent*)itt.next();
	TypeFilter ui_army_filter;
	ui_army_filter.addRequirement(UIBitmapComponent::typeID);
	ui_army_filter.addRequirement(UIDrawPosComponent::typeID);
	ui_army_filter.addRequirement(UIArmyReader::typeID);
	EntityIterator ui_armies = getEntitiesByFilter(ui_army_filter);

	for (FilteredEntity ui_armies : ui_armies.entities)
	{
		UIArmyReader* p_army = ui_armies.getComponent<UIArmyReader>();
		UIBitmapComponent* army_bitmaps = ui_armies.getComponent<UIBitmapComponent>();
		//First find the right army then the right bitmap in the army by taking the winners point -1 because a system before this one adds one to the score
		if (p_army->playerID == winner && army_bitmaps->mBitmapID == p_gl->mPlayerPoints[winner]-1)
		{
			this->mpD2D->SetBitmapTint(army_bitmaps->mpBitmap, army_bitmaps->mpTintedBitmap, 255, 255, 0);//change the bitmap color to yellow 
		}
	}
}
