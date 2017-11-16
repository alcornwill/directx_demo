#include "GameObjectManager.h"

#include "DXUT.h"
#include "GameObject.h"
#include "ShaderPair.h"

void GameObjectManager::registerGameObject(GameObject *gameObject)
{
	gameObjects[gameObjectCount] = gameObject;
	gameObjectCount++;
}

void GameObjectManager::render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	// this is not very optimal because:
	//   we always set the mesh, regardless of whether it was already set
	//   we always set the shader too (if there is one)
	// and even if we were doing that properly
	// to minimize overhead you'd have to sort the gameobjects inplace
	// or have some way of rendering them in order

	for (int i = 0; i < MaxGameObjects; i++)
	{
		GameObject *gameObject = gameObjects[i];
		if (!gameObject) break;
		if (gameObject->shaderPair) gameObject->shaderPair->setShader(pd3dDevice, pd3dImmediateContext);
		gameObject->render(pd3dDevice, pd3dImmediateContext);
	}
}

void GameObjectManager::updateTransforms()
{
	for (int i = 0; i < MaxGameObjects; i++)
	{
		GameObject *gameObject = gameObjects[i];
		if (!gameObject) break;
		gameObject->updateTransform();
	}
}

GameObjectManager::~GameObjectManager()
{
	// delete game objects
	for (int i = 0; i < MaxGameObjects; i++)
	{
		GameObject * gameObject = gameObjects[i];
		if (!gameObject) break;
		delete gameObject;
	}
}