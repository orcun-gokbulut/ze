//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageResolving.h
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
#include "ZEGraphics/ZEGRViewport.h"

class ZEGRShader;
class ZEGRSampler;
class ZEGRTexture;
class ZEGRBuffer;
class ZEGRContext;
class ZEGRRenderStateData;
class ZERNRenderer;
class ZEGRRenderTarget;

enum ZERNResolveFilterType
{
	ZERN_FILTER_NONE	= 0,
	ZERN_FILTER_BOX
};

class ZERNStageResolving : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags							DirtyFlags;

		ZEHolder<ZEGRRenderStateData>	ResolveGBuffersRenderStateData;
		ZEHolder<ZEGRRenderStateData>	ResolveAllGBuffersRenderStateData;
		ZEHolder<ZEGRRenderStateData>	ResolveCustomRenderStateData;

		ZEHolder<ZEGRBuffer>			ConstantBuffer;

		ZEHolder<ZEGRTexture>			ResolvedInputTexture;
		ZEHolder<ZEGRTexture>			ResolvedGBuffer1;
		ZEHolder<ZEGRTexture>			ResolvedGBuffer2;
		ZEHolder<ZEGRTexture>			ResolvedGBuffer3;
		ZEHolder<ZEGRTexture>			ResolvedDepthTexture;

		ZEHolder<const ZEGRTexture>		InputTexture;
		ZEHolder<const ZEGRTexture>		GBuffer1;
		ZEHolder<const ZEGRTexture>		GBuffer2;
		ZEHolder<const ZEGRTexture>		GBuffer3;
		ZEHolder<const ZEGRTexture>		DepthTexture;

		ZEString						CustomInputname;

		bool							ResolveAllGbuffers;

		struct
		{
			ZERNResolveFilterType		FilterType;
			float						Reserved0[3];
		} Constants;

		virtual bool					InitializeInternal();
		virtual bool					DeinitializeInternal();

		bool							UpdateRenderStates();
		bool							UpdateConstantBuffers();
		bool							Update();

		virtual void					CreateOutput(const ZEString& Name);

	public:
		virtual ZEInt					GetId() const;
		virtual const ZEString&			GetName() const;

		void							SetCustomResolveFilter(ZERNResolveFilterType FilterType);
		ZERNResolveFilterType			GetCustomResolveFilter() const;

		void							SetResolveAllGbuffers(bool ResolveAllGbuffers);
		bool							GetResolveAllGbuffers() const;

		void							SetCustomInput(const ZEString& Inputname);

		virtual void					Resized(ZEUInt Width, ZEUInt Height);

		virtual bool					Setup(ZEGRContext* Context);
		virtual void					CleanUp(ZEGRContext* Context);

										ZERNStageResolving();
		virtual							~ZERNStageResolving();
};
