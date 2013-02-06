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

#include "ZETypes.h"

typedef ZEUInt32 ZEMaterialFlags;
#define ZE_MTF_NONE						0
#define ZE_MTF_PRE_Z_PASS				1
#define ZE_MTF_G_BUFFER_PASS			2
#define ZE_MTF_L_BUFFER_PASS			4
#define ZE_MTF_FORWARD_PASS				8
#define ZE_MTF_SUPPORTS_SKINNING		16
#define ZE_MTF_SUPPORTS_MORPHING		32
#define ZE_MTF_SUPPORTS_INSTANCING		64

enum ZEMaterialTransparancyMode
{
	ZE_MTM_NONE					= 0,
	ZE_MTM_REGULAR				= 2,
	ZE_MTM_ADDAPTIVE			= 3,
	ZE_MTM_SUBTRACTIVE			= 4
}; 

enum ZEMaterialOpacityComponent
{
	ZE_MOC_CONSTANT				= 0,
	ZE_MOC_BASE_MAP_ALPHA		= 1,
	ZE_MOC_OPACITY_MAP			= 2
};

class ZERenderer;
class ZERenderStage;
class ZERenderCommand;

class ZEMaterial
{
	protected:
		bool					ShadowCaster;
		bool					ShadowReciver;
		bool					LightningEnabled;

								ZEMaterial();
		virtual					~ZEMaterial();

	public:
		virtual ZEUInt32		GetHash() const = 0;
		virtual ZEMaterialFlags	GetMaterialFlags() const = 0;

		// Options
		void					SetShadowCaster(bool Value);
		bool					GetShadowCaster() const;

		void					SetShadowReciver(bool Value);
		bool					GetShadowReciver() const;

		void					SetLightningEnabled(bool Enabled);
		bool					GetLightningEnabled() const;

		virtual void			Destroy();
		virtual void			UpdateMaterial();

		virtual bool			SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		virtual void			AdvanceAnimation(float TimeElapsed);
};

#endif
