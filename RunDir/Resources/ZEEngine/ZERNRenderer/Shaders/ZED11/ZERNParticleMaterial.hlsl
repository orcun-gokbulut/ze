//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNParticleMaterial.hlsl
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

#ifndef __ZERN_PARTICLE_MATERIAL_H__
#define __ZERN_PARTICLE_MATERIAL_H__

#include "ZERNTransformations.hlsl"
#include "ZERNGBuffer.hlsl"
#include "ZERNScene.hlsl"

struct ZERNParticleMaterial_Instance_Attributes
{
	float3										Position;
	float										Reserved0;
	float2										Size;
	float2										Cos_NegSin;
	float4										Color;
};

struct ZERNParticleMaterial_VertexShader_Output
{
	float4										Position									: SV_Position;
	float2										Texcoord									: TEXCOORD0;
	float4										Color										: COLOR0;
	float										DepthView									: TEXCOORD1;
};                                                                      					
			
struct ZERNParticleMaterial_PixelShader_Input                           					
{                                                                       					
	float4										PositionViewport							: SV_Position;
	float2										Texcoord									: TEXCOORD0;
	float4										Color										: COLOR0;
	float										DepthView									: TEXCOORD1;
};

cbuffer ZERNParticleMaterial_Constants_Material												: register(ZERN_SHADER_CONSTANT_MATERIAL)
{
	float3										ZERNParticleMaterial_AmbientColor;
	float										ZERNParticleMaterial_Opacity;
	
	float3										ZERNParticleMaterial_DiffuseColor;
	bool										ZERNParticleMaterial_SceneAmbientEnabled;
	
	float3										ZERNParticleMaterial_EmissiveColor;
	float										ZERNParticleMaterial_AlphaCullLimit;
	
	bool										ZERNParticleMaterial_SoftParticle;
	float										ZERNParticleMaterial_DistanceMax;
	float										ZERNParticleMaterial_ContrastPower;
	float										ZERNParticleMaterial_Reserved0;
};

cbuffer ZERNParticleMaterial_Constants_Draw_Transform										: register(ZERN_SHADER_CONSTANT_DRAW_TRANSFORM)
{
	float4x4									ZERNParticleMaterial_WorldMatrix;
};

cbuffer ZERNParticleMaterial_ParticleConstants												: register(b8)
{
	ZERNParticleMaterial_Instance_Attributes	Instances[1000];
};

SamplerState									ZERNParticleMaterial_Sampler				: register(s0);

Texture2D<float4>								ZERNParticleMaterial_DiffuseMap			 	: register(t5);
Texture2D<float3>								ZERNParticleMaterial_EmissiveMap			: register(t6);
Texture2D<float3>								ZERNParticleMaterial_NormalMap				: register(t7);
Texture2D<float>								ZERNParticleMaterial_OpacityMap				: register(t8);

ZERNParticleMaterial_VertexShader_Output ZERNParticleMaterial_VertexShader_Main(uint VertexID : SV_VertexID, uint InstanceID : SV_InstanceID)
{
	float2 Vertex = ((VertexID & uint2(2, 1)) << uint2(0, 1)) - 1.0f;
	
	float2 VertexRotated;
	VertexRotated.x = dot(Vertex, float2(Instances[InstanceID].Cos_NegSin));
	VertexRotated.y = dot(Vertex, float2(-Instances[InstanceID].Cos_NegSin.y, Instances[InstanceID].Cos_NegSin.x));
	VertexRotated *= Instances[InstanceID].Size;
	float3 VertexWorld = ZERNTransformations_ViewToWorld(float4(VertexRotated, 0.0f, 0.0f));
	
	float3 CenterWorld = mul(ZERNParticleMaterial_WorldMatrix, float4(Instances[InstanceID].Position, 1.0f)).xyz;
	float3 PositionWorld = CenterWorld + VertexWorld;
	
	ZERNParticleMaterial_VertexShader_Output Output;
	Output.Position = ZERNTransformations_WorldToProjection(float4(PositionWorld, 1.0f));
	Output.Texcoord = (uint2(VertexID, VertexID + 1) & uint2(2, 1)) >> uint2(1, 0);
	Output.Color = Instances[InstanceID].Color;
	Output.DepthView = Output.Position.w;
	
	return Output;
}

float4 ZERNParticleMaterial_PixelShader_Main(ZERNParticleMaterial_PixelShader_Input Input) : SV_Target0
{
	float Alpha = 1.0f;
	#ifdef ZERN_PM_ALPHA_CULL
			Alpha = ZERNParticleMaterial_Opacity;
		#ifdef ZERN_PM_OPACITY_MAP
			Alpha = ZERNParticleMaterial_OpacityMap.Sample(ZERNParticleMaterial_Sampler, Input.Texcoord);
		#elif defined ZERN_PM_DIFFUSE_MAP
			Alpha = ZERNParticleMaterial_DiffuseMap.Sample(ZERNParticleMaterial_Sampler, Input.Texcoord).w;
		#endif
		clip(Alpha - ZERNParticleMaterial_AlphaCullLimit);
	#endif
	
	float3 BaseColor = float3(1.0f, 1.0f, 1.0f);
	#ifdef ZERN_PM_DIFFUSE_MAP
		BaseColor = ZERNParticleMaterial_DiffuseMap.Sample(ZERNParticleMaterial_Sampler, Input.Texcoord).rgb;	
	#endif
	
	float3 AmbientColor = BaseColor * (ZERNParticleMaterial_SceneAmbientEnabled ? ZERNScene_AmbientColor : ZERNParticleMaterial_AmbientColor);
	float3 DiffuseColor = BaseColor * ZERNParticleMaterial_DiffuseColor;
	float3 EmissiveColor = ZERNParticleMaterial_EmissiveColor;
	
	#ifdef ZERN_PM_EMISSIVE_MAP
		EmissiveColor *= ZERNParticleMaterial_EmissiveMap.Sample(ZERNParticleMaterial_Sampler, Input.Texcoord).rgb;
	#endif
	
	#ifdef ZERN_PM_VERTEX_COLOR
		DiffuseColor *= Input.Color;
		AmbientColor *= Input.Color;
	#endif
	
	if (ZERNParticleMaterial_SoftParticle)
	{
		float DestDepthView = ZERNTransformations_HomogeneousToViewDepth(ZERNGBuffer_GetDepth(Input.PositionViewport.xy));
		
		float DistanceDepth = DestDepthView - Input.DepthView;
		if (DistanceDepth < 0.001f)
			discard;
		
		float DepthNormalized = saturate(DistanceDepth / ZERNParticleMaterial_DistanceMax);
		Alpha = pow(DepthNormalized, ZERNParticleMaterial_ContrastPower);
	}
	
	return float4(AmbientColor + EmissiveColor + DiffuseColor, Alpha);
}

#endif
