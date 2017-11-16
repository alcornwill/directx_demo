#pragma once

#include <windows.h>
#include "DXUT.h"
#include <xnamath.h>

class Renderer;
class Camera;
class ShaderPair;

const int MaxShaders = 10;

class RenderingContext
{
private:
	void addRenderer(Renderer * newRenderer);
	int rendererCount = 0;
	void addShader(ShaderPair * newShader);
	int shaderCount = 0;
public:
	// you can make as many cameras as you want, but only one will be rendered (atm)
	Camera * primaryCamera = NULL;

	// shared rendering resources
	ID3D11SamplerState  *samLinear = NULL;
	ID3D11SamplerState  *samMirror = NULL;
	ID3D11BlendState	*blendState = NULL;

	WCHAR * defaultTextureFilePath = L"Media\\defaultTexture.png";
	ID3D11ShaderResourceView *defaultTexture = NULL;

	// projection matrix stuff
	XMMATRIX projectionMatrix;
	float fov = XM_PIDIV2; // pi / 2 (90 degrees)
	float nearPlane = 5;
	float farPlane = 20000;

	Renderer * renderers[10] = {};
	ShaderPair *shaders[MaxShaders] = {};
	RenderingContext();
	~RenderingContext();
	void constructShaders(ID3D11Device* pd3dDevice);
	void recalculateProjectionMatrix(float g_width, float g_height);
	void renderPerFrame(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
};