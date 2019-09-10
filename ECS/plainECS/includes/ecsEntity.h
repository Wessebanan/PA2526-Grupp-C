#pragma once

#include <map>
#include <vector>
#include "ecsGlobals.h"
#include "ecsBaseComponent.h"

namespace ecs
{
	struct Entity
	{
		ID getID();

		// Non-templated
		ID getComponentID(TypeID _typeID);
		bool hasComponentOfType(TypeID _typeID);

		// Templated
		template <typename T> ID getComponentID();
		template <typename T> bool hasComponentOfType();

	private:
		ID id;
		std::map<TypeID, ID> componentIDs;

		friend class ECSEntityManager;
		friend class EntityComponentSystem;
	};

	struct FilteredEntity
	{
		Entity* entity;
		std::map<TypeID, BaseComponent*> components;

		template <typename T>
		T* getComponent() { return (components.count(T::typeID)) ? (T*)components[T::typeID] : nullptr; }
	};

	struct EntityIterator
	{
		std::vector<FilteredEntity> entities;
	};


	/*
	*	Templated functions
	*/

	template<typename T>
	inline ID Entity::getComponentID()
	{
		return ID();
	}
	template<typename T>
	inline bool Entity::hasComponentOfType()
	{
		return false;
	}
}