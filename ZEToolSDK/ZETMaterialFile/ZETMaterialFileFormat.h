//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMaterialFileFormat.h
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
#ifndef __ZET_MATERIAL_FILE_FORMAT_H__
#define __ZET_MATERIAL_FILE_FORMAT_H__

#include "ZETTypes.h"
#include "ZEDS/ZEVariant.h"
#include "ZEMath/ZEVector.h"

#define ZE_MTLF_MAX_NAME_SIZE					128
#define ZE_MTLF_MAX_FILENAME_SIZE				256

#define ZE_FILE_MAKEVERSION(Major, Minor)		((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor))

#define ZE_MTLF_VERSION							ZE_FILE_MAKEVERSION(0, 1)
#define ZE_MTLF_HEADER							((ZEDWORD)((ZEDWORD)'ZEMF' + (ZEDWORD)'MTRL'))

#define	ZE_MTLF_MATERIAL_CHUNKID				((ZEDWORD)(ZE_MTLF_HEADER + (ZEDWORD)'FIXD'))
#define	ZE_MTLF_TEXTURE_CHUNK_ID				((ZEDWORD)(ZE_MTLF_HEADER + (ZEDWORD)'TEXT'))
#define	ZE_MTLF_ANIMATION_CHUNKID				((ZEDWORD)(ZE_MTLF_HEADER + (ZEDWORD)'ANIM'))
#define	ZE_MTLF_ANIMATION_FRAME_CHUNKID			((ZEDWORD)(ZE_MTLF_HEADER + (ZEDWORD)'ANKF'))

struct ZEMaterialFileHeaderChunk
{
	ZEDWORD							HEADER;
	ZEDWORD							Version;
	ZEDWORD							MaterialType;
	ZEDWORD							TextureCount;
	ZEDWORD							AnimationCount;
};

struct ZEMaterialFileTextureChunk
{
	ZEDWORD							ChunkId;
	char							FileName[ZE_MTLF_MAX_NAME_SIZE];
	ZEDWORD							TextureType;
	ZEDWORD							InternalSize;
};

struct ZEMaterialFileAnimationChunk
{
	ZEDWORD							ChunkId;
	char							Name[ZE_MTLF_MAX_NAME_SIZE];
	ZEDWORD							FrameCount;
};

struct ZEMaterialFileAnimationFrameChunk
{
	ZEDWORD							ChunkId;
	ZEDWORD							KeyCount;
};

struct ZEMaterialFileAnimationKeyChunk
{
	ZEDWORD							PropertyIndex;
	ZEVariant						Value;
};

struct ZEMaterialFileFixedMaterialChunk
{
	ZEDWORD							ChunkId;

	bool							TwoSided;
	bool							LightningEnabled;
	bool							Wireframe;
	ZEDWORD							TransparancyMode;
	bool							RecivesShadow;
	unsigned int					TransparancyCullLimit;

	bool							AmbientEnabled;
	ZEVector3						AmbientColor;
	float							AmbientFactor;

	bool							DiffuseEnabled;
	ZEVector3						DiffuseColor;
	float							DiffuseFactor;
	ZEINT32							DiffuseMap;
	ZEINT32							DiffuseMapAddressModeU;
	ZEINT32							DiffuseMapAddressModeV;


	ZEINT32							NormalMap;
	ZEINT32							NormalMapAddressModeU;
	ZEINT32							NormalMapAddressModeV;

	ZEINT32							ParallaxMap;
	ZEINT32							ParallaxMapAddressModeU;
	ZEINT32							ParallaxMapAddressModeV;

	bool							SpecularEnabled;
	ZEVector3						SpecularColor;
	float							SpecularFactor;
	float							SpecularShininess;	
	ZEINT32							SpecularMap;
	ZEINT32							SpecularMapAddressModeU;
	ZEINT32							SpecularMapAddressModeV;

	bool							EmmisiveEnabled;
	ZEVector3						EmmisiveColor;
	float							EmmisiveFactor;
	ZEINT32							EmmisiveMap;
	ZEINT32							EmmisiveMapAddressModeU;
	ZEINT32							EmmisiveMapAddressModeV;

	bool							OpacityEnabled;
	float							Opacity;
	ZEINT32							OpacityMap;
	ZEINT32							OpacityMapAddressModeU;
	ZEINT32							OpacityMapAddressModeV;

	bool							DetailMapEnabled;
	ZEVector2						DetailMapTiling;
	ZEINT32							DetailDiffuseMap;
	ZEINT32							DetailDiffuseMapAddressModeU;
	ZEINT32							DetailDiffuseMapAddressModeV;
	ZEINT32							DetailNormalMap;
	ZEINT32							DetailNormalMapAddressModeU;
	ZEINT32							DetailNormalMapAddressModeV;

	bool							ReflectionEnabled;
	float							ReflectionFactor;
	ZEINT32							ReflectionMap;
	ZEINT32							ReflectionMapAddressModeU;
	ZEINT32							ReflectionMapAddressModeV;
	ZEINT32							ReflectionMapAddressModeW;

	bool							RefractionEnabled;
	float							RefractionFactor;
	float							RefractionIndex;
	ZEINT32							RefractionMap;
	ZEINT32							RefractionMapAddressModeU;
	ZEINT32							RefractionMapAddressModeV;
	ZEINT32							RefractionMapAddressModeW;

	bool							LightMapEnabled;
	float							LightMapFactor;
	ZEINT32							LightMap;
	ZEINT32							LightMapAddressModeU;
	ZEINT32							LightMapAddressModeV;

	bool							DistortionEnabled;
	float							DistortionFactor;
	ZEINT32							DistortionMap;
	ZEINT32							DistortionMapAddressModeU;
	ZEINT32							DistortionMapAddressModeV;	
};

#endif
