#include "GameObject.h"

#include "Camera.h"
#include "RenderingContext.h"
#include "EventManager.h"
#include "GameObjectManager.h"
#include "MeshManager.h"
#include "GameContext.h"
#include "ObjMeshInfo.h"
#include "ShaderPair.h"
#include "Utils.h"

GameObject::GameObject()
{
	// register self
	gameContext->gameObjectManager->registerGameObject(this);

	// subscribe to events
	__hook(&EventManager::OnFrameMove, gameContext->eventManager, &GameObject::onFrameMove);
}

GameObject::GameObject(int meshIndex) : GameObject()
{
	sharedMesh = gameContext->meshManager->meshes[meshIndex]->mesh();

	// set the default shader
	shaderPair = gameContext->renderingContext->shaders[0];

	// set default renderer
	renderer = gameContext->renderingContext->renderers[0];
}

GameObject::~GameObject()
{
	//gameObjectManager->unregisterGameObject(this);
  
	// unsubscribe from events
	__unhook(&EventManager::OnFrameMove, gameContext->eventManager, &GameObject::onFrameMove);
}

// this could update AllRotations and Direction as well
void GameObject::updateTransform()
{
	// updates its own local transformation matrix
	transformMatrix = Utils::getTransformMatrix(position, rotation, scale);
}

void GameObject::render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext)
{
	if (!sharedMesh) return;

	// render
	renderer->render(pd3dDevice, pd3dImmediateContext, this);
}

void GameObject::onFrameMove(double fTime, float fElapsedTime)
{
	if (customBehaviour)
		customBehaviour(this, fTime);
}
