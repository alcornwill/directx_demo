#pragma once

#include "ShaderPair.h"

// this is a lot of work just to implement an abstract class... c++ is stupid
class PhongShader : public ShaderPair
{
public:
	struct CB_VS_PER_OBJECT
	{
		XMMATRIX matWorldViewProj;
		XMMATRIX matWorld;				// needed to transform the normals
	};

	struct CB_PS_PER_OBJECT
	{
		XMFLOAT4 m_vObjectColor;
	};

	struct CB_PS_PER_FRAME
	{
		XMVECTOR m_vLightDirAmbient;	// Vector pointing at the light
		float fAmbient;
	};
	
	~PhongShader();

	WCHAR *vsFilePath() override;
	LPCSTR vsName() override;
	WCHAR *psFilePath() override;
	LPCSTR psName() override;

	// constant buffers
	ID3D11Buffer               *g_pcbVSPerObject = NULL;
	ID3D11Buffer               *g_pcbPSPerObject = NULL;
	ID3D11Buffer               *g_pcbPSPerFrame = NULL;

	// used for PerFrame lighting
	float    fAmbient = 0.4f;
	XMVECTOR vectorLightDirection = XMVectorSet(-1, 1, -2, 0); // 4th value unused

	void onFrameMove(double fTime, float fElapsedTime) override;
	void createBuffers(ID3D11Device* pd3dDevice) override;
	void setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext) override;
	void updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color) override;
	void updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext) override;
};