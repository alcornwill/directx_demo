#pragma once

#include "Renderer.h"

class StandardRenderer : public Renderer
{
public:
	void render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, GameObject * gameObject) override;
};