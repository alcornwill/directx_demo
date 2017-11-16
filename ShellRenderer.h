#pragma once

#include "Renderer.h"
#include "vector3.h"

#ifndef PI
#define PI           3.14159265358979323846  /* pi */
#endif

class ShellRenderer : public Renderer
{
public:
	void render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, GameObject* gameObject) override;

	int numLayers = 30;
	float w = 7; // grass width
	float h = 7; // grass height

	float from =-0.2*2*PI;
	float length = 0.1*2*PI; // to is length greater than from
	float colorOffset = 0.03;
};
