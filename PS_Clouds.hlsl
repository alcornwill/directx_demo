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

Texture2D    g_txDiffuse : register(t0); // unused - just the grass texture
Texture2D	 g_txClouds1 : register(t1);
Texture2D	 g_txClouds2 : register(t2);

SamplerState g_samLinear : register(s0);


struct PS_INPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
};


// adds two different cloud textures together with an offset
float4 main(PS_INPUT Input) : SV_TARGET
{
	float txScale = 2;
	float4 clouds1 = g_txClouds1.Sample(g_samLinear, (Input.vTexcoord*txScale) + (offsetUV.xy*txScale));
	float4 clouds2 = g_txClouds2.Sample(g_samLinear, (Input.vTexcoord*txScale) + (offsetUV.zw*txScale));
	float4 sum = clouds1 + clouds2 - float4(1, 1, 1, 0);
	sum *= 3;
	sum.w = sum.x;
	sum.xyz = clamp(sum.xyz, 0.9, 1); // get rid of grey bits
	sum = clamp(sum, 0, 1);


	return sum * g_vObjectColor;
}
