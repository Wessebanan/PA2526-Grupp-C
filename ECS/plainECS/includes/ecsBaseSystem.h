#pragma once

#include <vector>
#include "ecsUser.h"
#include "ecsEventListener.h"
#include "ecsEntity.h"
#include "ecsEventIterator.h"
#include "ecsGlobals.h"
#include "ecsIDGenerator.h"
#include "ecsTypeFilter.h"

namespace ecs
{
	class BaseSystem;
	typedef void(*SystemFreeFunction)(BaseSystem* _systemPtr);

	enum SystemUpdateType { Undefined, EntityUpdate, MultiEntityUpdate, EventReader, EventListenerOnly };

	/*
	*	BaseSystem is a top level class in the system inheritance.
	*	This class handles all non-type specific information about the
	*	system, like the list of required component type IDs.
	*/

	class BaseSystem : public ECSUser, public ECSEventListener
	{
	public:
		BaseSystem() {}
		virtual ~BaseSystem() {}

		virtual TypeID getTypeID() { return 0; }
		virtual std::string getName() { return ""; }
		virtual SystemFreeFunction getFreeFunction() { return nullptr; }

		virtual void updateEntity(FilteredEntity &_entityInfo, float _delta) {}
		virtual void updateMultipleEntities(EntityIterator &_entities, float _delta) {}
		virtual void readEvent(BaseEvent &_event, float _delta) {}

	protected:

		SystemUpdateType updateType = Undefined;
		union
		{
			TypeFilter componentFilter;
			TypeFilter eventFilter;
		};
		TypeFilter eventListeningFilter;

		//static IDGenerator<TypeID> typeIDGenerator;
		static TypeID typeIDCounter;
		friend class EntityComponentSystem;
	};

	/*
	*	ECSSystem is the class all user systems will inherit from.
	*	This class handles all type specific information, like type ID.
	*/

	template <typename T>
	class ECSSystem : public BaseSystem
	{
	public:
		static const TypeID typeID;
		static const std::string name;
		static const SystemFreeFunction freeFunction;

		virtual TypeID getTypeID() { return T::typeID; }
		virtual std::string getName() { return T::name; }
		virtual SystemFreeFunction getFreeFunction() { return T::freeFunction; }

		ECSSystem() {}
		virtual ~ECSSystem() {}
	};

	/*
	*	Define a dynamic free function. This function will use the proper
	*	delete operator of the actual system type pointer. This could be done
	*	through virtual destructors, but then the user might forget
	*	to make the destructor virtual in the user written systems.
	*/

	template <typename T>
	void systemFree(BaseSystem* _systemPtr)
	{
		T* typedPtr = (T*)_systemPtr;
		delete typedPtr;
		typedPtr = nullptr;
	}

	/*
	*	Initialize all static variables
	*/

	template <typename T>
	const TypeID ECSSystem<T>::typeID(BaseSystem::typeIDCounter++);

	template <typename T>
	const std::string ECSSystem<T>::name(__nameof<T>());

	template <typename T>
	const SystemFreeFunction ECSSystem<T>::freeFunction(systemFree<T>);
}