#include <iostream>
#include "ecs.h"

#ifdef _DEBUG
	#define _crtdbg_map_alloc
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

using namespace std;
using namespace ecs;

#define ENTITY_COUNT 7

struct TestEvent : public ECSEvent<TestEvent>
{
	TestEvent(int _data) : data(_data) {};
	int data;
};

class EventBroadcasterSystem : public ECSSystem<EventBroadcasterSystem>
{
public:
	EventBroadcasterSystem()
	{
		updateType = Actor;
	}
	virtual ~EventBroadcasterSystem() {}

	void act(float _delta) override
	{
		TestEvent e = TestEvent(1001);
		for (int i = 0; i < 9; i++)
		{
			createEvent(e);
			e.data += 1001;
		}
	}
};

class EventReadingSystem : public ECSSystem<EventReadingSystem>
{
public:
	EventReadingSystem()
	{
		updateType = EventReader;
		typeFilter.addRequirement(TestEvent::typeID);
	}
	virtual ~EventReadingSystem() {}

	void readEvent(BaseEvent& _event, float _delta) override
	{
		cout << "\t\t\t[" << __nameof<EventReadingSystem>() << "] Reading event data: " << static_cast<TestEvent*>(&_event)->data << "\n";
	}
};

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	EntityComponentSystem ecs;
	ecs.createSystem<EventBroadcasterSystem>(0);
	ecs.createSystem<EventReadingSystem>(1);

	ecs.update(0.5f);
	getchar();

	//CompTypeMemDesc compDesc[] =
	//{
	//	{HealthComponent::typeID, HealthComponent::size, 10},
	//	{PoisonComponent::typeID, PoisonComponent::size, 10}
	//};

	//ECSDesc ecsDesc;
	//ecsDesc.compTypeMemDescs = compDesc;
	//ecsDesc.compTypeCount = 2;
	//ecsDesc.systemLayerCount = 3;

	//EntityComponentSystem myECS;
	////myECS.initialize(ecsDesc);
	//myECS.createSystem<systems::EntityKillCounter>(2);
	//myECS.createSystem<systems::PoisonSystem>();
	//myECS.createSystem<systems::EntityKillSystem>();
	//myECS.createSystem<systems::EventPrintingSystem>();

	//TypeID entities[ENTITY_COUNT];
	//HealthComponent health;
	//for (size_t i = 0; i < ENTITY_COUNT; i++)
	//{
	//	health.health = (float)i + 1;
	//	entities[i] = myECS.createEntity(health)->getID();
	//}

	//myECS.update(0.12345f);
	//cout << endl;

	//PoisonComponent poison; poison.tickDamage = 1.f;
	//for (size_t i = 0; i < ENTITY_COUNT; i++)
	//{
	//	myECS.createComponent(entities[i], poison);
	//}
	////system("pause");

	//myECS.update(0.12345f);
	//system("pause");

	return 0;
}