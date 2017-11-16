#pragma once

#include "ObjMesh.h"

class ObjMeshInfo
{
	void specialLoad(ID3D11Device *pd3dDevice);
public:
	~ObjMeshInfo();

	LPCTSTR file;
	ObjMesh objMesh;
	bool fromFile = true;
	LPCTSTR textureFilePath; // this is a hack

	LPCTSTR *filePath() { return &file; }
	ObjMesh *mesh() { return &objMesh; }
	void loadMesh(ID3D11Device* pd3dDevice);
	void unloadMesh();
};