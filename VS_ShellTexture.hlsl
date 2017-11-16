//**************************************************************************//
// VERTEX SHADER for the DXUT meshes sample.								//
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
// File: BasicHLSL11_VS.hlsl
//
// The vertex shader file for the BasicHLSL11 sample.  
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


cbuffer cbPerObject : register( b0 )
{
	matrix		g_mWorldViewProjection;	
	matrix		g_mWorld;				
};

//float2 windVec = float2(1.0, 0.0); // this is normalized
//float windStrength = 0.5;

cbuffer cbWindOffset : register(b1)
{
	float2 offset1;
	float2 offset2;
};

Texture2D	 g_txWind1   : register(t0);
Texture2D	 g_txWind2	 : register(t1);

SamplerState g_samLinear : register(s0);

struct VS_INPUT
{
	float4 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
};


//**************************************************************************//
// Vertex shader output structure.  This is usually plugges into the Pixel	//
// shader input, so the pixel shader input  structure.						//
//																			//
// NOTE: Pos has a different samentic to the structure above; get it wrong	//
// and nothing works.  That's because the pixel shader is in a different	//
// stage in the rendering pipeline.											//
//**************************************************************************//
struct VS_OUTPUT
{
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float4 vPosition	: SV_POSITION;
};


//**************************************************************************//
// Vertex Shader.															//
//**************************************************************************//
VS_OUTPUT main( VS_INPUT Input )
{
	float scale = 1;
	float4 vWind1 = g_txWind1.SampleLevel(g_samLinear, (Input.vTexcoord*scale) + (offset1*scale), 0);
	float4 vWind2 = g_txWind2.SampleLevel(g_samLinear, (Input.vTexcoord*scale) + (offset2*scale), 0);
	// add the wind values (which gives us a wind value from 0 to 2)
	// then ignore the 0 - 1 part. Should look sort of random
	float vWind = clamp(vWind1.x + vWind2.x, 1, 2) - 1;
	float4 pos = Input.vPosition;
	pos.x += vWind/2.0;

	VS_OUTPUT Output;
	Output.vPosition = mul(pos, g_mWorldViewProjection );
	Output.vNormal = mul(Input.vNormal, (float3x3)g_mWorld );
	Output.vTexcoord = Input.vTexcoord;
	
	return Output;
}