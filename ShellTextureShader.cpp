#include "ShellTextureShader.h"

#include "GameContext.h"
#include "RenderingContext.h"

WCHAR* ShellTextureShader::vsFilePath()
{
	return L"VS_ShellTexture.hlsl";
}

LPCSTR ShellTextureShader::vsName()
{
	return "main";
}

WCHAR* ShellTextureShader::psFilePath()
{
	return L"PS_ShellTexture.hlsl";
}

LPCSTR ShellTextureShader::psName()
{
	return "main";
}

ShellTextureShader::~ShellTextureShader()
{
	SAFE_RELEASE(cbVSPerObject);
	SAFE_RELEASE(cbPSPerObject);
	SAFE_RELEASE(cbVSWindOffset);
	SAFE_RELEASE(txNoise);
	SAFE_RELEASE(txWind1);
	SAFE_RELEASE(txWind2);
}

void ShellTextureShader::createBuffers(ID3D11Device* pd3dDevice)
{
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbVSPerObject);
	DXUT_SetDebugName(cbVSPerObject, "CB_VS_PER_OBJECT_SHELL");

	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbPSPerObject);
	DXUT_SetDebugName(cbPSPerObject, "CB_PS_PER_OBJECT_SHELL");

	Desc.ByteWidth = sizeof(CB_VS_WIND_OFFSET);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbVSWindOffset);
	DXUT_SetDebugName(cbVSWindOffset, "CB_VS_WIND_OFFSET_SHELL");

	// also initialize the wind textures
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Media\\Grass\\Wind1.png",
		NULL, NULL,
		&txWind1,
		NULL);
	DXUT_SetDebugName(txWind1, "TEX_WIND1_SHELL");

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Media\\Grass\\Wind2.png",
		NULL, NULL,
		&txWind2,
		NULL);
	DXUT_SetDebugName(txWind2, "TEX_WIND2_SHELL");

	// load the noise texture
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		noiseTextureFilePath,
		NULL, NULL,
		&txNoise,
		NULL);
	DXUT_SetDebugName(txNoise, "TEX_NOISE_SHELL");
}

void ShellTextureShader::updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color)
{
	// update per object resources
	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(worldMatrix);
	CBMatrices.matWorldViewProj = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(cbVSPerObject, 0, NULL, &CBMatrices, 0, 0);

	CB_PS_PER_OBJECT CBPerObject;
	CBPerObject.m_vObjectColor = color;
	CBPerObject.offsetUV = XMFLOAT4(offsetU, offsetV,0,0); // last two floats to align bytes
	pd3dImmediateContext->UpdateSubresource(cbPSPerObject, 0, NULL, &CBPerObject, 0, 0);

	// should really be in PerFrame...
	CB_VS_WIND_OFFSET CBWindOffset;
	CBWindOffset.offset1 = XMFLOAT2(windOffset1x, windOffset1z);
	CBWindOffset.offset2 = XMFLOAT2(windOffset2x, windOffset2z);
	pd3dImmediateContext->UpdateSubresource(cbVSWindOffset, 0, NULL, &CBWindOffset, 0, 0);
}

void ShellTextureShader::updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext)
{
	// putting stuff in here doesn't work lol
}

// set as the current shader
void ShellTextureShader::setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetSamplers(0, 1, &gameContext->renderingContext->samLinear);
	pd3dImmediateContext->VSSetSamplers(0, 1, &gameContext->renderingContext->samMirror);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &cbPSPerObject);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &cbVSPerObject);
	pd3dImmediateContext->VSSetConstantBuffers(1, 1, &cbVSWindOffset);
	pd3dImmediateContext->PSSetShaderResources(1, 1, &txNoise);
	pd3dImmediateContext->VSSetShaderResources(0, 1, &txWind1);
	pd3dImmediateContext->VSSetShaderResources(1, 1, &txWind2);
}

void ShellTextureShader::onFrameMove(double fTime, float fElapsedTime)
{
	// gentle breeze
	windOffset1x += rand() % 12 * fElapsedTime * windSpeed;
	windOffset1z -= rand() % 100 * fElapsedTime * windSpeed;
	windOffset2x -= rand() % 16 * fElapsedTime * windSpeed;
	windOffset2z -= rand() % 78 * fElapsedTime * windSpeed;
}