#include "CollisionObject.h"

CollisionObject::~CollisionObject()
{
	delete mNext; 
	mNext = nullptr;
}
