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
float4x4 ViewMatrix : register(c8);
float4 VertexShaderParameters[4] : register(c13);

float4x4 LocalTransform : register(c20);

// Pixel Shader Constants
/////////////////////////////////////////////////////////////////////////////////////////
// Fixed Material Properties
float4 PipelineParamatersPS[1] : register(ps, c0);
float4 MaterialParametersPS[3] : register(ps, c1);

#define	MaterialAmbientColor    MaterialParametersPS[0].xyz
#define	MaterialDiffuseColor    MaterialParametersPS[1].xyz
#define ScreenToTextureParams	PipelineParamatersPS[0]
#define HeightAdjustments		VertexShaderParameters[0].xy
#define HeightAdjustmentsNormal	VertexShaderParameters[0].zw
#define TextureOffset			VertexShaderParameters[1].xy
#define TextureScale			VertexShaderParameters[1].zw
#define BlendTreshold			VertexShaderParameters[2]
#define ColorTextureOffset		VertexShaderParameters[3].xy
#define ColorTextureScale		VertexShaderParameters[3].zw


// Textures
/////////////////////////////////////////////////////////////////////////////////////////
sampler2D HeightTexture : register(s0);
sampler2D ColorTexture  : register(s4);
sampler2D DetailNormalTexture : register(s5);

float2 GetTexcoord(float3 LocalPosition)
{
	return TextureScale * float2(LocalPosition.x, -LocalPosition.z) + TextureOffset;
}

float2 GetGlobalTexcoord(float3 WorldPosition)
{
	float2 B = ColorTextureScale * float2(WorldPosition.x, -WorldPosition.z) + ColorTextureOffset;
	return float2(B.x, 1.0 - B.y);
}

float GetBlendFactor(float3 LocalPosition)
{
	float2 Blend = (abs(LocalPosition.xz) - BlendTreshold.x) / BlendTreshold.y;
	return saturate(max(Blend.x, Blend.y));
}

float3 GetLocalPosition(float3 VertexPosition)
{
	return mul(LocalTransform, float4(VertexPosition, 1.0f));
}

float GetHeight(float3 LocalPosition, float BlendFactor)
{
	float2 Height = tex2Dlod(HeightTexture, float4(GetTexcoord(LocalPosition), 0.0f, 1.0f)).rg;
	return HeightAdjustments.x + HeightAdjustments.y * lerp(Height.x, Height.y, BlendFactor);
}

float GetHeightLow(float3 LocalPosition)
{
	return HeightAdjustmentsNormal.x + HeightAdjustmentsNormal.y * tex2Dlod(HeightTexture, float4(GetTexcoord(LocalPosition), 0.0f, 1.0f)).g;
}

float GetHeightHigh(float3 LocalPosition)
{
	return HeightAdjustmentsNormal.x + HeightAdjustmentsNormal.y * tex2Dlod(HeightTexture, float4(GetTexcoord(LocalPosition), 0.0f, 1.0f)).r;
}

float3 GetVertexNormal(float3 LocalPosition, float BlendFactor)
{
	float3 LeftPosition = LocalPosition + float3(-1.0f, 0.0f,  0.0f);
	float3 RightPosition = LocalPosition + float3(1.0f, 0.0f, 0.0f);
	float3 ForwardPosition = LocalPosition + float3(0.0f, 0.0f, 1.0f);
	float3 BackwardPosition = LocalPosition + float3(0.0f, 0.0f, -1.0f);

	LocalPosition.y = GetHeightHigh(LocalPosition);
	LeftPosition.y = GetHeightHigh(LeftPosition);
	RightPosition.y = GetHeightHigh(RightPosition);
	ForwardPosition.y = GetHeightHigh(ForwardPosition);
	BackwardPosition.y = GetHeightHigh(BackwardPosition);
	
	float3 NormalHigh = normalize(cross(LeftPosition - LocalPosition, ForwardPosition - LocalPosition));
	NormalHigh += normalize(cross(BackwardPosition - LocalPosition, LeftPosition - LocalPosition));
	NormalHigh += normalize(cross(RightPosition - LocalPosition, BackwardPosition - LocalPosition));
	NormalHigh += normalize(cross(ForwardPosition - LocalPosition, RightPosition - LocalPosition));
	NormalHigh = normalize(0.25f * NormalHigh);

	LeftPosition = LocalPosition + float3(-2.0f, 0.0f,  0.0f);
	RightPosition = LocalPosition + float3(2.0f, 0.0f, 0.0f);
	ForwardPosition = LocalPosition + float3(0.0f, 0.0f, 2.0f);
	BackwardPosition = LocalPosition + float3(0.0f, 0.0f, -2.0f);
	
	LocalPosition.y = GetHeightLow(LocalPosition);
	LeftPosition.y = GetHeightLow(LeftPosition);
	RightPosition.y = GetHeightLow(RightPosition);
	ForwardPosition.y = GetHeightLow(ForwardPosition);
	BackwardPosition.y = GetHeightLow(BackwardPosition);

	float3 NormalLow = normalize(cross(LeftPosition - LocalPosition, ForwardPosition - LocalPosition));
	NormalLow += normalize(cross(BackwardPosition - LocalPosition, LeftPosition - LocalPosition));
	NormalLow += normalize(cross(RightPosition - LocalPosition, BackwardPosition - LocalPosition));
	NormalLow += normalize(cross(ForwardPosition - LocalPosition, RightPosition - LocalPosition));
	NormalLow = normalize(0.25f * NormalLow);
	
	return normalize(lerp(NormalHigh, NormalLow, BlendFactor));
}

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
	float3 Position : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 Texcoord : TEXCOORD2;
	float3 LocalPosition : TEXCOORD3;
	float2 CullBlendFactor : TEXCOORD4;
	float2 GlobalTexcoord : TEXCOORD5;
	float3 Tangent : TEXCOORD6;
	float3 Binormal : TEXCOORD7;
};

ZETerrainMaterial_GBuffer_VSOutput ZETerrainMaterial_GBuffer_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_GBuffer_VSOutput Output;
	float3 LocalPosition = GetLocalPosition(Input.Position);
	
	Output.Texcoord = GetTexcoord(LocalPosition);
	LocalPosition.y = GetHeight(LocalPosition, GetBlendFactor(LocalPosition));

	float4 WorldPosition = mul(WorldMatrix, float4(LocalPosition, 1.0f));
	Output.Position_ = mul(ViewProjMatrix, WorldPosition);
	Output.Position = mul(ViewMatrix, WorldPosition);
	
	Output.GlobalTexcoord = GetGlobalTexcoord(WorldPosition);

	float3 Normal = GetVertexNormal(LocalPosition, GetBlendFactor(LocalPosition));
	float3 Binormal = normalize(cross(float3(1.0f, 0.0f, 0.0f), Normal));
	float3 Tangent = normalize(cross(Normal, Binormal));
	
	Output.Normal = mul((float3x3)ViewMatrix, Normal);
	Output.Tangent = mul((float3x3)ViewMatrix, Tangent);
	Output.Binormal = mul((float3x3)ViewMatrix, Binormal);
	
	Output.LocalPosition = LocalPosition;
	Output.CullBlendFactor.x = GetHeight(LocalPosition, GetBlendFactor(LocalPosition));
	Output.CullBlendFactor.y = GetBlendFactor(LocalPosition);
	
	return Output;
}

// Pixel Shader
struct ZETerrainMaterial_GBuffer_PSInput
{
	float3 Position : TEXCOORD0;
	float3 Normal : TEXCOORD1;
	float2 Texcoord : TEXCOORD2;
	float3 LocalPosition : TEXCOORD3;
	float2 CullBlendFactor : TEXCOORD4;
	float2 GlobalTexcoord : TEXCOORD5;
	float3 Tangent : TEXCOORD6;
	float3 Binormal : TEXCOORD7;
	
};

ZEGBuffer ZETerrainMaterial_GBuffer_PixelShader(ZETerrainMaterial_GBuffer_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;
	
	if (Input.CullBlendFactor.x == 0)
	{
		discard;
		return GBuffer;
	}
	
	float3 PolygonNormal = normalize(cross(ddx(Input.LocalPosition), ddy(Input.LocalPosition)));
	//Input.Normal = GetVertexNormal(Input.LocalPosition, GetBlendFactor(Input.LocalPosition)); //Input.CullBlendFactor.y);
	//Input.Normal = lerp(PolygonNormal, Input.Normal, dot(PolygonNormal, Input.Normal) + 0.5f);
	
	ZEGBuffer_SetViewPosition(GBuffer, Input.Position);
	
	
	float DetailBlend = saturate(length(Input.Position) * 0.01f);
	float3 Normal = 2.0f * tex2D(DetailNormalTexture, Input.GlobalTexcoord * 1000.0f).xyz - 1.0f;
	Normal = normalize(Normal.x * normalize(Input.Tangent) + Normal.y * normalize(Input.Binormal) + Normal.z * normalize(Input.Normal));
	ZEGBuffer_SetViewNormal(GBuffer, normalize(lerp(Normal, Input.Normal, DetailBlend)));

	//ZEGBuffer_SetViewNormal(GBuffer, Input.Normal);
	
	ZEGBuffer_SetSpecularGlossiness(GBuffer, 0.1f);	
	ZEGBuffer_SetSubSurfaceScatteringFactor(GBuffer, 0.0f);

	return GBuffer;
}

// Forward Pass
/////////////////////////////////////////////////////////////////////////////////////////
struct ZETerrainMaterial_ForwardPass_VSOutput
{
	float4 Position_ : POSITION0;
	float2 Texcoord : TEXCOORD0;
	float Cull : TEXCOORD1;
};

ZETerrainMaterial_ForwardPass_VSOutput ZETerrainMaterial_ForwardPass_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_ForwardPass_VSOutput Output;
	
	float3 LocalPosition = GetLocalPosition(Input.Position);
	LocalPosition.y = GetHeight(LocalPosition, GetBlendFactor(LocalPosition));
		
	float4 WorldPosition = mul(WorldMatrix, float4(LocalPosition, 1.0f));
	Output.Texcoord = GetGlobalTexcoord(WorldPosition);
	Output.Position_ = mul(ViewProjMatrix, WorldPosition);
	Output.Cull = GetHeight(LocalPosition, GetBlendFactor(LocalPosition));
	
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
	float Cull : TEXCOORD1;
};

ZETerrainMaterial_ForwardPass_PSOutput ZETerrainMaterial_ForwardPass_PixelShader(ZETerrainMaterial_ForwardPass_PSInput Input)
{
	ZETerrainMaterial_ForwardPass_PSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	if (Input.Cull == 0.0f)
	{
		discard;
		return Output;
	}
	
	float2 ScreenPosition = Input.ScreenPosition * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		

	float3 BaseColor = tex2D(ColorTexture, Input.Texcoord).rgb;
	
	float3 AmbientColor = MaterialAmbientColor;
	AmbientColor *= BaseColor;
	Output.Color.rgb += AmbientColor;

	float3 DiffuseColor = MaterialDiffuseColor;
	DiffuseColor *= BaseColor;
	DiffuseColor *= ZELBuffer_GetDiffuse(ScreenPosition);
	Output.Color.rgb += DiffuseColor;
	
	return Output;
}
