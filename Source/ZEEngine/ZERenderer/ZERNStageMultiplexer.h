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

#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRDefinitions.h"

enum ZERNStageMultiplexerMode
{
	ZERN_SMM_NONE,
	ZERN_SMM_SINGLE,
	ZERN_SMM_VERTICAL,
	ZERN_SMM_HORIZONTAL,
	ZERN_SMM_2x2,
	ZERN_SMM_CUSTOM
};

class ZEGRTexture;
class ZEGRRenderState;
class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;

class ZERNStageMultiplexer : public ZERNStage
{
	private:
		ZERNStageBuffer						Inputs[ZEGR_MAX_VIEWPORT_SLOT];
		ZEHolder<const ZEGRTexture>			InputTextures[ZEGR_MAX_VIEWPORT_SLOT];
		ZEGRViewport						CustomViewports[ZEGR_MAX_VIEWPORT_SLOT];

		ZEHolder<const ZEGRRenderTarget>	OutputRenderTarget;
		ZEHolder<ZEGRTexture2D>				OutputTexture;

		ZEHolder<ZEGRRenderStateData>		RenderStateData;

		ZERNStageMultiplexerMode			Mode;

		bool								UpdateInputOutputs();

		void								DrawSingle(ZEGRContext* Context);
		void								DrawVertical2(ZEGRContext* Context);
		void								DrawHorizontal2(ZEGRContext* Context);
		void								Draw2x2(ZEGRContext* Context);

		virtual bool						InitializeSelf();
		virtual void						DeinitializeSelf();

	public:
		virtual ZEInt						GetId() const;
		virtual const ZEString&				GetName() const;

		void								SetMode(ZERNStageMultiplexerMode Mode);
		ZERNStageMultiplexerMode			GetMode();

		void								SetInput(ZEUInt Index, ZERNStageBuffer Buffer);
		void								SetInput(ZEUInt Index, ZEGRTexture* Input);
		const ZEGRTexture*					GetInput(ZEUInt Index) const;

		void								SetCustomViewport(ZEUInt Index, const ZEGRViewport& Viewport);
		const ZEGRViewport&					GetCustomViewport(ZEUInt Index) const;

		virtual const ZEGRRenderTarget*		GetProvidedInput(ZERNStageBuffer Input) const;
		virtual const ZEGRTexture2D*		GetOutput(ZERNStageBuffer Output) const;

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStageMultiplexer();
};
