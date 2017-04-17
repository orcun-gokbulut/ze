//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageShadowing.h
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

#include "ZERNStage.h"

#include "ZEPointer/ZEHolder.h"
#include "ZERNRenderer.h"
#include "ZELight.h"
#include "ZEExport.ZEEngine.h"

class ZEGRTexture;
class ZERNCommandDirectionalLight;
class ZERNCommandSpotLightShadow;
class ZERNCommandProjectiveLight;

class ZE_EXPORT_ZEENGINE ZERNStageShadowing : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags							DirtyFlags;
		ZERNRenderer					ShadowRenderer;

		ZEUInt							CascadeCount;
		ZEUInt							DirectionalShadowMapCount;
		ZEUInt							SpotShadowMapCount;
		ZEUInt							ProjectiveShadowMapCount;

		ZEUInt							DirectionalShadowMapIndex;
		ZEUInt							SpotShadowMapIndex;
		ZEUInt							ProjectiveShadowMapIndex;

		ZERNLightShadowResolution		DirectionalShadowMapsResolution;
		ZERNLightShadowResolution		SpotShadowMapsResolution;
		ZERNLightShadowResolution		ProjectiveShadowMapsResolution;

		ZEHolder<ZEGRTexture>			DirectionalShadowMaps;
		ZEHolder<ZEGRTexture>			SpotShadowMaps;
		ZEHolder<ZEGRTexture>			ProjectiveShadowMaps;

		virtual bool					InitializeInternal();
		virtual bool					DeinitializeInternal();

		void							GenerateDirectionalShadowMaps(ZEGRContext* Context, const ZERNCommandDirectionalLight& DirectionalLight);
		void							GenerateSpotShadowMaps(ZEGRContext* Context, const ZERNCommandSpotLightShadow& SpotLight);
		void							GenerateProjectiveShadowMaps(ZEGRContext* Context, const ZERNCommandProjectiveLight& ProjectiveLight);

		virtual void					CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt					GetId() const;
		virtual const ZEString&			GetName() const;

		void							SetCascadeCount(ZEUInt Count);
		ZEUInt							GetCascadeCount() const;

		void							SetDirectionalShadowMapCount(ZEUInt Count);
		ZEUInt							GetDirectionalShadowMapCount() const;

		void							SetSpotShadowMapCount(ZEUInt Count);
		ZEUInt							GetSpotShadowMapCount() const;

		void							SetProjectiveShadowMapCount(ZEUInt Count);
		ZEUInt							GetProjectiveShadowMapCount() const;

		void							SetDirectionalShadowMapsResolution(ZERNLightShadowResolution Resolution);
		ZERNLightShadowResolution		GetDirectionalShadowMapsResolution() const;

		void							SetSpotShadowMapsResolution(ZERNLightShadowResolution Resolution);
		ZERNLightShadowResolution		GetSpotShadowMapsResolution() const;

		void							SetProjectiveShadowMapsResolution(ZERNLightShadowResolution Resolution);
		ZERNLightShadowResolution		GetProjectiveShadowMapsResolution() const;

		virtual bool					Setup(ZEGRContext* Context);
		virtual void					CleanUp(ZEGRContext* Context);

										ZERNStageShadowing();
		virtual							~ZERNStageShadowing();
};
