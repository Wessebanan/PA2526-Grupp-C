#include "Fbx_Loader.h"

// Anonymous namespace for Fbx_Loader
namespace {
	static FbxManager* gpFbxSdkManager = nullptr;

	unsigned int FindJointIndexUsingName(const std::string& inJointName, ModelLoader::Skeleton* skeleton)
	{
		for (unsigned int i = 0; i < skeleton->joints.size(); ++i)
		{
			if (skeleton->joints[i].mName == inJointName)
			{
				return i;
			}
		}

		throw std::exception("Skeleton information in FBX file is corrupted or invalid.");
	}

	DirectX::XMFLOAT4X4 FbxAMatrixToXMFLOAT4X4(FbxAMatrix* toConvert)
	{
		DirectX::XMFLOAT4X4 new_mat;
		// Convert FbxMatrix to XMFLOAT
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				new_mat.m[i][j] = static_cast<float>(toConvert->Get(i, j));
			}
		}
		return new_mat;
	}

	FbxMesh* FindMesh(FbxNode* currentNode)
	{
		FbxMesh* mesh = nullptr;
		if (mesh = currentNode->GetMesh())
		{
			return mesh;
		}
		else
		{
			for (int i = 0; i < currentNode->GetChildCount(); ++i)
			{
				FbxNode* child = currentNode->GetChild(i);
				if (mesh = FindMesh(child))
				{
					return mesh;
				}
			}
			return nullptr;
		}
	}

	void LoadUV(fbxsdk::FbxMesh* pMesh, std::vector<DirectX::XMFLOAT2>* pOutUVVector)
	{
		fbxsdk::FbxStringList uv_set_name_list;
		pMesh->GetUVSetNames(uv_set_name_list);

		// Only supporting one UV set, get the first in the list
		const char* uv_set_name = uv_set_name_list.GetStringAt(0);
		fbxsdk::FbxGeometryElementUV* uv_element = pMesh->GetElementUV(uv_set_name);

		// if no UV found
		if (!uv_element)
			return;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (uv_element->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			uv_element->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;
		// if mode is not direct, it is index based
		const bool use_index = uv_element->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int index_count = (use_index) ? uv_element->GetIndexArray().GetCount() : 0;

		const int poly_count = pMesh->GetPolygonCount();
		DirectX::XMFLOAT2 uv;

		// If uvs are stored per polygon vertex we need to convert them to per control point
		if (uv_element->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			// Store a list of control points we've already processed
			std::map<int, bool> processed_cp;
			fbxsdk::FbxArray<FbxVector2> new_uvs;
			FbxArray<int> new_index_to_direct;

			for (int i = 0; i < pMesh->GetControlPointsCount(); ++i)
				processed_cp.insert_or_assign(i, false);
			// Access control points by polygon
			for (int polygon = 0; polygon < pMesh->GetPolygonCount(); ++polygon)
			{
				// Access control points by polygon vertex
				for (int polygon_vertex = 0; polygon_vertex < pMesh->GetPolygonSize(polygon); ++polygon_vertex)
				{
					int control_point = pMesh->GetPolygonVertex(polygon, polygon_vertex);
					// If control point not already processed
					if (processed_cp.find(control_point)->second == false)
					{
						FbxVector2 uv = uv_element->GetDirectArray().GetAt(control_point);
						new_uvs.Add(uv);
						new_index_to_direct.Add(control_point);
						// Add point to list of processed points
						processed_cp.insert_or_assign(control_point, true);
					}
				}
			}
			// Create the new index array
			uv_element->SetMappingMode(FbxLayerElement::eByControlPoint);
			uv_element->GetIndexArray().Clear();
			uv_element->GetIndexArray().Resize(new_index_to_direct.GetCount());
			int* index_array = (int*)uv_element->GetIndexArray().GetLocked();
			for (int i = 0; i < new_index_to_direct.GetCount(); i++)
			{
				index_array[i] = new_index_to_direct.GetAt(i);
			}
			uv_element->GetIndexArray().Release((void**)& index_array);

			// Create the new direct array
			uv_element->GetDirectArray().Clear();
			uv_element->GetDirectArray().Resize(new_uvs.GetCount());
			FbxVector2* direct_array = (FbxVector2*)uv_element->GetDirectArray().GetLocked();
			for (int j = 0; j < new_uvs.GetCount(); ++j)
			{
				direct_array[j] = new_uvs.GetAt(j);
			}
			uv_element->GetDirectArray().Release((void**)& direct_array);
		}
		// Process UVs
		if (uv_element->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{

			for (int i = 0; i < pMesh->GetControlPointsCount(); ++i)
			{
				// Different type of indexing in vector based on reference mode
				switch (uv_element->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					uv.x = (float)uv_element->GetDirectArray().GetAt(i).mData[0];
					uv.y = (float)uv_element->GetDirectArray().GetAt(i).mData[1];
					break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int index = uv_element->GetIndexArray().GetAt(i);
					uv.x = (float)uv_element->GetDirectArray().GetAt(index).mData[0];
					uv.y = (float)uv_element->GetDirectArray().GetAt(index).mData[1];
					break;
				}
				default:
					throw std::exception("Invalid Fbx UV Reference");
				}
				pOutUVVector->push_back(uv);
			}
		}
	}

	void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int myIndex, int inParentIndex, ModelLoader::Skeleton* inSkeleton)
	{
		if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType()
			&& inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			ModelLoader::Joint curr_joint;
			curr_joint.mParentIndex = inParentIndex;
			curr_joint.mName = inNode->GetName();
			inSkeleton->joints.push_back(curr_joint);
		}
		for (int i = 0; i < inNode->GetChildCount(); ++i)
		{
			ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), (int)inSkeleton->joints.size(), myIndex, inSkeleton);
		}
	}

	// Traverse the Fbx and populate the Skeleton struct with the joints inside
	void ProcessSkeletonHierarchy(FbxNode* inRootNode, ModelLoader::Skeleton* inSkeleton)
	{
		for (int child_index = 0; child_index < inRootNode->GetChildCount(); ++child_index)
		{
			FbxNode* curr_node = inRootNode->GetChild(child_index);
			ProcessSkeletonHierarchyRecursively(curr_node, 0, -1, inSkeleton);
		}
	}

	void CheckSumOfWeights(std::vector<ModelLoader::ControlPointInfo>* jointData) {
		for (auto it = jointData->begin(); it != jointData->end(); ++it)
		{
			double sum_of_weights = 0.0;
			for (unsigned int i = 0; i < 4; ++i)
			{
				sum_of_weights += it->weightPairs[i].weight;
			}
			if ((sum_of_weights - 1.0) > 0.0005 || (sum_of_weights - 1.0) < -0.0005)
			{
				throw std::exception("Vertex weights do not add up to 1, please normalize the vertex weights.");
			}
		}
	}

	void ProcessJointsAndAnimations(FbxNode* inNode, ModelLoader::Skeleton* skeleton, std::vector<ModelLoader::ControlPointInfo>* jointData)
	{
		FbxMesh* curr_mesh = inNode->GetMesh();
		if (curr_mesh)
		{
			unsigned int num_deformers = curr_mesh->GetDeformerCount();
			if (num_deformers > 1)
			{
				throw std::exception("Mesh contains more than one deformer, the engine does not support this.");
			}
			// geometry transform is potentially something included with the model
			// not all modeling programs provide this
			// including it for safety because I don't know if blender does or not
			//FbxAMatrix geometry_transform = GetGeometryTransformation(inNode);
			std::vector<std::vector<ModelLoader::IndexWeightPair>> temp(jointData->size(), std::vector<ModelLoader::IndexWeightPair>());

			//unsigned long long animation_length = 0;

			// Loop through deformers
			// Deformer is basically a skeleton
			// Most likely only one exists in the mesh
			for (unsigned int deformer_index = 0; deformer_index < num_deformers; ++deformer_index)
			{
				FbxSkin* curr_skin = static_cast<FbxSkin*>(curr_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));

				if (!curr_skin)
				{
					continue;
				}
				// Clusters contain links, which are basically joints
				unsigned int num_of_clusters = curr_skin->GetClusterCount();
				FbxNode* boneRootNode = curr_skin->GetCluster(0)->GetLink();
				skeleton->joints[0].mBoneGlobalTransform = boneRootNode->EvaluateGlobalTransform();
				// -----------------------------------------------
				// Process joints
				// -----------------------------------------------
				for (unsigned int cluster_index = 0; cluster_index < num_of_clusters; ++cluster_index)
				{
					// Collect info about the current joint
					FbxCluster* curr_cluster = curr_skin->GetCluster(cluster_index);
					std::string curr_joint_name = curr_cluster->GetLink()->GetName();
					unsigned int curr_joint_index = FindJointIndexUsingName(curr_joint_name, skeleton);
					ModelLoader::Joint* curr_joint = &skeleton->joints[curr_joint_index];
					skeleton->joints[curr_joint_index].mNode = curr_cluster->GetLink();


					// Get index weight pairs - https://www.gamedev.net/articles/programming/graphics/how-to-work-with-fbx-sdk-r3582
					// https://github.com/Larry955/FbxParser/blob/master/FbxParser/FbxParser.cpp - Row 781 & 993

					// Fbx has every joint store the vertices it affects, we need to reverse this relationship
					unsigned int num_of_indices = curr_cluster->GetControlPointIndicesCount();
					for (unsigned int i = 0; i < num_of_indices; ++i)
					{
						ModelLoader::IndexWeightPair curr_index_weight_pair;
						curr_index_weight_pair.index = curr_joint_index;
						curr_index_weight_pair.weight = curr_cluster->GetControlPointWeights()[i];
						temp[curr_cluster->GetControlPointIndices()[i]].push_back(curr_index_weight_pair);
						curr_joint->mConnectedVertexIndices.push_back(curr_joint_index);
					}
					int anim_stack_count = inNode->GetScene()->GetSrcObjectCount<FbxAnimStack>();
					FbxAnimStack* curr_anim_stack = FbxCast<FbxAnimStack>(inNode->GetScene()->GetSrcObject<FbxAnimStack>());
				}

				// -----------------------------------------------
				// Process Animations
				// -----------------------------------------------
				int anim_stack_count = inNode->GetScene()->GetSrcObjectCount<FbxAnimStack>();

				// Before we calculate any offset matrices we need to find the TPOSE and calculate the bindpose inverse matrices
				bool has_tpose = false;
				// Find the TPOSE animation and calculate matrices
				for (int anim_stack_index = 0; anim_stack_index < anim_stack_count && !has_tpose; ++anim_stack_index)
				{
					FbxAnimStack* curr_anim_stack = FbxCast<FbxAnimStack>(inNode->GetScene()->GetSrcObject<FbxAnimStack>(anim_stack_index));
					FbxString anim_stack_name = curr_anim_stack->GetName();
					std::string animation_name = anim_stack_name.Buffer();
					std::string ending = animation_name.substr(animation_name.length() - 5);
					// convert to uppercase in case animator forgot
					std::transform(ending.begin(), ending.end(), ending.begin(), ::toupper);
					if (ending == std::string("TPOSE"))
					{
						has_tpose = true;
						for (unsigned int cluster_index = 0; cluster_index < num_of_clusters; ++cluster_index)
						{
							// Collect info about the current joint
							FbxCluster* curr_cluster = curr_skin->GetCluster(cluster_index);
							std::string curr_joint_name = curr_cluster->GetLink()->GetName();
							unsigned int curr_joint_index = FindJointIndexUsingName(curr_joint_name, skeleton);
							ModelLoader::Joint* curr_joint = &skeleton->joints[curr_joint_index];

							// Evaluate the baseline global transform for the joint at t = 0 and create the global bindpose inverse matrix
							FbxDouble3 rotInf = curr_cluster->GetLink()->LclRotation.EvaluateValue(FBXSDK_TIME_INFINITE);
							FbxDouble3 translInf = curr_cluster->GetLink()->LclTranslation.EvaluateValue(FBXSDK_TIME_INFINITE);
							FbxAMatrix bone_local_transform;
							bone_local_transform = FbxAMatrix(translInf, rotInf, FbxVector4(1.0f, 1.0f, 1.0f));

							if (curr_joint_index == 0)
							{
								curr_joint->mBoneGlobalTransform = bone_local_transform;
							}
							else
							{
								// FbxAMatrix performs matrix multiplication in REVERSE order, M1 * M2 is multiplied with M2 from the left
								curr_joint->mBoneGlobalTransform = skeleton->joints[skeleton->joints[curr_joint_index].mParentIndex].mBoneGlobalTransform * bone_local_transform;

							}
							curr_joint->mGlobalBindposeInverse = curr_joint->mBoneGlobalTransform.Inverse() * FbxAMatrix(FbxVector4(0.0f, 0.0f, 0.0f), FbxVector4(-90.0f, 0.0f, 0.0f), FbxVector4(1.0f, -1.0f, 1.0f));
							curr_joint->mXMFLOATGlobalBindposeInverse = FbxAMatrixToXMFLOAT4X4(&curr_joint->mGlobalBindposeInverse);
						}
					}

				}
				// If the mesh is animated but does not contain a TPOSE animation, throw an error
				if (anim_stack_count > 0 && !has_tpose)
				{
					throw std::exception("Animated mesh does not have a 1 frame tpose animation/action named \"TPOSE\", please create this animation.");
				}
				std::vector<ModelLoader::AnimationSet> tempAnimationSetsVector;
				tempAnimationSetsVector.reserve(anim_stack_count - 1);

				// Process each animation stack in the file
				for (int anim_stack_index = 0; anim_stack_index < anim_stack_count; ++anim_stack_index)
				{
					FbxAnimStack* curr_anim_stack = FbxCast<FbxAnimStack>(inNode->GetScene()->GetSrcObject<FbxAnimStack>(anim_stack_index));
					FbxString anim_stack_name = curr_anim_stack->GetName();
					std::string animation_name = anim_stack_name.Buffer();
					std::string ending = animation_name.substr(animation_name.length() - 5);
					// convert to uppercase in case animator forgot
					std::transform(ending.begin(), ending.end(), ending.begin(), ::toupper);
					// Skip the TPOSE "animation", we don't want it as a useable animation
					if (ending == std::string("TPOSE"))
					{
						continue;
					}

					// Get animation information
					FbxTime start = curr_anim_stack->GetLocalTimeSpan().GetStart();
					FbxTime end = curr_anim_stack->GetLocalTimeSpan().GetStop();
					unsigned long long animation_length = (unsigned long long)(end.GetFrameCount(FbxTime::eFrames60) - start.GetFrameCount(FbxTime::eFrames60) + 1);

					// Create the AnimationSet for this animation
					ModelLoader::AnimationSet tempAnimSet;
					tempAnimSet.frameCount = (unsigned int)animation_length;
					tempAnimSet.animationName = animation_name;

					std::vector<DirectX::XMFLOAT4X4> tempJointOffsetMatrixVector;

					// bindposes have already been calculated at this point, we just need the offset matrices for each joint
					for (unsigned int cluster_index = 0; cluster_index < num_of_clusters; ++cluster_index)
					{
						// Collect info about the current joint
						FbxCluster* curr_cluster = curr_skin->GetCluster(cluster_index);
						std::string curr_joint_name = curr_cluster->GetLink()->GetName();
						unsigned int curr_joint_index = FindJointIndexUsingName(curr_joint_name, skeleton);
						ModelLoader::Joint* curr_joint = &skeleton->joints[curr_joint_index];
						skeleton->joints[curr_joint_index].mNode = curr_cluster->GetLink();

						// Pre-reserve slots in the vector for the keyframes
						curr_joint->mAnimationVector.reserve(animation_length); // LEGACY CODE

						unsigned int loopCounter = 0;
						for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames60); i <= end.GetFrameCount(FbxTime::eFrames60); ++i)
						{
							ModelLoader::KeyFrame current_keyframe;

							FbxTime curr_time;
							curr_time.SetFrame(i, FbxTime::eFrames60);

							current_keyframe.mFrameNum = i;

							// Evaluate the local transform of the joint at the current time
							inNode->GetScene()->SetCurrentAnimationStack(curr_anim_stack);
							FbxDouble3 rot = curr_cluster->GetLink()->LclRotation.EvaluateValue(curr_time, true);
							FbxDouble3 transl = curr_cluster->GetLink()->LclTranslation.EvaluateValue(curr_time, true);
							current_keyframe.mLocalTransform = FbxAMatrix(transl, rot, FbxVector4(1.0f, 1.0f, 1.0f));
							// If joint is root, use local transform as global
							// else, multiply with parent global first
							if (curr_joint_index == 0)
							{
								current_keyframe.mGlobalTransform = current_keyframe.mLocalTransform;
							}
							else
							{
								// FbxAMatrix performs matrix multiplication in REVERSE order, M1 * M2 is multiplied with M2 from the left
								current_keyframe.mGlobalTransform = skeleton->joints[skeleton->joints[curr_joint_index].mParentIndex].mAnimationVector[loopCounter].mGlobalTransform * current_keyframe.mLocalTransform;
							}
							FbxAMatrix offset_matrix;
							// FbxAMatrix performs matrix multiplication in REVERSE order, M1 * M2 is multiplied with M2 from the left
							offset_matrix = FbxAMatrix(FbxVector4(0.0f, 0.0f, 0.0f), FbxVector4(-90.0f, 0.0f, 0.0f), FbxVector4(1.0f, -1.0f, 1.0f)) * (current_keyframe.mGlobalTransform * curr_joint->mGlobalBindposeInverse);
							// Matrix needs to be transposed before sending to the GPU
							current_keyframe.mOffsetMatrix = FbxAMatrixToXMFLOAT4X4(&(offset_matrix.Transpose())); // LEGACY
							curr_joint->mAnimationVector.push_back(current_keyframe); // LEGACY

							tempJointOffsetMatrixVector.push_back(FbxAMatrixToXMFLOAT4X4(&(offset_matrix.Transpose())));
							skeleton->joints[curr_joint_index].mAnimationVector[loopCounter].mGlobalTransform = current_keyframe.mGlobalTransform;
							loopCounter++;
						}
					}

					// Fill the Animation Set with the collected data
					tempAnimSet.animationData = new DirectX::XMFLOAT4X4[animation_length * num_of_clusters];
					for (unsigned int frame_index = 0; frame_index < (unsigned int)animation_length; ++frame_index)
					{
						for (unsigned int joint_index = 0; joint_index < num_of_clusters; ++joint_index)
						{
							tempAnimSet.animationData[frame_index * num_of_clusters + joint_index] = tempJointOffsetMatrixVector[joint_index * animation_length + frame_index];
						}
					}
					tempAnimationSetsVector.push_back(tempAnimSet);
				}
				// Save the processed animation data in the skeleton
				for (int i = 0; i < tempAnimationSetsVector.size(); ++i)
				{
					skeleton->animations[i] = tempAnimationSetsVector[i];
					skeleton->animationFlags[i] = 0;
				}
			}
			skeleton->jointCount = (unsigned int)skeleton->joints.size();
			skeleton->frameData = new DirectX::XMFLOAT4X4[skeleton->jointCount];
			// Check if any vertex has more than 4 weights assigned
			for (unsigned int i = 0; i < temp.size(); ++i)
			{
				for (unsigned int frame_index = 0; frame_index < temp[i].size(); ++frame_index)
				{
					(*jointData)[i].weightPairs[frame_index] = temp[i][frame_index];
					if (frame_index >= MAX_NUM_WEIGHTS_PER_VERTEX)
					{
						throw std::exception("Mesh contains vertices with more than 4 bone weights. The engine does not support this.");
					}
				}
			}
			// Check so that the vertex weights add up to one
			CheckSumOfWeights(jointData);
		}
	}
}

HRESULT ModelLoader::LoadFBX(const std::string& fileName, std::vector<DirectX::XMFLOAT3>* pOutVertexPosVector, std::vector<int>* pOutIndexVector,
	std::vector<DirectX::XMFLOAT3>* pOutNormalVector, std::vector<DirectX::XMFLOAT2>* pOutUVVector, ModelLoader::Skeleton* pOutSkeleton, std::vector<ModelLoader::ControlPointInfo>* pOutCPInfoVector)
{
	if (!pOutVertexPosVector || !pOutIndexVector || !pOutNormalVector || !pOutUVVector || !pOutSkeleton || !pOutCPInfoVector)
	{
		throw std::exception("One or more input vectors to LoadFBX were nullptr.");
	}
	// Create the FbxManager if it does not already exist
	if (gpFbxSdkManager == nullptr)
	{
		gpFbxSdkManager = FbxManager::Create();

		FbxIOSettings* p_io_settings = FbxIOSettings::Create(gpFbxSdkManager, IOSROOT);
		gpFbxSdkManager->SetIOSettings(p_io_settings);
	}
	// Create Importer
	FbxImporter* p_importer = FbxImporter::Create(gpFbxSdkManager, "");

	// Create scene
	auto scene_deleter = [](FbxScene* scene) {scene->Destroy(); };
	std::unique_ptr<FbxScene, decltype(scene_deleter)> p_fbx_scene(FbxScene::Create(gpFbxSdkManager, ""), scene_deleter);

	// Import model
	bool b_success = p_importer->Initialize(fileName.c_str(), -1, gpFbxSdkManager->GetIOSettings());

	//FbxAxisSystem direct_x_axis_system(FbxAxisSystem::eDirectX);
	//if (p_fbx_scene.get()->GetGlobalSettings().GetAxisSystem() != direct_x_axis_system)
	//{
	//	direct_x_axis_system.ConvertScene(p_fbx_scene.get());
	//}
	// Handle failed import
	if (!b_success)
	{
		FbxString error = p_importer->GetStatus().GetErrorString();
		OutputDebugStringA("error: Call to FbxImporter::Initialize() failed.\n");

		char buffer[100];
		sprintf_s(buffer, "error: Error returned: %s\n", error.Buffer());
		OutputDebugStringA(buffer);
		// Error checking, print to visual studio debug window
		if (p_importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion) {
			int lFileMajor, lFileMinor, lFileRevision;
			int lSDKMajor, lSDKMinor, lSDKRevision;
			// Get the file version number generate by the FBX SDK.
			FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
			p_importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
			sprintf_s(buffer, "error: FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			OutputDebugStringA(buffer);

			sprintf_s(buffer, "error: FBX file format version for file '%s' is %d.%d.%d\n", fileName.c_str(), lFileMajor, lFileMinor,
				lFileRevision);
			OutputDebugStringA(buffer);
		}
		sprintf_s(buffer, "error: Failed to initialize the importer for '%s'.\n", fileName.c_str());
		OutputDebugStringA(buffer);
		return E_FAIL;;
	}

	b_success = p_importer->Import(p_fbx_scene.get());
	if (!b_success) return E_FAIL;

	// Importer is no longer needed, remove from memory
	p_importer->Destroy();


	FbxNode* p_fbx_root_node = p_fbx_scene->GetRootNode();

	if (p_fbx_root_node)
	{
		::ProcessSkeletonHierarchy(p_fbx_root_node, pOutSkeleton);

		FbxNode* root_child = p_fbx_scene->GetRootNode();
		FbxMesh* p_mesh = ::FindMesh(p_fbx_root_node);
		if (p_mesh)
		{

			// Make sure the mesh is triangulated
			if (!p_mesh->IsTriangleMesh())
			{
				throw std::exception("Mesh contains non-triangles, please triangulate the mesh.");
			}

			FbxVector4* p_vertices = p_mesh->GetControlPoints();

			// Populate the returned index vector
			int* p_indices = p_mesh->GetPolygonVertices();
			int index_count = p_mesh->GetPolygonVertexCount();
			pOutIndexVector->insert(pOutIndexVector->end(), &p_indices[0], &p_indices[index_count]);

			// Load UVs and populate the returned UV vector
			::LoadUV(p_mesh, pOutUVVector);
			// Transform the mesh using the appropriate root transformation matrix

			
			for (int j = 0; j < p_mesh->GetControlPointsCount(); ++j)
			{
				FbxAMatrix conversion_transform = FbxAMatrix(FbxVector4(0.0f, 0.0f, 0.0f), FbxVector4(-90.0f, 0.0f, 0.0f), FbxVector4(1.0f, -1.0f, 1.0f));
				DirectX::XMFLOAT3 vertex_pos;
				FbxVector4 fbx_vertex = p_vertices[j];
				fbx_vertex = conversion_transform.MultT(p_vertices[j]);
				vertex_pos.x = (float)fbx_vertex.mData[0];
				vertex_pos.y = (float)fbx_vertex.mData[1];
				vertex_pos.z = (float)fbx_vertex.mData[2];
				pOutVertexPosVector->push_back(vertex_pos);


				// Save to control point info map 
				//control_points_info[j].ctrlPoint = p_mesh->GetControlPointAt(j);

				fbxsdk::FbxVector4 normal;
				// If the mesh normals are not in "per vertex" mode, re-generate them to be useable by this parser
				if (p_mesh->GetElementNormal(0)->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				{
					p_mesh->GenerateNormals(true, true, true);
				}
				// Only allowing one normal per element currently
				fbxsdk::FbxGeometryElementNormal* le_normal = p_mesh->GetElementNormal(0);

				switch (le_normal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					normal = le_normal->GetDirectArray().GetAt(j).mData;
					break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int index = le_normal->GetIndexArray().GetAt(j);
					normal = le_normal->GetDirectArray().GetAt(index).mData;
					break;
				}
				default:
					throw std::exception("Invalid Fbx Normal Reference");
				}
				DirectX::XMFLOAT3 vertex_normal;

				// Transform the normals in the same manner as the vertices
				// Slightly different transform for normals to correct them
				conversion_transform = FbxAMatrix(FbxVector4(0.0f, 0.0f, 0.0f), FbxVector4(-90.0f, 0.0f, 0.0f), FbxVector4(-1.0f, 1.0f, -1.0f));
				normal = conversion_transform.MultT(normal);
				normal.Normalize();
				vertex_normal.x = (float)normal.mData[0];
				vertex_normal.y = (float)normal.mData[1];
				vertex_normal.z = (float)normal.mData[2];
				pOutNormalVector->push_back(vertex_normal);

			}
			// Flip the winding order
			for (unsigned int i = 0, count = (unsigned int)pOutIndexVector->size(); i < count - 2; i += 3)
			{
				std::swap((*pOutIndexVector)[i], (*pOutIndexVector)[i + 2]);
			}

			if (pOutSkeleton->joints.size() > 0)
			{
				// In progress, very likely to break
				// Prepare the vector with ControlPointInfo objects to be written to in any order
				pOutCPInfoVector->resize(p_mesh->GetControlPointsCount());
				::ProcessJointsAndAnimations(p_mesh->GetNode(), pOutSkeleton, pOutCPInfoVector);
			}

		}
		else
		{
			throw std::exception("No mesh found in .fbx file");
		}
	}
	return S_OK;
}