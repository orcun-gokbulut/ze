//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageTextureOutput.h
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

class ZEGRTexture;
class ZEGRRenderState;
class ZEGRShader;
class ZEGRRenderStateData;
class ZEGRRenderTarget;
class ZEGRConstantBuffer;

class ZERNStageTextureOutput : public ZERNStage
{
	private:
		ZEArray<const ZEGRTexture*>		InputTextures;
		const ZEGRRenderTarget*			OutputRenderTarget;

		ZEHolder<ZEGRShader>			ScreenCoverVertexShaderPositionTexcoord;
		ZEHolder<ZEGRShader>			PixelShader;
		ZEHolder<ZEGRRenderStateData>	RenderStateData;
		ZEHolder<ZEGRConstantBuffer>	ConstantBuffer;

		ZEGRViewport					Viewports[ZEGR_MAX_VIEWPORT_SLOT];

		bool							AutoSplit;

		struct
		{
			ZEUInt						TextureIndex;
			ZEUInt						Reserved[3];
		}Constants;

		virtual bool					InitializeSelf();
		virtual void					DeinitializeSelf();

	public:
		virtual ZEInt					GetId() const;
		virtual const ZEString&			GetName() const;

		void							SetInputs(ZEGRTexture** Inputs, ZESize Count);
		const ZEGRTexture*const*		GetInputs() const;

		void							SetViewports(const ZEGRViewport* Viewports, ZESize Count);
		const ZEGRViewport*				GetViewports() const;

		void							SetAutoSplit(bool AutoSplit);
		bool							IsAutoSplit() const;

		virtual bool					Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands);
		virtual void					CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context);

										ZERNStageTextureOutput();
};
