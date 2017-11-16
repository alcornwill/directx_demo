#include "ObjMeshInfo.h"

#include <fstream>		
#include <string>		
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <vector>

//**************************************************************************//
// A couple of functions to trim the start and and of a C++ string, which	//
// strangely doesn't have a trim() method.									//
//																			//
// I nicked them from the Internet, which is why they are incomprehensible.	//
//**************************************************************************//
std::wstring TrimStart(std::wstring s)
{
	s.erase(s.begin(), std::find_if(s.begin(),
		s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::wstring TrimEnd(std::wstring s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// represents how the obj file represents an index (which we don't use when rendering)
struct Index
{
	USHORT v = 0;
	USHORT vt = 0;
	USHORT vn = 0;

	USHORT vertex = 0; // used later for simplifying
	bool newMaterial = false; // used later for submeshes
	WCHAR materialName[200]; // could probably get rid of this
};

struct Material
{
	WCHAR materialName[200];
	bool hasTexture = false;
	WCHAR textureFilePath[200];
};

struct MeshMaterial
{
	ObjMeshSubset subset;
	WCHAR textureFilePath[200];
	bool hasTexture = false;
};

// doesn't parse any properties yet
// but it reads the texture files
void readMtlFile(std::vector <Material> *textureFiles, LPCTSTR mtlFile, WCHAR *folder)
{
	std::wifstream fileStream;
	std::wstring line;

	fileStream.open(mtlFile);
	bool isOpen = fileStream.is_open();

	if (!isOpen)
		throw std::logic_error("mtl file not found");

	while (std::getline(fileStream, line))
	{
		line = TrimStart(line);

		// material
		if (line.compare(0, 7, L"newmtl ") == 0)
		{
			Material material;

			WCHAR first[20];

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%7s%s", first, &material.materialName);
			
			textureFiles->push_back(material);
			continue;
		}

		// filename of texture
		if (line.compare(0, 7, L"map_Kd ") == 0)
		{
			WCHAR first[10], fileName[200];

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%7s%s", first, &fileName);

			// construct the full filepath
			WCHAR fullTextureFilePath[200];
			wcscpy(fullTextureFilePath, folder);
			wcsncat(fullTextureFilePath, fileName, 200);

			textureFiles->back().hasTexture = true;
			wcscpy(textureFiles->back().textureFilePath, fullTextureFilePath);
			continue;
		}
	}
}

void getFolder(WCHAR * folder, WCHAR * wfile)
{
	// now we need to get the containing folder of the obj file
	// this only works if the mtl and jpg files are in the same folder
	// but it shouldn't matter where that folder is

	std::vector <WCHAR *> result;

	WCHAR * res = wcstok(wfile, L"\\");
	while (res)
	{
		result.push_back(res);
		res = wcstok(NULL, L"\\");
	}

	// the last element of the result vector should be the filename, and everything else can be concatenated to make the containing folder

	USHORT resSize = result.size();

	// make the folder string empty
	wcscpy(folder, L"");

	WCHAR *slash = L"\\";

	for (int i = 0; i < resSize - 1; i++) // -1 to exclude last element
	{
		wcscat(folder, result[i]);
		wcscat(folder, slash);
	}
}

void ObjMeshInfo::specialLoad(ID3D11Device* pd3dDevice)
{
	// loads but not from file
	// this is a bit of a hack used by mesh generation stuff

	// create the texture resource
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		textureFilePath,
		NULL, NULL,
		&objMesh.subsets->txtr,
		NULL);
	DXUT_SetDebugName(objMesh.subsets->txtr, "GENERATED_MESH_TEXTURE");

	// Create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * objMesh.numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = objMesh.vertices;

	pd3dDevice->CreateBuffer(&bd, &InitData, &objMesh.vb);
	DXUT_SetDebugName(objMesh.vb, "GENERATED_VERTEX_BUFFER");


	// create the index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(USHORT) * objMesh.numIndices;

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = objMesh.indexes;

	pd3dDevice->CreateBuffer(&bd, &InitData, &objMesh.ib);
	DXUT_SetDebugName(objMesh.ib, "GENERATED_INDEX_BUFFER");
}

void ObjMeshInfo::loadMesh(ID3D11Device* pd3dDevice)
{
	if (!fromFile)
	{
		specialLoad(pd3dDevice);
		return;
	}

	std::wifstream          fileStream;
	std::wstring            line;
	std::vector <XMFLOAT3> vectorVertices(0);
	std::vector <XMFLOAT3> vectorNormals(0);
	std::vector <XMFLOAT2> vectorUVs(0);
	std::vector <Index>    vectorIndices(0);
	std::vector <Material> vectorTextureFiles(0);

	fileStream.open(file);
	bool isOpen = fileStream.is_open();		//debugging only.

	// convert LPCTSTR to WCHAR
	WCHAR wfile[200];
	wcscpy(wfile, file);

	// get the folder so we can prepend it to filenames later
	WCHAR folder[200];
	getFolder(folder, wfile);


	bool newMaterial = false;
	WCHAR materialName[200];

	// now we can start to read the obj file!
	while (std::getline(fileStream, line))
	{
		line = TrimStart(line);

		// materials
		if (line.compare(0, 7, L"usemtl ") == 0)
		{
			WCHAR first[20];
			// I'm actually treating a 'material' as a submesh. not sure if this is correct

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%7s%s", first, materialName);

			// we want the next index to have the newMaterial flag
			newMaterial = true;

			continue;
		}

		//******************************************************************//
		// If true, we have found a vertex.  Read it in as a 2 character	//
		// string, followed by 3 decimal numbers.	Suprisingly the C++		//
		// string has no split() method.   I am using really old stuff,		//
		// fscanf.  There  must be a better way, use regular expressions?	//
		//******************************************************************//
		if (line.compare(0, 2, L"v ") == 0)  //"v space"
		{
			WCHAR first[10];
			float x, y, z;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%f%f%f", first, &x, &y, &z);

			XMFLOAT3 v = XMFLOAT3(x, y, z);
			vectorVertices.push_back(v);
			continue;
		}

		// normals
		if (line.compare(0, 3, L"vn ") == 0)  //"vn space"
		{
			WCHAR first[10];
			float x, y, z;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f%f", first, &x, &y, &z);

			XMFLOAT3 v = XMFLOAT3(x, y, z);
			vectorNormals.push_back(v);
			continue;
		}

		// texture coordinates
		if (line.compare(0, 3, L"vt ") == 0)  //"vt space"
		{
			WCHAR first[10];
			float u, v;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%3s%f%f", first, &u, &v);

			XMFLOAT2 vert = XMFLOAT2(u, v); 
			vectorUVs.push_back(vert);
			continue;
		}

		//******************************************************************//
		// If true, we have found a face.   Read it in as a 2 character		//
		// string, followed by 3 decimal numbers.	Suprisingly the C++		//
		// string has no split() method.   I am using really old stuff,		//
		// fscanf.  There must be a better way, use regular expressions?	//
		//																	//
		// It assumes the line is in the format								//
		// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...							//
		//******************************************************************// 
		if (line.compare(0, 2, L"f ") == 0)  //"f space"
		{
			WCHAR first[10];
			WCHAR slash1[10];
			WCHAR slash2[10];
			WCHAR slash3[10];
			WCHAR slash4[10];
			WCHAR slash5[10];
			WCHAR slash6[10];

			UINT v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%2s%d%1s%d%1s%d%d%1s%d%1s%d%d%1s%d%1s%d", first,
				&v1, slash1, &vt1, slash2, &vn1,
				&v2, slash3, &vt2, slash4, &vn2,
				&v3, slash5, &vt3, slash6, &vn3);

			Index input;

			input.v = v1 - 1;
			input.vt = vt1 - 1;
			input.vn = vn1 - 1;

			if (newMaterial)
			{
				input.newMaterial = true;
				wcscpy(input.materialName, materialName);
				newMaterial = false;
			}

			vectorIndices.push_back(input);
			input.newMaterial = false;

			input.v = v2 - 1;
			input.vt = vt2 - 1;
			input.vn = vn2 - 1;
			vectorIndices.push_back(input);

			input.v = v3 - 1;
			input.vt = vt3 - 1;
			input.vn = vn3 - 1;
			vectorIndices.push_back(input);

			continue;
		}

		// get mtl file name
		if (line.compare(0, 7, L"mtllib ") == 0)
		{
			WCHAR first[10], mtlFileName[200];

			WCHAR oldStyleStr[200];
			wcscpy(oldStyleStr, line.c_str());
			swscanf(oldStyleStr, L"%7s%s", first, &mtlFileName);

			// read the mtl file
			// we need to prepend the folder name and stuff to the mtl file name
			WCHAR fullMtlFilePath[200];
			wcscpy(fullMtlFilePath, folder);
			wcsncat(fullMtlFilePath, mtlFileName, 200);

			readMtlFile(&vectorTextureFiles, fullMtlFilePath, folder);

			continue;
		}
	}


	//******************************************************************//
	// SIMPLIFICATION													//
	//******************************************************************//

	// so the problem with obj files is that the indices are stored as v/vt/vn
	// whereas directx can only handle indices of ...vertices (as far as I'm aware)
	// so what we need to do is find the indices that are completely unique (completely original combination of v/vt/vn)
	// and make them vertices, referencing them with 1 number instead of 3

	std::vector <SimpleVertex>	simplifiedVertices(0);
	std::vector <Index>			uniqueIndices(0);
	std::vector <MeshMaterial>		vectorSubsets(0);


	USHORT uniqueIndexCount = 0;
	USHORT subsetIndexCount = 0;
	USHORT totalIndexCount = 0;

	// for each index
	for (int i = 0; i < vectorIndices.size(); i++)
	{
		Index idx = vectorIndices[i];

		// if idx is the first index of a new material
		if (idx.newMaterial)
		{
			// if this is not the first subset, we need to set the index count of the previous subset
			if (vectorSubsets.size() > 0)
			{
				vectorSubsets.back().subset.ic = subsetIndexCount;
				subsetIndexCount = 0;
			}
		
			// create a new subset
			// this means we're creating a new subset even if we've already created one for the same material earlier on
			// (and if the obj file doesn't start with a material this will go horribly wrong)
			MeshMaterial meshMaterial;
			meshMaterial.subset.is = totalIndexCount;
			meshMaterial.subset.vs = simplifiedVertices.size();
			// also, we need to wipe the uniqueIndices vector, because we don't want indices from different subsets referencing the same vertices
			uniqueIndices.clear();
			uniqueIndexCount = 0;

			// find the material properties
			// which at the moment consists of just the texture
			for (int i = 0; i < vectorTextureFiles.size(); i++)
			{
				// if the material name matches
				if (wcscmp(idx.materialName, vectorTextureFiles[i].materialName) == 0)
				{
					Material * material = &vectorTextureFiles[i];
					if (material->hasTexture)
					{
						wcscpy(meshMaterial.textureFilePath, material->textureFilePath);
						meshMaterial.hasTexture = true;
					}

					//meshMaterial.subset.color = vectorTextureFiles[i].color;
				}
			}

			vectorSubsets.push_back(meshMaterial);
		}


		std::vector <Index> matchingV(0);

		// of uniqueIndices, find matching v
		for (size_t i = 0; i < uniqueIndices.size(); i++)
		{
			Index vert = uniqueIndices[i];
			if (idx.v == vert.v)
			{
				// add to matchingV
				matchingV.push_back(vert);
			}
		}

		std::vector <Index> matchingVn(0);
		// of the ones we found, find matching vn
		for (size_t i = 0; i < matchingV.size(); i++)
		{
			Index vert = matchingV[i];
			if (idx.vn == vert.vn)
			{
				// add to matchingVn
				matchingVn.push_back(vert);
			}
		}

		std::vector <Index> matchingVt(0);
		// of the ones we found, find matching vt
		for (size_t i = 0; i < matchingVn.size(); i++)
		{
			Index vert = matchingVn[i];
			if (idx.vt == vert.vt)
			{
				// add to matchingVn
				matchingVt.push_back(vert);
			}
		}

		USHORT thisIndex;

		// if matchingVt is empty then this is unique
		if (matchingVt.size() == 0)
		{
			// create a uniqueIndex
			Index uniqueIndex = idx;
			uniqueIndex.vertex = uniqueIndexCount;
			uniqueIndices.push_back(uniqueIndex);

			// this is where we also create the vertex
			SimpleVertex newVertex;
			// we have to get the data from the vectors
			newVertex.Pos = vectorVertices[idx.v];
			newVertex.TexUV = vectorUVs[idx.vt];
			newVertex.VecNormal = vectorNormals[idx.vn];
			
			simplifiedVertices.push_back(newVertex);

			// also do this
			thisIndex = uniqueIndexCount;
			uniqueIndexCount++;
		}
		else
		{
			// otherwise there should be one left, meaning we got a complete match
			// now we can reference that vertex instead
			thisIndex = matchingVt[0].vertex;
		}
		
		vectorIndices[i].vertex = thisIndex;
		subsetIndexCount++;
		totalIndexCount++;
	}

	// we need to set the index count of the final subset here
	// we're not checking if vectorSubsets.size() is zero because that should never happen!
	vectorSubsets.back().subset.ic = subsetIndexCount;

	//******************************************************************//
	// SUBSETS															//
	//******************************************************************//

	objMesh.numSs = vectorSubsets.size();
	objMesh.subsets = new ObjMeshSubset[objMesh.numSs];
	for (int i = 0; i < objMesh.numSs; i++)
	{
		objMesh.subsets[i] = vectorSubsets[i].subset;

		// also do the textures while we're here
		ObjMeshSubset * subset = &objMesh.subsets[i];
		subset->hastxtrz = vectorSubsets[i].hasTexture;

		if (subset->hastxtrz)
		{
			// create the texture resource
			D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
				vectorSubsets[i].textureFilePath,
				NULL, NULL,
				&subset->txtr,		// This is returned.
				NULL);
			DXUT_SetDebugName(subset->txtr, "MESH_TEXTURE");
		}
	}

	//******************************************************************//
	// VERTICES															//
	//******************************************************************//

	objMesh.numVertices = simplifiedVertices.size();
	objMesh.vertices = new SimpleVertex[objMesh.numVertices];
	for (int i = 0; i < objMesh.numVertices; i++)
		objMesh.vertices[i] = simplifiedVertices[i];


	//******************************************************************//
	// INDICES															//
	//******************************************************************//

	objMesh.numIndices = vectorIndices.size();
	objMesh.indexes = new USHORT[objMesh.numIndices];
	for (int i = 0; i < objMesh.numIndices; i++)
		objMesh.indexes[i] = vectorIndices[i].vertex;


	//******************************************************************//
	// BUFFERS															//
	//******************************************************************//

	// Create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * objMesh.numVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = objMesh.vertices;

	pd3dDevice->CreateBuffer(&bd, &InitData, &objMesh.vb);
	DXUT_SetDebugName(objMesh.vb, "VERTEX_BUFFER");

	// create the index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(USHORT) * objMesh.numIndices;

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = objMesh.indexes;

	pd3dDevice->CreateBuffer(&bd, &InitData, &objMesh.ib);
	DXUT_SetDebugName(objMesh.ib, "INDEX_BUFFER");
}

ObjMeshInfo::~ObjMeshInfo()
{
	SAFE_RELEASE(objMesh.vb);
	SAFE_RELEASE(objMesh.ib);

	for (int i = 0; i < objMesh.numSs; i++)
	{
		ObjMeshSubset * subset = &objMesh.subsets[i];

		if (subset->hastxtrz)
			// unload textures
			SAFE_RELEASE(subset->txtr);
	}

	// delete subsets
	delete[] objMesh.subsets;
	// delete vertices
	delete[] objMesh.vertices;
	// delete indices
	delete[] objMesh.indexes;
}