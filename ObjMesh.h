#pragma once

#include <Windows.h>
#include <xnamath.h>
#include "DXUT.h"

struct ObjMeshSubset
{
	UINT  ic = 0;
	INT  vs = 0;
	UINT  is = 0;

	UINT indexCount() { return ic; }
	INT indexStart() { return is; }
	UINT vertexStart() { return vs; }

	bool hastxtrz = false;
	bool hasTexture()  { return hastxtrz; }

	//XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
	//XMFLOAT4 getColor() override {
	//	return color;
	//}

	ID3D11ShaderResourceView *txtr = NULL;
	ID3D11ShaderResourceView *texture() { return txtr; }

	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology() { return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 VecNormal;
	XMFLOAT2 TexUV;
};

class ObjMesh
{
public:
	SimpleVertex *vertices = NULL;
	USHORT  *indexes = NULL;

	UINT		 numSs = 0;
	ObjMeshSubset *subsets = NULL;

	USHORT       numVertices = 0;
	USHORT       numIndices = 0;

	ID3D11Buffer* vb = NULL;
	ID3D11Buffer* ib = NULL;

	ID3D11Buffer* vertexBuffer() { return vb; }
	ID3D11Buffer* indexBuffer() { return ib; }
	UINT stride() { return sizeof(SimpleVertex); }
	DXGI_FORMAT ibFormat() { return DXGI_FORMAT_R16_UINT; } // sdkmesh is DXGI_FORMAT_R32B32_UINT I think. whatever that means
	UINT numSubsets() { return numSs; }
	ObjMeshSubset *getSubset(UINT index) { return &subsets[index]; }
};

