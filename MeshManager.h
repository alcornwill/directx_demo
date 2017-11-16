#pragma once

#include "DXUT.h"

class ObjMesh;
class ObjMeshInfo;

const int MaxMeshes = 10;

class MeshManager
{
private:
	void addMesh(LPCTSTR filePath);
	int meshCount = 0;
public:
	int generateGrid(float length, float width, int divX, int divY, bool hasTexture, LPCTSTR textureFilePath);
	MeshManager();
	~MeshManager();
	ObjMeshInfo * meshes[MaxMeshes] = {}; // meshes are shared between objects
	void createMeshes(ID3D11Device* pd3dDevice);
};