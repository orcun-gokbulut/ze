//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNCloud.hlsl
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

#ifndef __ZERN_CLOUD_H__
#define __ZERN_CLOUD_H__

#include "ZERNScreenCover.hlsl"
#include "ZERNTransformations.hlsl"
#include "ZERNMath.hlsl"
#include "ZERNSamplers.hlsl"
#include "ZERNScene.hlsl"

#define INPUT_CPOINTS	16
#define OUTPUT_CPOINTS	16

struct ZERNCloud_Plane_VertexShader_Input
{
	float3			Position									: POSITION0;
	float2			TexCoord									: TEXCOORD0;
};

struct ZERNCloud_Plane_HullShader_Input
{
	float3			Position									: POSITION0;
	float2			TexCoord									: TEXCOORD0;
};

struct ZERNCloud_Plane_HullShader_ConstantData_Output
{
	float 			Edges[4]									: SV_TessFactor;
	float 			Inside[2]									: SV_InsideTessFactor;
};

struct ZERNCloud_Plane_DomainShader_Input
{
	float3			Position									: POSITION0;
	float2			TexCoord									: TEXCOORD0;
};

struct ZERNCloud_Plane_PixelShader_Input
{
	float4			Position									: SV_Position;
	float2			TexCoord									: TEXCOORD0;
	float3			PositionWorld								: TEXCOORD1;
};

cbuffer ZERNCloud_Plane_Constants								: register(b9)
{	
	float3			ZERNCloud_LightColor;
	float			ZERNCloud_PlaneSubdivision;

	float3			ZERNCloud_Inscattering;
	uint			ZERNCloud_CloudCoverage;
	
	float3			ZERNCloud_LightDirection;
	float			ZERNCloud_CloudDensity;
	
	float2			ZERNCloud_Translation;
	float2			ZERNCloud_TileFactor;
};

Texture2D			ZERNCloud_CloudTexture						: register(t5);

ZERNCloud_Plane_HullShader_Input ZERNCloud_Plane_VertexShader_Main(ZERNCloud_Plane_VertexShader_Input Input)
{
	ZERNCloud_Plane_HullShader_Input Output;
	Output.Position = Input.Position;
	Output.TexCoord = Input.TexCoord;
	
	return Output;
}

ZERNCloud_Plane_HullShader_ConstantData_Output ZERNCloud_Plane_HullShader_Constant(InputPatch<ZERNCloud_Plane_HullShader_Input, INPUT_CPOINTS> Patch, uint PatchID : SV_PrimitiveID)
{
	ZERNCloud_Plane_HullShader_ConstantData_Output Output;

	Output.Edges[0] = ZERNCloud_PlaneSubdivision;
	Output.Edges[1] = ZERNCloud_PlaneSubdivision;
	Output.Edges[2] = ZERNCloud_PlaneSubdivision;
	Output.Edges[3] = ZERNCloud_PlaneSubdivision;
	
	Output.Inside[0] = ZERNCloud_PlaneSubdivision;
	Output.Inside[1] = ZERNCloud_PlaneSubdivision;
	
	return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(OUTPUT_CPOINTS)]
[patchconstantfunc("ZERNCloud_Plane_HullShader_Constant")]
ZERNCloud_Plane_DomainShader_Input ZERNCloud_Plane_HullShader_Main(InputPatch<ZERNCloud_Plane_HullShader_Input, INPUT_CPOINTS> Patch, uint I : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
	ZERNCloud_Plane_DomainShader_Input Output;

	Output.Position	= Patch[I].Position;
	Output.TexCoord	= Patch[I].TexCoord;

	return Output;
}

float4 BernsteinBasis(float T)
{
    float OneMinusT = 1.0f - T;

    return float4( OneMinusT * OneMinusT * OneMinusT,
                   3.0f * T * OneMinusT * OneMinusT,
                   3.0f * T * T * OneMinusT,
                   T * T * T );
}

float3 CubicBezierSumPosition(const OutputPatch<ZERNCloud_Plane_DomainShader_Input, OUTPUT_CPOINTS> Patch, float4 BasisU, float4 BasisV)
{
    float3 Result = 0.0f;
	
    Result  = BasisV.x * (BasisU.x * Patch[0].Position  + BasisU.y * Patch[1].Position  + BasisU.z * Patch[2].Position  + BasisU.w * Patch[3].Position );
    Result += BasisV.y * (BasisU.x * Patch[4].Position  + BasisU.y * Patch[5].Position  + BasisU.z * Patch[6].Position  + BasisU.w * Patch[7].Position );
    Result += BasisV.z * (BasisU.x * Patch[8].Position  + BasisU.y * Patch[9].Position  + BasisU.z * Patch[10].Position + BasisU.w * Patch[11].Position);
    Result += BasisV.w * (BasisU.x * Patch[12].Position + BasisU.y * Patch[13].Position + BasisU.z * Patch[14].Position + BasisU.w * Patch[15].Position);

    return Result;
}

float2 CubicBezierSumTexCoord(const OutputPatch<ZERNCloud_Plane_DomainShader_Input, OUTPUT_CPOINTS> Patch, float4 BasisU, float4 BasisV)
{
    float2 Result = 0.0f;
	
    Result  = BasisV.x * (BasisU.x * Patch[0].TexCoord  + BasisU.y * Patch[1].TexCoord  + BasisU.z * Patch[2].TexCoord  + BasisU.w * Patch[3].TexCoord );
    Result += BasisV.y * (BasisU.x * Patch[4].TexCoord  + BasisU.y * Patch[5].TexCoord  + BasisU.z * Patch[6].TexCoord  + BasisU.w * Patch[7].TexCoord );
    Result += BasisV.z * (BasisU.x * Patch[8].TexCoord  + BasisU.y * Patch[9].TexCoord  + BasisU.z * Patch[10].TexCoord + BasisU.w * Patch[11].TexCoord);
    Result += BasisV.w * (BasisU.x * Patch[12].TexCoord + BasisU.y * Patch[13].TexCoord + BasisU.z * Patch[14].TexCoord + BasisU.w * Patch[15].TexCoord);

    return Result;
}

[domain("quad")]
ZERNCloud_Plane_PixelShader_Input ZERNCloud_Plane_DomainShader_Main(ZERNCloud_Plane_HullShader_ConstantData_Output Input, float2 UV : SV_DomainLocation, const OutputPatch<ZERNCloud_Plane_DomainShader_Input, OUTPUT_CPOINTS> Patch)
{
	float4 BasisU = BernsteinBasis(UV.x);
	float4 BasisV = BernsteinBasis(UV.y);

	float3 Position = CubicBezierSumPosition(Patch, BasisU, BasisV);
	float2 TexCoord = CubicBezierSumTexCoord(Patch, BasisU, BasisV);

	Position += ZERNView_Position;
	TexCoord += ZERNCloud_Translation;

	ZERNCloud_Plane_PixelShader_Input Output;
	Output.Position = ZERNTransformations_WorldToProjection(float4(Position, 1.0f));
	Output.Position.z = 0.0f;
	Output.TexCoord = TexCoord;
	Output.PositionWorld = Position;
	
	return Output;
}

float4 ZERNCloud_Plane_PixelShader_Main(ZERNCloud_Plane_PixelShader_Input Input) : SV_Target0
{
	float4 CloudSample = ZERNCloud_CloudTexture.SampleLevel(ZERNSampler_LinearWrap, ZERNCloud_TileFactor * Input.TexCoord, 0.0f);
	
	if (!any(CloudSample))
		discard;
	
	float3 ViewDirection = normalize(ZERNView_Position - Input.PositionWorld);
	float CosLightView = dot(ZERNCloud_LightDirection, ViewDirection);
	
	const float G = 0.94f;
	const float GG = G * G;
	const float AngularFactor = (1.0f / (8.0f * ZERNMath_PI));
	float PhaseMie = AngularFactor * ((1.0f - GG) * (1.0f + CosLightView * CosLightView)) / ((2.0f + GG) * pow(abs(1.0f + GG - 2.0f * G * CosLightView), 1.5f));
	
	float Density = 0.25f * dot(CloudSample, float4((ZERNCloud_CloudCoverage >= 1), (ZERNCloud_CloudCoverage >= 2), (ZERNCloud_CloudCoverage >= 3), (ZERNCloud_CloudCoverage >= 4)));
	float Transmittance = exp(-Density * ZERNCloud_CloudDensity);

	return float4((ZERNCloud_LightColor * PhaseMie + ZERNCloud_Inscattering) * Density * Transmittance + ZERNScene_AmbientColor, max(1.0f - Transmittance, 0.0f));
}

#endif
