#pragma once

#include "DXUT.h"

class InputManager
{
public:
	bool cursorLeft;
	bool cursorRight;
	bool cursorUp;
	bool cursorDown;
	bool space;

	// you could make this virtual and have different implementations of it
	void onKeyboard(UINT key, bool value);
};
