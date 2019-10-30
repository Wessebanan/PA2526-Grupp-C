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
			const SCENE_OBJECT BIOME_OBJECTS[BIOME_COUNT][OBJECT_TYPE_COUNT] =
			{
				{WINTERTREE, TOWER, BOX },
				{GIANTSKULL, CAGE, BOX},
				{FRUITTREE, BARREL, BOX},
				{CACTUS, COWSKULL, BARREL}
			};
		public:
			SCENE_OBJECT mObject;
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