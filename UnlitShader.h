#pragma once

#include "ShaderPair.h"

class UnlitShader : public ShaderPair
{
public:
	struct CB_VS_PER_OBJECT
	{
		XMMATRIX matWorldViewProj;
		XMMATRIX matWorld;			
	};

	struct CB_PS_PER_OBJECT
	{
		XMFLOAT4 m_vObjectColor;
	};

	~UnlitShader();

	WCHAR *vsFilePath() override;
	LPCSTR vsName() override;
	WCHAR *psFilePath() override;
	LPCSTR psName() override;

	// constant buffers
	ID3D11Buffer               *cbVSPerObject = NULL;
	ID3D11Buffer               *cbPSPerObject = NULL;

	void onFrameMove(double fTime, float fElapsedTime) override;
	void createBuffers(ID3D11Device* pd3dDevice) override;
	void setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext) override;
	void updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color) override;
	void updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext) override;
};