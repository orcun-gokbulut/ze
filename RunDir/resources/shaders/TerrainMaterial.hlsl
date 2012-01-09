//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - TerrainMaterial.hlsl
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"

//* Vertex Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Transformation
float4x4 ViewProjMatrix : register(vs, c0);
float4x4 WorldMatrix : register(vs, c4);
float4x4 ViewMatrix : register(vs, c8);
float4 VertexShaderParameters : register(c13);

// Pixel Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////
// Fixed Material Properties
float4 PipelineParamatersPS[10] : register(ps, c0);
float4 MaterialParametersPS[2] : register(ps, c10);

#define	MaterialAmbientColor    MaterialParametersPS[0].xyz
#define	MaterialDiffuseColor    MaterialParametersPS[1].xyz
#define ScreenToTextureParams	PipelineParamatersPS[0]
#define TextureSize				VertexShaderParameters.xy
#define HeightAdjustments		VertexShaderParameters.zw


// Textures
/////////////////////////////////////////////////////////////////////////////////////////
sampler2D HeightTexture : register(vs, s0);
sampler2D ColorTexture  : register(s4);
sampler2D NormalTexture : register(s5);

struct ZETerrainMaterial_VSInput 
{
	float4 Position 	: POSITION0;
};

// G-Buffer Pass
/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
struct ZETerrainMaterial_GBuffer_VSOutput 
{
	float4 Position_ : POSITION0;
	float3 ViewPosition : TEXCOORD0;
	float2 Texcoord : TEXCOORD1;
	float3 Matrix0 : TEXCOORD2;
	float3 Matrix1 : TEXCOORD3;
	float3 Matrix2 : TEXCOORD4;
};

ZETerrainMaterial_GBuffer_VSOutput ZETerrainMaterial_GBuffer_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_GBuffer_VSOutput Output;

	float4 WorldPosition = mul(WorldMatrix, Input.Position);
	Output.Texcoord = (float2(WorldPosition.x, -WorldPosition.z) + TextureSize / 2) / TextureSize + 1.0f / TextureSize * 2.0f;
	float Height = tex2Dlod(HeightTexture, float4(Output.Texcoord, 0.0f, 1.0f)).r;
	WorldPosition.y  = HeightAdjustments.x + HeightAdjustments.y * Height;

	Output.Position_ = mul(ViewProjMatrix, WorldPosition);
	Output.ViewPosition = mul(ViewMatrix, WorldPosition);
	Output.Matrix0 = (float3)ViewMatrix[0];
	Output.Matrix1 = (float3)ViewMatrix[1];
	Output.Matrix2 = (float3)ViewMatrix[2];
	return Output;
}

// Pixel Shader
struct ZETerrainMaterial_GBuffer_PSInput
{
	float3 ViewPosition : TEXCOORD0;
	float2 Texcoord : TEXCOORD1;
	float3 Matrix0 : TEXCOORD2;
	float3 Matrix1 : TEXCOORD3;
	float3 Matrix2 : TEXCOORD4;
};

ZEGBuffer ZETerrainMaterial_GBuffer_PixelShader(ZETerrainMaterial_GBuffer_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;
	
	float3x3 Matrix = float3x3(Input.Matrix0, Input.Matrix1, Input.Matrix2);
	
	float3 PolygonDispX = ddx(Input.ViewPosition);
	float3 PolygonDispY = ddy(Input.ViewPosition);
	float3 PolygonNormal = normalize(cross(PolygonDispX, PolygonDispY));

	float PixelSize = 1.0f / TextureSize;
	
	float s0 = HeightAdjustments.y * tex2D(NormalTexture, Input.Texcoord).r;		       	
	float s1 = HeightAdjustments.y * tex2D(NormalTexture, Input.Texcoord + float2(-PixelSize.x, 0.0f)).r;		       	
	float s2 = HeightAdjustments.y * tex2D(NormalTexture, Input.Texcoord + float2(0.0f, -PixelSize.x)).r;		       	
	float s3 = HeightAdjustments.y * tex2D(NormalTexture, Input.Texcoord + float2(PixelSize.x, 0.0f)).r;		       	
	float s4 = HeightAdjustments.y * tex2D(NormalTexture, Input.Texcoord + float2(0.0f, PixelSize.x)).r;		
	
	float3 v1 = normalize(float3(-1.0f, s1 - s0, 0.0f));
	float3 v2 = normalize(float3(0.0f, s2 - s0, -1.0f));
	float3 v3 = normalize(float3(1.0f, s3 - s0, 0.0f));
	float3 v4 = normalize(float3(0.0f, s4 - s0, 1.0f));
	 
	float3 n1 = normalize(cross(v1, v2));	
	float3 n2 = normalize(cross(v2, v3));
	float3 n3 = normalize(cross(v3, v4));
	float3 n4 = normalize(cross(v4, v1));
	
	float3 n = normalize(n1 + n2 + n3 + n4);
	
	//float coef = 0.02;
	//float3 Normal = normalize(float3((s1 - s3), 1.0f, (s2 - s4)));
	ZEGBuffer_SetViewNormal(GBuffer, mul(Matrix, -n));
	// Smootering group fix
	/*if (dot(PolygonNormal, VertexNormal) < 0.3)*/
	//	ZEGBuffer_SetViewNormal(GBuffer, PolygonNormal);
	/*else
		ZEGBuffer_SetViewNormal(GBuffer, VertexNormal);*/
	
	//ZEGBuffer_SetViewNormal(GBuffer, tex2D);
		
	//ZEGBuffer_SetViewNormal(GBuffer, normalize(mul(Matrix, (2.0f * tex2D(NormalTexture, Input.Texcoord).rbg - 1.0f) * float3(1.0f, 1.0f / HeightAdjustments.y, 1.0f))));

	ZEGBuffer_SetViewPosition(GBuffer, Input.ViewPosition);
	ZEGBuffer_SetSpecularGlossiness(GBuffer, 120.0f);	
	ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, 0.0f);

	return GBuffer;
}

// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct ZETerrainMaterial_ForwardPass_VSOutput
{
	float4 Position_ : POSITION0;
	float2 Texcoord : TEXCOORD0;
};

ZETerrainMaterial_ForwardPass_VSOutput ZETerrainMaterial_ForwardPass_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_ForwardPass_VSOutput Output;

	float4 WorldPosition = mul(WorldMatrix, Input.Position);
	
	Output.Texcoord = (float2(WorldPosition.x, -WorldPosition.z) + TextureSize / 2) / TextureSize + 1.0f / TextureSize * 2.0f;
	float Height = tex2Dlod(HeightTexture, float4(Output.Texcoord, 0.0f, 1.0f)).r;
	WorldPosition.y  = HeightAdjustments.x + HeightAdjustments.y * Height;

	Output.Position_ = mul(ViewProjMatrix, WorldPosition);
	
	return Output;
}

struct ZETerrainMaterial_ForwardPass_PSOutput
{
	float4	Color : COLOR0;
};

struct ZETerrainMaterial_ForwardPass_PSInput
{
	float3 ScreenPosition : VPOS;
	float2 Texcoord : TEXCOORD0;
};

ZETerrainMaterial_ForwardPass_PSOutput ZETerrainMaterial_ForwardPass_PixelShader(ZETerrainMaterial_ForwardPass_PSInput Input)
{
	ZETerrainMaterial_ForwardPass_PSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		

	float3 BaseColor = tex2D(ColorTexture, Input.Texcoord).rgb;
	
	float3 AmbientColor = MaterialAmbientColor;
	AmbientColor *= BaseColor;
	Output.Color.rgb += AmbientColor;

	float3 DiffuseColor = MaterialDiffuseColor;
	DiffuseColor = BaseColor;
	DiffuseColor *= ZELBuffer_GetDiffuse(ScreenPosition);
	Output.Color.rgb += DiffuseColor;

	return Output;
}
