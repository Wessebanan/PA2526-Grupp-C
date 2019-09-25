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

// WARNING TO END USER --------------------!!!!!!!!!!!!!!!!----------------------
// If you are looking for the model loader, use ModelLoader::Mesh found in Mesh.h
// WARNING TO END USER --------------------!!!!!!!!!!!!!!!!----------------------

#define MAX_NUM_WEIGHTS_PER_VERTEX 4

namespace ModelLoader
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
		IndexWeightPair weightPairs[4];
	};

	struct Joint {
		FbxString jointName;
		int currentIndex;	//index of current joint	
		int parentIndex;	//index to its parent joint
		FbxAMatrix globalMatrix;
		FbxAMatrix localMatrix;
		FbxNode* mNode;

		FbxAMatrix mGlobalBindposeInverse;

		Joint() :
			mNode(nullptr)
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
	// Throws exceptions for failed loads, catch and read message for explanation.
	HRESULT LoadFBX(const std::string& fileName, std::vector<DirectX::XMFLOAT3>* pOutVertexPosVector, std::vector<int>* pOutIndexVector,
		std::vector<DirectX::XMFLOAT3>* pOutNormalVector, std::vector<DirectX::XMFLOAT2>* pOutUVVector, Skeleton* pOutSkeleton, std::vector<ControlPointInfo>* pOutCPInfoVector);

}