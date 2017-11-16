#include "StandardRenderer.h"

#include "GameObject.h"
#include "GameContext.h"
#include "RenderingContext.h"
#include "Camera.h"
#include "ShaderPair.h"

void StandardRenderer::render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, GameObject* gameObject)
{
	// simplest possible renderer

	// get world matrix
	XMMATRIX worldMatrix = gameObject->transformMatrix;

	// only supports one parent at moment
	if (gameObject->parent)
	{
		// for some reason it breaks if you remove this line...
		// it could be something to do with the warning C4316
		XMMATRIX parentTransform = gameObject->parent->transformMatrix;

		worldMatrix *= parentTransform;
	}

	GameContext * gameContext = gameObject->gameContext;

	XMMATRIX viewMatrix = gameContext->renderingContext->primaryCamera->viewMatrix;
	XMMATRIX projectionMatrix = gameContext->renderingContext->projectionMatrix;
	XMMATRIX matWorldViewProjection = worldMatrix * viewMatrix * projectionMatrix;

	// update constant buffers
	gameObject->shaderPair->updateBuffersPerObject(pd3dImmediateContext, worldMatrix, matWorldViewProjection, XMFLOAT4(1, 1, 1, 1));

	setMesh(pd3dImmediateContext, gameObject->sharedMesh);

	renderMesh(pd3dImmediateContext, gameObject->sharedMesh);
}
