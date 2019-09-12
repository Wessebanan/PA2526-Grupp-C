#pragma once

#ifndef HANDLEINPUTSYSTEM_H
#define HANDLEINPUTSYSTEM_H

#include "InputSystems.h"
#include "InputComponents.h"

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

		class HandleInputSystem : public ecs::ECSSystem<HandleInputSystem>
		{
		public:
			HandleInputSystem();
			~HandleInputSystem();


			void updateEntity(FilteredEntity& _entityInfo, float _delta) override;
		private:
		};
	}
}




#endif // !HANDLEINPUTSYSTEM_H