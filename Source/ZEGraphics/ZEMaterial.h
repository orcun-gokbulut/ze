//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterial.h
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

#include "ZEMeta\ZEClass.h"

enum ZEMaterialType
{
	ZE_MTT_NONE,
	ZE_MTT_NON_ILLUMUNATED,
	ZE_MTT_DEFERRED,
	ZE_MTT_FORWARD,
};

typedef ZEDWORD ZEMaterialFlags;
#define ZE_MTF_NONE						0
#define ZE_MTF_PREZ_PASS				1
#define ZE_MTF_G_BUFFER_PASS			2
#define ZE_MTF_SUPPORTS_SKINNING		4
#define ZE_MTF_SUPPORTS_MORPHING		8
#define ZE_MTF_SUPPORTS_INSTANCING		16

enum ZETextureFilterMode
{
	ZE_TF_AUTO					= 0,
	ZE_TF_FORCE_POINT			= 1,
	ZE_TF_FORCE_LINEAR			= 2,
	ZE_TF_FORCE_TRILINEAR		= 3,
	ZE_TF_FORCE_ANISOTROPHIC	= 4,
};

enum ZETextureAddressMode
{
	ZE_TAM_WRAP					= 0,
	ZE_TAM_CLAMP				= 1,
	ZE_TAM_MIRROR				= 2,
	ZE_TAM_BORDER				= 3,
};

enum ZEMaterialTransparancyMode
{
	ZE_MTM_NONE		= 0,
	ZE_MTM_ALPHACULL			= 1,
	ZE_MTM_REGULAR				= 2,
	ZE_MTM_ADDAPTIVE			= 3,
	ZE_MTM_SUBTRACTIVE			= 4,
}; 

enum ZEMaterialOpacityComponent
{
	ZE_MOC_CONSTANT				= 0,
	ZE_MOC_DIFFUSE_ALPHA		= 1,
	ZE_MOC_OPACITYMAP			= 2,
};

class ZERenderer;
class ZERenderOrder;

ZE_META_CLASS_DESCRIPTION(ZEMaterial)

class ZEMaterial : public ZEClass
{
	ZE_META_CLASS() 
	protected:
										ZEMaterial();
		virtual							~ZEMaterial();

	public:
		virtual ZEMaterialType			GetMaterialType() const = 0;
		virtual ZEMaterialFlags			GetMaterialFlags() const = 0;

		// SetUp
		virtual void					SetZero() = 0;
		
		// Render Pass
		virtual bool					SetupPreZPass(ZERenderer* Renderer, ZERenderOrder* RenderOrder) const;
		virtual bool					SetupGBufferPass(ZERenderer* Renderer, ZERenderOrder* RenderOrder) const;
		virtual bool					SetupForwardPass(ZERenderer* Renderer, ZERenderOrder* RenderOrder) const;
		virtual bool					SetupShadowPass() const;	

		virtual void					UpdateMaterial();

		virtual void					AdvanceAnimation(float TimeElapsed);

		virtual void					Release() = 0;
		virtual void					Destroy();
};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta>
		<class name="ZEMaterial" noinstance="true" description="Base class of materials."/>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
#endif




