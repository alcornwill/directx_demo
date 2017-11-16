#include "FollowPlayer.h"

#include "ShaderPair.h"

void FollowPlayer::onFrameMove(double fTime, float fElapsedTime)
{
	GameObject::onFrameMove(fTime, fElapsedTime);

	position = player->position + positionOffset;

	// also adjust the texture uv
	shaderPair->offsetU = fmod(position.x, 400) / 400.0;
	shaderPair->offsetV = fmod(position.z, 400) / 400.0;
}
