#pragma once

#include <windows.h>
#include "DXUT.h"
#include <xnamath.h>

#include "GameInfo.h"

class ObjMesh;

// abstract
class ShaderPair : public GameInfo
{
public:
	// not all shaders have to use this...
	float offsetU = 0;
	float offsetV = 0;

	// variables to store the compiled shaders
	ID3D11VertexShader *vertexShader = NULL;
	ID3D11PixelShader *pixelShader = NULL;

	// resources
	ID3D11InputLayout *vertexLayout11 = NULL;

	void constructShaders(ID3D11Device* pd3dDevice);

	ShaderPair();
	virtual ~ShaderPair();

	static HRESULT compileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void renderPerObject(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color, ObjMesh *mesh);
	void renderPerFrame(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
	void setShader(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);

	virtual WCHAR *vsFilePath() = 0;
	virtual LPCSTR vsName() = 0;
	virtual WCHAR *psFilePath() = 0;
	virtual LPCSTR psName() = 0;

	virtual void onFrameMove(double fTime, float fElapsedTime) = 0;
	virtual void createBuffers(ID3D11Device* pd3dDevice) = 0;
	virtual void setConstantBuffers(ID3D11DeviceContext* pd3dImmediateContext) = 0;
	virtual void updateBuffersPerObject(ID3D11DeviceContext* pd3dImmediateContext, const XMMATRIX &worldMatrix, const XMMATRIX &matWorldViewProjection, XMFLOAT4 color) = 0;
	virtual void updateBuffersPerFrame(ID3D11DeviceContext* pd3dImmediateContext) = 0;
};