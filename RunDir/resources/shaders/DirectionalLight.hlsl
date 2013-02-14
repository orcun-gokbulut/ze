//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - DirectionalLight.hlsl
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


#ifndef __DIRECTIONAL_LIGHT_HLSL__
#define __DIRECTIONAL_LIGHT_HLSL__

#include "GBuffer.hlsl"
#include "LBuffer.hlsl"
#include "Shadow.hlsl"

#define ZE_MAX_CASCADE_COUNT	4

SamplerState		ShadowMapSampler					: register(s5);
Texture2D<float>	ShadowMaps[ZE_MAX_CASCADE_COUNT]	: register(t5);

cbuffer TransformationsVS : register(b0)
{
	float4x4 InvProjMatrix		: packoffset(c0);
};

cbuffer LightParametersPS : register(b0)
{
	float3	Color				: packoffset(c0.x);
	float	Intensity			: packoffset(c0.w);
	float3	Direction			: packoffset(c1.x);
	float2	PixelSize			: packoffset(c2.x);
};

cbuffer ShadowParametersPS : register(b1)
{
	float	DepthScaledBias		: packoffset(c4.x);
	float	SlopeScaledBias		: packoffset(c4.y);
	float	ShadowDistance		: packoffset(c4.z);
	float	ShadowFadeDistance	: packoffset(c4.w);
	float	PenumbraScale		: packoffset(c5.x);
	float	ShadowMapTexelSize	: packoffset(c5.y);
	
	struct
	{
		float	FarZ;
		float	NearZ;
		float	Depth;
		
	}CascadeData[ZE_MAX_CASCADE_COUNT] : packoffset(c6.x);
};

struct ZEDirectionalLight_VSInput
{
	float3 Position 	: POSITION0;
};

struct ZEDirectionalLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEDirectionalLight_VSOutput ZEDirectionalLight_VertexShader(ZEDirectionalLight_VSInput Input)
{
	ZEDirectionalLight_VSOutput Output;
	
	float4 ViewSpacePos = mul(InvProjMatrix, float4(Input.Position, 1.0f));
	
	Output.ViewVector = ZEGBuffer_GetViewVector(ViewSpacePos);
	Output.Position = float4(Input.Position, 1.0f);
	
	return Output;
}

struct ZEDirectionalLight_PSInput
{
	float3 ViewVector		: TEXCOORD0;
	float4 ScreenPosition	: SV_Position;
};

ZELBuffer ZEDirectionalLight_PixelShader(ZEDirectionalLight_PSInput Input)
{
	ZELBuffer LBuffer = (ZELBuffer)0.0f;
		
	float2 ScreenPosition = Input.ScreenPosition.xy * PixelSize;
	
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
	
	float AngularAttenuation = dot(Direction, Normal);
	if (abs(AngularAttenuation) - 0.01f < 0.0f)
		discard;
	
	
	float4 Output = (float4)0.0f;
	if (AngularAttenuation > 0.0f)
	{
		Output.rgb = Intensity * Color;
	
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(Direction + ViewDirection);
		
		float Specular = pow(abs(dot(Normal, HalfVector)), SpecularPower);
		float Luminance = ZELBuffer_GetLuminance(Output.rgb);
		Output.a =  Specular * Luminance;
		
		Output *= AngularAttenuation;
	}
	else
	{
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = Intensity * Color;
			Output.a = 0.0f;
			
			Output *= SubSurfaceScatteringFactor;
			Output *= -AngularAttenuation;
		}
	}
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

	return LBuffer;
}

#endif
