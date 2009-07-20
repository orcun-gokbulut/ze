//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Material.h
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
#ifndef __ZE_MATERIAL_H__ 
#define __ZE_MATERIAL_H__

#include "ZEMath/ZEMath.h"
#include "Definitions.h"
#include "Texture.h"
#include "Vertex.h"
#include "Shader.h"

enum ZEShaderType
{
	ZE_ST_MATERIAL,
	ZE_ST_POSTEFFECT,
	ZE_ST_PARTICLE,
	ZE_ST_OTHER
};

class ZEShader
{
	public:
		virtual ZEShaderType			GetShaderType() = 0;
		virtual size_t					GetLightPerPass(ZELightType LightType) = 0;

		virtual bool					Setup() = 0;
		virtual bool					PreLightningPass() = 0;
		virtual bool					LightPass(ZELightType Light, ZESmartArray<ZERLLight*> Lights) = 0;
};


enum ZEMaterialType
{
	ZE_MT_FIXEDMATERIAL,
	ZE_MT_CUSTOMMATERIAL,
	ZE_MT_HLFXMATERIAL
};

enum ZETransparancyMode
{
	ZE_TM_NOTRANSPARACY	= 0,
	ZE_TM_ALPHACULL = 1,
	ZE_TM_REGULAR = 2,
	ZE_TM_ADDAPTIVE	= 3,
	ZE_TM_SUBTRACTIVE = 4,
};
class ZEShader
{
	public:
		bool							TwoSided;
		bool							LightningEnabled;
		bool							Wireframe;
		ZETransparancyMode				TransparancyMode;
		bool							RecivesShadow;
		unsigned int					TransparancyCullLimit;


		virtual bool					BeginPass(ZEMaterialPassType PassType, ZERenderListLi) = 0;

		virtual	void					SetZero() = 0;
};

class ZEDefaultMaterial : public ZEMaterial
{
	private:
		ZEShader*						Shader;
		unsigned int					ShaderComponents;

	public:
		virtual
		union
		{
			struct
			{
				ZEVector3				AmbientColor;
				float					Opasity;
				ZEVector3				DiffuseColor;
				float					Reserved0;
				ZEVector3				SpecularColor;
				float					SpecularFactor;
				ZEVector3				EmmisiveColor;
				float					EmmisiveFactor;
				float					ReflectionFactor;
				float					RefractionFactor;
				ZEVector2				DetailMapTiling;
			};
			float						PixelShaderConstants[16];
		};

		union
		{
			struct
			{
				const ZETexture*		DiffuseMap;
				const ZETexture*		NormalMap;
				const ZETexture*		SpecularMap;
				const ZETexture*		EmmisiveMap;
				const ZETexture*		OpacityMap;
				const ZETexture*		DetailMap;
				const ZETexture*		DetailNormalMap;
				const ZECubeTexture*	EnvironmentMap;
				const ZETexture*		LightMap;
			};
			ZETexture*					Textures[9];
		};

		void							SetShaderComponents(unsigned int ShaderComponents);
		unsigned int					GetShaderComponents() const;

		virtual void					Release() const;

		virtual void					SetZero();

		virtual const ZEShader*			GetShader() const;

		virtual const ZEVector4* 		GetVertexShaderConstants(int* Count) const;
		virtual const ZEVector4* 		GetPixelShaderConstants(int* Count) const;
		virtual const ZETextureBase**	GetTextures(int* Count) const;

										ZEDefaultMaterial();
										~ZEDefaultMaterial();
};

#endif
