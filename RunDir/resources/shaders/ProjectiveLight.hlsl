//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ProjectiveLight.hlsl
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


#ifndef __PROJECTIVE_LIGHT_HLSL__
#define __PROJECTIVE_LIGHT_HLSL__

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"
#include "Shadow.hlsl"
#include "PipelineConstants.hlsl"

SamplerState		ProjectionMapSampler		: register(s5);
Texture2D<float4>	ProjectionMap				: register(t5);

cbuffer LightVSParameters0 : register(b0)
{
	float4x4 WorldMatrix		: packoffset(c0);
};

cbuffer LightPSParameters0 : register(b0)
{
	float3	Color				: packoffset(c0.x);
	float	Intensity			: packoffset(c0.w);
	float3	Attenuation			: packoffset(c1.x);
};

cbuffer LightPSParameters1 : register(b1)
{
	float3 ViewSpaceCenter		: packoffset(c0);
	float4x4 ProjectionMatrix	: packoffset(c1);
};

//cbuffer ShadowParametersPS : register(b1)
//{
//	float	DepthScaledBias		: packoffset(c0.x);
//	float	SlopeScaledBias		: packoffset(c0.y);
//	float	ShadowDistance		: packoffset(c0.z);
//	float	ShadowFadeDistance	: packoffset(c0.w);
//	float	PenumbraScale		: packoffset(c1.x);
//	float	ShadowMapTexelSize	: packoffset(c1.y);
//};

struct ZEProjectiveLight_VSInput
{
	float3 Position 	: POSITION0;
};

struct ZEProjectiveLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEProjectiveLight_VSOutput ZEProjectiveLight_VertexShader(ZEProjectiveLight_VSInput Input)
{
	ZEProjectiveLight_VSOutput Output;
	
	float4x4 WorldViewMatrix = mul(ZEViewMatrix, WorldMatrix);
	float4x4 WorldViewProjMatrix = mul(ZEProjMatrix, WorldViewMatrix);

	float4 ViewSpacePos = mul(WorldViewMatrix, float4(Input.Position, 1.0f));
	
	Output.Position = mul(WorldViewProjMatrix, float4(Input.Position, 1.0f));
	Output.ViewVector = ZEGBuffer_GetViewVector(ViewSpacePos);

	return Output;
}

struct ZEProjectiveLight_PSInput
{
	float3 ViewVector		: TEXCOORD0;
	float4 ScreenPosition	: SV_Position;
};

ZELBuffer ZEProjectiveLight_PixelShader(ZEProjectiveLight_PSInput Input)
{
	ZELBuffer LBuffer = (ZELBuffer)0.0f;

	float2 ScreenPosition = Input.ScreenPosition.xy * ZEInvViewDimension;
	
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
	
	float3 LightDisplacement = ViewSpaceCenter - ViewPosition;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;

	float AngularAttenuation = dot(LightDirection, Normal);
	
	float3 AttenuationMultiplier = float3(1.0f, LightDistance, LightDistance * LightDistance);
	float DistanceAttenuation = 1.0f / dot(Attenuation, AttenuationMultiplier);

	float FinalAttenuation = AngularAttenuation * DistanceAttenuation;

	if (ZE_LIGHT_ZERO_CHECK(FinalAttenuation))
		discard;

	float4 TextureCoord = mul(ProjectionMatrix, float4(ViewPosition, 1.0f));
	float2 SampleCoord = TextureCoord.xy / TextureCoord.w;
	float3 SampleColor = ProjectionMap.SampleLevel(ProjectionMapSampler, SampleCoord, -1.0f).rgb;
	
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(LightDirection + ViewDirection);

		float Specular = pow(abs(dot(Normal, HalfVector)), SpecularPower);
		float Luminance = ZELBuffer_GetLuminance(Output.rgb);
		
		Output.rgb = Intensity * Color * SampleColor;
		Output.a =  Specular * Luminance;
	
		Output *= FinalAttenuation;
	}
	else
	{
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = Intensity * SampleColor * Color;
			Output.a = 0.0f;
			
			Output *= SubSurfaceScatteringFactor;
			Output *= -FinalAttenuation;
		}
	}
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

	return LBuffer;
}

#endif
