//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightOmniProjective.h
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

#include "ZEMath/ZEBSphere.h"

class ZEGRTexture;

class ZELightOmniProjective : public ZELight
{
	ZE_OBJECT
	private:
		ZEHolder<const ZEGRTexture>		ProjectionTexture;
		ZEString						ProjectionTextureFileName;
		mutable ZEViewSphere			ViewVolume;

		void							LoadProjectionTexture();

		virtual ZEEntityResult			LoadInternal();
		virtual ZEEntityResult			UnloadInternal();

										ZELightOmniProjective();
		virtual							~ZELightOmniProjective();

	public:
		virtual ZELightType				GetLightType() const;
		virtual ZESize					GetViewCount() const;

		void							SetProjectionTextureFile(const ZEString& Filename);
		const ZEString&					GetProjectionTextureFile() const;

		void							SetProjectionTexture(const ZEGRTexture* Texture);
		const ZEGRTexture*				GetProjectionTexture();

		virtual const ZEViewVolume&		GetViewVolume(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&		GetViewTransform(ZESize Index = 0) const;
		virtual const ZEMatrix4x4&		GetProjectionTransform(ZESize Index = 0) const;

		static ZELightOmniProjective*	CreateInstance();
}
ZE_META_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZELightOmniProjective.png");
ZE_META_ATTRIBUTE_PROPERTY(ProjectionTextureFileName, ZEDEditor.PropertyEditor.Semantic, "Asset", "ZEDTextureAsset");
