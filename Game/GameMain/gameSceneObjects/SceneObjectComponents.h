#pragma once
#include "ecsComponentIncludes.h"
#include "SceneObjectGlobals.h"

namespace ecs
{
	namespace components
	{
		struct SceneObjectComponent : public ECSComponent<SceneObjectComponent>
		{
		private:
			const GAME_OBJECT_TYPE BIOME_OBJECTS[BIOME_COUNT][OBJECT_TYPE_COUNT] =
			{
				{GAME_OBJECT_TYPE_WINTERTREE,	GAME_OBJECT_TYPE_TOWER,		GAME_OBJECT_TYPE_BOX },
				{GAME_OBJECT_TYPE_GIANTSKULL,	GAME_OBJECT_TYPE_CAGE,		GAME_OBJECT_TYPE_BOX},
				{GAME_OBJECT_TYPE_FRUITTREE,	GAME_OBJECT_TYPE_BARREL,	GAME_OBJECT_TYPE_BOX},
				{GAME_OBJECT_TYPE_CACTUS,		GAME_OBJECT_TYPE_COWSKULL,	GAME_OBJECT_TYPE_BARREL}
			};
		public:
			GAME_OBJECT_TYPE mObject;
			OBJECT_TYPE mObjectType;

			// change the object depending on the biome
			void ChangeModelByBiome(BIOME bio)
			{
				this->mObject = BIOME_OBJECTS[bio][this->mObjectType];
			}

			std::string GetFilepath() { return this->mFilepath; };
		private:
			std::string mFilepath = "";
		};
	}
}