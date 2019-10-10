#pragma once
#include "ecsComponentIncludes.h"
#include "SceneObjectGlobals.h"

namespace ecs
{
	namespace components
	{
		struct SceneObjectComponent : public ECSComponent<SceneObjectComponent>
		{
		public:
			SCENEOBJECT mObject;
			OBJECTTYPE mObjectType;

			// change the object depending on the biome
			void ChangeModelByBiome(BIOME bio)
			{
				this->mObject = (SCENEOBJECT)((int)(this->mObjectType) + ((int)(bio) * 3));

				this->AssignModel();
			}

			// places the filepath to eachfilepath
			bool AssignModel()
			{
				switch (this->mObject)
				{
				case SCENEOBJECT::VINES:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::MUD:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::WAGON:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::CLIFF:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::ROCKS:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::CAMP:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::TREES:
					this->mFilepath = "gameSceneObjects/models/rock.fbx";
					break;
				case SCENEOBJECT::FLOWERS:
					this->mFilepath = "gameSceneObjects/models/rock.fbx";
					break;
				case SCENEOBJECT::VILAGE:
					this->mFilepath = "gameSceneObjects/models/rock.fbx";
					break;
				case SCENEOBJECT::SANDSTONE:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::PALMS:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				case SCENEOBJECT::PYRAMIDS:
					this->mFilepath = "gameSceneObjects/models/tree.fbx";
					break;
				default:
					// returns false if the sceneobject fails to assign
					return false;
					break;
				}

				return true;
			};

			std::string GetFilepath() { return this->mFilepath; };
		private:
			std::string mFilepath = "";
		};
	}
}