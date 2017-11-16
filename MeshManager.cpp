#include "MeshManager.h"

#include "ObjMesh.h"
#include "ObjMeshInfo.h"


int MeshManager::generateGrid(float length, float width, int squaresX, int squaresZ, bool hasTexture, LPCTSTR textureFilePath)
{
	// create ObjMesh
	ObjMeshInfo * meshInfo = new ObjMeshInfo;
	meshInfo->fromFile = false;
	meshInfo->textureFilePath = textureFilePath; // this is pretty hacky
	//ZeroMemory(&meshInfo->objMesh, sizeof(ObjMesh));
	ObjMesh * mesh = &meshInfo->objMesh;
	mesh->numSs = 1; // one subset

	int verticesX = squaresX + 1;
	int verticesZ = squaresZ + 1;

	mesh->numVertices = verticesX*verticesZ;
	mesh->vertices = new SimpleVertex[mesh->numVertices];

	float cellX = width / squaresX;
	float cellZ = length / squaresZ;

	for (int z = 0; z < verticesZ; z++)
	{
		for (int x = 0; x < verticesX; x++)
		{
			// create the four vertices of the square
			int idx = (verticesX*z) + x;

			mesh->vertices[idx].Pos = XMFLOAT3((cellX*x) - (width / 2), 0, (cellZ*z) - (length / 2));
			mesh->vertices[idx].TexUV = XMFLOAT2((cellX*x) / width, (cellZ*z) / length);
			mesh->vertices[idx].VecNormal = XMFLOAT3(0, 1, 0); // up, normalized
		}
	}

	mesh->numIndices = squaresX*squaresZ * 6; // 2 triplets of indices for each square
	mesh->indexes = new USHORT[mesh->numIndices];

	for (int z = 0; z < verticesZ; z++)
	{
		for (int x = 0; x < verticesX; x++)
		{
			if (z == squaresZ || x == squaresX) continue;

			// create the two indices of the square
			int idx = (squaresX*z) + x;
			int vertidx = (verticesX*z) + x;

			mesh->indexes[idx * 6] = vertidx;
			mesh->indexes[1 + (idx * 6)] = verticesX + vertidx;
			mesh->indexes[2 + (idx * 6)] = verticesX + 1 + vertidx;

			mesh->indexes[3 + (idx * 6)] = vertidx;
			mesh->indexes[4 + (idx * 6)] = verticesX + 1 + vertidx;
			mesh->indexes[5 + (idx * 6)] = 1 + vertidx;
		}
	}

	// create subset
	mesh->subsets = new ObjMeshSubset;
	mesh->subsets->vs = 0;
	mesh->subsets->ic = mesh->numIndices;
	mesh->subsets->is = 0;

	// also set the texture if there is one
	if (hasTexture)
	{
		mesh->subsets->hastxtrz = true;
	}

	// add new mesh to list
	meshes[meshCount] = meshInfo;
	meshCount++;
	return meshCount - 1;
}

void MeshManager::addMesh(LPCTSTR filePath)
{
	ObjMeshInfo * mesh = new ObjMeshInfo();
	// this is unsafe because I didn't make a setter accessor on meshinfo
	mesh->file = filePath;
	meshes[meshCount] = mesh;
	meshCount++;
}

MeshManager::MeshManager()
{
	// create meshes
	// gerring rid of SDKMeshes until I get everything working
	addMesh(L"Media\\Skybox\\skybox.obj");			// 0
	addMesh(L"Media\\Grass\\grass.obj");			// 1
	addMesh(L"Media\\Rocks\\rock-low-poly-01.obj"); // 2
	addMesh(L"Media\\Rocks\\rock-low-poly-02.obj"); // 3
	addMesh(L"Media\\Rocks\\rock-low-poly-03.obj"); // 4
	addMesh(L"Media\\Rocks\\rock-low-poly-04.obj"); // 5
	addMesh(L"Media\\Rocks\\rock-low-poly-05.obj"); // 6
}

void MeshManager::createMeshes(ID3D11Device* pd3dDevice)
{
	// create the meshes
	for (int i = 0; i < MaxMeshes; i++)
	{
		ObjMeshInfo * meshInfo = meshes[i];
		if (!meshInfo) break;
		meshInfo->loadMesh(pd3dDevice);
	}
}

MeshManager::~MeshManager()
{
	// delete meshes
	for (int i = 0; i < MaxMeshes; i++)
	{
		ObjMeshInfo *meshInfo = meshes[i];
		if (!meshInfo) break;
		delete meshInfo;
	}
}
