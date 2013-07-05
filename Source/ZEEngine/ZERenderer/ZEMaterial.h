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
#include "ZEDS/ZEArray.h"
#include "ZERenderStage.h"

enum ZETransparancyMode
{
	ZE_TM_NONE				= 0,
	ZE_TM_REGULAR			= 1,
	ZE_TM_ADDAPTIVE			= 2,
	ZE_TM_SUBTRACTIVE		= 3
};

enum ZEOpacityComponent
{
	ZE_OC_CONSTANT			= 0,
	ZE_OC_BASE_MAP_ALPHA	= 1,
	ZE_OC_OPACITY_MAP		= 2
};

class ZERenderCommand;

class ZEMaterial
{
	friend class ZERenderer;
	friend class ZERenderCommand;

	private:
		struct ZEMaterialState
		{
			bool				InUse : 1;
			bool				Deleted : 1;

		} State;

	protected:
		ZERenderStageType		EnabledStages;
		ZERenderStageType		SupportedStages;

								ZEMaterial();
		virtual					~ZEMaterial();

	public:
		virtual void			Destroy();

		virtual void			EnableStage(ZERenderStageType Stage);
		virtual void			DisableStage(ZERenderStageType Stage);

		virtual void			WriteToFile(const ZEString& FilePath);
		virtual void			ReadFromFile(const ZEString& FilePath);

		virtual bool			SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand);

		virtual ZESize			GetHash() const = 0;
		virtual bool			UpdateMaterial() = 0;
};

#endif
