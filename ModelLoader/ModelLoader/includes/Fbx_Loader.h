#pragma once
#include <fbxsdk.h>
#include <vector>
#include <d3d11.h>
#include <cassert>
#include <map>
#include <unordered_map>
#include <DirectXMath.h>
#include <memory>

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


	// Each vertex contains up to four IndexWeightPairs
	struct ControlPointInfo
	{
		IndexWeightPair weightPairs[4];
	};


	//This stores the information of each key frame of each Joint
	struct KeyFrame {
		FbxLongLong mFrameNum;
		FbxAMatrix mGlobalTransform;	//global transform not including bindpose inverse transform matrix
		FbxAMatrix mLocalTransform;
		FbxAMatrix mOffsetMatrix;		// final offset matrix, this is what needs to be sent to the GPU
	};

	struct Joint {
		std::string mName;
		int mParentIndex;	//index to its parent joint

		std::vector<KeyFrame> mAnimationVector;
		FbxNode* mNode;

		FbxAMatrix mGlobalBindposeInverse;
		FbxAMatrix mBoneGlobalTransform;
		FbxAMatrix mBoneLocalTransform;
		FbxAMatrix mOffsetMatrix;

		Joint() :
			mNode(nullptr)
		{
			mParentIndex = -1;
		}
	};

	struct Skeleton {
		std::vector<Joint> joints;
	};


	// Used for loading the very basics of an FBX
	// Input: std::string file name of FBX file, pointers to std::vectors to append the data to
	// Output: Appends XMFLOAT3 vertex positions and int indices to provided vectors
	HRESULT LoadFBX(const std::string& fileName, std::vector<DirectX::XMFLOAT3>* pOutVertexPosVector, std::vector<int>* pOutIndexVector,
		std::vector<DirectX::XMFLOAT3>* pOutNormalVector, std::vector<DirectX::XMFLOAT2>* pOutUVVector, Skeleton* pOutSkeleton, std::vector<ControlPointInfo>* pOutCPInfoVector);


}