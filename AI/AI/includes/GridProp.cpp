#include "GridProp.h"

GridProp* GridProp::mInstance = nullptr;

GridProp::GridProp()
{

};

GridProp::~GridProp()
{

};

int2 GridProp::GetSize()
{
	return this->mMapSize;
};

void GridProp::SetSize(int rows, int columns)
{
	this->mMapSize.x = rows;
	this->mMapSize.y = columns;
};

GridProp* GridProp::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new GridProp();
	}
	return mInstance;
}