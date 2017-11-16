#include "PhongShader.h"

#include "GameContext.h"
#include "RenderingContext.h"

WCHAR *PhongShader::vsFilePath()
{
	return L"VS_Standard.hlsl";
}

LPCSTR PhongShader::vsName()
{
	return "main";
}

WCHAR *PhongShader::psFilePath()
{
	return L"PS_Phong.hlsl";
}

LPCSTR PhongShader::psName()
{
	return "main";
}

PhongShader::~PhongShader()
{
	SAFE_RELEASE( g_pcbVSPerObject );
	SAFE_RELEASE( g_pcbPSPerObject );
	SAFE_RELEASE( g_pcbPSPerFrame );
}

void PhongShader::createBuffers(ID3D11Device* pd3dDevice)
{
	// Create the 3 constant buffers, using the same buffer descriptor to create all three
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbVSPerObject);
	DXUT_SetDebugName(g_pcbVSPerObject, "CB_VS_PER_OBJECT");

	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbPSPerObject);
	DXUT_SetDebugName(g_pcbPSPerObject, "CB_PS_PER_OBJECT");

	Desc.ByteWidth = sizeof(CB_PS_PER_FRAME);
	pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbPSPerFrame);
	DXUT_SetDebugName(g_pcbPSPerFrame, "CB_PS_PER_FRAME");
}

void PhongShader::updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color)
{
	// update per object resources
	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(worldMatrix);
	CBMatrices.matWorldViewProj = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(g_pcbVSPerObject, 0, NULL, &CBMatrices, 0, 0);

	CB_PS_PER_OBJECT CBPerObject;
	CBPerObject.m_vObjectColor = color;
	pd3dImmediateContext->UpdateSubresource(g_pcbPSPerObject, 0, NULL, &CBPerObject, 0, 0);
}

void PhongShader::updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext)
{
	CB_PS_PER_FRAME CBPerFrame;
	CBPerFrame.m_vLightDirAmbient = vectorLightDirection;
	CBPerFrame.fAmbient = fAmbient;
	pd3dImmediateContext->UpdateSubresource(g_pcbPSPerFrame, 0, NULL, &CBPerFrame, 0, 0);
}

// set as the current shader
void PhongShader::setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->PSSetSamplers(0, 1, &gameContext->renderingContext->samLinear);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pcbPSPerObject);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pcbVSPerObject);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pcbPSPerFrame);
}

void PhongShader::onFrameMove(double fTime, float fElapsedTime)
{
}