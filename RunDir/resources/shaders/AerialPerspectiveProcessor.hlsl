//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AerialPerspectiveProcessor.hlsl
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

#ifndef __ZE_AERIAL_PERSPECTIVE_PROCESSOR_HLSL__
#define __ZE_AERIAL_PERSPECTIVE_PROCESSOR_HLSL__

#include	"GBuffer.hlsl"

sampler2D	ColorTexture			: register(s3);

float2		PixelSize				: register(c0);
float		ScatterSymmetry			: register(c1);
float3		BetaMie					: register(c2);
float3		BetaRayleigh			: register(c3);
float		SunIntensity			: register(c4);
float3		SunDirection			: register(c6);
float3		CameraPosition			: register(c7);
float3		SunColor				: register(c8);
float		ExtinctionFactor		: register(c9);

float4x4	InvViewMatrix			: register(c10);
float4x4	InvProjMatrix			: register(c14);

struct VSInput
{
	float3 Position		: POSITION0;
	float2 TexCoord		: TEXCOORD0;
};

struct VSOutput
{
	float4 Position		: POSITION0;
	float2 TexCoord		: TEXCOORD0;
	float3 ViewVector	: TEXCOORD1;
};

struct PSInput
{
	float2 TexCoord		: TEXCOORD0;
	float3 ViewVector	: TEXCOORD1;
};

struct PSOutput
{
	float4 Color		: COLOR0;
};

VSOutput VSMain(VSInput Input)
{
	VSOutput Output = (VSOutput)0.0f;

	Output.Position = float4(sign(Input.Position).xyz, 1.0f);
	Output.TexCoord = Input.TexCoord + 0.5f * PixelSize;

	// Transfrom clip space positions to view space with InvProjMatrix
	Output.ViewVector = ZEGBuffer_GetViewVector(mul(InvProjMatrix, Output.Position));

	return Output;
}

#define PI	3.14159f

float GetMiePhase(float Angle)
{
	static const float Constant = 1.0f / (4.0f * PI);
	return  Constant * pow(1.0f - ScatterSymmetry, 2.0f) / pow(1.0f + ScatterSymmetry*ScatterSymmetry - 2.0f * ScatterSymmetry * Angle, 1.5f);
}

float GetRayleighPhase(float Angle)
{
	static const float Constant = 3.0f / (16.0f * PI);
	return Constant * (1.0f + pow(Angle, 2.0f));
}

/*
// ALTERNATIVE EXTINCTION CALCULATION
// HAIGHT BASED FALLOFF IS TAKEN INTO ACCOUNT

static const float MieFalloff = 0.8333f;
static const float RayleighFalloff = 0.1136f;

float CalculateExtinctionFactor(float3 PixelWorldPos, float Angle, float CameraHeight)
{
	//PixelWorldPos /= 1000;
	//CameraHeight /= 1000;

	// Mie / Haze
	float Km = -MieConstant / (MieFalloff * Angle);
	float Hm = exp(-MieFalloff * CameraHeight);
	float Um = exp(-MieFalloff * PixelWorldPos.y);
	float Em = exp(-Km * (Hm - Um));

	// Rayleigh / Molecule
	float Kr = -RayleighConstant / (RayleighFalloff * Angle);
	float Hr = exp(-RayleighFalloff * CameraHeight);
	float Ur = exp(-RayleighFalloff * PixelWorldPos.y);
	float Er = exp(-Kr * (Hr - Ur));

	return Em * Er;
}
*/

float4 PSMain(PSInput Input) : COLOR0
{
	float4 Color = tex2D(ColorTexture, Input.TexCoord);

	float3 ViewPosition = ZEGBuffer_GetViewPosition(Input.TexCoord, Input.ViewVector);
	float4 WorldPosition = mul(InvViewMatrix, float4(ViewPosition, 1.0f));

	float3 PixelDirection = WorldPosition.xyz - CameraPosition.xyz;
	float PixelDistance = length(PixelDirection);
	PixelDirection /= PixelDistance;

	// View and sun angle
	float Angle = dot(SunDirection, PixelDirection);

	// Camera optical depth
	float2 CameraDeptMieRay = (float2)0.0f;							// x=Mie, y=Ray
	CameraDeptMieRay.x = exp(-CameraPosition.y / (15000.0f));		// Mie
	CameraDeptMieRay.y = exp(-CameraPosition.y / ( 5000.0f));		// Ray

	// Pixel optical depth
	float2 PixelDepthMieRay = (float2)0.0f;							// x=Mie, y=Ray
	PixelDepthMieRay.x = exp(-WorldPosition.y / (15000.0f));		// Mie
	PixelDepthMieRay.y = exp(-WorldPosition.y / ( 5000.0f));		// Ray

	float2 AverageDepthMieRay = (float2)0.0f;						// x=Mie, y=Ray
	AverageDepthMieRay = (CameraDeptMieRay + PixelDepthMieRay) * 0.5f;
	AverageDepthMieRay *= PixelDistance;
	
	// Extinction
	float3 Extinction = (float3)0.0f;
	Extinction = AverageDepthMieRay.x * BetaMie + AverageDepthMieRay.y * BetaRayleigh;
	Extinction = exp(-Extinction * ExtinctionFactor);

	// In scattering for mie and rayleigh
	float3 InScatter = (float3)0.0f;
	float3 MieScattering = GetMiePhase(Angle) * BetaMie;
	float3 RayScattering = GetRayleighPhase(Angle) * BetaRayleigh;
	InScatter = (MieScattering + RayScattering) / (BetaMie + BetaRayleigh);
	InScatter *= (1.0f - Extinction);

	InScatter *= SunColor;
	InScatter *= SunIntensity;
	
	float4 LinVisualization = float4(InScatter, 1.0f);
	float4 ExtVisualization = float4(Extinction, 1.0f);
	float4 ColVisualization = float4(Color.rgb * Extinction + InScatter, 1.0f);

	return ColVisualization;
}

#endif
