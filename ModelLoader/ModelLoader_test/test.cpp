#include "pch.h"
#include <Mesh.h>

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

// Check that importing returns E_FAIL with bad filepath
TEST(FailureTesting, BadFilePath) {
	Mesh testMesh;
	EXPECT_EQ(testMesh.LoadFBX("erhhherhjn.w4e4e9"), E_FAIL);
}

// Check that the vectors are nullptr if importing fails
TEST(FailureTesting, GetsReturningNullptr) {
	Mesh testMesh;
	testMesh.LoadFBX("eherjherh.q39ut");
	EXPECT_TRUE(testMesh.GetVertexPositionVector() == nullptr);
	EXPECT_TRUE(testMesh.GetIndexVector() == nullptr);
	EXPECT_TRUE(testMesh.GetUVVector() == nullptr);
	EXPECT_TRUE(testMesh.GetNormalVector() == nullptr);
}

// Check that LoadFBX returns success if provided a valid file
TEST(SuccessTesting, ImportingValidMesh) {
	Mesh testMesh;
	EXPECT_TRUE(SUCCEEDED(testMesh.LoadFBX("cat.fbx")));
}

// Check that vertex imported correctly
TEST(SuccessTesting, CorrectVertexPos) {
	Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first vertex is supposed to be -71.3173523
	EXPECT_TRUE(testMesh.GetVertexPositionVector()->front().x - -71.3173523f < 0.0001f);
}

TEST(SuccessTesting, CorrectNormal) {
	Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first Normal is supposed to be 0.982920110
	EXPECT_TRUE(testMesh.GetNormalVector()->front().x - 0.982920110 < 0.0001f);
}

TEST(SuccessTesting, CorrectUV) {
	Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// X component of first UV is supposed to be 0.205933005
	EXPECT_TRUE(testMesh.GetUVVector()->front().x - 0.205933005 < 0.0001f);
}

TEST(SuccessTesting, CorrectIndex) {
	Mesh testMesh;
	testMesh.LoadFBX("cat.fbx");
	// First index is supposed to be 321
	EXPECT_EQ(testMesh.GetIndexVector()->front(), 321);
}