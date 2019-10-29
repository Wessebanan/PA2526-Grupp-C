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

				//this->mObject = (SCENE_OBJECT)((int)(this->mObjectType) + ((int)(bio) * 3));
				this->mObject = BIOME_OBJECTS[bio][this->mObjectType];

				this->AssignModel();
			}

			// places the filepath to eachfilepath
			bool AssignModel()
			{
				//switch (this->mObject)
				//{
				//case SCENE_OBJECT::SNOWMAN:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::ANGEL:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::IGLOO:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::CLIFF:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::ROCKS:
				//	this->mFilepath = "gameSceneObjects/models/rock.fbx";
				//	break;
				//case SCENE_OBJECT::CAMP:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::TREES:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::FLOWERS:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::VILAGE:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::SANDSTONE:
				//	this->mFilepath = "gameSceneObjects/models/rock.fbx";
				//	break;
				//case SCENE_OBJECT::PALMS:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//case SCENE_OBJECT::PYRAMIDS:
				//	this->mFilepath = "gameSceneObjects/models/tree.fbx";
				//	break;
				//default:
				//	 returns false if the sceneobject fails to assign
				//	return false;
				//	break;
				//}

				return true;
			};

			std::string GetFilepath() { return this->mFilepath; };
		private:
			std::string mFilepath = "";
		};
	}
}