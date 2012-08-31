//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialFileFormat.h
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
#ifndef __ZE_MATERIAL_FILE_FORMAT_H__
#define __ZE_MATERIAL_FILE_FORMAT_H__

#include "ZETypes.h"

#define ZE_MTLF_MAX_NAME_SIZE					128
#define ZE_MTLF_MAX_FILENAME_SIZE				256

#define ZE_FILE_MAKEVERSION(Major, Minor)		((((ZEUInt32)(Major)) << 16) + (ZEUInt32)(Minor))

#define ZE_MTLF_VERSION							ZE_FILE_MAKEVERSION(0, 1)
#define ZE_MTLF_HEADER							((ZEUInt32)((ZEUInt32)'ZEMF' + (ZEUInt32)'MTRL'))

#define	ZE_MTLF_MATERIAL_CHUNKID				((ZEUInt32)(ZE_MTLF_HEADER + (ZEUInt32)'FIXD'))
#define	ZE_MTLF_TEXTURE_CHUNK_ID				((ZEUInt32)(ZE_MTLF_HEADER + (ZEUInt32)'TEXT'))
#define	ZE_MTLF_ANIMATION_CHUNKID				((ZEUInt32)(ZE_MTLF_HEADER + (ZEUInt32)'ANIM'))
#define	ZE_MTLF_ANIMATION_FRAME_CHUNKID			((ZEUInt32)(ZE_MTLF_HEADER + (ZEUInt32)'ANKF'))

struct ZEMaterialFileHeaderChunk
{
	ZEUInt32							HEADER;
	ZEUInt32							Version;
	ZEUInt32							MaterialType;
	ZEUInt32							TextureCount;
	ZEUInt32							AnimationCount;
};

struct ZEMaterialFileTextureChunk
{
	ZEUInt32							ChunkId;
	char							FileName[ZE_MTLF_MAX_NAME_SIZE];
	ZEUInt32							TextureType;
	ZEUInt32							InternalSize;
};

struct ZEMaterialFileAnimationChunk
{
	ZEUInt32							ChunkId;
	char							Name[ZE_MTLF_MAX_NAME_SIZE];
	ZEUInt32							FrameCount;
};

struct ZEMaterialFileAnimationFrameChunk
{
	ZEUInt32							ChunkId;
	ZEUInt32							KeyCount;
};

struct ZEMaterialFileAnimationKeyChunk
{
	ZEUInt32							PropertyIndex;
	ZEVariant						Value;
};

struct ZEMaterialFileFixedMaterialChunk
{
	ZEUInt32						ChunkId;

	bool							TwoSided;
	bool							LightningEnabled;
	bool							Wireframe;
	ZEMaterialTransparancyMode		TransparancyMode;
	bool							RecivesShadow;
	ZEUInt					TransparancyCullLimit;

	bool							AmbientEnabled;
	ZEVector3						AmbientColor;
	float							AmbientFactor;

	bool							DiffuseEnabled;
	ZEVector3						DiffuseColor;
	float							DiffuseFactor;
	ZEInt32							DiffuseMap;
	ZEInt32							DiffuseMapAddressModeU;
	ZEInt32							DiffuseMapAddressModeV;

	bool							NormalMapEnabled;
	ZEInt32							NormalMap;
	ZEInt32							NormalMapAddressModeU;
	ZEInt32							NormalMapAddressModeV;

	bool							ParallaxMapEnabled;
	ZEInt32							ParallaxMap;
	ZEInt32							ParallaxMapAddressModeU;
	ZEInt32							ParallaxMapAddressModeV;

	bool							SpecularEnabled;
	ZEVector3						SpecularColor;
	float							SpecularFactor;
	float							SpecularShininess;	
	ZEInt32							SpecularMap;
	ZEInt32							SpecularMapAddressModeU;
	ZEInt32							SpecularMapAddressModeV;

	bool							EmmisiveEnabled;
	ZEVector3						EmmisiveColor;
	float							EmmisiveFactor;
	ZEInt32							EmmisiveMap;
	ZEInt32							EmmisiveMapAddressModeU;
	ZEInt32							EmmisiveMapAddressModeV;

	bool							OpacityEnabled;
	float							Opacity;
	ZEInt32							OpacityMap;
	ZEInt32							OpacityMapAddressModeU;
	ZEInt32							OpacityMapAddressModeV;

	bool							DetailMapEnabled;
	ZEVector2						DetailMapTiling;
	ZEInt32							DetailDiffuseMap;
	ZEInt32							DetailDiffuseMapAddressModeU;
	ZEInt32							DetailDiffuseMapAddressModeV;
	ZEInt32							DetailNormalMap;
	ZEInt32							DetailNormalMapAddressModeU;
	ZEInt32							DetailNormalMapAddressModeV;

	bool							ReflectionEnabled;
	float							ReflectionFactor;
	ZEInt32							ReflectionMap;
	ZEInt32							ReflectionMapAddressModeU;
	ZEInt32							ReflectionMapAddressModeV;
	ZEInt32							ReflectionMapAddressModeW;

	bool							RefractionEnabled;
	float							RefractionFactor;
	float							RefractionIndex;
	ZEInt32							RefractionMap;
	ZEInt32							RefractionMapAddressModeU;
	ZEInt32							RefractionMapAddressModeV;
	ZEInt32							RefractionMapAddressModeW;

	bool							LightMapEnabled;
	float							LightMapFactor;
	ZEInt32							LightMap;
	ZEInt32							LightMapAddressModeU;
	ZEInt32							LightMapAddressModeV;

	bool							DistortionEnabled;
	float							DistortionFactor;
	ZEInt32							DistortionMap;
	ZEInt32							DistortionMapAddressModeU;
	ZEInt32							DistortionMapAddressModeV;	
};

#endif
