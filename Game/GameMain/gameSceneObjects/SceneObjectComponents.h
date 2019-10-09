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

			// places the filepath to eachfilepath
			bool AssignModel()
			{
				switch (this->mObject)
				{
				case SCENEOBJECT::VINES:
					this->mFilepath = "Models/vines.fbx";
					break;
				case SCENEOBJECT::MUD:
					this->mFilepath = "Models/mud.fbx";
					break;
				case SCENEOBJECT::WAGON:
					this->mFilepath = "Models/wagon.fbx";
					break;
				case SCENEOBJECT::CLIFF:
					this->mFilepath = "Models/mountain.fbx";
					break;
				case SCENEOBJECT::ROCKS:
					this->mFilepath = "Models/rocks.fbx";
					break;
				case SCENEOBJECT::CAMP:
					this->mFilepath = "Models/camp.fbx";
					break;
				case SCENEOBJECT::TREES:
					this->mFilepath = "Models/trees.fbx";
					break;
				case SCENEOBJECT::FLOWERS:
					this->mFilepath = "Models/flowers.fbx";
					break;
				case SCENEOBJECT::VILAGE:
					this->mFilepath = "Models/vilage.fbx";
					break;
				case SCENEOBJECT::SANDSTONE:
					this->mFilepath = "Models/sandstone.fbx";
					break;
				case SCENEOBJECT::PALMS:
					this->mFilepath = "Models/palms.fbx";
					break;
				case SCENEOBJECT::PYRAMIDS:
					this->mFilepath = "Models/pyramids.fbx";
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