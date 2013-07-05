//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialDefault.h
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
#ifndef __ZE_MATERIAL_DEFAULT_H__ 
#define __ZE_MATERIAL_DEFAULT_H__

#include "ZETypes.h"
#include "ZEMaterial.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZESamplerState.h"

#define ZE_MDC_NONE					0
#define ZE_MDC_AMBIENT				1 << 0
#define ZE_MDC_DIFFUSE				1 << 1
#define ZE_MDC_BASE_MAP				1 << 2
#define ZE_MDC_NORMAL_MAP			1 << 3
#define ZE_MDC_PARALLAX_MAP			1 << 4
#define ZE_MDC_SPECULAR				1 << 5
#define ZE_MDC_SPECULAR_MAP			1 << 6
#define ZE_MDC_EMISSIVE				1 << 7
#define ZE_MDC_EMISSIVE_MAP			1 << 8
#define ZE_MDC_OPACITY				1 << 9
#define ZE_MDC_OPACITY_MAP			1 << 10
#define ZE_MDC_OPACITY_CONSTANT		1 << 11
#define ZE_MDC_OPACITY_BASE_ALPHA	1 << 12
#define ZE_MDC_DETAIL_BASE_MAP		1 << 13
#define ZE_MDC_DETAIL_NORMAL_MAP	1 << 14
#define ZE_MDC_REFLECTION			1 << 15
#define ZE_MDC_REFRACTION			1 << 16
#define ZE_MDC_LIGHT_MAP			1 << 17
#define ZE_MDC_ALPHA_CULL			1 << 18
#define ZE_MDC_VERTEX_COLOR			1 << 19
#define ZE_MDC_SKINNING				1 << 20
#define ZE_MDC_SSAO					1 << 21
typedef	ZEFlagsBase<ZEUInt32>		ZEMaterialDefaultComponents;

__declspec(align(16))
struct ZEMaterialDefaultProperties
{
	float			Opacity;
	float			AlphaCullLimit;	
	float			SpecularShininess;
	float			SubSurfScatFactor;
	
	ZEVector2		DetailMapTiling;
	float			DetailDistanceFull;
	float			DetailDistanceFade;

	ZEVector3		AmbientColor;
	float			AmbientFactor;

	ZEVector3		DiffuseColor;
	float			DiffuseFactor;
	
	ZEVector3		EmissiveColor;
	float			EmissiveFactor;
	
	ZEVector3		SpecularColor;
	float			SpecularFactor;
	
	float			RefractionIndex;
	float			RefractionFactor;
	float			ReflectionFactor;
};

class ZEShader;

struct ZEMaterialDefaultShaders
{
	ZEShader*				ShadowPassVS;
	ZEShader*				ShadowPassPS;
	ZEShader*				GeometryPassVS;
	ZEShader*				GeometryPassPS;
	ZEShader*				AccumulationPassVS;
	ZEShader*				AccumulationPassPS;
	ZEShader*				TransparentPassVS;
	ZEShader*				TransparentPassPS;
};

struct ZEMaterialDefaultSamplers
{
	ZESamplerState			BaseMap;
	ZESamplerState			NormalMap;
	ZESamplerState			ParallaxMap;
	ZESamplerState			SpecularMap;
	ZESamplerState			EmissiveMap;
	ZESamplerState			OpacityMap;
	ZESamplerState			DetailBaseMap;
	ZESamplerState			DetailNormalMap;
	ZESamplerState			EnvironmentMap;
	ZESamplerState			LightMap;
};

class ZETexture2D;
class ZETextureCube;

struct ZEMaterialDefaultTextures
{
	ZETexture2D*			BaseMap;
	ZETexture2D*			NormalMap;
	ZETexture2D*			ParallaxMap;
	ZETexture2D*			SpecularMap;
	ZETexture2D*			EmissiveMap;
	ZETexture2D*			OpacityMap;
	ZETexture2D*			DetailBaseMap;
	ZETexture2D*			DetailNormalMap;
	ZETextureCube*			EnvironmentMap;
	ZETexture2D*			LightMap;
};

class ZETexture2DResource;
class ZETextureCubeResource;

struct ZEMaterialDefaultResources
{
	ZETexture2DResource*	BaseMap;
	ZETexture2DResource*	NormalMap;
	ZETexture2DResource*	ParallaxMap;
	ZETexture2DResource*	SpecularMap;
	ZETexture2DResource*	EmissiveMap;
	ZETexture2DResource*	OpacityMap;
	ZETexture2DResource*	DetailBaseMap;
	ZETexture2DResource*	DetailNormalMap;
	ZETextureCubeResource*	EnvironmentMap;
	ZETexture2DResource*	LightMap;
};

class ZEConstantBuffer;

struct ZEMaterialDefaultBuffers
{
	ZEConstantBuffer*		MaterialProperties;
};

class ZERenderStage;
class ZERenderCommand;

class ZEMaterialDefault : public ZEMaterial
{
	friend class ZERenderer;

	protected:
		ZEMaterialDefaultComponents		OldComponents;

		void							DestroyShaders();
		void							DestroyBuffers();

		void							UpdateShaders();
		void							UpdateBuffers();

		bool							SetupPassShadow(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool							SetupPassGeometry(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool							SetupPassTransparent(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);
		bool							SetupPassAccumulation(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

										ZEMaterialDefault();
		virtual							~ZEMaterialDefault();

	public:
		bool							ZCull;
		bool							ZWrite;
		bool							TwoSided;
		bool							Wireframe;

		bool							ShadowCaster;
		bool							ShadowReciever;

		ZEOpacityComponent				OpacityComponent;
		ZETransparancyMode				TransparencyMode;

		ZEMaterialDefaultShaders		Shaders;
		ZEMaterialDefaultBuffers		Buffers;
		ZEMaterialDefaultSamplers		Samplers;
		ZEMaterialDefaultTextures		Textures;
		ZEMaterialDefaultResources		Resources;
		ZEMaterialDefaultComponents		Components;

		virtual ZESize					GetHash() const;

		virtual bool					UpdateMaterial();

		void							WriteToFile(const ZEString& FilePath);
		void							ReadFromFile(const ZEString& FilePath);

		virtual bool					SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		static ZEMaterialDefault*		CreateInstance();
};

#endif
