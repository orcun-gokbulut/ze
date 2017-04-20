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
#include "ZERNShading.hlsl"
#include "ZERNRenderer.hlsl"
#include "ZERNScreenCover.hlsl"

#define ZE_PEF_LIGHT_RECEIVER						0x1
#define ZE_PEF_RENDER_STREAKS						0x2
#define ZE_PEF_BILLBOARD_VIEW						0x4
#define ZE_PEF_BILLBOARD_AXIS						0x8
#define ZE_PEF_TILED_RENDERING						0x10
#define ZE_PEF_VELOCITY_PER_PARTICLE				0x20
#define ZE_PEF_ACCELERATION_PER_PARTICLE			0x40
#define ZE_PEF_SIZE_PER_PARTICLE					0x80
#define ZE_PEF_COLOR_PER_PARTICLE					0x100
#define ZE_PEF_TEXCOORDS_PER_PARTICLE				0x200
#define ZE_PEF_ROTATION_PER_PARTICLE				0x400
#define ZE_PEF_ANGULAR_ACCELERATION_PER_PARTICLE	0x800
#define ZE_PEF_ANGULAR_VELOCITY_PER_PARTICLE		0x1000
#define ZE_PEF_TOTAL_LIFE_PER_PARTICLE				0x2000

struct ZERNParticleMaterial_VertexShader_Output
{
	float4										Position									: SV_Position;
	float2										TexCoord									: TEXCOORD0;
	uint										ParticleGlobalIndex							: TEXCOORD1;
};

struct ZERNParticleMaterial_PixelShader_Input
{
	float4										PositionViewport							: SV_Position;
	float2										TexCoord									: TEXCOORD0;
	uint										ParticleGlobalIndex							: TEXCOORD1;
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

SamplerState									ZERNParticleMaterial_Sampler				: register(s0);

Texture2D<float4>								ZERNParticleMaterial_DiffuseMap			 	: register(t5);
Texture2D<float3>								ZERNParticleMaterial_EmissiveMap			: register(t6);
Texture2D<float3>								ZERNParticleMaterial_NormalMap				: register(t7);
Texture2D<float>								ZERNParticleMaterial_OpacityMap				: register(t8);

ZERNShading_Surface ZERNParticleMaterial_GetSurfaceFromResources(ZERNShading_Particle Particle, float3 PositionView, float2 TexCoord, float2 PositionViewport)
{
	float Alpha = ZERNParticleMaterial_Opacity * Particle.Color.a;
	
	#ifdef ZERN_PM_OPACITY_MAP
		Alpha *= ZERNParticleMaterial_OpacityMap.Sample(ZERNParticleMaterial_Sampler, TexCoord);
	#elif defined ZERN_PM_DIFFUSE_MAP
		Alpha *= ZERNParticleMaterial_DiffuseMap.SampleLevel(ZERNParticleMaterial_Sampler, TexCoord, 0).w;
	#endif
	
	#ifdef ZERN_PM_ALPHA_CULL
		clip(Alpha - ZERNParticleMaterial_AlphaCullLimit);
	#endif
	
	float3 BaseColor = Particle.Color.rgb;
	#ifdef ZERN_PM_DIFFUSE_MAP
		BaseColor *= ZERNParticleMaterial_DiffuseMap.SampleLevel(ZERNParticleMaterial_Sampler, TexCoord, 0).rgb;	
	#endif
	
	float3 AmbientColor = BaseColor * (ZERNParticleMaterial_SceneAmbientEnabled ? ZERNScene_AmbientColor : ZERNParticleMaterial_AmbientColor);
	float3 DiffuseColor = BaseColor * ZERNParticleMaterial_DiffuseColor;
	float3 EmissiveColor = ZERNParticleMaterial_EmissiveColor;
	
	#ifdef ZERN_PM_EMISSIVE_MAP
		EmissiveColor *= ZERNParticleMaterial_EmissiveMap.Sample(ZERNParticleMaterial_Sampler, TexCoord).rgb;
	#endif
	
	if (ZERNParticleMaterial_SoftParticle)
	{
		float DestDepthView = ZERNTransformations_HomogeneousToViewDepth(ZERNGBuffer_GetDepth(PositionViewport));

		float DistanceDepth = DestDepthView - PositionView.z;

		float DepthNormalized = saturate(DistanceDepth / ZERNParticleMaterial_DistanceMax);
		Alpha *= pow(DepthNormalized, ZERNParticleMaterial_ContrastPower);
	}
	
	ZERNShading_Surface Surface;
	Surface.PositionView = PositionView;
	Surface.NormalView = normalize(-PositionView);
	Surface.Diffuse = DiffuseColor;
	Surface.SubsurfaceScattering = 0.0f;
	Surface.Specular = 0.0f;
	Surface.SpecularPower = 1.0f;
	Surface.Ambient = AmbientColor;
	Surface.Opacity = Alpha;
	Surface.Emissive = EmissiveColor;
	
	return Surface;
}

float4 ZERNParticleMaterial_Shade(ZERNShading_Particle Particle, float3 PositionView, float2 TexCoord, float2 PositionViewport)
{
	ZERNShading_Surface Surface = ZERNParticleMaterial_GetSurfaceFromResources(Particle, PositionView, TexCoord, PositionViewport);
	
	if (Particle.LightReceiver)
		return float4(ZERNShading_Shade(PositionViewport, Surface), Surface.Opacity);
	else
		return float4(Surface.Ambient + Surface.Emissive + Surface.Diffuse, Surface.Opacity);
}

ZERNParticleMaterial_VertexShader_Output ZERNParticleMaterial_VertexShader_Main(uint VertexID : SV_VertexID, uint InstanceID : SV_InstanceID)
{
	float2 Vertex = ((uint2(VertexID + 2, VertexID) & uint2(2, 1)) << uint2(0, 1)) - 1.0f;
	
	ZERNShading_Emitter Emitter = ZERNShading_Emitters[0];
	uint ParticleGlobalIndex = Emitter.StartOffset + InstanceID;
	ZERNShading_Particle Particle;
	Particle.Position = ZERNShading_ParticlePool[ParticleGlobalIndex].xyz;
	Particle.Size = Emitter.Size;
	Particle.Rotation = Emitter.Rotation;
	
	if ((Emitter.Flags & ZE_PEF_SIZE_PER_PARTICLE) == ZE_PEF_SIZE_PER_PARTICLE)
		Particle.Size *= ZERNShading_ParticlePool[ParticleGlobalIndex += Emitter.ParticleCount].xy;
	
	if ((Emitter.Flags & ZE_PEF_ROTATION_PER_PARTICLE) == ZE_PEF_ROTATION_PER_PARTICLE)
		Particle.Rotation += ZERNShading_ParticlePool[ParticleGlobalIndex += Emitter.ParticleCount].x;
	
	float Sin, Cos;
	sincos(Particle.Rotation, Sin, Cos);
		
	float2 VertexTransformed;
	VertexTransformed.x = dot(Vertex, float2(Cos, -Sin));
	VertexTransformed.y = dot(Vertex, float2(Sin, Cos));
	
	VertexTransformed.xy *= Particle.Size;
	
	float3 FrontWorld = normalize(ZERNView_Position - Particle.Position);
	float3 VertexWorld = float3(0.0f, 0.0f, 0.0f);
	if (Emitter.Flags & ZE_PEF_BILLBOARD_VIEW)
	{
		float3 RightWorld = normalize(cross(float3(0.0f, 1.0f, 0.0f), FrontWorld));
		float3 UpWorld = cross(FrontWorld, RightWorld);
		
		VertexWorld = VertexTransformed.x * RightWorld + VertexTransformed.y * UpWorld;
	}
	else if (Emitter.Flags & ZE_PEF_BILLBOARD_AXIS)
	{
		float3 Axis = (float3)(uint3(Emitter.Axis >> 16, Emitter.Axis >> 8, Emitter.Axis) & 0xff);
		float3 RightWorld = normalize(cross(Axis, FrontWorld));
		VertexWorld = VertexTransformed.x * RightWorld + VertexTransformed.y * Axis;
	}
	
	ZERNParticleMaterial_VertexShader_Output Output;
	Output.Position = ZERNTransformations_WorldToProjection(float4(Particle.Position + VertexWorld, 1.0f));
	Output.TexCoord = (uint2(VertexID, VertexID + 1) & uint2(2, 1)) >> uint2(1, 0);
	
	Output.TexCoord = Emitter.TexCoords.xy + (Emitter.TexCoords.zw - Emitter.TexCoords.xy) * Output.TexCoord;
	
	if ((Emitter.Flags & ZE_PEF_TEXCOORDS_PER_PARTICLE) == ZE_PEF_TEXCOORDS_PER_PARTICLE)
	{
		float4 MinMaxTexCoords = ZERNShading_ParticlePool[ParticleGlobalIndex += Emitter.ParticleCount];
		Output.TexCoord = MinMaxTexCoords.xy + (MinMaxTexCoords.zw - MinMaxTexCoords.xy) * Output.TexCoord;
	}
	
	Output.ParticleGlobalIndex = ParticleGlobalIndex + Emitter.ParticleCount;
	
	return Output;
}

float4 ZERNParticleMaterial_PixelShader_Main(ZERNParticleMaterial_PixelShader_Input Input) : SV_Target0
{
	ZERNShading_Emitter Emitter = ZERNShading_Emitters[0];
	ZERNShading_Particle Particle;
	Particle.Color = Emitter.Color;

	if ((Emitter.Flags & ZE_PEF_COLOR_PER_PARTICLE) == ZE_PEF_COLOR_PER_PARTICLE)
		Particle.Color *= ZERNShading_ParticlePool[Input.ParticleGlobalIndex];
	
	Particle.LightReceiver = (Emitter.Flags & ZE_PEF_LIGHT_RECEIVER) == ZE_PEF_LIGHT_RECEIVER;
	float3 PositionView = ZERNTransformations_ViewportToView(Input.PositionViewport.xy, ZERNGBuffer_GetDimensions(), Input.PositionViewport.z);

	return ZERNParticleMaterial_Shade(Particle, PositionView, Input.TexCoord, Input.PositionViewport.xy);
}

float4 ZERNParticleMaterial_Tiled_PixelShader(float4 PositionViewport : SV_Position) : SV_Target0
{
	float2 GBufferDimensions = ZERNGBuffer_GetDimensions();
	uint2 TileId = floor(PositionViewport.xy) / TILE_PARTICLE_DIM;
	uint TileIndex = TileId.y * (GBufferDimensions.x / TILE_PARTICLE_DIM) + TileId.x;
	uint TileStartOffset = TileIndex * TILE_PARTICLE_TOTAL_COUNT;
	uint TileParticleCount = ZERNShading_TileParticleIndices[TileStartOffset];
	
	float4 ResultColor = 0.0f;
	
	for (uint I = 0; I < TileParticleCount; I++)
	{
		uint ParticleIndex = ZERNShading_TileParticleIndices[TileStartOffset + TILE_PARTICLE_HEADER_COUNT + I];
		
		uint EmitterId = ParticleIndex >> 20;
		uint ParticleGlobalIndex = ParticleIndex & 0xfffff;
		
		ZERNShading_Emitter Emitter = ZERNShading_Emitters[EmitterId];
		float4 PositionRadius = ZERNShading_ParticlePool[ParticleGlobalIndex];
		ParticleGlobalIndex += Emitter.ParticleCount;

		ZERNShading_Particle Particle;
		Particle.Position = PositionRadius.xyz;
		Particle.Radius = PositionRadius.w;
		Particle.Color = Emitter.Color;
		Particle.Size = Emitter.Size;
		Particle.Rotation = Emitter.Rotation;
		Particle.LightReceiver = (Emitter.Flags & ZE_PEF_LIGHT_RECEIVER) == ZE_PEF_LIGHT_RECEIVER;
		
		if ((Emitter.Flags & ZE_PEF_SIZE_PER_PARTICLE) == ZE_PEF_SIZE_PER_PARTICLE)
		{
			Particle.Size *= ZERNShading_ParticlePool[ParticleGlobalIndex].xy;
			ParticleGlobalIndex += Emitter.ParticleCount;
		}
		
		if ((Emitter.Flags & ZE_PEF_ROTATION_PER_PARTICLE) == ZE_PEF_ROTATION_PER_PARTICLE)
		{
			Particle.Rotation += ZERNShading_ParticlePool[ParticleGlobalIndex].x;
			ParticleGlobalIndex += Emitter.ParticleCount;
		}
		
		float3 RayDirectionView = ZERNTransformations_ViewportToView(PositionViewport.xy, GBufferDimensions, ZERNGBuffer_GetDepth(PositionViewport.xy));
		RayDirectionView = normalize(RayDirectionView);
		
		float3 ParticlePointView = (Particle.Position.z / RayDirectionView.z) * RayDirectionView;
		float2 VectorToPoint = (ParticlePointView.xy - Particle.Position.xy) / Particle.Size;
		
		float Sin, Cos;
		sincos(Particle.Rotation, Sin, Cos);
		
		float2 VectorToPointRotated;
		VectorToPointRotated.x = dot(VectorToPoint.xy, float2(Cos, -Sin));
		VectorToPointRotated.y = dot(VectorToPoint.xy, float2(Sin, Cos));
		
		float2 TexCoord = VectorToPointRotated * 0.5f + 0.5f;
		TexCoord = Emitter.TexCoords.xy + (Emitter.TexCoords.zw - Emitter.TexCoords.xy) * TexCoord;

		if ((Emitter.Flags & ZE_PEF_TEXCOORDS_PER_PARTICLE) == ZE_PEF_TEXCOORDS_PER_PARTICLE)
		{
			float4 MinMaxTexCoords = ZERNShading_ParticlePool[ParticleGlobalIndex];
			TexCoord = MinMaxTexCoords.xy + (MinMaxTexCoords.zw - MinMaxTexCoords.xy) * TexCoord;
			ParticleGlobalIndex += Emitter.ParticleCount;
		}
		
		if ((Emitter.Flags & ZE_PEF_COLOR_PER_PARTICLE) == ZE_PEF_COLOR_PER_PARTICLE)
		{
			Particle.Color *= ZERNShading_ParticlePool[ParticleGlobalIndex];
			ParticleGlobalIndex += Emitter.ParticleCount;
		}
		
		if (all(TexCoord >= 0.0f && TexCoord <= 1.0f))
		{
			float4 Color = ZERNParticleMaterial_Shade(Particle, ParticlePointView, TexCoord, PositionViewport.xy);
			
			ResultColor.rgb = (1.0f - ResultColor.a) * (Color.rgb * Color.a) + ResultColor.rgb;
			ResultColor.a = Color.a + (1.0f - Color.a) * ResultColor.a;
		}
	}
	
	return ResultColor;
}

#endif
