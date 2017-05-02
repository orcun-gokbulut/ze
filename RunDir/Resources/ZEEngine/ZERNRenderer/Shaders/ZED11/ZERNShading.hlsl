//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNShading.hlsl
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

#ifndef __ZERN_SHADING_H__
#define __ZERN_SHADING_H__

#include "ZERNGBuffer.hlsl"
#include "ZERNView.hlsl"
#include "ZERNSamplers.hlsl"
#include "ZERNTransformations.hlsl"

#define SAMPLE_COUNT					4

#define MAX_EMITTER						1000
#define MAX_DEFERRED_LIGHT				1024
#define MAX_TILED_LIGHT					1021
#define MAX_POINT_LIGHT_SHADOW			2
#define MAX_SPOT_LIGHT_SHADOW			4
#define MAX_PROJECTIVE_LIGHT			8
#define MAX_PROJECTIVE_LIGHT_SHADOW		4
#define MAX_DIRECTIONAL_LIGHT			2
#define MAX_DIRECTIONAL_LIGHT_SHADOW	1
#define MAX_CASCADE						4

#define TILE_LIGHT_HEADER_COUNT			3
#define TILE_LIGHT_TOTAL_COUNT			(TILE_LIGHT_HEADER_COUNT + MAX_TILED_LIGHT)

#define TILE_PARTICLE_HEADER_COUNT		1
#define TILE_PARTICLE_COUNT				1023
#define TILE_PARTICLE_TOTAL_COUNT		(TILE_PARTICLE_HEADER_COUNT + TILE_PARTICLE_COUNT)

#define TILE_PARTICLE_DIM				16
#define TILE_LIGHT_DIM					32

#define TILE_PARTICLE_SIZE				(TILE_PARTICLE_DIM * TILE_PARTICLE_DIM)
#define TILE_LIGHT_SIZE					(TILE_LIGHT_DIM * TILE_LIGHT_DIM)

#define WORK_COUNT						4

struct ZERNShading_PointLight
{
	float3										PositionView;
	float										Range;
	
	float3										Color;
	float										FalloffExponent;
};

struct ZERNShading_SpotLight
{
	float3										PositionView;
	float										Range;
	
	float3										DirectionView;
	float										CosOuterConeAngle;
	
	float3										Color;
	float										FalloffExponent;
	
	float										CosInnerConeAngle;
	bool										CastShadow;
	uint										ShadowIndex;
	uint										Reserved;
};

struct ZERNShading_SpotLightShadow
{
	float4x4									ViewTransform;
	float4x4									ProjectionTransform;
	uint										ShadowSampleCount;
	float										ShadowSampleLength;
	float										ShadowDepthBias;
	float										ShadowNormalBias;
};

struct ZERNShading_ProjectiveLight
{
	float3										PositionView;
	float										Range;
	
	float3										Color;
	float										FalloffExponent;
	
	float4x4									ProjectionTransform;
	
	bool										CastShadow;
	uint										ShadowMapIndex;
	float2										Reserved;
	uint										ShadowSampleCount;
	float										ShadowSampleLength;
	float										ShadowDepthBias;
	float										ShadowNormalBias;
};

struct ZERNShading_Cascade
{
	float4x4									ViewTransform;
	float4x4									ProjectionTransform;
	
	uint										SampleCount;
	float										SampleLength;
	float										DepthBias;
	float										NormalBias;
};

struct ZERNShading_DirectionalLight
{
	float3										DirectionView;
	bool										CastShadow;
	
	float3										Color;
	uint										CascadeCount;
	
	uint										ShadowMapIndex;
	float3										Reserved;
	
	ZERNShading_Cascade							Cascades[MAX_CASCADE];
};

struct ZERNShading_Surface
{
	float3										PositionView;
	float3										NormalView;
	float3										Diffuse;
	float										SubsurfaceScattering;
	float3										Specular;
	float										SpecularPower;
	float3										Ambient;
	float										Opacity;
	float3										Emissive;
};

struct ZERNShading_Particle
{
	float3										Position;
	float										Radius;
	float4										Color;
	float2										Size;
	float										Rotation;
	bool										LightReceiver;
};

struct ZERNShading_Emitter
{
	uint										Axis;
	uint										Flags;
	
	uint										StartOffset;
	uint										ParticleCount;
	
	uint										ColorStartOffset;
	float2										Size;
	float										Rotation;
	float4										TexCoords;
	float4										Color;
};

cbuffer ZERNShading_EdgeDetection_Constants														: register(b9)
{
	float										ZERNShading_EdgeDetection_DepthThreshold;
	float										ZERNShading_EdgeDetection_NormalThreshold;
	float2										ZERNShading_EdgeDetection_Reserved;
};

cbuffer	ZERNShading_Constants																	: register(b8)
{
	ZERNShading_DirectionalLight				ZERNShading_DirectionalLights[MAX_DIRECTIONAL_LIGHT];
	uint										ZERNShading_DirectionalLightCount;
	uint										ZERNShading_PointLightCount;
	uint										ZERNShading_SpotLightCount;
	uint										ZERNShading_ProjectiveLightCount;
	
	uint										ZERNShading_TileCountX;
	float3										ZERNShading_Reserved;
};

cbuffer ZERNShading_PointLights_Constants														: register(b9)
{
	ZERNShading_PointLight						ZERNShading_PointLights[MAX_TILED_LIGHT];
};

cbuffer ZERNShading_SpotLights_Constants														: register(b10)
{
	ZERNShading_SpotLight						ZERNShading_SpotLights[MAX_TILED_LIGHT];
};

cbuffer ZERNShading_SpotLightShadow_Constants													: register(b11)
{
	ZERNShading_SpotLightShadow					ZERNShading_SpotLightsShadow[MAX_SPOT_LIGHT_SHADOW];
};

cbuffer ZERNShading_ProjectiveLight_Constants													: register(b9)
{
	ZERNShading_ProjectiveLight					ZERNShading_ProjectiveLightConstant;
};

cbuffer ZERNFog_Constants																		: register(b12)
{
	float										ZERNShading_FogDensity;
	float										ZERNShading_FogStartDistance;
	float										ZERNShading_FogHeight;
	float										ZERNShading_FogReserved0;
	
	float3										ZERNShading_FogColor;
	float										ZERNShading_FogReserved1;
};

cbuffer ZERNShading_Emitters_Constants															: register(b13)
{
	ZERNShading_Emitter							ZERNShading_Emitters[MAX_EMITTER];
};

Texture2D<float4>								ZERNShading_ProjectionTexture					: register(t10);
Texture2DArray<float>							ZERNShading_CascadeShadowMaps					: register(t11);
Texture2DArray<float>							ZERNShading_SpotShadowMaps						: register(t12);
Texture2DArray<float>							ZERNShading_ProjectiveShadowMaps				: register(t13);
Texture2D<float2>								ZERNShading_RandomVectors						: register(t14);

StructuredBuffer<uint>							ZERNShading_TileLightIndices					: register(t15);
StructuredBuffer<uint>							ZERNShading_TileParticleIndices					: register(t16);
Buffer<float4>									ZERNShading_ParticlePool						: register(t17);

static const float2 ZERNShading_PoissonDiskSamples16[] = 
{
	float2(0.7336021f, -0.6101369f),
	float2(0.9601513f, -0.0785616f),
	float2(0.3773608f, -0.8469894f),
	float2(0.5185271f, 0.1443706f),
	float2(0.03093804f, -0.4597275f),
	float2(-0.4946301f, 0.1093941f),
	float2(-0.08905835f, -0.9754086f),
	float2(-0.3824577f, -0.6261504f),
	float2(-0.05106043f, 0.2872932f),
	float2(0.4576186f, -0.2664742f),
	float2(-0.05316915f, 0.723882f),
	float2(0.5617343f, 0.7523296f),
	float2(-0.5127173f, 0.826395f),
	float2(-0.8760068f, -0.2534642f),
	float2(-0.8184632f, 0.431774f),
	float2(0.8985078f, 0.4366908f)
};

static const float2 ZERNShading_PoissonDiskSamples32[] = 
{
	{-0.975402, -0.0711386},
	{-0.920347, -0.41142},
	{-0.883908, 0.217872},
	{-0.884518, 0.568041},
	{-0.811945, 0.90521},
	{-0.792474, -0.779962},
	{-0.614856, 0.386578},
	{-0.580859, -0.208777},
	{-0.53795, 0.716666},
	{-0.515427, 0.0899991},
	{-0.454634, -0.707938},
	{-0.420942, 0.991272},
	{-0.261147, 0.588488},
	{-0.211219, 0.114841},
	{-0.146336, -0.259194},
	{-0.139439, -0.888668},
	{0.0116886, 0.326395},
	{0.0380566, 0.625477},
	{0.0625935, -0.50853},
	{0.125584, 0.0469069},
	{0.169469, -0.997253},
	{0.320597, 0.291055},
	{0.359172, -0.633717},
	{0.435713, -0.250832},
	{0.507797, -0.916562},
	{0.545763, 0.730216},
	{0.56859, 0.11655},
	{0.743156, -0.505173},
	{0.736442, -0.189734},
	{0.843562, 0.357036},
	{0.865413, 0.763726},
	{0.872005, -0.927},
};

static const float2 ZERNShading_PoissonDiskSamples64[] = 
{
	{-0.934812, 0.366741},
	{-0.918943, -0.0941496},
	{-0.873226, 0.62389},
	{-0.8352, 0.937803},
	{-0.822138, -0.281655},
	{-0.812983, 0.10416},
	{-0.786126, -0.767632},
	{-0.739494, -0.535813},
	{-0.681692, 0.284707},
	{-0.61742, -0.234535},
	{-0.601184, 0.562426},
	{-0.607105, 0.847591},
	{-0.581835, -0.00485244},
	{-0.554247, -0.771111},
	{-0.483383, -0.976928},
	{-0.476669, -0.395672},
	{-0.439802, 0.362407},
	{-0.409772, -0.175695},
	{-0.367534, 0.102451},
	{-0.35313, 0.58153},
	{-0.341594, -0.737541},
	{-0.275979, 0.981567},
	{-0.230811, 0.305094},
	{-0.221656, 0.751152},
	{-0.214393, -0.0592364},
	{-0.204932, -0.483566},
	{-0.183569, -0.266274},
	{-0.123936, -0.754448},
	{-0.0859096, 0.118625},
	{-0.0610675, 0.460555},
	{-0.0234687, -0.962523},
	{-0.00485244, -0.373394},
	{0.0213324, 0.760247},
	{0.0359813, -0.0834071},
	{0.0877407, -0.730766},
	{0.14597, 0.281045},
	{0.18186, -0.529649},
	{0.188208, -0.289529},
	{0.212928, 0.063509},
	{0.23661, 0.566027},
	{0.266579, 0.867061},
	{0.320597, -0.883358},
	{0.353557, 0.322733},
	{0.404157, -0.651479},
	{0.410443, -0.413068},
	{0.413556, 0.123325},
	{0.46556, -0.176183},
	{0.49266, 0.55388},
	{0.506333, 0.876888},
	{0.535875, -0.885556},
	{0.615894, 0.0703452},
	{0.637135, -0.637623},
	{0.677236, -0.174291},
	{0.67626, 0.7116},
	{0.686331, -0.389935},
	{0.691031, 0.330729},
	{0.715629, 0.999939},
	{0.8493, -0.0485549},
	{0.863582, -0.85229},
	{0.890622, 0.850581},
	{0.898068, 0.633778},
	{0.92053, -0.355693},
	{0.933348, -0.62981},
	{0.95294, 0.156896},
};

static const float2 ZERNShading_PoissonDiskSamples128[] = 
{
	{-0.9406119, 0.2160107},
	{-0.920003, 0.03135762},
	{-0.917876, -0.2841548},
	{-0.9166079, -0.1372365},
	{-0.8978907, -0.4213504},
	{-0.8467999, 0.5201505},
	{-0.8261013, 0.3743192},
	{-0.7835162, 0.01432008},
	{-0.779963, 0.2161933},
	{-0.7719588, 0.6335353},
	{-0.7658782, -0.3316436},
	{-0.7341912, -0.5430729},
	{-0.6825727, -0.1883408},
	{-0.6777467, 0.3313724},
	{-0.662191, 0.5155144},
	{-0.6569989, -0.7000636},
	{-0.6021447, 0.7923283},
	{-0.5980815, -0.5529259},
	{-0.5867089, 0.09857152},
	{-0.5774597, -0.8154474},
	{-0.5767041, -0.2656419},
	{-0.575091, -0.4220052},
	{-0.5486979, -0.09635002},
	{-0.5235587, 0.6594529},
	{-0.5170338, -0.6636339},
	{-0.5114055, 0.4373561},
	{-0.4844725, 0.2985838},
	{-0.4803245, 0.8482798},
	{-0.4651957, -0.5392771},
	{-0.4529685, 0.09942394},
	{-0.4523471, -0.3125569},
	{-0.4268422, 0.5644538},
	{-0.4187512, -0.8636028},
	{-0.4160798, -0.0844868},
	{-0.3751733, 0.2196607},
	{-0.3656596, -0.7324334},
	{-0.3286595, -0.2012637},
	{-0.3147397, -0.0006635741},
	{-0.3135846, 0.3636878},
	{-0.3042951, -0.4983553},
	{-0.2974239, 0.7496996},
	{-0.2903037, 0.8890813},
	{-0.2878664, -0.8622097},
	{-0.2588971, -0.653879},
	{-0.2555692, 0.5041648},
	{-0.2553292, -0.3389159},
	{-0.2401368, 0.2306108},
	{-0.2124457, -0.09935001},
	{-0.1877905, 0.1098409},
	{-0.1559879, 0.3356432},
	{-0.1499449, 0.7487829},
	{-0.146661, -0.9256138},
	{-0.1342774, 0.6185387},
	{-0.1224529, -0.3887629},
	{-0.116467, 0.8827716},
	{-0.1157598, -0.539999},
	{-0.09983152, -0.2407187},
	{-0.09953719, -0.78346},
	{-0.08604223, 0.4591112},
	{-0.02128129, 0.1551989},
	{-0.01478849, 0.6969455},
	{-0.01231739, -0.6752576},
	{-0.005001599, -0.004027164},
	{0.00248426, 0.567932},
	{0.00335562, 0.3472346},
	{0.009554717, -0.4025437},
	{0.02231783, -0.1349781},
	{0.04694207, -0.8347212},
	{0.05412609, 0.9042216},
	{0.05812819, -0.9826952},
	{0.1131321, -0.619306},
	{0.1170737, 0.6799788},
	{0.1275105, 0.05326218},
	{0.1393405, -0.2149568},
	{0.1457873, 0.1991508},
	{0.1474208, 0.5443151},
	{0.1497117, -0.3899909},
	{0.1923773, 0.3683496},
	{0.2110928, -0.7888536},
	{0.2148235, 0.9586087},
	{0.2152219, -0.1084362},
	{0.2189204, -0.9644538},
	{0.2220028, -0.5058427},
	{0.2251696, 0.779461},
	{0.2585723, 0.01621339},
	{0.2612841, -0.2832426},
	{0.2665483, -0.6422054},
	{0.2939872, 0.1673226},
	{0.3235748, 0.5643662},
	{0.3269232, 0.6984669},
	{0.3425438, -0.1783788},
	{0.3672505, 0.4398117},
	{0.3755714, -0.8814359},
	{0.379463, 0.2842356},
	{0.3822978, -0.381217},
	{0.4057849, -0.5227674},
	{0.4168737, -0.6936938},
	{0.4202749, 0.8369391},
	{0.4252189, 0.03818182},
	{0.4445904, -0.09360636},
	{0.4684285, 0.5885228},
	{0.4952184, -0.2319764},
	{0.5072351, 0.3683765},
	{0.5136194, -0.3944138},
	{0.519893, 0.7157083},
	{0.5277841, 0.1486474},
	{0.5474944, -0.7618791},
	{0.5692734, 0.4852227},
	{0.582229, -0.5125455},
	{0.583022, 0.008507785},
	{0.6500257, 0.3473313},
	{0.6621304, -0.6280518},
	{0.6674218, -0.2260806},
	{0.6741871, 0.6734863},
	{0.6753459, 0.1119422},
	{0.7083091, -0.4393666},
	{0.7106963, -0.102099},
	{0.7606754, 0.5743545},
	{0.7846709, 0.2282225},
	{0.7871446, 0.3891495},
	{0.8071781, -0.5257092},
	{0.8230689, 0.002674922},
	{0.8531976, -0.3256475},
	{0.8758298, -0.1824844},
	{0.8797691, 0.1284946},
	{0.926309, 0.3576975},
	{0.9608918, -0.03495717},
	{0.972032, 0.2271516}
};

ZERNShading_Surface ZERNShading_GetSurfaceFromGBuffers(float2 PositionViewport
//#ifdef MSAA_ENABLED
, float SampleIndex = 0
//#endif
)
{
	#ifdef MSAA_ENABLED
		float2 GBufferDimensions;
		float Dummy;
		ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimensions.x, GBufferDimensions.y, Dummy);
		float Depth = ZERNGBuffer_DepthBuffer.Load(PositionViewport, SampleIndex).x;
		float4 VelocitySpecularPower = ZERNGBuffer_Buffer1.Load(PositionViewport, SampleIndex);
		float4 DiffuseSubsurface = ZERNGBuffer_Buffer2.Load(PositionViewport, SampleIndex);
		float4 NormalSpecularColor = ZERNGBuffer_Buffer3.Load(PositionViewport, SampleIndex);
	#else
		float2 GBufferDimensions;
		ZERNGBuffer_DepthBuffer.GetDimensions(GBufferDimensions.x, GBufferDimensions.y);
		float Depth = ZERNGBuffer_DepthBuffer.Load(int3(PositionViewport, 0)).x;
		float4 VelocitySpecularPower = ZERNGBuffer_Buffer1.Load(int3(PositionViewport, 0));
		float4 DiffuseSubsurface = ZERNGBuffer_Buffer2.Load(int3(PositionViewport, 0));
		float4 NormalSpecularColor = ZERNGBuffer_Buffer3.Load(int3(PositionViewport, 0));
	#endif
	
	ZERNShading_Surface Surface;
	Surface.PositionView = ZERNTransformations_ViewportToView(PositionViewport.xy, GBufferDimensions, Depth);
	Surface.SpecularPower = VelocitySpecularPower.w;
	Surface.Diffuse = DiffuseSubsurface.xyz;
	Surface.SubsurfaceScattering = DiffuseSubsurface.w;
	Surface.NormalView = normalize(NormalSpecularColor.xyz * 2.0f - 1.0f);
	Surface.Specular = NormalSpecularColor.www;
	
	return Surface;
}

bool ZERNShading_EdgeDetection_MSAA(ZERNShading_Surface Surfaces[SAMPLE_COUNT])
{
	bool EdgePixel = false;
	[unroll]
	for (uint S = 1; S < SAMPLE_COUNT; S++)
	{
		EdgePixel = EdgePixel || 
		(abs(Surfaces[S].PositionView.z - Surfaces[0].PositionView.z) > ZERNShading_EdgeDetection_DepthThreshold) || 
		(dot(Surfaces[S].NormalView, Surfaces[0].NormalView) < ZERNShading_EdgeDetection_NormalThreshold);
	}
	
	return EdgePixel;
}

float4 ZERNShading_CalculateFogColor(float3 PositionView)
{
	float Distance = length(PositionView);
	float Value = max(0.0f, Distance - ZERNShading_FogStartDistance) * ZERNShading_FogDensity;
	float FogFactor = exp(-Value * Value);

	return float4(ZERNShading_FogColor, max(1.0f - FogFactor, 0.0f));
}

float ZERNShading_CalculateVisibility(uint SampleCount, float SampleLength, Texture2D<float> ShadowMap, float2 ShadowMapDimensions, float3 TexCoordDepth)
{
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNShading_RandomVectors.SampleLevel(ZERNSampler_PointMirror, 1024.0f * TexCoordDepth.xy, 0.0f) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for (uint I = 0; I < SampleCount; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNShading_PoissonDiskSamples64[I], RandomVector);
		float2 Offset = RandomOrientedSample * SampleLength / ShadowMapDimensions;
		Visibility += ShadowMap.SampleCmpLevelZero(ZERNSampler_ComparisonLinearPointClamp, TexCoordDepth.xy + Offset, TexCoordDepth.z);
	}
	
	Visibility /= SampleCount;
	
	return Visibility;
}

float ZERNShading_CalculateVisibilityArray(uint SampleCount, float SampleLength, Texture2DArray<float> ShadowMap, uint ShadowMapIndex, float2 ShadowMapDimensions, float3 TexCoordDepth)
{		
	float Visibility = 0.0f;
	
	float2 RandomVector = ZERNShading_RandomVectors.SampleLevel(ZERNSampler_PointMirror, 1024.0f * TexCoordDepth.xy, 0.0f) * 2.0f - 1.0f;
	RandomVector = normalize(RandomVector);
	
	for (uint I = 0; I < SampleCount; I++)
	{
		float2 RandomOrientedSample = reflect(ZERNShading_PoissonDiskSamples64[I], RandomVector);
		float2 Offset = RandomOrientedSample * SampleLength / ShadowMapDimensions;
		Visibility += ShadowMap.SampleCmpLevelZero(ZERNSampler_ComparisonLinearPointClamp, float3(TexCoordDepth.xy + Offset, ShadowMapIndex), TexCoordDepth.z);
	}
	
	Visibility /= SampleCount;
	
	return Visibility;
}

bool ZERNShading_InsideLightVolume(float4x4 VolumeProjectionTransform, float3 PositionVector, inout float3 OutTexCoordDepth)
{
	float4 TexCoordDepth = mul(VolumeProjectionTransform, float4(PositionVector, 1.0f));
	OutTexCoordDepth = TexCoordDepth.xyz / TexCoordDepth.w;
	
	if (all(OutTexCoordDepth >= 0.0f && OutTexCoordDepth <= 1.0f))
		return true;
	else
		return false;
}

float3 ZERNShading_Diffuse_Lambert(float3 LightDirectionView, ZERNShading_Surface Surface)
{
	return (1.0f - Surface.SubsurfaceScattering) * Surface.Diffuse;
}

float3 ZERNShading_Specular_BlinnPhong(float3 LightDirectionView, ZERNShading_Surface Surface)
{	
	float3 ViewDirection = normalize(-Surface.PositionView);
	float3 HalfVector = normalize(ViewDirection + LightDirectionView);
	float NdotH = max(0.0f, dot(Surface.NormalView, HalfVector));
	
	return pow(NdotH, Surface.SpecularPower) * Surface.Specular;
}

float3 ZERNShading_TotalBRDF(float3 LightDirectionView, ZERNShading_Surface Surface)
{
	float3 ResultSurfaceDiffuse = ZERNShading_Diffuse_Lambert(LightDirectionView, Surface);
	float3 ResultSurfaceSpecular = ZERNShading_Specular_BlinnPhong(LightDirectionView, Surface);
	
	return (ResultSurfaceDiffuse + ResultSurfaceSpecular);
}

float ZERNShading_DistanceAttenuation(float FalloffExponent, float Range, float DistanceToLight)
{
	return saturate((Range - DistanceToLight) / Range) / (((DistanceToLight * DistanceToLight) / Range) + exp(FalloffExponent * DistanceToLight / Range));
}

float3 ZERNShading_DirectionalShadowing(ZERNShading_DirectionalLight DirectionalLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	
	if (DirectionalLight.CastShadow)
	{	
		if (Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			for (uint CascadeIndex = 0; CascadeIndex < DirectionalLight.CascadeCount; CascadeIndex++)
			{
				ZERNShading_Cascade Cascade = DirectionalLight.Cascades[CascadeIndex];
				
				float3 PositionLightView = mul(Cascade.ViewTransform, float4(Surface.PositionView, 1.0f)).xyz;
				float3 NormalLightView = mul(Cascade.ViewTransform, float4(Surface.NormalView, 0.0f)).xyz;
				PositionLightView += normalize(NormalLightView) * Cascade.NormalBias;
				
				float3 TexCoordDepth;
				if (ZERNShading_InsideLightVolume(Cascade.ProjectionTransform, PositionLightView, TexCoordDepth))
				{
					float2 ShadowMapDimensions;
					float Index = 0.0f;
					ZERNShading_CascadeShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
					
					TexCoordDepth.z += Cascade.DepthBias;
					Visibility = ZERNShading_CalculateVisibilityArray(Cascade.SampleCount, Cascade.SampleLength, ZERNShading_CascadeShadowMaps, DirectionalLight.ShadowMapIndex + CascadeIndex, ShadowMapDimensions, TexCoordDepth);
					
					break;
				}
			}
		}
	}
	
	return Visibility;
}

float3 ZERNShading_DirectionalShading(ZERNShading_DirectionalLight DirectionalLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	float NdotL = dot(DirectionalLight.DirectionView, Surface.NormalView);
	NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
	if (NdotL > 0.0f)
	{	
		ResultColor = ZERNShading_TotalBRDF(DirectionalLight.DirectionView, Surface) * DirectionalLight.Color * NdotL;
		ResultColor *= ZERNShading_DirectionalShadowing(DirectionalLight, Surface);
	}
	
	return ResultColor;
}

float3 ZERNShading_ProjectiveShadowing(ZERNShading_ProjectiveLight ProjectiveLight, ZERNShading_Surface Surface, float3 TexCoordDepth)
{
	float Visibility = 1.0f;
	
	if (ProjectiveLight.CastShadow)
	{
		if (Surface.PositionView.z < ZERNView_ShadowDistance)
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNShading_ProjectiveShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
			
			TexCoordDepth.z += ProjectiveLight.ShadowDepthBias;
			Visibility = ZERNShading_CalculateVisibilityArray(ProjectiveLight.ShadowSampleCount, ProjectiveLight.ShadowSampleLength, ZERNShading_ProjectiveShadowMaps, ProjectiveLight.ShadowMapIndex, ShadowMapDimensions, TexCoordDepth);
		}
	}
	
	return Visibility;
}

float3 ZERNShading_ProjectiveShading(ZERNShading_ProjectiveLight ProjectiveLight, ZERNShading_Surface Surface)
{	
	float3 ResultColor = 0.0f;
	
	float3 VectorToLight = ProjectiveLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < ProjectiveLight.Range)
	{	
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		
		float NdotL = dot(LightDirectionView, Surface.NormalView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		if (NdotL > 0.0f)
		{		
			float3 TexCoordDepth;
			if (ZERNShading_InsideLightVolume(ProjectiveLight.ProjectionTransform, Surface.PositionView, TexCoordDepth))
			{			
				ProjectiveLight.Color *= ZERNShading_ProjectionTexture.SampleLevel(ZERNSampler_LinearBorderZero, TexCoordDepth.xy, 0.0f).rgb;
					
				ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * ProjectiveLight.Color * NdotL * ZERNShading_DistanceAttenuation(ProjectiveLight.FalloffExponent, ProjectiveLight.Range, DistanceToLight);
				ResultColor *= ZERNShading_ProjectiveShadowing(ProjectiveLight, Surface, TexCoordDepth);
			}
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_PointShading(ZERNShading_PointLight PointLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	float3 VectorToLight = PointLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < PointLight.Range)
	{		
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		
		float NdotL = dot(LightDirectionView, Surface.NormalView);
		NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
		if (NdotL > 0.0f)
		{		
			ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * PointLight.Color * NdotL * ZERNShading_DistanceAttenuation(PointLight.FalloffExponent, PointLight.Range, DistanceToLight);
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_SpotShadowing(ZERNShading_SpotLight SpotLight, ZERNShading_Surface Surface)
{
	float Visibility = 1.0f;
	
	[branch]
	if (SpotLight.CastShadow)
	{
		ZERNShading_SpotLightShadow SpotLightShadow = ZERNShading_SpotLightsShadow[SpotLight.ShadowIndex];

		float3 PositionLightView = mul(SpotLightShadow.ViewTransform, float4(Surface.PositionView, 1.0f)).xyz;
		float3 NormalLightView = mul(SpotLightShadow.ViewTransform, float4(Surface.NormalView, 0.0f)).xyz;
		PositionLightView += normalize(NormalLightView) * SpotLightShadow.ShadowNormalBias;
		
		float3 TexCoordDepth;
		if (ZERNShading_InsideLightVolume(SpotLightShadow.ProjectionTransform, PositionLightView, TexCoordDepth))
		{
			float2 ShadowMapDimensions;
			float Index = 0.0f;
			ZERNShading_SpotShadowMaps.GetDimensions(ShadowMapDimensions.x, ShadowMapDimensions.y, Index);
			
			TexCoordDepth.z += SpotLightShadow.ShadowDepthBias;
			Visibility = ZERNShading_CalculateVisibilityArray(SpotLightShadow.ShadowSampleCount, SpotLightShadow.ShadowSampleLength, ZERNShading_SpotShadowMaps, SpotLight.ShadowIndex, ShadowMapDimensions, TexCoordDepth);
		}
	}

	return Visibility;
}

float3 ZERNShading_SpotShading(ZERNShading_SpotLight SpotLight, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;

	float3 VectorToLight = SpotLight.PositionView - Surface.PositionView;
	float DistanceToLight = length(VectorToLight);
	if (DistanceToLight < SpotLight.Range)
	{
		float3 LightDirectionView = VectorToLight / DistanceToLight;
		float CosAlpha = dot(SpotLight.DirectionView, LightDirectionView);
		
		if (CosAlpha > SpotLight.CosOuterConeAngle)
		{
			float NdotL = dot(LightDirectionView, Surface.NormalView);
			NdotL = (Surface.SubsurfaceScattering != 0.0f) ? abs(NdotL) : max(0.0f, NdotL);
			if (NdotL > 0.0f)
			{
				ResultColor = ZERNShading_TotalBRDF(LightDirectionView, Surface) * SpotLight.Color * NdotL * 
				pow(saturate((CosAlpha - SpotLight.CosOuterConeAngle) / (SpotLight.CosInnerConeAngle - SpotLight.CosOuterConeAngle)), SpotLight.FalloffExponent) * 
				ZERNShading_DistanceAttenuation(SpotLight.FalloffExponent, SpotLight.Range, DistanceToLight);

				ResultColor *= ZERNShading_SpotShadowing(SpotLight, Surface);
			}
		}
	}
	
	return ResultColor;
}

float3 ZERNShading_Shade(float2 PositionViewport, ZERNShading_Surface Surface)
{
	float3 ResultColor = 0.0f;
	
	uint2 TileId = floor(PositionViewport) / TILE_LIGHT_DIM;
	uint TileIndex = TileId.y * ZERNShading_TileCountX + TileId.x;
	uint TileStartOffset = TileIndex * TILE_LIGHT_TOTAL_COUNT;
	
	uint TilePointLightCount = ZERNShading_TileLightIndices[TileStartOffset];
	#ifdef ZERN_SHADING_TRANSPARENT
		uint TilePointLightCountTransparent = ZERNShading_TileLightIndices[TileStartOffset + 1];
		uint TileSpotLightCountTransparent = ZERNShading_TileLightIndices[TileStartOffset + 2];
		
		TilePointLightCount += TilePointLightCountTransparent;
	#endif
	
	[unroll(2)]
	for (uint I = 0; I < ZERNShading_DirectionalLightCount; I++)
		ResultColor += ZERNShading_DirectionalShading(ZERNShading_DirectionalLights[I], Surface);
	
	uint TailCount = TilePointLightCount & (WORK_COUNT - 1);
	uint BufferIndex = TileStartOffset + TILE_LIGHT_HEADER_COUNT;
	uint LightIndices[WORK_COUNT];
	
	[unroll(WORK_COUNT - 1)]
	for (uint JJ = 0; JJ < TailCount; JJ++)
	{
		LightIndices[JJ] = ZERNShading_TileLightIndices[BufferIndex++];
		
		ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[LightIndices[JJ]], Surface);
	}
	
	for (uint J = TailCount; J < TilePointLightCount; J += WORK_COUNT)
	{
		[unroll]
		for (int W = 0; W < WORK_COUNT; W++)
			LightIndices[W] = ZERNShading_TileLightIndices[BufferIndex++];
		
		[unroll]
		for (int WW = 0; WW < WORK_COUNT; WW++)
			ResultColor += ZERNShading_PointShading(ZERNShading_PointLights[LightIndices[WW]], Surface);
	}
	
	#ifdef ZERN_SHADING_TRANSPARENT
		TailCount = (TileSpotLightCountTransparent & (WORK_COUNT - 1));
		
		[unroll(WORK_COUNT - 1)]
		for (uint KK = 0; KK < TailCount; KK++)
		{
			LightIndices[KK] = ZERNShading_TileLightIndices[BufferIndex++];
			
			ResultColor += ZERNShading_SpotShading(ZERNShading_SpotLights[LightIndices[KK]], Surface);
		}
		
		for (uint K = TailCount; K < TileSpotLightCountTransparent; K += WORK_COUNT)
		{
			[unroll]
			for (int W = 0; W < WORK_COUNT; W++)
				LightIndices[W] = ZERNShading_TileLightIndices[BufferIndex++];
			
			[unroll]
			for (int WW = 0; WW < WORK_COUNT; WW++)
				ResultColor += ZERNShading_SpotShading(ZERNShading_SpotLights[LightIndices[WW]], Surface);
		}
		
		ResultColor += Surface.Ambient + Surface.Emissive;
		
		float4 FogColor = ZERNShading_CalculateFogColor(Surface.PositionView);
		ResultColor = lerp(ResultColor, FogColor.rgb, FogColor.a);
	#endif
	
	return ResultColor;	
}

#endif
