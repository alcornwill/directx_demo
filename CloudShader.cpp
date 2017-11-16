#include "CloudShader.h"

#include "GameContext.h"
#include "RenderingContext.h"

WCHAR *CloudShader::vsFilePath()
{
	return L"VS_Standard.hlsl";
}

LPCSTR CloudShader::vsName()
{
	return "main";
}

WCHAR *CloudShader::psFilePath()
{
	return L"PS_Clouds.hlsl";
}

LPCSTR CloudShader::psName()
{
	return "main";
}

CloudShader::~CloudShader()
{
	SAFE_RELEASE(cbVSPerObject);
	SAFE_RELEASE(cbPSPerObject);
	SAFE_RELEASE(txWind1);
	SAFE_RELEASE(txWind2);
}

void CloudShader::createBuffers(ID3D11Device* pd3dDevice)
{
	// Create the 3 constant buffers, using the same buffer descriptor to create all three
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbVSPerObject);
	DXUT_SetDebugName(cbVSPerObject, "CB_VS_PER_OBJECT_CLOUD");

	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbPSPerObject);
	DXUT_SetDebugName(cbPSPerObject, "CB_PS_PER_OBJECT_CLOUD");

	// also initialize the wind textures
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Media\\Grass\\Wind1.png",
		NULL, NULL,
		&txWind1,
		NULL);
	DXUT_SetDebugName(txWind1, "TEX_WIND1_CLOUD");

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Media\\Grass\\Wind2.png",
		NULL, NULL,
		&txWind2,
		NULL);
	DXUT_SetDebugName(txWind2, "TEX_WIND2_CLOUD");
}

void CloudShader::updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color)
{
	// update per object resources
	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(worldMatrix);
	CBMatrices.matWorldViewProj = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(cbVSPerObject, 0, NULL, &CBMatrices, 0, 0);

	CB_PS_PER_OBJECT CBPerObject;
	CBPerObject.m_vObjectColor = color;
	CBPerObject.offsetUV = XMFLOAT4(windOffset1x, windOffset1z, windOffset2x, windOffset2z); // recycling this buffer for both cloud texture UVs
	pd3dImmediateContext->UpdateSubresource(cbPSPerObject, 0, NULL, &CBPerObject, 0, 0);
}

void CloudShader::updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext)
{
}

// set as the current shader
void CloudShader::setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetSamplers(0, 1, &gameContext->renderingContext->samMirror);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &cbPSPerObject);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &cbVSPerObject);
	pd3dImmediateContext->PSSetShaderResources(1, 1, &txWind1);
	pd3dImmediateContext->PSSetShaderResources(2, 1, &txWind2);
}

void CloudShader::onFrameMove(double fTime, float fElapsedTime)
{
	// we also need another sampler so that it reflects the texture when it wraps
	windOffset1x += rand() % 26 * fElapsedTime * windSpeed;
	windOffset1z -= rand() % 100 * fElapsedTime * windSpeed;
	windOffset2x += rand() % 16 * fElapsedTime * windSpeed;
	windOffset2z += rand() % 3 * fElapsedTime * windSpeed;
}