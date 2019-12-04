#include "UISystems.h"
#include "..//gameAI/AISystems.h"


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

	mpD2D->DrawBitmap(p_UI_bitmap_comp->mpBitmap, UI_pos_comp->mDrawArea);
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


	if (!p_UI_bitmap_comp || !p_guide_linker || !p_UI_pos_comp)
	{
		return;
	}

	mElapsedTime += _delta/8;

	float per_image_time = 3.0f;

	int curr_guide = ((int)(mElapsedTime / per_image_time));
	
	if (curr_guide > 7)
		curr_guide = 7;

	if (p_UI_bitmap_comp->mName == string("guide" + to_string(curr_guide)))
	{
		p_UI_pos_comp->mDrawArea.bottom = 1000;

		UIDrawPosComponent* p_prev_pos = getComponentFromKnownEntity<UIDrawPosComponent>(p_guide_linker->mPrev);
		if (p_prev_pos)
			p_prev_pos->mDrawArea.bottom = 200;

		if (mElapsedTime > per_image_time * 8)
		{
			p_UI_pos_comp->mDrawArea.bottom = 200;
			RemoveSystem<UIGuideSystem>();
		}
	}
}
