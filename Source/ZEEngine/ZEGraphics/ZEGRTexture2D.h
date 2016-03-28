//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture2D.h
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

#include "ZEGRTexture.h"

#include "ZETypes.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;

class ZEGRTexture2D : public ZEGRTexture
{
	friend class ZEGRContext;
	friend class ZEGRGraphicsModule;

	protected:
		ZEUInt											Width;
		ZEUInt											Height;
		ZEUInt											ArrayCount;

		virtual bool									Initialize(ZEUInt Width, ZEUInt Height, ZEUInt ArrayCount, ZEUInt LevelCount, ZEUInt SampleCount, ZEGRFormat Format, bool RenderTarget = false, bool DepthStencil = false, bool UAV = false);	
		virtual void									Deinitialize();

														ZEGRTexture2D();

	public:
		ZEUInt											GetWidth() const;
		ZEUInt											GetHeight() const;
		ZEUInt											GetArrayCount() const;
		ZEVector2										GetPixelSize() const;

		virtual ZEGRResourceType						GetResourceType() const;
		virtual ZEGRTextureType							GetTextureType() const;

		virtual void									GenerateMipMaps() = 0;

		virtual	ZEHolder<const ZEGRRenderTarget>		GetRenderTarget(ZEUInt MipLevel = 0) const = 0;
		virtual ZEHolder<const ZEGRDepthStencilBuffer>	GetDepthStencilBuffer(bool ReadOnly = false, ZEUInt ArrayIndex = 0) const = 0;

		virtual bool									UpdateSubResource(ZEUInt DestArrayIndex, ZEUInt DestLevel, const void* SrcData, ZESize SrcRowPitch) = 0;

		static ZEHolder<ZEGRTexture2D>					CreateInstance(ZEUInt Width, ZEUInt Height, ZEUInt ArrayCount, ZEUInt LevelCount, ZEUInt SampleCount, ZEGRFormat Format, bool RenderTarget = false, bool DepthStencil = false, bool UAV = false);
};
