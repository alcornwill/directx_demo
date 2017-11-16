#pragma once

#include "GameObject.h"

// would be cool to have a walking AI
class WalkingObject : public GameObject
{
public:
	float speed = 50;
	XMVECTOR initVecDirLocal = XMVectorSet(0, 0, -1, 0);

	WalkingObject() : GameObject() {};
	WalkingObject(int meshIndex) : GameObject(meshIndex) {};

	void onFrameMove(double fTime, float fElapsedTime) override;
};