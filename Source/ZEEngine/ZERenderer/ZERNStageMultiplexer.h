//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageMultiplexer.h
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
#include "ZEExport.ZEEngine.h"

ZE_EXPORT_ZEENGINE ZE_ENUM(ZERNStageMultiplexerMode)
{
	ZERN_SMM_NONE,
	ZERN_SMM_SINGLE,
	ZERN_SMM_VERTICAL,
	ZERN_SMM_HORIZONTAL,
	ZERN_SMM_2x2,
	ZERN_SMM_CUSTOM
};

class ZEGRTexture;
class ZEGRRenderStateData;
class ZEGRRenderTarget;
class ZERNStageDisplay;

class ZE_EXPORT_ZEENGINE ZERNStageMultiplexer : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags								DirtyFlags;
		ZERNStageMultiplexerMode			Mode;
		ZEList2<ZERNStageDisplay>			Displays;

		ZEHolder<ZEGRRenderStateData>		RenderStateData;
		
		ZEHolder<ZEGRTexture>				OutputTexture;

		const ZEGRRenderTarget*				OutputRenderTarget;

		void								DrawSingle(ZEGRContext* Context);
		void								DrawVertical2(ZEGRContext* Context);
		void								DrawHorizontal2(ZEGRContext* Context);
		void								Draw2x2(ZEGRContext* Context);
		void								DrawCustom(ZEGRContext* Context);

		virtual bool						InitializeInternal();
		virtual bool						DeinitializeInternal();

		virtual void						CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetMode(ZERNStageMultiplexerMode Mode);
		ZERNStageMultiplexerMode			GetMode();

		const ZEList2<ZERNStageDisplay>&	GetInputs();

		void								AddInput(ZERNStageDisplay* Input);
		void								RemoveInput(ZERNStageDisplay* Input);

		virtual void						Resized(ZEUInt Width, ZEUInt Height);

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

		virtual bool						Unserialize(ZEMLReaderNode* StageNode);

											ZERNStageMultiplexer();
		virtual								~ZERNStageMultiplexer();
};
