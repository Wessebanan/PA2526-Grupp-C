#include "PhysicsHelperFunctions.h"


float helper_functions::CalculateDistance(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	float x_diff = p1.x - p2.x;
	float y_diff = p1.y - p2.y;
	float z_diff = p1.z - p2.z;

	// Returns the floating point distance according to the distance formula.
	return sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
}
void helper_functions::CreateOBB(DirectX::XMFLOAT3(&vertices)[8], DirectX::XMFLOAT3 min_point, DirectX::XMFLOAT3 max_point)
{
	// Creating the vertices for the box with the min and max points.

	// Near z:	2 3
	//			0 1
	vertices[0] = min_point;
	vertices[1] = DirectX::XMFLOAT3(max_point.x, min_point.y, min_point.z);
	vertices[2] = DirectX::XMFLOAT3(min_point.x, max_point.y, min_point.z);
	vertices[3] = DirectX::XMFLOAT3(max_point.x, max_point.y, min_point.z);
	
	// Far z:	6 7
	//			4 5
	vertices[4] = DirectX::XMFLOAT3(min_point.x, min_point.y, max_point.z);
	vertices[5] = DirectX::XMFLOAT3(max_point.x, min_point.y, max_point.z);
	vertices[6] = DirectX::XMFLOAT3(min_point.x, max_point.y, max_point.z);
	vertices[7] = max_point;
}