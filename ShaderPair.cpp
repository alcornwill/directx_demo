#include "ShaderPair.h"

#include "SDKmisc.h"
#include "EventManager.h"
#include "GameContext.h"
#include "ObjMesh.h"
#include "Utils.h"

ShaderPair::ShaderPair()
{
	// subscribe to onFrameMove
	__hook(&EventManager::OnFrameMove, gameContext->eventManager, &ShaderPair::onFrameMove);
}

ShaderPair::~ShaderPair()
{
	// release resources
	SAFE_RELEASE(vertexLayout11);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);

	// unsubscribe events
	__unhook(&EventManager::OnFrameMove, gameContext->eventManager, &ShaderPair::onFrameMove);
}

// input layout has to be the same as in the vertex shader
// currently we have same for each shader...?
const D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// ok I got rid of the V_RETURN macros because they were breaking it for no reason
// but I probably shouldn't have
void ShaderPair::constructShaders(ID3D11Device* pd3dDevice)
{
	// compile shaders
	ID3DBlob* vertexShaderBuffer = NULL;
	compileShaderFromFile(vsFilePath(), vsName(), "vs_5_0", &vertexShaderBuffer);
	ID3DBlob* pixelShaderBuffer = NULL;
	compileShaderFromFile(psFilePath(), psName(), "ps_5_0", &pixelShaderBuffer);

	// Create the pixel and vertex shaders
	pd3dDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	DXUT_SetDebugName(vertexShader, "VERTEX_SHADER");
	pd3dDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	DXUT_SetDebugName(pixelShader, "PIXEL_SHADER");

	// create vertex layout
	pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &vertexLayout11);
	DXUT_SetDebugName(vertexLayout11, "VERTEX_LAYOUT");

	// clean up
	SAFE_RELEASE(vertexShaderBuffer);
	SAFE_RELEASE(pixelShaderBuffer);

	// create constant buffers
	createBuffers(pd3dDevice);
}

//**************************************************************************//
// Compile the shader file.  These files aren't pre-compiled (well, not		//
// here, and are compiled on he fly).										//
//**************************************************************************//
HRESULT ShaderPair::compileShaderFromFile(WCHAR* szFileName,		// File Name
	LPCSTR szEntryPoint,		// Namee of shader
	LPCSTR szShaderModel,		// Shader model
	ID3DBlob** ppBlobOut)	// Blob returned
{
	HRESULT hr = S_OK;

	// find the file
	WCHAR str[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szFileName));

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(str, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		WCHAR errorCharsW[200];
		if (pErrorBlob != NULL)
		{
			Utils::charStrToWideChar(errorCharsW, static_cast<char *>(pErrorBlob->GetBufferPointer()));
			MessageBox(0, errorCharsW, L"Error", 0);
		}
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	SAFE_RELEASE(pErrorBlob);

	return S_OK;
}

// doesn't actually 'render' anything...
void ShaderPair::renderPerFrame(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	updateBuffersPerFrame(pd3dImmediateContext);
}

// set as the current shader
void ShaderPair::setShader(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	pd3dImmediateContext->IASetInputLayout(vertexLayout11);
	pd3dImmediateContext->VSSetShader(vertexShader, NULL, 0);
	pd3dImmediateContext->PSSetShader(pixelShader, NULL, 0);

	setConstantBuffers(pd3dImmediateContext);
}