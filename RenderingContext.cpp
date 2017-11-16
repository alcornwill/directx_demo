#include "RenderingContext.h"

#include "PhongShader.h"
#include "UnlitShader.h"
#include "StandardRenderer.h";
#include "ShellRenderer.h"
#include "ShellTextureShader.h"
#include "CloudShader.h"

RenderingContext::RenderingContext()
{
	addShader(new PhongShader());			// 0
	addShader(new UnlitShader());			// 1
	addShader(new ShellTextureShader());	// 3
	addShader(new CloudShader());			// 4

	addRenderer(new StandardRenderer());	// 0
	addRenderer(new ShellRenderer());		// 1
}

void RenderingContext::constructShaders(ID3D11Device* pd3dDevice)
{
	// create shared linear sampler
	D3D11_SAMPLER_DESC samDesc;
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MipLODBias = 0.0f;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
	samDesc.MinLOD = 0;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pd3dDevice->CreateSamplerState(&samDesc, &samLinear);
	DXUT_SetDebugName(samLinear, "SAMPLER_LINEAR");

	// create shared mirror sampler
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	pd3dDevice->CreateSamplerState(&samDesc, &samMirror);
	DXUT_SetDebugName(samMirror, "SAM_MIRROR_CLOUD");

	// create shared blend state
	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	// Create an alpha enabled blend state description.
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pd3dDevice->CreateBlendState(&BlendState, &blendState);

	// load the 'default texture'
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		defaultTextureFilePath,
		NULL, NULL,
		&defaultTexture,
		NULL);
	DXUT_SetDebugName(defaultTexture, "DEFAULT_TEXTURE");

	// for each shader pair
	for (int i = 0; i < MaxShaders; i++)
	{
		ShaderPair * shader = shaders[i];
		if (!shader) break;
		shader->constructShaders(pd3dDevice);
	}
}

void RenderingContext::recalculateProjectionMatrix(float g_width, float g_height)
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fov,
		g_width / static_cast<FLOAT>(g_height), // Aspect ratio.
		nearPlane,						// Near clipping plane.
		farPlane);					// Far clipping plane.
}

void RenderingContext::addRenderer(Renderer* newRenderer)
{
	renderers[rendererCount] = newRenderer;
	rendererCount++;
}

void RenderingContext::addShader(ShaderPair * newShader)
{
	shaders[shaderCount] = newShader;
	shaderCount++;
}

void RenderingContext::renderPerFrame(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{

	// update per frame constant buffers
	for (int i = 0; i < MaxShaders; i++)
	{
		ShaderPair * shader = shaders[0];
		if (!shader) break;
		shader->renderPerFrame(pd3dDevice, pd3dImmediateContext);
	}
}

RenderingContext::~RenderingContext()
{
	// delete shaders
	for (int i = 0; i < MaxShaders; i++)
	{
		ShaderPair * shader = shaders[i];
		if (!shader) break;
		delete shader;
	}

	// release resources
	SAFE_RELEASE(blendState);
	SAFE_RELEASE(samLinear);
	SAFE_RELEASE(samMirror);
	SAFE_RELEASE(defaultTexture);
}