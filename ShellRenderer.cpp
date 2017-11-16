#include "ShellRenderer.h"

#include "GameObject.h"
#include "GameContext.h"
#include "RenderingContext.h"
#include "Camera.h"
#include "ShaderPair.h"
#include "Utils.h"


void ShellRenderer::render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, GameObject * gameObject)
{
	// shell renderer. The technique they use to render hair.

	XMMATRIX viewMatrix = gameContext->renderingContext->primaryCamera->viewMatrix;
	XMMATRIX projectionMatrix = gameContext->renderingContext->projectionMatrix;

	setMesh(pd3dImmediateContext, gameObject->sharedMesh);

	XMMATRIX worldMatrix;
	XMMATRIX matWorldViewProjection;

	float ofsx = cos(from)*w;
	float ofsy = sin(from)*h;

	float step = length / numLayers;

	float color = 0.0;
	// for each layer
	for (float i = from; i < from+length; i+=step)
	{
		// render, using trigonometry to create nice bendy grass
		// the vertex shader should modify the vertex normals, making the blades wave up and down
		worldMatrix = gameObject->transformMatrix * XMMatrixTranslation(0, -ofsx+cos(i)*w, -ofsy+sin(i)*h);
		matWorldViewProjection = worldMatrix * viewMatrix * projectionMatrix;	
		gameObject->shaderPair->updateBuffersPerObject(pd3dImmediateContext,
			worldMatrix, matWorldViewProjection,
			XMFLOAT4(color, color, color, 1));
		renderMesh(pd3dImmediateContext, gameObject->sharedMesh);

		// bottom of the grass is darker
		color += colorOffset;
	}
}