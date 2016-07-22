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
#include "ZETexture/ZETextureOptions.h"

class ZEGRRenderTarget;
class ZEGRDepthStencilBuffer;

class ZEGRTexture2D : public ZEGRTexture
{
	ZE_OBJECT
	ZE_DISALLOW_COPY(ZEGRTexture2D)
	friend class ZEGRContext;
	friend class ZEGRGraphicsModule;
	protected:
		ZEUInt											Width;
		ZEUInt											Height;
		ZEUInt											ArrayCount;
		ZEUInt											SampleCount;

		virtual bool									Initialize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format, ZEGRResourceUsage Usage, ZEFlags BindFlag, ZEUInt ArrayCount, ZEUInt SampleCount, const void* Data);	
		virtual void									Deinitialize();

														ZEGRTexture2D();

	public:
		ZEUInt											GetWidth() const;
		ZEUInt											GetHeight() const;
		ZEUInt											GetArrayCount() const;
		ZEUInt											GetSampleCount() const;
		ZEVector2										GetPixelSize() const;

		virtual ZEGRResourceType						GetResourceType() const;
		virtual ZEGRTextureType							GetTextureType() const;

		virtual	const ZEGRRenderTarget*					GetRenderTarget(ZEUInt Level = 0, ZEUInt ArrayIndex = 0) const = 0;
		virtual const ZEGRDepthStencilBuffer*			GetDepthStencilBuffer(bool ReadOnly = false, ZEUInt ArrayIndex = 0) const = 0;

		virtual void									UpdateSubResource(ZEUInt DestArrayIndex, ZEUInt DestLevel, ZERect* DestRect, const void* SrcData, ZESize SrcRowPitch) = 0;
		virtual bool									Lock(void** Buffer, ZESize* RowPitch = NULL, ZESize* SlicePitch = NULL, ZEUInt ArrayIndex = 0, ZEUInt Level = 0) = 0;
		virtual void									Unlock() = 0;

		static ZEHolder<ZEGRTexture2D>					CreateInstance(
																		ZEUInt Width, 
																		ZEUInt Height, 
																		ZEUInt LevelCount, 
																		ZEGRFormat Format, 
																		ZEGRResourceUsage Usage = ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, 
																		ZEFlags BindFlags = ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_RENDER_TARGET, 
																		ZEUInt ArrayCount = 1, 
																		ZEUInt SampleCount = 1, 
																		const void* Data = NULL);

		static ZEHolder<ZEGRTexture2D>					CreateFromFile(const ZEString& Filename, const ZEGRTextureOptions& TextureOptions);
};
