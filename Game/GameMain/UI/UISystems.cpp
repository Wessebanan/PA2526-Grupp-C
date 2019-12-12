#include "UISystems.h"
#include "..//gameAI/AISystems.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameGameLoop/GameLoopEvents.h"
#include "../gameGameLoop/GameLoopComponents.h"
#include "AIGlobals.h"



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
	
	mpD2D->PrintText(UITextComp->mStrText, UI_pos_comp->mDrawArea, UI_color_comp->mColor, UITextComp->text_size);
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
	components::UIBitmapComponent* p_UI_bitmap_comp = _entityInfo.getComponent<components::UIBitmapComponent>();
	if (!p_UI_bitmap_comp->to_draw)
	{
		return;
	}

	components::UIDrawPosComponent* UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
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

ecs::systems::UIGuideSystem::UIGuideSystem()
{
	updateType = SystemUpdateType::EntityUpdate;
	typeFilter.addRequirement(components::UIBitmapComponent::typeID);
	typeFilter.addRequirement(components::UIDrawPosComponent::typeID);
	typeFilter.addRequirement(components::GuideLinkerComponent::typeID);

	mElapsedTime = 0.0f;
}

ecs::systems::UIGuideSystem::~UIGuideSystem()
{
}

void ecs::systems::UIGuideSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	components::UIBitmapComponent* p_UI_bitmap_comp = _entityInfo.getComponent<components::UIBitmapComponent>();
	components::UIDrawPosComponent* p_UI_pos_comp = _entityInfo.getComponent<components::UIDrawPosComponent>();
	components::GuideLinkerComponent* p_guide_linker = _entityInfo.getComponent<components::GuideLinkerComponent>();

	// Sanity check
	if (!p_UI_bitmap_comp || !p_guide_linker || !p_UI_pos_comp) return;

	// This function runs 8 times
	mElapsedTime += _delta / 8.0f;

	float per_image_time[8] = 
	{
		3.0f,
		5.0f,
		6.0f,
		8.0f,

		6.0f,
		7.0f,
		5.0f,
		3.0f,
	};

	float time_sum = 0.0f;
	for (size_t i = 0; i < 8; i++) 
		time_sum += per_image_time[i];

	//int curr_guide = ((int)(mElapsedTime / per_image_time));

	float passed_image = 0.0f;
	int i;
	for (i = 0; passed_image < mElapsedTime && i < 8; i++)
	{
		passed_image += per_image_time[i];
	}
	int curr_guide = i - 1;
	
	// Safty check
	if (curr_guide < 0)
		curr_guide = 0;

	if (p_UI_bitmap_comp->mName == string("guide" + to_string(curr_guide)))
	{
		p_UI_pos_comp->mDrawArea.bottom = 1000;

		UIDrawPosComponent* p_prev_pos = getComponentFromKnownEntity<UIDrawPosComponent>(p_guide_linker->mPrev);
		if (p_prev_pos)
			p_prev_pos->mDrawArea.bottom = 200;

		// When all images are done, remove the system
		if (mElapsedTime > time_sum)
		{
			p_UI_pos_comp->mDrawArea.bottom = 200;
			RemoveSystem<UIGuideSystem>();
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
	UIBitmapComponent* p_bitmap_comp;
	ComponentIterator itt = getComponentsOfType(components::UserNameComponent::typeID);
	components::UserNameComponent* p_name_comp = (components::UserNameComponent*)itt.next();
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
				p_bitmap_comp = ui_unit.getComponent<UIBitmapComponent>();

				std::string player_name = p_name_comp->names[(int)p_army->playerID];
				getComponentFromKnownEntity<UITextComponent>(p_army->getEntityID())->mStrText = std::wstring(player_name.begin(), player_name.begin()+10);
				p_army_comp = p_army;

				p_unit_reader_comp->armyID = p_army_comp->getEntityID();
				p_unit_reader_comp->unitID = p_army_comp->unitIDs.at((int)p_unit_reader_comp->unitPlacement);
				
				p_bitmap_comp->to_draw = true;

				break;
			}
		}
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

	Entity* p_unit_entity = getEntity(p_unit_reader_comp->unitID);
	if (!p_unit_entity || p_unit_entity->hasComponentOfType<DeadComponent>())
	{
		p_bitmap_comp->to_draw = false;
		return;
	}

	ColorComponent* p_color_comp = getComponentFromKnownEntity<ColorComponent>(p_unit_reader_comp->unitID);
	HealthComponent* p_health_comp = getComponentFromKnownEntity<HealthComponent>(p_unit_reader_comp->unitID);
	int player_ID = p_unit_reader_comp->playerID;

	/*
		Check if unit exist, else set default color.
	*/
	Color army_colors[] =
	{
		PLAYER1_COLOR,
		PLAYER2_COLOR,
		PLAYER3_COLOR,
		PLAYER4_COLOR
	};
	if (p_color_comp)
	{
		//without flashes
		/*mpD2D->SetBitmapTint(p_bitmap_comp->mpBitmap, p_bitmap_comp->mpTintedBitmap,
			std::floorf((float)army_colors[player_ID].r * p_health_comp->mHealth / p_health_comp->mBaseHealth),
			std::floorf((float)army_colors[player_ID].g * p_health_comp->mHealth / p_health_comp->mBaseHealth),
			std::floorf((float)army_colors[player_ID].b * p_health_comp->mHealth / p_health_comp->mBaseHealth));*/

		mpD2D->SetBitmapTint(p_bitmap_comp->mpBitmap, p_bitmap_comp->mpTintedBitmap, p_color_comp->red, p_color_comp->green, p_color_comp->blue); //with flashes
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
	if (_event.getTypeID() != events::RoundEndEvent::typeID)
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

ecs::systems::UIGameRestartSystem::UIGameRestartSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(events::GameReStartEvent::typeID);
}

ecs::systems::UIGameRestartSystem::~UIGameRestartSystem()
{
}

void ecs::systems::UIGameRestartSystem::readEvent(BaseEvent& _event, float _delta)
{
	if (_event.getTypeID() != events::GameReStartEvent::typeID)
	{
		return;
	}
	UIBitmapComponent* army_bitmaps;
	TypeFilter ui_army_filter;
	ui_army_filter.addRequirement(UIBitmapComponent::typeID);
	ui_army_filter.addRequirement(UIDrawPosComponent::typeID);
	ui_army_filter.addRequirement(UIArmyReader::typeID);
	EntityIterator ui_armies = getEntitiesByFilter(ui_army_filter);

	for (FilteredEntity ui_armies : ui_armies.entities)
	{
		army_bitmaps = ui_armies.getComponent<UIBitmapComponent>();
		this->mpD2D->SetBitmapTint(army_bitmaps->mpBitmap, army_bitmaps->mpTintedBitmap, 255, 255, 255);
	}
}

ecs::systems::UnitComponentPrinter::UnitComponentPrinter()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(UIDrawPosComponent::typeID);
	typeFilter.addRequirement(UITextComponent::typeID);
	typeFilter.addRequirement(UIDrawColorComponent::typeID);
	typeFilter.addRequirement(UIECSText::typeID);
}

ecs::systems::UnitComponentPrinter::~UnitComponentPrinter()
{
	//
}

void ecs::systems::UnitComponentPrinter::updateEntity(FilteredEntity& unit, float delta)
{
	UITextComponent* p_text = unit.getComponent<UITextComponent>();
	UIECSText* p_ecs = unit.getComponent<UIECSText>();

	p_text->mStrText = L"ENTITY " + std::to_wstring(unit.entity->getID());

	Entity* e = getEntity(p_ecs->unitEntityID);

	BaseComponent* p;
	for (std::pair<TypeID, ID> compInfo : e->componentIDs)
	{
		p = getComponent(compInfo.first, compInfo.second);
		std::string name = p->getName();

		std::wstring printName = L"";
		for (int i = 0; i < name.size(); i++)
		{
			const char c = name[i];
			if (c == 'C' && (i+2 < name.size()) && name[i+1] == 'o' && name[i+2] == 'm')
				break;
			printName.push_back(c);
		}

		p_text->mStrText += L"\n" + printName;
	}
}

ecs::systems::UnitComponentCreator::UnitComponentCreator()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(events::CreateComponentEvent::typeID);
	subscribeEventCreation(events::RemoveComponentEvent::typeID);
}

ecs::systems::UnitComponentCreator::~UnitComponentCreator()
{
	//
}

void ecs::systems::UnitComponentCreator::onEvent(TypeID _eventType, BaseEvent* _event)
{
	if (_eventType == events::CreateComponentEvent::typeID)
	{
		events::CreateComponentEvent* e = static_cast<events::CreateComponentEvent*>(_event);

		if (e->componentTypeID != UnitComponent::typeID)
		{
			return;
		}

		UnitComponent* p_comp = (UnitComponent*)getComponent(e->componentTypeID, e->componentID);

		UITextComponent text_comp;
		UIDrawPosComponent pos_comp;
		UIDrawColorComponent color_comp;
		UIECSText ecs_comp;

		const int units = getComponentCountOfType(UnitComponent::typeID);
		//const int units = 0;

		const int textWidth = 180.f;
		const int leftStartX = 300.f;
		const int rightStartX = 1100.f;

		switch (p_comp->playerID)
		{
		case 0:
		{
			color_comp.mColor = brushColors::Red;
			pos_comp.mDrawArea.left = leftStartX + unitsPerPlayer[p_comp->playerID] * textWidth;
			pos_comp.mDrawArea.right = pos_comp.mDrawArea.left + textWidth;
			pos_comp.mDrawArea.top = 10.f;
			pos_comp.mDrawArea.bottom = 100.f;
			break;
		}

		case 1:
		{
			color_comp.mColor = brushColors::Purple;
			pos_comp.mDrawArea.left = rightStartX + unitsPerPlayer[p_comp->playerID] * textWidth;
			pos_comp.mDrawArea.right = pos_comp.mDrawArea.left + textWidth;
			pos_comp.mDrawArea.top = 10.f;
			pos_comp.mDrawArea.bottom = 100.f;
			break;
		}

		case 2:
		{
			color_comp.mColor = brushColors::Blue;
			pos_comp.mDrawArea.left = leftStartX + unitsPerPlayer[p_comp->playerID] * textWidth;
			pos_comp.mDrawArea.right = pos_comp.mDrawArea.left + textWidth;
			pos_comp.mDrawArea.top = 700.f;
			pos_comp.mDrawArea.bottom = 1000.f;
			break;
		}

		case 3:
		{
			color_comp.mColor = brushColors::Green;
			pos_comp.mDrawArea.left = rightStartX + unitsPerPlayer[p_comp->playerID] * textWidth;
			pos_comp.mDrawArea.right = pos_comp.mDrawArea.left + textWidth;
			pos_comp.mDrawArea.top = 700.f;
			pos_comp.mDrawArea.bottom = 1000.f;
			break;
		}
		}

		unitsPerPlayer[p_comp->playerID]++;

		text_comp.mStrText = L"INITIAL TEXT";
		text_comp.text_size = 0;
		ecs_comp.unitEntityID = e->entityID;

		createEntity(text_comp, pos_comp, color_comp, ecs_comp);
		return;
	}

	else if (_eventType == events::RemoveComponentEvent::typeID)
	{
		events::RemoveComponentEvent* e = static_cast<events::RemoveComponentEvent*>(_event);

		if (e->componentTypeID != UnitComponent::typeID)
		{
			return;
		}

		ComponentIterator comps = getComponentsOfType<UIECSText>();

		UIECSText* p;
		while (p = (UIECSText*)comps.next())
		{
			if (p->unitEntityID == e->entityID)
			{
				removeEntity(p->getEntityID());
				return;
			}
		}
	}
}

ecs::systems::LayerPrinter::LayerPrinter()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(UIDrawPosComponent::typeID);
	typeFilter.addRequirement(UITextComponent::typeID);
	typeFilter.addRequirement(UIDrawColorComponent::typeID);
	typeFilter.addRequirement(UIECSLayerText::typeID);
}

ecs::systems::LayerPrinter::~LayerPrinter()
{
	//
}

void ecs::systems::LayerPrinter::updateEntity(FilteredEntity& entity, float delta)
{
	UITextComponent* p_text = entity.getComponent<UITextComponent>();
	UIECSLayerText* p_ecs = entity.getComponent<UIECSLayerText>();

	const int layer = p_ecs->layer;

	p_text->mStrText = getLayerString(layer);
}
