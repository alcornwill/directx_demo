#include "WalkingObject.h"

#include "GameContext.h"
#include "InputManager.h"

void WalkingObject::onFrameMove(double fTime, float fElapsedTime)
{
	GameObject::onFrameMove(fTime, fElapsedTime);

	if (gameContext->inputManager->cursorLeft)
	{
		rotation.y -= fElapsedTime * 3.0f;
	}

	if (gameContext->inputManager->cursorRight)
	{
		rotation.y += fElapsedTime * 3.0f;
	}

	// calculate AllRotations and Direction
	XMMATRIX matAllRotationsLocal = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR localDir = XMVector3TransformCoord(initVecDirLocal, matAllRotationsLocal);
	localDir = XMVector3Normalize(localDir);

	float adjustedSpeed = speed*fElapsedTime;

	if (gameContext->inputManager->cursorUp)
	{
		// move forwards
		position += vector3(XMVectorGetX(localDir) * adjustedSpeed,
			                XMVectorGetY(localDir) * adjustedSpeed,
			                XMVectorGetZ(localDir) * adjustedSpeed);
	}

	if (gameContext->inputManager->cursorDown)
	{
		// backwards
		position -= vector3(XMVectorGetX(localDir) * adjustedSpeed,
			                XMVectorGetY(localDir) * adjustedSpeed,
			                XMVectorGetZ(localDir) * adjustedSpeed);
	}
}
