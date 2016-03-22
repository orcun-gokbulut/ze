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
#include "ZERNLightScatteringCommon.hlsl"

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

cbuffer ZERNCloud_Plane_Constant_Draw_Transform					: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4		ZERNCloud_Plane_WorldMatrix;
};

cbuffer ZERNCloud_Plane_Constants								: register(b8)
{
	float			ZERNCloud_PlaneSubdivision;
	float			ZERNCloud_CloudCoverage;
	float			ZERNCloud_CloudDensity;
	float			ZERNCloud_Reserved;
	
	float3			ZERNCloud_LightDirection;
	float			ZERNCloud_LightIntensity;
	
	float2			ZERNCloud_Translation;
	float2			ZERNCloud_Reserved2;
};

cbuffer ZERNCloud_Blur_Constants								: register(b9)
{
	float4			ZERNCloud_Blur_KernelValues[11];
};

SamplerState		ZERNCloud_SamplerLinearWrap					: register(s0);

Texture2D			ZERNCloud_CloudTexture						: register(t5);
Texture2D<float3>	ZERNCloud_ColorTexture						: register(t6);

ZERNCloud_Plane_HullShader_Input ZERNCloud_Plane_VertexShader_Main(ZERNCloud_Plane_VertexShader_Input Input)
{
	ZERNCloud_Plane_HullShader_Input Output;
	Output.Position = Input.Position;
	Output.TexCoord = Input.TexCoord;
	
	return Output;
}

ZERNCloud_Plane_HullShader_ConstantData_Output ZERNCloud_Plane_HullShader_Constant(InputPatch<ZERNCloud_Plane_HullShader_Input, 16> Patch, uint PatchID : SV_PrimitiveID)
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
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("ZERNCloud_Plane_HullShader_Constant")]
ZERNCloud_Plane_DomainShader_Input ZERNCloud_Plane_HullShader_Main(InputPatch<ZERNCloud_Plane_HullShader_Input, 16> Patch, uint I : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
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

float3 CubicBezierSumPosition(const OutputPatch<ZERNCloud_Plane_DomainShader_Input, 16> Patch, float4 BasisU, float4 BasisV)
{
    float3 Result = 0.0f;
	
    Result  = BasisV.x * (BasisU.x * Patch[0].Position  + BasisU.y * Patch[1].Position  + BasisU.z * Patch[2].Position  + BasisU.w * Patch[3].Position );
    Result += BasisV.y * (BasisU.x * Patch[4].Position  + BasisU.y * Patch[5].Position  + BasisU.z * Patch[6].Position  + BasisU.w * Patch[7].Position );
    Result += BasisV.z * (BasisU.x * Patch[8].Position  + BasisU.y * Patch[9].Position  + BasisU.z * Patch[10].Position + BasisU.w * Patch[11].Position);
    Result += BasisV.w * (BasisU.x * Patch[12].Position + BasisU.y * Patch[13].Position + BasisU.z * Patch[14].Position + BasisU.w * Patch[15].Position);

    return Result;
}

float2 CubicBezierSumTexCoord(const OutputPatch<ZERNCloud_Plane_DomainShader_Input, 16> Patch, float4 BasisU, float4 BasisV)
{
    float2 Result = 0.0f;
	
    Result  = BasisV.x * (BasisU.x * Patch[0].TexCoord  + BasisU.y * Patch[1].TexCoord  + BasisU.z * Patch[2].TexCoord  + BasisU.w * Patch[3].TexCoord );
    Result += BasisV.y * (BasisU.x * Patch[4].TexCoord  + BasisU.y * Patch[5].TexCoord  + BasisU.z * Patch[6].TexCoord  + BasisU.w * Patch[7].TexCoord );
    Result += BasisV.z * (BasisU.x * Patch[8].TexCoord  + BasisU.y * Patch[9].TexCoord  + BasisU.z * Patch[10].TexCoord + BasisU.w * Patch[11].TexCoord);
    Result += BasisV.w * (BasisU.x * Patch[12].TexCoord + BasisU.y * Patch[13].TexCoord + BasisU.z * Patch[14].TexCoord + BasisU.w * Patch[15].TexCoord);

    return Result;
}

[domain("quad")]
ZERNCloud_Plane_PixelShader_Input ZERNCloud_Plane_DomainShader_Main(ZERNCloud_Plane_HullShader_ConstantData_Output Input, float2 UV : SV_DomainLocation, const OutputPatch<ZERNCloud_Plane_DomainShader_Input, 16> Patch)
{
	float4 BasisU = BernsteinBasis(UV.x);
	float4 BasisV = BernsteinBasis(UV.y);
	
	float3 Position = CubicBezierSumPosition(Patch, BasisU, BasisV);
	float2 TexCoord = CubicBezierSumTexCoord(Patch, BasisU, BasisV);
	
	float4 PositionWorld = mul(ZERNCloud_Plane_WorldMatrix, float4(Position, 1.0f));
	
	ZERNCloud_Plane_PixelShader_Input Output;
	Output.Position = ZERNTransformations_WorldToProjection(PositionWorld);
	Output.Position.z = 0.0f;
	Output.TexCoord = TexCoord;
	Output.PositionWorld = PositionWorld.xyz;
	
	return Output;
}

float4 ZERNCloud_Plane_PixelShader_Main(ZERNCloud_Plane_PixelShader_Input Input) : SV_Target0
{	
	float3 ViewDirection = normalize(ZERNView_Position - Input.PositionWorld);
	float3 LightDirectionWorld = normalize(ZERNCloud_LightDirection);
	float CosSunView = dot(LightDirectionWorld, ViewDirection);
	
	float G = 0.2f;
	float GG = G * G;
	float3 PhaseMie = (3.0f / (8.0f * PI)) * ((1.0f - GG) * (1.0f + CosSunView * CosSunView)) / ((2.0f + GG) * pow(1.0f + GG - 2.0f * G * CosSunView, 3.0f / 2.0f));
	
	float2 TexCoord = Input.TexCoord + ZERNCloud_Translation;
	float4 CloudSample = ZERNCloud_CloudTexture.SampleLevel(ZERNCloud_SamplerLinearWrap, 10.0f * TexCoord, 0.0f);
	
	float4 CloudCoverage = ZERNCloud_CloudCoverage - float4(1.0f, 0.75f, 0.50f, 0.25f);
	float4 CloudDensity = CloudCoverage * 0.666666f;
	float FinalDensity = dot(CloudSample, CloudDensity) * ZERNCloud_CloudDensity;

	float MieDensity = exp(-FinalDensity * FinalDensity);
	float3 ResultColor = PhaseMie * MieDensity * ZERNCloud_LightIntensity;
	
	return saturate(float4(ResultColor, FinalDensity));
}

float4 ZERNCloud_Blur_PixelShader_Main(ZERNCloud_Plane_PixelShader_Input Input) : SV_Target0
{	
	float2 TextureSize;
	ZERNCloud_CloudTexture.GetDimensions(TextureSize.x, TextureSize.y);
	float2 TexelOffset = 1.0f / TextureSize;
	float2 TexCoord = Input.Position.xy * TexelOffset;
	
	float4 ResultColor = 0.0f;
	for(int I = 0; I < 11; I++)
	{
		float4 SampleColor = ZERNCloud_CloudTexture.Sample(ZERNCloud_SamplerLinearWrap, TexCoord + ZERNCloud_Blur_KernelValues[I].xy * TexelOffset);
		ResultColor += SampleColor * ZERNCloud_Blur_KernelValues[I].w;
	}
	
	return saturate(ResultColor);
}

float3 ZERNCloud_Lighting_PixelShader_Main(ZERNCloud_Plane_PixelShader_Input Input) : SV_Target0
{
	float2 TextureDimensions;
	ZERNCloud_ColorTexture.GetDimensions(TextureDimensions.x, TextureDimensions.y);
	
	float3 PositionView = ZERNTransformations_ViewportToView(Input.Position.xy, TextureDimensions, 0.0f);
	float3 PositionWorld = ZERNTransformations_ViewToWorld(float4(PositionView, 1.0f));
	float3 ViewDirection = normalize(PositionWorld - ZERNView_Position);
	
	float3 RayStart = ZERNView_Position;
	float RayLength = ZERNCloud_ColorTexture.Load(int3(Input.Position.xy, 0)).x * 1000.0f;
	float3 RayEnd = ZERNView_Position + ViewDirection * RayLength;
	
	//float3 Extinction = exp(-ZERNCloud_CalculateExtinction(RayStart, RayEnd));
	
	return 5.0f;
	
	//return 1.0f - ZERNCloud_ColorTexture.Load(int3(Input.Position.xy, 0)).x;
}

#endif
