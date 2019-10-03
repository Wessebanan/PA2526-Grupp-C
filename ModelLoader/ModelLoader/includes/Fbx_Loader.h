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

namespace ModelLoader
{

	struct FbxVertex
	{
		float pos[3];
		float nor[3];
		float uv[2];
	};

	//This stores the information of each key frame of each Joint
	struct KeyFrame {
		FbxLongLong mFrameNum;
		FbxAMatrix mGlobalTransform;	//transform matrix
		FbxAMatrix mLocalTransform;
		FbxAMatrix mOffsetMatrix;
		KeyFrame* mNext;
		KeyFrame() :
			mNext(nullptr)
		{}
	};

	struct Joint {
		std::string mName;
		int mParentIndex;	//index to its parent joint
		KeyFrame* mAnimation;
		std::vector<KeyFrame> mAnimationVector;
		FbxNode* mNode;

		FbxAMatrix mGlobalBindposeInverse;
		FbxAMatrix mBoneGlobalTransform;
		FbxAMatrix mBoneLocalTransform;
		FbxAMatrix mOffsetMatrix;

		Joint() :
			mAnimation(nullptr),
			mNode(nullptr)
		{
			mParentIndex = -1;
		}
	};

	struct Skeleton {
		std::vector<Joint> joints;
	};

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

	// Used for loading the very basics of an FBX
	// Input: std::string file name of FBX file, pointers to std::vectors to append the data to
	// Output: Appends data to the provided vectors, returns HRESULT
	HRESULT LoadFBX(const std::string& fileName, std::vector<DirectX::XMFLOAT3>* pOutVertexPosVector, std::vector<int>* pOutIndexVector,
		std::vector<DirectX::XMFLOAT3>* pOutNormalVector, std::vector<DirectX::XMFLOAT2>* pOutUVVector, Skeleton* pOutSkeleton, std::vector<ControlPointInfo>* pOutCPInfoVector);

}