#include "Renderer.h"

#include "DXUT.h"
#include "ObjMesh.h"
#include "GameContext.h"
#include "RenderingContext.h"

void Renderer::setMesh(ID3D11DeviceContext* pd3dImmediateContext, ObjMesh *mesh)
{
	// set the input assembly
	UINT Strides;
	UINT Offset;
	ID3D11Buffer* pVB;
	pVB = mesh->vertexBuffer();
	Strides = mesh->stride();
	Offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &pVB, &Strides, &Offset);
	pd3dImmediateContext->IASetIndexBuffer(mesh->indexBuffer(), mesh->ibFormat(), 0);
}

void Renderer::renderMesh(ID3D11DeviceContext* pd3dImmediateContext, ObjMesh *mesh)
{
	// render per subset
	for (UINT subset = 0; subset < mesh->numSubsets(); ++subset)
	{
		// Get the subset
		ObjMeshSubset *pSubset = mesh->getSubset(subset);

		pd3dImmediateContext->IASetPrimitiveTopology(pSubset->primitiveTopology());

		// not using subset color anymore
		// updateBuffersPerObject(pd3dImmediateContext, worldMatrix, matWorldViewProjection, pSubset->getColor());

		// a much more efficient way of doing this would be to make sure every shader has as many texture registers as possible
		// then we can load as many textures as we can at the start
		// and just update the textureNumber

		if (pSubset->hasTexture())
		{
			ID3D11ShaderResourceView *texture = pSubset->texture();
			pd3dImmediateContext->PSSetShaderResources(0, 1, &texture);
		}
		else
		{
			// for some reason the default texture is full black. It 'just works' if you make it white instead
			pd3dImmediateContext->PSSetShaderResources(0, 1, &gameContext->renderingContext->defaultTexture);
		}

		pd3dImmediateContext->DrawIndexed(pSubset->indexCount(), pSubset->indexStart(), pSubset->vertexStart());
	}
}