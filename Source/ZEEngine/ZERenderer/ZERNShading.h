//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNShading.h
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

#pragma once

#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEVector.h"

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

#define TILE_PARTICLE_DIMENSION			16
#define TILE_PARTICLE_SIZE				(TILE_PARTICLE_DIMENSION * TILE_PARTICLE_DIMENSION)

#define TILE_LIGHT_DIMENSION			32
#define TILE_LIGHT_SIZE					(TILE_LIGHT_DIMENSION * TILE_LIGHT_DIMENSION)

struct ZERNShading_PointLight
{
	ZEVector3							PositionView;
	float								Range;

	ZEVector3							Color;
	float								FalloffExponent;
};

struct ZERNShading_SpotLight
{
	ZEVector3							PositionView;
	float								Range;

	ZEVector3							DirectionView;
	float								CosOuterConeAngle;

	ZEVector3							Color;
	float								FalloffExponent;

	float								CosInnerConeAngle;
	ZEBool32							CastShadow;
	ZEUInt								ShadowIndex;
	float								Reserved;
};

struct ZERNShading_SpotLightShadow
{
	ZEMatrix4x4							ViewTransform;
	ZEMatrix4x4							ProjectionTransform;
	ZEUInt								ShadowSampleCount;
	float								ShadowSampleLength;
	float								ShadowDepthBias;
	float								ShadowNormalBias;
};

struct ZERNShading_ProjectiveLight
{
	ZEVector3							PositionView;
	float								Range;

	ZEVector3							Color;
	float								FalloffExponent;
	
	ZEMatrix4x4							ProjectionTransform;
	
	ZEBool32							CastShadow;
	ZEUInt								ShadowMapIndex;
	ZEUInt								WorldTransformIndex;
	float								Reserved;

	ZEUInt								ShadowSampleCount;
	float								ShadowSampleLength;
	float								ShadowDepthBias;
	float								ShadowNormalBias;
};

struct ZERNShading_Cascade
{
	ZEMatrix4x4							ViewTransform;
	ZEMatrix4x4							ProjectionTransform;

	ZEUInt								SampleCount;
	float								SampleLength;
	float								DepthBias;
	float								NormalBias;
};

struct ZERNShading_DirectionalLight
{
	ZEVector3							DirectionView;
	ZEBool32							CastShadow;

	ZEVector3							Color;
	ZEUInt								CascadeCount;

	ZEUInt								ShadowMapIndex;
	ZEVector3							Reserved;

	ZERNShading_Cascade					Cascades[MAX_CASCADE];
};

struct ZERNShading_Constants
{
	ZERNShading_DirectionalLight		DirectionalLights[MAX_DIRECTIONAL_LIGHT];
	ZEUInt								DirectionalLightCount;
	ZEUInt								PointLightCount;
	ZEUInt								SpotLightCount;
	ZEUInt								ProjectiveLightCount;

	ZEUInt								TileCountX;
	ZEVector3							Reserved;
};

struct ZERNShading_EdgeDetectionConstants
{
	float								DepthThreshold;
	float								NormalThreshold;
	ZEVector2							Reserved;
};

struct ZERNShading_ParticleEmitter
{
	ZEUInt								Axis;
	ZEUInt								Flags;
	ZEUInt								StartOffset;
	ZEUInt								ParticleCount;

	ZEUInt								ColorStartOffset;
	ZEVector2							ParticleSize;
	float								ParticleRotation;
	ZEVector4							ParticleTexCoords;
	ZEVector4							ParticleColor;
};
