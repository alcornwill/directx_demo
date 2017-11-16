#pragma once

class GameContext;

// intended to be derived, allows anything to access GameContext
// should probably make it abstract but can't be bothered with encapsulation
class GameInfo
{
public:
	virtual ~GameInfo()
	{
	}

	static GameContext *gameContext;
};