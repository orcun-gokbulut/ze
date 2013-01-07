//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPointLight.h
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
#ifndef	__ZE_POINT_LIGHT_H__
#define __ZE_POINT_LIGHT_H__

#include "ZELight.h"
#include "ZEMath/ZEViewSphere.h"

class ZETexture2D;

ZE_META_OBJECT_DESCRIPTION(ZEPointLight);

class ZEPointLight  : public ZELight
{
	ZE_META_ENTITY(ZEPointLight)
	private:
		ZETexture2D*					FrontShadowMap;
		ZETexture2D*					BackShadowMap;

		ZEViewSphere					ViewVolume;
		ZEMatrix4x4						ViewProjectionMatrix;

										ZEPointLight();
		virtual							~ZEPointLight();

	public:
		ZELightType						GetLightType();

		virtual void					SetCastShadows(bool NewValue);

		ZETexture2D*					GetFrontShadowMap();
		ZETexture2D*					GetBackShadowMap();

		virtual void					Deinitialize();

		virtual ZESize					GetViewCount();
		virtual const ZEViewVolume&		GetViewVolume(ZESize Index = 0);
		virtual const ZEMatrix4x4&		GetViewTransform(ZESize Index = 0);

		virtual void					RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer);

		static ZEPointLight*			CreateInstance();
};

#endif

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEPointLight" parent="ZELight" description="Point Light"/>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/
