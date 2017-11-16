#pragma once

#include "GameObject.h"

// GameObject that automatically follows player
class FollowPlayer : public GameObject
{
public:
	GameObject * player = NULL;
	vector3 positionOffset = vector3(0, 0, 0);
	
	FollowPlayer() : GameObject() {};
	FollowPlayer(int meshIndex) : GameObject(meshIndex) {};

	void onFrameMove(double fTime, float fElapsedTime) override;
};