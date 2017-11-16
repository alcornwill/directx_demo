#include "InputManager.h"

void InputManager::onKeyboard(UINT key, bool value)
{
	switch (key)
	{
	case VK_LEFT:	cursorLeft = value; break;
	case VK_RIGHT:	cursorRight = value; break;
	case VK_UP:		cursorUp = value; break;
	case VK_DOWN:	cursorDown = value; break;
	case VK_SPACE : space = value; break;
	}
}

// I wanted to have some basic AI or boids
// so I was going to have a controller class
// which would be a wrapper of keyboard controls
// and then, for WalkingObject and FlyingObject
// they would just be able to plug in different controllers
// to change if it is player or AI controlled