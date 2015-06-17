//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SSAOProcessor.hlsl
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

#ifndef __ZE_SSAO_PROCESSOR_HLSL__
#define __ZE_SSAO_PROCESSOR_HLSL__

#include	"GBuffer.hlsl"

#define		SAMPLE_COUNT_MAX			32
#define		SAMPLE_COUNT				16

sampler2D	RandomTexture				: register(s3);
sampler2D	SSAOTexture					: register(s4);

float4		PixelParameters				: register(c0);
float4x4	InvProjectionMatrix			: register(c1);
#define		PixelSize					PixelParameters.xy
#define		RandomTileScale				PixelParameters.zw

float4		BlurParameters				: register(c1);
//#define	BlurRadius					BlurParameters.x	// NOT available yet
#define		BlurFalloff					BlurParameters.y
#define		BlurSharpness				BlurParameters.z
#define		BlurEdgeThreshold			BlurParameters.w

float4		SSAOParameters				: register(c2);
#define		SampleRadius				SSAOParameters.x
#define		Contrast					SSAOParameters.y

float4x4	ProjectionMatrix			: register(c3);
float4		Offsets[SAMPLE_COUNT_MAX]	: register(c7);

struct VSInput
{
	float3 Position			: POSITION0;
	float2 TexCoord			: TEXCOORD0;
};

struct VSOutput
{
	float4 Position			: POSITION0;
	float2 TexCoord			: TEXCOORD0;
	float2 TexCoordRand		: TEXCOORD1;
	float3 ViewVector		: TEXCOORD2;
};

struct PSInput
{
	float2 TexCoord			: TEXCOORD0;
	float2 TexCoordRand		: TEXCOORD1;
	float3 ViewVector		: TEXCOORD2;
	float4 ScreenPosition 	: VPOS;
};

struct PSOutput
{
	float4 Color			: COLOR0;
};

//--------------------------------------------------------------------------------
VSOutput VSMainCommon(VSInput Input)
{
	VSOutput Output = (VSOutput)0.0f;

	Output.Position = float4(Input.Position.xyz, 1.0f);
	Output.TexCoord = Input.TexCoord + 0.5f * PixelSize;
	Output.TexCoordRand = Output.TexCoord * RandomTileScale;
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(InvProjectionMatrix, Output.Position));

	return Output;
}

//----------------------------------------------------------------------------------
float4 PSMainAmbientOcclusion( PSInput Input ) : COLOR0
{
	float2 ScreenCoord = Input.ScreenPosition.xy * PixelSize + PixelSize * 0.5f;
	float3 ViewSpacePos = ZEGBuffer_GetViewPosition(ScreenCoord, Input.ViewVector);

	float3 RandomDir = tex2D(RandomTexture, Input.TexCoordRand).xyz;

	// Create random tangent/binormal which is orthogonal to normal
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenCoord);
	float3 Tangent = RandomDir - Normal * dot(Normal, RandomDir);
	float3 Binormal = cross(Normal, Tangent);

	float3x3 TBN = float3x3(Tangent.x,	Binormal.x,	Normal.x,
							Tangent.y,	Binormal.y, Normal.y,
							Tangent.z,	Binormal.z,	Normal.z);

	float PixelOcclusion = 0.0f;
	for (int I = 0; I < SAMPLE_COUNT; ++I)
	{
		// Transform offset from tangent to view space
		float3 SampleOffset = mul(TBN, Offsets[I].xyz) * SampleRadius;

		// Create sample point in view space and
		float4 SamplePoint = float4(ViewSpacePos + SampleOffset, 1.0f);

		// Calculate sample points view space depth
		float ViewDepth = length(SamplePoint.xyz);

		// Transform view space point to clip space
		SamplePoint = mul(ProjectionMatrix, SamplePoint);
		SamplePoint /= SamplePoint.w;

		// Transform clip space point to texture space
		SamplePoint *= float4(0.5f, -0.5f, 1.0f, 1.0f); // Invert y and scale xy
		SamplePoint += float4(0.5f,  0.5f, 0.0f, 0.0f); // Shift xy	
		
		// Get depth at SampleCoord
		float DepthSample = ZEGBuffer_GetDepth(SamplePoint.xy);

		// 1) Check occlusion
		// 2) Check false occlusion
		// 3) Check if sample lies outside texture coords(0-1)
		// 4) Check if sample falls into same texture coord with target pixel
		float Occlusion = DepthSample < ViewDepth;
		float FalseOcclusion = abs(DepthSample - ViewDepth) < SampleRadius;
		float CoordCheck = !any((SamplePoint < 0.0f) || (SamplePoint > 1.0f));
		float RangeCheck = !any(abs(SamplePoint.xy - ScreenCoord) < PixelSize);
		
		PixelOcclusion += Occlusion * FalseOcclusion * CoordCheck * RangeCheck;
	}

	float OcclusionFactor = 1.0f - saturate(PixelOcclusion / SAMPLE_COUNT * Contrast);
	return OcclusionFactor.xxxx;
}

#define		BlurRadius		4.0f

//-------------------------------------------------------------------------
float BlurFunction(float2 uv, float r, float center_c, float center_d, inout float w_total)
{
    float d = ZEGBuffer_GetDepth(uv);
	float c = tex2D(SSAOTexture, uv).r;

    float ddiff = d - center_d;
    float w = exp(-r * r * BlurFalloff - ddiff * ddiff * BlurSharpness);
    w_total += w;

    return w * c;
}

//-------------------------------------------------------------------------
float4 PSMainBlurHorizontal( PSInput Input ) : COLOR0
{
    float b = 0.0f;
    float w_total = 0.0f;
    float center_c = tex2D(SSAOTexture, Input.TexCoord).r;
    float center_d = ZEGBuffer_GetDepth(Input.TexCoord);
    
    for (float r = -BlurRadius; r <= BlurRadius; ++r)
    {
        float2 uv = Input.TexCoord + float2(r * PixelSize.x , 0.0f);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    return (b / w_total).xxxx;
}

//-------------------------------------------------------------------------
float4 PSMainBlurVertical( PSInput Input ) : COLOR0
{
    float b = 0.0f;
    float w_total = 0.0f;
    float center_c = tex2D(SSAOTexture, Input.TexCoord).r;
    float center_d = ZEGBuffer_GetDepth(Input.TexCoord);
    
    for (float r = -BlurRadius; r <= BlurRadius; ++r)
    {
        float2 uv = Input.TexCoord + float2(0.0f, r * PixelSize.y);
        b += BlurFunction(uv, r, center_c, center_d, w_total);
    }

    return (b / w_total).xxxx;
}

#endif
