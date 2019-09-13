#include <iostream>
#include "ecs.h"

using namespace std;
using namespace ecs;

#define ENTITY_COUNT 1

int main()
{
	CompTypeMemDesc compDesc[] =
	{
		{HealthComponent::typeID, HealthComponent::size, 10},
		{PoisonComponent::typeID, PoisonComponent::size, 10}
	};

	ECSDesc ecsDesc;
	ecsDesc.compTypeMemDescs = compDesc;
	ecsDesc.compTypeCount = 2;
	ecsDesc.systemLayerCount = 3;

	EntityComponentSystem myECS;
	//myECS.initialize(ecsDesc);
	myECS.createSystem<systems::EntityKillCounter>(2);
	myECS.createSystem<systems::PoisonSystem>();
	myECS.createSystem<systems::EntityKillSystem>();
	myECS.createSystem<systems::EventPrintingSystem>();

	TypeID entities[ENTITY_COUNT];
	HealthComponent health;
	for (size_t i = 0; i < ENTITY_COUNT; i++)
	{
		health.health = (float)i + 1;
		entities[i] = myECS.createEntity(health)->getID();
	}

	myECS.update(0.12345f);
	//system("pause");
	cout << endl;

	PoisonComponent poison; poison.tickDamage = 1.f;
	for (size_t i = 0; i < ENTITY_COUNT; i++)
	{
		myECS.createComponent(entities[i], poison);
	}
	//system("pause");

	myECS.update(0.12345f);
	system("pause");

	return 0;
}