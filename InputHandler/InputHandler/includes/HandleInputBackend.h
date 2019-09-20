#pragma once

#ifndef HANDLEInputBackend_H
#define HANDLEInputBackend_H

#include "InputBackend.h"
#include "InputComponents.h"
#include "InputBackendComponent.h"

namespace ecs
{
	namespace systems
	{
		class HandleKeyboardSystem : public ecs::ECSSystem<HandleKeyboardSystem>
		{
		public:
			HandleKeyboardSystem();
			~HandleKeyboardSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		class HandleMouseSystem : public ecs::ECSSystem<HandleMouseSystem>
		{
		public:
			HandleMouseSystem();
			~HandleMouseSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

		class HandleWebSystem : public ecs::ECSSystem<HandleWebSystem>
		{
		public:
			HandleWebSystem();
			~HandleWebSystem();
			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};

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




#endif // !HANDLEInputBackend_H