#pragma once

#include <windows.h>
#include "DXUT.h"
#include <xnamath.h>

#include "vector3.h"
#include "GameInfo.h"
#include "renderer.h"

class ShaderPair;
class NigSound;
class ObjMesh;

class GameObject : public GameInfo
{
public:
	GameObject();
	GameObject(int meshIndex);
	~GameObject();

	ObjMesh *sharedMesh = NULL;

	vector3 position = vector3(0, 0, 0);
	vector3 rotation = vector3(0, 0, 0); // in radians
	vector3 scale = vector3(1, 1, 1);


	// at the moment you can only have a depth of one
	// i.e if the parent has a parent it won't work
	GameObject * parent = NULL;
	//GameObject * child[MaxChildren];

	Renderer *renderer = NULL;
	ShaderPair *shaderPair = NULL;

	XMMATRIX transformMatrix = XMMatrixIdentity();

	void(*customBehaviour)(GameObject * _this, float fTime) = NULL;
	virtual void onFrameMove(double fTime, float fElapsedTime); 
	void updateTransform();
	void render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
};