#pragma once
#include "ecsSystemIncludes.h"
#include "InitInputComponents.h"
#include "InitInputBackendComponent.h"

namespace ecs
{
	namespace systems
	{
		// saves the keyboard
		class HandleKeyboardSystem : public ecs::ECSSystem<HandleKeyboardSystem>
		{
		public:
			HandleKeyboardSystem();
			~HandleKeyboardSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// svaes the mouses delta and keys 
		class HandleMouseSystem : public ecs::ECSSystem<HandleMouseSystem>
		{
		public:
			HandleMouseSystem();
			~HandleMouseSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// All buttons are saved
		class HandleWebSystem : public ecs::ECSSystem<HandleWebSystem>
		{
		public:
			HandleWebSystem();
			~HandleWebSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		// the backend in updated
		class HandleInputBackend : public ecs::ECSSystem<HandleInputBackend>
		{
		public:
			HandleInputBackend();
			~HandleInputBackend();


			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};
	}
}
