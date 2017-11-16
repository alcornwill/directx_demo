#pragma once

#include "ShaderPair.h"

class CloudShader : public ShaderPair
{
	float windOffset1x = 0;
	float windOffset1z = 0.3;
	float windOffset2x = 0.4;
	float windOffset2z = 0.12;
public:
	float windSpeed = 0.0001;

	struct CB_VS_PER_OBJECT
	{
		XMMATRIX matWorldViewProj;
		XMMATRIX matWorld;
	};

	struct CB_PS_PER_OBJECT
	{
		XMFLOAT4 m_vObjectColor;
		XMFLOAT4 offsetUV;
	};

	~CloudShader();

	WCHAR *vsFilePath() override;
	LPCSTR vsName() override;
	WCHAR *psFilePath() override;
	LPCSTR psName() override;

	// resources
	ID3D11Buffer               *cbVSPerObject = NULL;
	ID3D11Buffer               *cbPSPerObject = NULL;
	ID3D11ShaderResourceView   *txWind1 = NULL;
	ID3D11ShaderResourceView   *txWind2 = NULL;

	void onFrameMove(double fTime, float fElapsedTime) override;
	void createBuffers(ID3D11Device* pd3dDevice) override;
	void setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext) override;
	void updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color) override;
	void updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext) override;
};