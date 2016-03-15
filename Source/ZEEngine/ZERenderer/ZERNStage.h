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

#include "ZEInitializable.h"

#include "ZETypes.h"
#include "ZEDS\ZELink.h"
#include "ZEDS\ZEList2.h"

class ZEGRContext;
class ZERNRenderer;
class ZEGRRenderState;
class ZERNCommand;
class ZEString;
class ZEGRTexture2D;
class ZEGRRenderTarget;

template<typename Type> class ZEList2;

// GBuffer

enum ZERNStageBuffer
{
	ZERN_SO_NONE,
	ZERN_SO_COLOR,
	ZERN_SO_DEPTH,
	ZERN_SO_NORMAL,
	ZERN_SO_GBUFFER_DIFFUSE,
	ZERN_SO_GBUFFER_SPECULAR,
	ZERN_SO_ACCUMULATION,
	ZERN_SO_HDR,
	ZERN_SO_AMBIENT_OCCLUSION
};

class ZERNStage : public ZEInitializable
{
	friend class ZERNRenderer;
	private:
		ZERNRenderer*						Renderer;
		ZELink<ZERNStage>					Link;
		ZEList2<ZERNCommand>				Commands;
		bool								Enabled;
		
	protected:
		const ZEGRTexture2D*				GetPrevOutput(ZERNStageBuffer Input) const;
		const ZEGRRenderTarget*				GetNextProvidedInput(ZERNStageBuffer RenderTarget) const;

	public:
		virtual ZEInt						GetId() const = 0;
		virtual const ZEString&				GetName() const = 0;
		ZERNRenderer*						GetRenderer() const;

		ZERNStage*							GetPrevStage() const;
		ZERNStage*							GetNextStage() const;

		void								SetEnabled(bool Enable);
		bool								GetEnabled() const;

		const ZEList2<ZERNCommand>&			GetCommands();

		virtual const ZEGRTexture2D*		GetOutput(ZERNStageBuffer Output) const;
		virtual const ZEGRRenderTarget*		GetProvidedInput(ZERNStageBuffer Input) const;

		virtual bool						Setup(ZEGRContext* Context);
		virtual void						CleanUp(ZEGRContext* Context);

											ZERNStage();

		static ZEGRRenderState				GetRenderState();
};
