#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class BoundingVolume
{
public:
	BoundingVolume();
	virtual ~BoundingVolume();

	virtual void Generate(XMFLOAT3* points, const unsigned int size) = 0;

	virtual bool IntersectSphere()	= 0;
	virtual bool IntersectOBB()		= 0;
	virtual bool IntersectAABB()	= 0;
private:

};

class Sphere : public BoundingVolume
{
public:
	Sphere() {};
	~Sphere() {};
	void Generate(XMFLOAT3* points, const unsigned int size);

	bool IntersectSphere();
	bool IntersectOBB();
	bool IntersectAABB();
private:
	XMFLOAT3 mCenter;
	float mRadius;
};

class OBB : public BoundingVolume
{

};

class AABB : public BoundingVolume
{
public:
	AABB() {};
	~AABB() {};
	void Generate(XMFLOAT3* points, const unsigned int size);

	bool IntersectSphere();
	bool IntersectOBB();  
	bool IntersectAABB();
		
private:
	XMFLOAT3 mMin;
	XMFLOAT3 mMax;
	XMFLOAT3 mCenter;
};