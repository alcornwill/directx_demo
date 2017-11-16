#include "Utils.h"

#include "vector3.h"

//**************************************************************************//
// Convert an old chracter (char *) string to a WCHAR * string.  There must //
// be something built into Visual Studio to do this for me, but I can't		//
// find it - Nigel.															//
//**************************************************************************//

void Utils::charStrToWideChar(WCHAR *dest, char *source)
{
	int length = strlen(source);
	for (int i = 0; i <= length; i++)
		dest[i] = static_cast<WCHAR>(source[i]);
}

XMMATRIX Utils::getTransformMatrix(vector3 position, vector3 rotation, vector3 scale)
{
	// updates its own local transformation matrix
	XMMATRIX translateMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotateXMatrix = XMMatrixRotationX(rotation.x);
	XMMATRIX rotateYMatrix = XMMatrixRotationY(rotation.y);
	XMMATRIX rotateZMatrix = XMMatrixRotationZ(rotation.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	return rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix * translateMatrix;
}
