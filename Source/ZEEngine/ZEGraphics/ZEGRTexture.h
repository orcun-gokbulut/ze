//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture.h
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

#include "ZEGRResource.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGRFormat.h"

enum ZEGRTextureType
{
	ZEGR_TT_NONE	= 0,
	ZEGR_TT_2D		= 1,
	ZEGR_TT_3D		= 2,
	ZEGR_TT_CUBE	= 3
};

struct ZERect
{
	ZEInt x, y;
	ZEInt Width, Height;
};

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;

class ZEGRTexture : public ZEGRResource
{
	friend class ZEGRContext;
	private:
		ZEGRFormat											Format;
		ZEUInt												LevelCount;

		struct BoundStage
		{
			bool											BoundAsShaderResource;
			bool											BoundAsUnorderedAccess;
			ZEInt											Slot;
		};

	protected:
		ZEArray<BoundStage>									BoundStages;
		mutable ZEArray<ZEHolder<ZEGRRenderTarget>>			RenderTargets;
		mutable ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>	DepthStencilBuffers;

		void												SetFormat(ZEGRFormat Format);
		void												SetLevelCount(ZEUInt LevelCount);

		void												SetBoundStage(ZEGRShaderType Shader, ZEInt Slot, bool BoundAsShaderResource = true, bool BoundAsUnorderedAccess = false);
		const ZEArray<BoundStage>&							GetBoundStages() const;

		const ZEArray<ZEHolder<ZEGRRenderTarget>>&			GetRenderTargets() const;
		const ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>&	GetDepthStencilBuffers() const;

		static ZESize										CalculateSize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format);
		static ZESize										CalculateLevelCount(ZEUInt Width, ZEUInt Height, ZEGRFormat Format);

															ZEGRTexture();
		virtual												~ZEGRTexture();

	public:
		ZEGRFormat											GetFormat() const;
		ZEUInt												GetLevelCount() const;

		virtual ZEGRTextureType								GetTextureType() const = 0;
};