//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Lights.hlsl
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
#include "Shadow.hlsl"

SamplerState	ProjectionMapSampler			: register(s5);
SamplerState	OmniProjectionMapSampler		: register(s6);

Texture2D<float4>	ProjectionMap				: register(t5);
TextureCube<float4>	OmniProjectionMap			: register(t6);


/************************************************************************/
/*                            Point Light                               */
/************************************************************************/

cbuffer LightParametersVS : register(b0)
{
	float4x4 WorldMatrix			: packoffset(c0);
	float4x4 WorldViewMatrix		: packoffset(c4);
	float4x4 WorldViewProjMatrix	: packoffset(c8);
};

struct ZEPointLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEPointLight_VSOutput ZEPointLight_VertexShader(in float3 Position : POSITION0)
{
	ZEPointLight_VSOutput Output;
	
	Output.Position = mul(WorldViewProjMatrix, float4(Position, 1.0f));
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, float4(Position, 1.0f)));
	
	return Output;
}

struct ZEPointLight_PSInput
{
	float3 ViewVector		: TEXCOORD0;
	float4 ScreenPosition	: SV_Position;
};

cbuffer LightParametersPS : register(b0)
{
	float2		PixelSize			: packoffset(c0);
	// <<< 8 byte padding
	float3		Position			: packoffset(c1);
	float		Range				: packoffset(c1.w);
	float3		Color				: packoffset(c2);
	float		Intensity			: packoffset(c2.w);
	float3		Attenuation			: packoffset(c3);
	// <<< 4 byte padding
	float3		Direction			: packoffset(c4);
	float		FOV					: packoffset(c4.w);
	float4x4	ProjectionMatrix	: packoffset(c5);
	
};

ZELBuffer ZEPointLight_PixelShader(ZEPointLight_PSInput Input)
{
	ZELBuffer LBuffer = (ZELBuffer)0.0f;
	
	float2 ScreenPosition = Input.ScreenPosition.xy * PixelSize;
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = Position - ViewPosition;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	float DistanceAttenuation = 1.0f / dot(Attenuation, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation * DistanceAttenuation == 0.0f)
		discard;

	// Temporarly moved here
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
	if (AngularAttenuation > 0.0f)
	{	
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		

		Output.rgb = Color * Intensity;
		Output.a = pow(abs(dot(Normal, HalfVector)), SpecularPower) * dot(Output.rgb, float3(0.299f, 0.587f, 0.114f));
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
	}
	else
	{
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = SubSurfaceScatteringFactor * -AngularAttenuation * DistanceAttenuation * Intensity * Color;
			Output.a = 0.0f;
		}
	}

	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);
	
	return LBuffer;
}

// Directional Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ZEDirectionalLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEDirectionalLight_VSOutput ZEDirectionalLight_VertexShader(float3 Position : POSITION0)
{
	ZEDirectionalLight_VSOutput Output;
	
	Output.Position = float4(Position, 1.0f);

	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, float4(Position, 1.0f)));
	
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
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);

	// Light Derived Parameters
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float AngularAttenuation = dot(Direction, Normal);
	
	if (AngularAttenuation == 0.0f)
		discard;

	// Temporarly moved here
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
	
	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(Direction + ViewDirection);
		
		Output.rgb = Intensity * Color;
		Output.a = pow(abs(dot(Normal, HalfVector)), SpecularPower) * dot(Output.rgb, float3(0.299f, 0.587f, 0.114f));;
		Output *= AngularAttenuation;
	}
	else
	{
		
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = SubSurfaceScatteringFactor * -AngularAttenuation * Intensity * Color;
			Output.a = 0.0f;
		}
	}
				
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

	return LBuffer;
}

// Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ZEProjectiveLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEProjectiveLight_VSOutput ZEProjectiveLight_VertexShader(float3 Position : POSITION0)
{
	ZEProjectiveLight_VSOutput Output;
	
	Output.Position = mul(WorldViewProjMatrix, float4(Position, 1.0f));
	
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, float4(Position, 1.0f)));
	
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

	float2 ScreenPosition = Input.ScreenPosition.xy * PixelSize;
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = Position - ViewPosition;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;

	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	float DistanceAttenuation = 1.0f / dot(Attenuation, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation * DistanceAttenuation == 0.0f)
		discard;

	// temporarly moved here
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
	float4 TextureLookup = mul(ProjectionMatrix, float4(ViewPosition, 1.0f));
	float3 ProjLightColor = ProjectionMap.SampleLevel(ProjectionMapSampler, TextureLookup.xy / TextureLookup.w, -1.0f).rgb * Color;
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);
	
	if (AngularAttenuation > 0.0f)
	{
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(LightDirection + ViewDirection);

		Output.rgb = Intensity * ProjLightColor;
		Output.a = pow(abs(dot(Normal, HalfVector)), SpecularPower) * dot(Output.rgb, float3(0.299f, 0.587f, 0.114f));

		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;		
	}
	else
	{
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = SubSurfaceScatteringFactor * -AngularAttenuation * DistanceAttenuation * Intensity * ProjLightColor;
			Output.a = 0.0f;
		}
	}
	
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);

	return LBuffer;
}

// Omni Projective Light
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ZEOmniProjectiveLight_VSOutput
{
	float3 ViewVector	: TEXCOORD0;
	float4 Position		: SV_Position;
};

ZEOmniProjectiveLight_VSOutput ZEOmniProjectiveLight_VertexShader(float3 Position : POSITION0)
{
	ZEOmniProjectiveLight_VSOutput Output;
	
	Output.Position = mul(WorldViewProjMatrix, float4(Position, 1.0f));
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(WorldViewMatrix, float4(Position, 1.0f)));
	
	return Output;
}

struct ZEOmniProjectiveLight_PSInput
{
	float3 ViewVector		: TEXCOORD0;
	float4 ScreenPosition	: SV_Position;
};

ZELBuffer ZEOmniProjectiveLight_PixelShader(ZEOmniProjectiveLight_PSInput Input)
{
	ZELBuffer LBuffer = (ZELBuffer)0.0f;
	
	float2 ScreenPosition = Input.ScreenPosition.xy * PixelSize;
	float3 ViewPosition = ZEGBuffer_GetViewPosition(ScreenPosition, Input.ViewVector);
	float3 Normal = ZEGBuffer_GetViewNormal(ScreenPosition);

	float3 LightDisplacement = Position - ViewPosition;
	float LightDistance = length(LightDisplacement);
	float3 LightDirection = LightDisplacement / LightDistance;
	
	float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float AngularAttenuation = saturate(dot(LightDirection, Normal));
	float DistanceAttenuation = 1.0f / dot(Attenuation, float3(1.0f, LightDistance, LightDistance * LightDistance));

	if (AngularAttenuation * DistanceAttenuation == 0.0f)
		discard;

	// temporarly moved here
	float SpecularPower = ZEGBuffer_GetSpecularPower(ScreenPosition);
	float3 TextureLookup = mul((float3x3)ProjectionMatrix, LightDirection);
	float3 ProjLightColor = OmniProjectionMap.SampleLevel(OmniProjectionMapSampler, TextureLookup, -1.0f).rgb * Color;
	float SubSurfaceScatteringFactor = ZEGBuffer_GetSubSurfaceScatteringFactor(ScreenPosition);

	if (AngularAttenuation > 0.0f)
	{	
		float3 ViewDirection = normalize(-ViewPosition);
		float3 HalfVector = normalize(LightDirection + ViewDirection);		

		Output.rgb = Intensity * ProjLightColor;
		Output.a = pow(abs(dot(Normal, HalfVector)), SpecularPower) * dot(Output.rgb, float3(0.299f, 0.587f, 0.114f));
		
		Output *= AngularAttenuation;
		Output *= DistanceAttenuation;
	}
	else
	{
		if (SubSurfaceScatteringFactor > 0.0f)
		{
			Output.rgb = SubSurfaceScatteringFactor * -AngularAttenuation * DistanceAttenuation * Intensity * ProjLightColor;
			Output.a = 0.0f;
		}
	}
		
	ZELBuffer_SetDiffuse(LBuffer, Output.rgb);
	ZELBuffer_SetSpecular(LBuffer, Output.a);
	
	return LBuffer;
}
