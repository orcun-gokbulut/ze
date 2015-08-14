//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightProjective.h
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

#include "ZELight.h"

#include "ZEDS/ZEString.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEGraphics/ZEGRHolder.h"

class ZEGRTexture2D;
class ZETexture2DResource;

class ZELightProjective : public ZELight
{
	ZE_OBJECT

	private:
		float							FOV;
		float							AspectRatio;
		ZEViewFrustum					ViewVolume;
		ZEGRHolder<ZEGRTexture2D>		ProjectionTexture;
		ZEGRHolder<ZEGRTexture2D>		ShadowMap;
		ZEMatrix4x4						ViewProjectionMatrix;
		
		ZETexture2DResource*			ProjectionTextureResource;
		ZEString						ProjectionTextureFile;

		virtual bool					InitializeSelf();
		virtual bool					DeinitializeSelf();

										ZELightProjective();
		virtual							~ZELightProjective();

	public:
		ZELightType						GetLightType();

		void							SetFOV(float FOV);
		float							GetFOV() const;

		void							SetAspectRatio(float AspectRatio);
		float							GetAspectRatio() const;

		void							SetProjectionTextureFile(const ZEString& Filename);
		const ZEString&					GetProjectionTextureFile() const;

		void							SetProjectionTexture(ZEGRTexture2D* Texture);
		ZEGRTexture2D*					GetProjectionTexture() const;

		virtual ZESize					GetViewCount();
		virtual const ZEViewVolume&		GetViewVolume(ZESize Index = 0);
		virtual const ZEMatrix4x4&		GetViewTransform(ZESize Index = 0);

		ZEGRTexture2D*					GetShadowMap();

		static ZELightProjective*		CreateInstance();
};
