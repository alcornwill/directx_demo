#include "Camera.h"
#include "GameObject.h"

#include "GameContext.h"
#include "InputManager.h"

// gotta make this a first person camera
void Camera::update(float fTime, float elapsedTime)
{
	if (gameContext->inputManager->space && fTime - lastChanged >1)
	{
		topDown = !topDown;
		lastChanged = fTime;
	}

	// make camera spin around
	//lookAt = XMVectorSet(sin(fTime) * 5, 1, cos(fTime) * 5, 1);
	XMVECTOR eye;

	if (parent)
	{
		if (!topDown)
		{
			// is follow cam
			lookAt = XMVectorSet(parent->position.x, parent->position.y + 10, parent->position.z, 0);

			// calculate eye position
			// first we have to get the direction of the parent
			XMMATRIX matAllRotationsLocal = XMMatrixRotationRollPitchYaw(parent->rotation.x, parent->rotation.y, parent->rotation.z);
			XMVECTOR initVecDirLocal = XMVectorSet(0, 0, 1, 0);
			XMVECTOR localDir;

			localDir = XMVector3TransformCoord(initVecDirLocal, matAllRotationsLocal);
			localDir = XMVector3Normalize(localDir);

			eye = XMVectorSet(parent->position.x + XMVectorGetX(localDir),
				parent->position.y + XMVectorGetY(localDir) + 10, // make it a bit up
				parent->position.z + XMVectorGetZ(localDir),
				0);
		}
		else
		{
			// is top down cam
			lookAt = XMVectorSet(parent->position.x, parent->position.y, parent->position.z, 0);

			XMMATRIX matAllRotationsLocal = XMMatrixRotationRollPitchYaw(parent->rotation.x, parent->rotation.y, parent->rotation.z);
			XMVECTOR initVecDirLocal = XMVectorSet(0, 0, 1, 0);
			XMVECTOR localDir;

			localDir = XMVector3TransformCoord(initVecDirLocal, matAllRotationsLocal);
			localDir = XMVector3Normalize(localDir);

			localDir *= -50;

			eye = XMVectorSet(parent->position.x + XMVectorGetX(localDir),
				parent->position.y + XMVectorGetY(localDir) + 50, // make it a bit up
				parent->position.z + XMVectorGetZ(localDir),
				0);
		}
	}
	else
	{
		eye = XMVectorSet(position.x, position.y, position.z, 1);
	}
	
	viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);
}