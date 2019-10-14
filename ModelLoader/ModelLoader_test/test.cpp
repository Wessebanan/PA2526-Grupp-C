#include "pch.h"
#include <Mesh.h>

int main(int argc, char** argv) {
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
	// First index is supposed to be 322
	EXPECT_EQ(testMesh.GetIndexVector()->front(), 322);
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
	// First index is supposed to be 322
	EXPECT_EQ(testMesh.GetIndexVector()->front(), 322);
}

TEST(AnimationTesting, SuccessfulAnimationLoad) {
	ModelLoader::Mesh testMesh("AnimTest.fbx");
	EXPECT_TRUE(testMesh.HasSkeleton());
	EXPECT_TRUE(testMesh.GetSkeleton() != nullptr);
	EXPECT_TRUE(testMesh.GetBlendWeights() != nullptr);
}

TEST(AnimationTesting, ModelWithNoAnimation) {
	ModelLoader::Mesh testMesh("cat.fbx");
	EXPECT_TRUE(testMesh.GetSkeleton() == nullptr);
	EXPECT_TRUE(testMesh.GetBlendWeights() == nullptr);
}

TEST(AnimationTesting, CorrectNumberOfJoints) {
	ModelLoader::Mesh testMesh("AnimTest.fbx");	
	ASSERT_TRUE(testMesh.HasSkeleton());
	EXPECT_EQ(testMesh.GetSkeleton()->joints.size(), 22);
}

TEST(AnimationTesting, CorrectNumberOfWeights) {
	ModelLoader::Mesh testMesh("AnimTest.fbx");
	ASSERT_TRUE(testMesh.HasSkeleton());
	EXPECT_EQ(testMesh.GetBlendIndices()->size(), 274 * 4);
	EXPECT_EQ(testMesh.GetBlendWeights()->size(), 274 * 3);
}