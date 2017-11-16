#include "UnlitShader.h"

#include "GameContext.h"
#include "RenderingContext.h"

WCHAR *UnlitShader::vsFilePath()
{
	return L"VS_Standard.hlsl";
}

LPCSTR UnlitShader::vsName()
{
	return "main";
}

WCHAR *UnlitShader::psFilePath()
{
	return L"PS_ColoredDiffuse.hlsl";
}

LPCSTR UnlitShader::psName()
{
	return "main";
}

UnlitShader::~UnlitShader()
{
	SAFE_RELEASE(cbVSPerObject);
	SAFE_RELEASE(cbPSPerObject);
}

void UnlitShader::createBuffers(ID3D11Device* pd3dDevice)
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
	DXUT_SetDebugName(cbVSPerObject, "CB_VS_PER_OBJECT_UNLIT");

	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &cbPSPerObject);
	DXUT_SetDebugName(cbPSPerObject, "CB_PS_PER_OBJECT_UNLIT");
}

void UnlitShader::updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color)
{
	// update per object resources
	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(worldMatrix);
	CBMatrices.matWorldViewProj = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(cbVSPerObject, 0, NULL, &CBMatrices, 0, 0);

	CB_PS_PER_OBJECT CBPerObject;
	CBPerObject.m_vObjectColor = color;
	pd3dImmediateContext->UpdateSubresource(cbPSPerObject, 0, NULL, &CBPerObject, 0, 0);
}

void UnlitShader::updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext)
{
}

// set as the current shader
void UnlitShader::setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetSamplers(0, 1, &gameContext->renderingContext->samLinear);

	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &cbPSPerObject);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &cbVSPerObject);
}

void UnlitShader::onFrameMove(double fTime, float fElapsedTime)
{
}