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

//* Constants
/////////////////////////////////////////////////////////////////////////////////////////
float4x4 ViewProjMatrix				: register(vs, c0);
float4x4 WorldMatrix				: register(vs, c4);
float4x4 ViewMatrix					: register(vs, c8);
float4x4 LocalMatrix				: register(vs, c12);
float4 VertexShaderParameters[6]	: register(vs, c16);
float4 PipelineParametersPS[1]		: register(ps, c0);
float4 MaterialParameters[3]		: register(ps, c1);

#define ElevationTexcoordTransform		VertexShaderParameters[0]
#define ElevationTexcoordTransformLow	VertexShaderParameters[1]
#define ElevationTextureSize			VertexShaderParameters[2].x
#define ElevationTexelSize				VertexShaderParameters[2].y
#define ElevationScale					VertexShaderParameters[2].z
#define ElevationOffset					VertexShaderParameters[2].w

#define ColorTexcoordTransform			VertexShaderParameters[3]
#define ColorTexcoordTransformLow		VertexShaderParameters[4]

#define BlendOffset						VertexShaderParameters[5].x
#define BlendScale						VertexShaderParameters[5].y



#define ScreenToTextureParams			PipelineParametersPS[0]
#define DiffuseColor					MaterialParameters[0].rgb
#define AmbientColor					MaterialParameters[1].rgb

// Textures
/////////////////////////////////////////////////////////////////////////////////////////
sampler2D ElevationTexture		: register(s0);
sampler2D ElevationTextureLow	: register(s1);
sampler2D ColorTexture			: register(s4);
sampler2D ColorTextureLow		: register(s5);

float2 GetTexcoord(float3 WorldPosition, float4 Transform)
{
	return Transform.xy * float2(WorldPosition.x, -WorldPosition.z) + Transform.zw;
}

float GetBlendFactor(float3 LocalPosition)
{
	float2 Blend = (abs(LocalPosition.xz) - BlendOffset) * BlendScale;
	return saturate(max(Blend.x, Blend.y));
}

float3 GetLocalPosition(float3 VertexPosition)
{
	return mul(LocalMatrix, float4(VertexPosition, 1.0f)).xyz;
}

float SampleElevation(float3 LocalPosition, float BlendFactor)
{
	float2 Texcoord = GetTexcoord(LocalPosition, ElevationTexcoordTransform);
	float Elevation = tex2Dlod(ElevationTexture, float4(Texcoord, 0.0f, 0.0f)).r;

	if (false)//BlendFactor > 0.0f)
	{
		float2 TexcoordLow = GetTexcoord(LocalPosition, ElevationTexcoordTransformLow);

		float ElevationLow00 = tex2Dlod(ElevationTextureLow, float4(TexcoordLow, 0.0f, 0.0f)).r;
		float ElevationLow01 = tex2Dlod(ElevationTextureLow, float4(TexcoordLow + float2(ElevationTexelSize, 0.0f), 0.0f, 0.0f)).r;
		float ElevationLow10 = tex2Dlod(ElevationTextureLow, float4(TexcoordLow + float2(0.0f, ElevationTexelSize), 0.0f, 0.0f)).r;
		float ElevationLow11 = tex2Dlod(ElevationTextureLow, float4(TexcoordLow + float2(ElevationTexelSize, ElevationTexelSize), 0.0f, 0.0f)).r;
	
		float2 Frac = frac(Texcoord * ElevationTextureSize);
		float ElevationLow = lerp(lerp(ElevationLow00, ElevationLow01, Frac.x), lerp(ElevationLow10, ElevationLow11, Frac.x), Frac.y);
		Elevation = lerp(Elevation, ElevationLow, BlendFactor);
	}

	return ElevationScale * Elevation + ElevationOffset;
}

float3 GetVertexNormal(float3 LocalPosition, float BlendFactor)
{
	float3 LeftPosition = LocalPosition + float3(-1.0f, 0.0f,  0.0f);
	float3 RightPosition = LocalPosition + float3(1.0f, 0.0f, 0.0f);
	float3 ForwardPosition = LocalPosition + float3(0.0f, 0.0f, 1.0f);
	float3 BackwardPosition = LocalPosition + float3(0.0f, 0.0f, -1.0f);

	LeftPosition.y = SampleElevation(LeftPosition, BlendFactor);
	RightPosition.y = SampleElevation(RightPosition, BlendFactor);
	ForwardPosition.y = SampleElevation(ForwardPosition, BlendFactor);
	BackwardPosition.y = SampleElevation(BackwardPosition, BlendFactor);
	
	float3 Normal = normalize(cross(LeftPosition - LocalPosition, ForwardPosition - LocalPosition));
	Normal += normalize(cross(BackwardPosition - LocalPosition, LeftPosition - LocalPosition));
	Normal += normalize(cross(RightPosition - LocalPosition, BackwardPosition - LocalPosition));
	Normal += normalize(cross(ForwardPosition - LocalPosition, RightPosition - LocalPosition));
	Normal = normalize(0.25f * Normal);

	return Normal;
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
};

ZETerrainMaterial_GBuffer_VSOutput ZETerrainMaterial_GBuffer_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_GBuffer_VSOutput Output;

	float4 WorldPosition = mul(WorldMatrix, float4(Input.Position.xyz, 1.0f));
	float BlendFactor = GetBlendFactor(WorldPosition.xyz);

	WorldPosition.y = SampleElevation(WorldPosition.xyz, BlendFactor);
	Output.Position = mul(ViewMatrix, WorldPosition).xyz;
	Output.Position_ = mul(ViewProjMatrix, WorldPosition);

	float3 Normal = GetVertexNormal(WorldPosition.xyz, BlendFactor);
	Output.Normal = mul((float3x3)ViewMatrix, Normal);
	
	return Output;
}

// Pixel Shader
struct ZETerrainMaterial_GBuffer_PSInput
{
	float3 Position : TEXCOORD0;
	float3 Normal : TEXCOORD1;
};

ZEGBuffer ZETerrainMaterial_GBuffer_PixelShader(ZETerrainMaterial_GBuffer_PSInput Input)
{
	ZEGBuffer GBuffer = (ZEGBuffer)0;
	
	/*if (Input.Position.y == 0)
	{
		discard;
		return GBuffer;
	}*/
	
	ZEGBuffer_SetViewPosition(GBuffer, Input.Position);
	ZEGBuffer_SetViewNormal(GBuffer, Input.Normal);

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
	float2 TexcoordLow : TEXCOORD1;
	float BlendFactor : TEXCOORD2;
};

ZETerrainMaterial_ForwardPass_VSOutput ZETerrainMaterial_ForwardPass_VertexShader(ZETerrainMaterial_VSInput Input)
{
	ZETerrainMaterial_ForwardPass_VSOutput Output;
	
	float4 WorldPosition = mul(WorldMatrix, float4(Input.Position.xyz, 1.0f));
	Output.BlendFactor = GetBlendFactor(WorldPosition.xyz);

	WorldPosition.y =  SampleElevation(WorldPosition.xyz, Output.BlendFactor);
	Output.Position_ = mul(ViewProjMatrix, WorldPosition);

	Output.Texcoord = GetTexcoord(WorldPosition.xyz, ColorTexcoordTransform);
	Output.TexcoordLow = GetTexcoord(WorldPosition.xyz, ColorTexcoordTransformLow);

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
	float2 TexcoordLow : TEXCOORD1;
	float BlendFactor : TEXCOORD2;
};

ZETerrainMaterial_ForwardPass_PSOutput ZETerrainMaterial_ForwardPass_PixelShader(ZETerrainMaterial_ForwardPass_PSInput Input)
{
	ZETerrainMaterial_ForwardPass_PSOutput Output;
	Output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float2 ScreenPosition = Input.ScreenPosition.xy * ScreenToTextureParams.xy + ScreenToTextureParams.zw;		

	float3 TextureColor = tex2D(ColorTexture, Input.Texcoord).rgb;
	float3 TextureColorLow = tex2D(ColorTextureLow, Input.TexcoordLow).rgb;
	float3 BaseColor = TextureColor; //lerp(TextureColor, TextureColorLow, Input.BlendAndHeight.x);
		
	Output.Color.rgb  = BaseColor * AmbientColor;
	Output.Color.rgb += BaseColor * DiffuseColor * ZELBuffer_GetDiffuse(ScreenPosition);
	
	Output.Color.rgb = Input.BlendFactor.xxx;

	return Output;
}
