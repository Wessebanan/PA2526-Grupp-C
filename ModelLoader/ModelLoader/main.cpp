#include <Mesh.h>
int main()
{
	ModelLoader::Mesh testMesh("DudeMesh4.fbx");
	ModelLoader::UniqueSkeletonData testUniqueData;
	testUniqueData.Init(testMesh.GetSkeleton());
	testUniqueData.UpdateAnimation(0.2f, ModelLoader::ANIMATION_TYPE::ATTACK2);
	return 0;
}