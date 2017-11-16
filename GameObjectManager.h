#pragma once

#include "DXUT.h"

class GameObject;

const int MaxGameObjects = 100;
const int MaxChildren = 10;

class GameObjectManager
{
public:
	// can't be bothererd to do dynamic memory allocation, 100 will do
	GameObject *gameObjects[MaxGameObjects] = {};
	int gameObjectCount = 0;

	~GameObjectManager();

	void registerGameObject(GameObject *gameObject);
	//void unregisterGameObject(GameObject *gameObject);
	void render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext);
	void updateTransforms();
};