#pragma once
#include "InputComponents.h"

#include "ecsSystemIncludes.h"
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

		class UITextSystem : public ECSSystem<UITextSystem>
		{
		public:
			UITextSystem();
			virtual ~UITextSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* D2D;
		};

		class UIDebugSystem : public ECSSystem<UIDebugSystem>
		{
		public:
			UIDebugSystem();
			virtual ~UIDebugSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* D2D;
		private:
			bool toRender = false;
			bool pressedLastUpdate = false;
		};

		class UIBitmapSystem : public ECSSystem<UIBitmapSystem>
		{
		public:
			UIBitmapSystem();
			virtual ~UIBitmapSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* D2D;
		};

		class UIRectSystem : public ECSSystem<UIRectSystem>
		{
		public:
			UIRectSystem();
			virtual ~UIRectSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* D2D;
		};

		class UISolidRectSystem : public ECSSystem<UISolidRectSystem>
		{
		public:
			UISolidRectSystem();
			virtual ~UISolidRectSystem();

			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
			Direct2D* D2D;
		};

		class UIPreRenderSystem : public ECSSystem<UIPreRenderSystem>
		{
		public:
			UIPreRenderSystem()
			{
				updateType = SystemUpdateType::Actor;
			}
			virtual ~UIPreRenderSystem()
			{
			}

			void act(float _delta) override
			{
				D2D->getHwndRenderTarget()->BeginDraw();
				D2D->getHwndRenderTarget()->Clear();
			}
			Direct2D* D2D;
		};

		class UIPostRenderSystem : public ECSSystem<UIPostRenderSystem>
		{
		public:
			UIPostRenderSystem()
			{
				updateType = SystemUpdateType::Actor;
			}
			virtual ~UIPostRenderSystem()
			{
				//
			}

			void act(float _delta) override
			{
				D2D->getHwndRenderTarget()->EndDraw();
			}
			Direct2D* D2D;
		};
	}
}