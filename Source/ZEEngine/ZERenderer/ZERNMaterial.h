//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMaterial.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEGraphics/ZEGRHolder.h"
#include "ZEGraphics/ZEGRResource.h"

enum ZEMaterialTransparancyMode
{
	ZE_MTM_NONE					= 0,
	ZE_MTM_REGULAR				= 2,
	ZE_MTM_ADDAPTIVE			= 3,
	ZE_MTM_SUBTRACTIVE			= 4,
}; 

enum ZEMaterialOpacityComponent
{
	ZE_MOC_CONSTANT				= 0,
	ZE_MOC_BASE_MAP_ALPHA		= 1,
	ZE_MOC_OPACITY_MAP			= 2,
};

class ZEGRContext;
class ZERNCommand;
class ZERNRenderer;
class ZERNStage;
class ZEGRRenderState;
class ZEGRRenderStateData;
class ZERNStageManager;

class ZERNMaterialStage
{
	public:
		ZERNStage*						Stage;
		ZEGRHolder<ZEGRRenderStateData> RenderState;

										ZERNMaterialStage();
};

#define ZEGR_MAX_RENDERER_STAGE_COUNT 20

class ZERNMaterial : public ZEGRResource
{
	ZE_OBJECT
	private:
		bool							ShadowCaster;
		bool							ShadowReceiver;
		bool							LightningEnabled;

		ZERNMaterialStage				Stages[ZEGR_MAX_RENDERER_STAGE_COUNT];
		ZESize							StageCount;

		void							AddStage(ZERNStage* Stage, ZEGRRenderStateData* State);
		ZEGRRenderStateData*			GetRenderState(ZERNStage* Stage);

	protected:
										ZERNMaterial();
		virtual							~ZERNMaterial();

	public:
		virtual ZEGRResourceType		GetResourceType();

		virtual void					SetShadowCaster(bool Value);
		bool							GetShadowCaster() const;

		virtual void					SetShadowReceiver(bool Value);
		bool							GetShadowReceiver() const;

		virtual void					SetLightningEnabled(bool Enabled);
		bool							GetLightningEnabled() const;

		virtual bool					SetupMaterial(ZEGRContext* Context, ZERNStage* Stage);
		virtual void					CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage);

		virtual void					UpdateMaterial();
};
