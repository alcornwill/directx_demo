//**************************************************************************//
// PIXEL SHADER for the DXUT meshes sample.									//
//																			//
// Modifications to the MS sample code is copyright of Dr Nigel Barlow,		//
// lecturer in computing, University of Plymouth, UK.						//
// email: nigel@soc.plymouth.ac.uk.											//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//**************************************************************************//
//--------------------------------------------------------------------------------------
// File: Tutorial 09 - Meshes Using DXUT Helper Classes_PS.hlsl
//
// The pixel shader file for the Tutorial 09 - Meshes Using DXUT Helper Classes sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


cbuffer cbPerObject : register(b0)
{
	float4		g_vObjectColor;
	float4		offsetUV;
};


Texture2D	 g_txDiffuse : register(t0);
Texture2D	 g_txNoise	 : register(t1); // should be a binary image

SamplerState g_samLinear : register(s0);


struct PS_INPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
};


// diffuse and object color, no lighting
float4 main(PS_INPUT Input) : SV_TARGET
{
	float txScale = 22;
	float4 vDiffuse = g_txDiffuse.Sample(g_samLinear, (Input.vTexcoord*txScale)+(offsetUV.xy*txScale));
	// we're using the noise texture to create individual blades
	// so that the base texture can just be a normal texture - you can't see through the floor
	float4 vNoise = g_txNoise.Sample(g_samLinear, (Input.vTexcoord*txScale) + (offsetUV.xy*txScale));
	vDiffuse *= g_vObjectColor;
	vDiffuse.w = vNoise.x; // only uses red channel
	return vDiffuse;
}
