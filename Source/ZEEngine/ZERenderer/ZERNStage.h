//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStage.h
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
#include "ZEInitializable.h"

#include "ZETypes.h"
#include "ZEDS/ZELink.h"
#include "ZEDS/ZEList2.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRFormat.h"
#include "ZEGraphics/ZEGRResource.h"

class ZEGRContext;
class ZEGRResource;
class ZEGRRenderState;
class ZERNRenderer;
class ZERNCommand;

ZE_ENUM(ZERNStageResourceUsageType)
{
	ZERN_SRUT_READ			= 1,
	ZERN_SRUT_WRITE			= 2,
	ZERN_SRUT_READ_WRITE	= 3
};

ZE_ENUM(ZERNStageResourceCreationFlag)
{
	ZERN_SRCF_GET_FROM_PREV	= 1,
	ZERN_SRCF_GET_OUTPUT	= 2,
	ZERN_SRCF_CREATE_OWN	= 4,
	ZERN_SRCF_REQUIRED		= 8,
};

typedef ZEFlags ZERNStageResourceCreationFlags;

class ZERNStageResource
{
	public:
		ZEHolder<const ZEGRResource>*		Resource;
		ZEString							Name;
		ZERNStageResourceUsageType			Usage;
		ZERNStageResourceCreationFlags		CreationFlags;
};

class ZERNStage : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZERNRenderer;
	private:
		ZERNRenderer*						Renderer;
		ZELink<ZERNStage>					Link;
		ZEList2<ZERNCommand>				Commands;
		bool								Enabled;

		ZEArray<ZERNStageResource>			InputResources;
		ZEArray<ZERNStageResource>			OutputResources;
	
	protected:
		const ZEArray<ZERNStageResource>&	GetInputResources() const;
		const ZEArray<ZERNStageResource>&	GetOutputResources() const;

		void								AddInputResource(ZEHolder<const ZEGRResource>* ResourcePointer, const ZEString& Name, ZERNStageResourceUsageType Usage, ZERNStageResourceCreationFlags CreationFlags);
		void								AddOutputResource(ZEHolder<const ZEGRResource>* ResourcePointer, const ZEString& Name, ZERNStageResourceUsageType Usage, ZERNStageResourceCreationFlags CreationFlags);

		void								RemoveInputResource(const ZEString& Name);
		void								RemoveOutputResource(const ZEString& Name);

		virtual void						CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt						GetId() const = 0;
		virtual const ZEString&				GetName() const = 0;
		
		ZERNRenderer*						GetRenderer() const;
		const ZEList2<ZERNCommand>&			GetCommands() const;

		void								SetEnabled(bool Enable);
		bool								GetEnabled() const;

		const ZEGRResource*					GetInput(const ZEString& ResourceName) const;
		const ZEGRResource*					GetOutput(const ZEString& ResourceName) const;

		virtual void						Resized(ZEUInt Width, ZEUInt Height);

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStage();

		static ZEGRRenderState				GetRenderState();
};
