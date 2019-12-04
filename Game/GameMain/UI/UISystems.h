#pragma once
#include "ecsSystemIncludes.h"
#include "../Input/InitInputComponents.h"
#include "..//gameAI/AIComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../gameGameLoop/GameLoopComponents.h"
#include "UIComponents.h"
#include "UIEvents.h"
#include "Direct2D.h"


namespace ecs
{
	namespace systems
	{

		//class CustomSystem : public ECSSystem<CustomSystem>
		//{
		//public:
		//	CustomSystem();
		//	virtual ~CustomSystem();

		//	// Override ONE of these
		//	void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		//	void updateMultipleEntities(EntityIterator& _entities, float _delta) override;
		//	void readEvent(BaseEvent& _event, float _delta) override;
		//	void act(float _delta) override;
		//};




		/*
			ALL UI SYSTEMS NEEDS A DIRECT2D THAT HAS A HWND RENDER TARGET INITIALIZED
		*/

		class UITextSystem : public ECSSystem<UITextSystem> //system for drawing text
			//it needs 3 components text, drawColor, and drawPos then the system draws
			//that text with that color at that position
		{
		public:
			UITextSystem();
			virtual ~UITextSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		};

		class UIDebugSystem : public ECSSystem<UIDebugSystem> //system for just debug print
			//it needs 2 components text and drawColor then the system draws
			//that text with that color at a pre-selected location
		{
		public:
			UIDebugSystem();
			virtual ~UIDebugSystem();

			bool toRender = false;
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		private:
			bool mPressedLastUpdate = false;
		};

		class UIBitmapSystem : public ECSSystem<UIBitmapSystem> //system for drawing bitmaps
			//it needs 2 components bitmap and drawPos then the system draws
			//that bitmap at that position
		{
		public:
			UIBitmapSystem();
			virtual ~UIBitmapSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		};

		class UIRectSystem : public ECSSystem<UIRectSystem> //system for drawing "wire mesh" rects
			//it needs 3 components Drawcolor, Thickness and drawPos then the system draws
			//that hollow rect with that color at that position
		{
		public:
			UIRectSystem();
			virtual ~UIRectSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		};

		class UISolidRectSystem : public ECSSystem<UISolidRectSystem> //system for drawing filled rects
			//it needs 2 components Drawcolor and drawPos then the system draws
			//that filled rect with that color at that position
		{
		public:
			UISolidRectSystem();
			virtual ~UISolidRectSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		};

		class UIPreRenderSystem : public ECSSystem<UIPreRenderSystem>
		{
		public:
			UIPreRenderSystem() //system for that needs to be before all drawing
			{
				updateType = SystemUpdateType::Actor;
			}
			virtual ~UIPreRenderSystem()
			{
			}

			void act(float _delta) override
			{
				mpD2D->GetpContext()->BeginDraw();
			}
			Direct2D* mpD2D;
		};

		class UIPostRenderSystem : public ECSSystem<UIPostRenderSystem>
		{
		public:
			UIPostRenderSystem() //system for that needs to be after all drawing
			{
				updateType = SystemUpdateType::Actor;
			}
			virtual ~UIPostRenderSystem()
			{
				//
			}

			void act(float _delta) override
			{
				mpD2D->GetpContext()->EndDraw();
			}
			Direct2D* mpD2D;
		};

		class UICountDownSystem : public ECSSystem<UICountDownSystem>
		{
		public:
			UICountDownSystem();
			void Init();
			virtual ~UICountDownSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			void onEvent(TypeID _eventType, BaseEvent* _event) override;
			Direct2D* mpD2D;
			float mSize = 0;
			int mCounter = 3;
			int mExpand_size = 500;
		};

		class UIGuideSystem : public ECSSystem<UIGuideSystem> //system for drawing filled rects
			//it needs 2 components Drawcolor and drawPos then the system draws
			//that filled rect with that color at that position
		{
		public:
			UIGuideSystem();
			virtual ~UIGuideSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* mpD2D;
		private:
			int mCounter = 7;
			float mElapsedTime;
		};

		class UIOverlayInitSystem : public ECSSystem<UIOverlayInitSystem>
		{
		public:
			UIOverlayInitSystem();
			virtual ~UIOverlayInitSystem();
			void readEvent(BaseEvent& _event, float _delta) override;

			Direct2D* mpD2D;
		};
		
		class UIUnitColorUpdateSystem : public ECSSystem<UIUnitColorUpdateSystem>
		{
		public:
			UIUnitColorUpdateSystem();
			virtual ~UIUnitColorUpdateSystem();

			void updateEntity(FilteredEntity& uiUnit, float delta) override;

			Direct2D* mpD2D;
		};

		class UIEndOfRoundSystem : public ECSSystem<UIEndOfRoundSystem>
		{
		public:
			UIEndOfRoundSystem();
			virtual ~UIEndOfRoundSystem();
			void readEvent(BaseEvent& _event, float _delta) override;

			Direct2D* mpD2D;
		};
	}

}