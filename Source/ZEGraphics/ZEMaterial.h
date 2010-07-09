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
	ZE_MT_NONE,
	ZE_MT_FIXED,
	ZE_MT_CUSTOM,
	ZE_MT_CGFX,
	ZE_MT_OTHERs
};
 
// ZEMaterialFlags
#define ZE_MF_NOCACHING
 
class ZERenderOrder;
class ZECamera;
class ZEPointLight;
class ZEProjectiveLight;
class ZEDirectionalLight;
class ZEOmniProjectiveLight;

ZE_META_CLASS_DESCRIPTION(ZEMaterial)

class ZEMaterial : public ZEClass
{
	ZE_META_CLASS() 
	protected:
										ZEMaterial();
		virtual							~ZEMaterial();

	public:
		virtual const char*				GetMaterialUID() const = 0;
		virtual unsigned int			GetMaterialFlags() const = 0;
		virtual ZEMaterialType			GetMaterialType() const = 0;

		// SetUp
		virtual void					SetZero() = 0;

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




