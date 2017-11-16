#pragma once
#include <windows.h>

#include <xnamath.h>
#include "vector3.h"

#include "GameInfo.h"

class GameObject;

// this is sort of a followcam sometimes
// I want to try to make an orthographic camera
class Camera : GameInfo
{
public:
	GameObject * parent = NULL;

	float lastChanged = 0; // last time the camera was changed
	bool topDown = false;

	vector3 position;

	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX viewMatrix;

	void update(float fTime, float elapsedTime);
};