#pragma once
#include "windows.h"
#include <xnamath.h>

struct vector3;

class Utils
{
public:
	static void charStrToWideChar(WCHAR *dest, char *source);
	static XMMATRIX getTransformMatrix(vector3 position, vector3 rotation, vector3 scale);
};
