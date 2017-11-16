#pragma once

// this is an abstract class
// if you want to render an object with multiple draw calls, in a special way, you need a new renderer
// renderers are shared, and each GameObject has a renderer and a shader

#include "DXUT.h"
#include "ObjMesh.h"
#include "GameInfo.h"

class GameObject;

class Renderer : public GameInfo
{
protected:
	void setMesh(ID3D11DeviceContext* pd3dImmediateContext, ObjMesh *mesh);
	void renderMesh(ID3D11DeviceContext* pd3dImmediateContext, ObjMesh *mesh); // this part of rendering is the same for all renderers

public:
	virtual void render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, GameObject* gameObject) = 0;
};
