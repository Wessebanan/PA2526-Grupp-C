#pragma once

#include <fbxsdk.h>
#include <vector>
#include <d3d11.h>
#include <cassert>
#include <map>
#include <unordered_map>
#include <DirectXMath.h>
#include <memory>

#pragma comment (lib, "libfbxsdk.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")

namespace FbxLoader
{

	struct FbxVertex
	{
		float pos[3];
		float nor[3];
		float uv[2];
	};

	//This stores the weight of each Control Point
	struct IndexWeightPair
	{
		unsigned int index;	//index of joint 
		double weight;		//weight of influence by the joint
		IndexWeightPair() :
			index(0), weight(0.0)
		{}
	};

	struct ControlPointInfo
	{
		FbxVector4 ctrlPoint;
		std::vector<IndexWeightPair> weightPairs;
	};

	//This stores the information of each key frame of each Joint
	struct KeyFrame {
		FbxLongLong frameNum;
		FbxAMatrix globalTransform;	//transform matrix
		KeyFrame* next;
		KeyFrame() :
			next(nullptr)
		{}
	};

	struct Joint {
		FbxString jointName;
		int currentIndex;	//index of current joint	
		int parentIndex;	//index to its parent joint
		FbxAMatrix globalMatrix;
		FbxAMatrix localMatrix;
		KeyFrame* animation;
		FbxNode* node;

		Joint() :
			animation(nullptr),
			node(nullptr)
		{
			localMatrix.SetIdentity();
			globalMatrix.SetIdentity();
			parentIndex = -1;
		}
	};

	struct Skeleton {
		std::vector<Joint> joints;
	};

	// Used for loading the very basics of an FBX
	// Input: std::string file name of FBX file, pointers to std::vectors to append the data to
	// Output: Appends data to the provided vectors, returns HRESULT
	HRESULT LoadFBX(const std::string& fileName, std::vector<DirectX::XMFLOAT3>* pOutVertexPosVector, std::vector<int>* pOutIndexVector,
		std::vector<DirectX::XMFLOAT3>* pOutNormalVector, std::vector<DirectX::XMFLOAT2>* pOutUVVector);

}