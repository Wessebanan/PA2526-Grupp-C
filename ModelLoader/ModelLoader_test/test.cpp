#include "pch.h"
#include <Mesh.h>



int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// Check that importing returns E_FAIL with bad filepath
TEST(FailureTesting, BadFilePath) {
	ModelLoader::Mesh testMesh;
	EXPECT_EQ(testMesh.LoadFBX("erhhherhjn.w4e4e9"), E_FAIL);
}

// Check that the vectors are nullptr if importing fails
TEST(FailureTesting, GetsReturningNullptr) {
	ModelLoader::Mesh testMesh;
	testMesh.LoadFBX("eherjherh.q39ut");
	EXPECT_TRUE(testMesh.GetVertexPositionVector() == nullptr);
	EXPECT_TRUE(testMesh.GetIndexVector() == nullptr);
	EXPECT_TRUE(testMesh.GetUVVector() == nullptr);
	EXPECT_TRUE(testMesh.GetNormalVector() == nullptr);
}

// Check so that importing a mesh without UVs has the expected results
TEST(FailureTesting, MeshWithoutUVs)
{
	ModelLoader::Mesh testMesh;
	ASSERT_TRUE(SUCCEEDED(testMesh.LoadFBX("PlaneWithoutUV.fbx")));
	// Mesh should return false for HasUVs and the UV vector should be nullptr
	EXPECT_FALSE(testMesh.HasUVs());
	EXPECT_TRUE(testMesh.GetUVVector() == nullptr);
}

// Check that LoadFBX returns success if provided a valid file
TEST(SuccessTesting, ImportingValidMesh) {
	ModelLoader::Mesh testMesh;
	EXPECT_TRUE(SUCCEEDED(testMesh.LoadFBX("cat.fbx")));
}

TEST(SuccessTesting, FilePathInConstructor) {
	ModelLoader::Mesh testMesh("cat.fbx");
	EXPECT_FALSE(testMesh.GetVertexPositionVector() == nullptr);
	EXPECT_FALSE(testMesh.GetIndexVector() == nullptr);
	EXPECT_FALSE(testMesh.GetUVVector() == nullptr);
	EXPECT_FALSE(testMesh.GetNormalVector() == nullptr);
}

// Check that vertex imported correctly
TEST(SuccessTesting, CorrectVertexPos) {
	ModelLoader::Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first vertex is supposed to be -71.3173523
	EXPECT_TRUE(testMesh.GetVertexPositionVector()->front().x - -71.3173523f < 0.0001f);
}

// Check that normal imported correctly
TEST(SuccessTesting, CorrectNormal) {
	ModelLoader::Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first Normal is supposed to be 0.982920110
	EXPECT_TRUE(testMesh.GetNormalVector()->front().x - 0.982920110 < 0.0001f);
}

// Check that UV imported correctly
TEST(SuccessTesting, CorrectUV) {
	ModelLoader::Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first UV is supposed to be 0.205933005
	EXPECT_TRUE(testMesh.GetUVVector()->front().x - 0.205933005 < 0.0001f);
}

// Check that index imported correctly
TEST(SuccessTesting, CorrectIndex) {
	ModelLoader::Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// First index is supposed to be 321
	EXPECT_EQ(testMesh.GetIndexVector()->front(), 321);
}

// Check that vertex imported correctly
TEST(SuccessTesting, CorrectVertexPos_FilePathConstructor) {
	ModelLoader::Mesh testMesh("cat.fbx");
	// X component of first vertex is supposed to be -71.3173523
	EXPECT_TRUE(testMesh.GetVertexPositionVector()->front().x - -71.3173523f < 0.0001f);
}

// Check that normal imported correctly
TEST(SuccessTesting, CorrectNormal_FilePathConstructor) {
	ModelLoader::Mesh testMesh("cat.fbx");
	// X component of first Normal is supposed to be 0.982920110
	EXPECT_TRUE(testMesh.GetNormalVector()->front().x - 0.982920110 < 0.0001f);
}

// Check that UV imported correctly
TEST(SuccessTesting, CorrectUV_FilePathConstructor) {
	ModelLoader::Mesh testMesh("cat.fbx");
	// X component of first UV is supposed to be 0.205933005
	EXPECT_TRUE(testMesh.GetUVVector()->front().x - 0.205933005 < 0.0001f);
}

// Check that index imported correctly
TEST(SuccessTesting, CorrectIndex_FilePathConstructor) {
	ModelLoader::Mesh testMesh("cat.fbx");
	// First index is supposed to be 321
	EXPECT_EQ(testMesh.GetIndexVector()->front(), 321);
}

TEST(SkeletonDataSuccessTesting, CorrectSkeleton) {
	ModelLoader::Mesh testMesh("SkeletonTest.fbx");
	EXPECT_TRUE(testMesh.HasSkeleton());
	ModelLoader::Skeleton* test1 = testMesh.GetSkeleton();
	EXPECT_GT(test1->joints.size(), 0);
	EXPECT_GT(testMesh.GetSkinningWeights()->size(), 0);
	std::vector<ModelLoader::ControlPointInfo>* test2 = testMesh.GetSkinningWeights();
	// Vector should contain one entry per vertex in the mesh
	EXPECT_EQ(testMesh.GetSkinningWeights()->size(), testMesh.GetVertexPositionVector()->size());
}

// Loader should still support non-skinned models without any issue
TEST(SkeletonDataSuccessTesting, MeshWithoutSkeleton)
{
	ModelLoader::Mesh testMesh("cat.fbx");
	EXPECT_FALSE(testMesh.HasSkeleton());
	EXPECT_EQ(testMesh.GetSkeleton(), nullptr);
	EXPECT_EQ(testMesh.GetSkinningWeights(), nullptr);
}

// Failure tests use the internal LoadFBX function instead of the Mesh structure to check that the thrown errors are correct.
// The mesh structure normally intercepts these throws and outputs a message box.
TEST(SkeletonDataFailureTesting, NullptrInputVectors) {
	// Expect throw because input vectors are nullptr
	try
	{
		ModelLoader::LoadFBX("SkeletonTest.fbx", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		FAIL() << "LoadFBX should throw an error.\n"; // This line is only reached if above statement does not throw
	}
	catch (std::exception e)
	{
		EXPECT_EQ(std::string(e.what()), std::string("One or more input vectors to LoadFBX were nullptr."));
	}
	catch (...)
	{
		FAIL() << "Unknown exception thrown.\n";
	}
}

TEST(SkeletonDataFailureTesting, NonNormalizedSkeletonWeights) {
	std::vector<DirectX::XMFLOAT3>* pVertexPosVector = nullptr;
	std::vector<int>* pIndexVector = nullptr;
	std::vector<DirectX::XMFLOAT3>* pNormalVector = nullptr;
	std::vector<DirectX::XMFLOAT2>* pUVVector = nullptr;
	ModelLoader::Skeleton* pSkeleton = nullptr;
	std::vector<ModelLoader::ControlPointInfo>* pSkinningWeights = nullptr;
	pIndexVector = new std::vector<int>;
	pVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
	pNormalVector = new std::vector<DirectX::XMFLOAT3>;
	pUVVector = new std::vector<DirectX::XMFLOAT2>;
	pSkeleton = new ModelLoader::Skeleton();
	pSkinningWeights = new std::vector<ModelLoader::ControlPointInfo>;
	try
	{
		ModelLoader::LoadFBX("FailSkeletonNonNormalized.fbx", pVertexPosVector, pIndexVector, pNormalVector, pUVVector, pSkeleton, pSkinningWeights);
		FAIL() << "LoadFBX should throw an error.\n";
	}
	catch (std::exception e)
	{
		EXPECT_EQ(std::string(e.what()), std::string("Vertex weights do not add up to 1, please normalize the vertex weights."));

	}
	catch (...)
	{
		FAIL() << "Unknown exception thrown.\n";
	}
	delete pIndexVector;
	delete pVertexPosVector;
	delete pNormalVector;
	delete pUVVector;
	delete pSkeleton;
	delete pSkinningWeights;
}

TEST(SkeletonDataFailureTesting, TooManyWeightsPerVertex) {
	std::vector<DirectX::XMFLOAT3>* pVertexPosVector = nullptr;
	std::vector<int>* pIndexVector = nullptr;
	std::vector<DirectX::XMFLOAT3>* pNormalVector = nullptr;
	std::vector<DirectX::XMFLOAT2>* pUVVector = nullptr;
	ModelLoader::Skeleton* pSkeleton = nullptr;
	std::vector<ModelLoader::ControlPointInfo>* pSkinningWeights = nullptr;
	pIndexVector = new std::vector<int>;
	pVertexPosVector = new std::vector<DirectX::XMFLOAT3>;
	pNormalVector = new std::vector<DirectX::XMFLOAT3>;
	pUVVector = new std::vector<DirectX::XMFLOAT2>;
	pSkeleton = new ModelLoader::Skeleton();
	pSkinningWeights = new std::vector<ModelLoader::ControlPointInfo>;
	try
	{
		ModelLoader::LoadFBX("FailSkeletonTooManyWeights.fbx", pVertexPosVector, pIndexVector, pNormalVector, pUVVector, pSkeleton, pSkinningWeights);
		FAIL() << "LoadFBX should throw an error.\n";
	}
	catch (std::exception e)
	{
		EXPECT_EQ(std::string(e.what()), std::string("Mesh contains vertices with more than 4 bone weights. The engine does not support this."));

	}
	catch (...)
	{
		FAIL() << "Unknown exception thrown.\n";
	}
	delete pVertexPosVector;
	delete pIndexVector;
	delete pNormalVector;
	delete pUVVector;
	delete pSkeleton;
	delete pSkinningWeights;
}