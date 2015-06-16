//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDepthStencilBuffer.cpp
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

#include "ZEDepthStencilBuffer.h"
#include "ZEGraphicsModule.h"

inline static ZESize GetPixelSize(ZEDepthStencilPixelFormat Format)
{
	ZESize Size = 0;
	switch(Format)
	{
		case ZE_DSPF_DEPTH16:
			Size = 2;
			break;
		case ZE_DSPF_DEPTH24_STENCIL8:
			Size = 4;
			break;
		case ZE_DSPF_DEPTHD32_FLOAT:
			Size = 4;
			break;
	}
	return Size;
}

inline static ZESize CalculateBufferSize(ZEUInt Width, ZEUInt Height, ZEDepthStencilPixelFormat Format)
{
	return Width * Height * GetPixelSize(Format);
}

ZEUInt ZEDepthStencilBuffer::GetWidth() const
{
	return Width;
}

ZEUInt ZEDepthStencilBuffer::GetHeight() const
{
	return Height;
}

ZEDepthStencilPixelFormat ZEDepthStencilBuffer::GetPixelFormat() const
{
	return PixelFormat;
}

void ZEDepthStencilBuffer::SetDebugName(const char* String)
{
#ifdef ZE_DEBUG_ENABLE
	DebugName = String;
#endif
}

const char* ZEDepthStencilBuffer::GetDebugName() const
{
#ifdef ZE_DEBUG_ENABLE
	return DebugName.ToCString();
#else
	return NULL;
#endif
}

void ZEDepthStencilBuffer::Destroy()
{
	delete this;
}

bool ZEDepthStencilBuffer::Create(ZEUInt Width, ZEUInt Height, ZEDepthStencilPixelFormat PixelFormat)
{
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(PixelFormat == ZE_DSPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(Width > 8192 || Height > 8192, "Depth stencil buffer dimensions exceeds the limits.");

	this->PixelFormat = PixelFormat;
	this->Height = Height;
	this->Width = Width;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil buffer created. Width: %u, Height: %u, PixelFormat: %u.", 
			Width, Height, PixelFormat);
#endif

	TotalSize += CalculateBufferSize(Width, Height, PixelFormat);
	TotalCount++;

	return true;
}

ZESize		ZEDepthStencilBuffer::TotalSize = 0;
ZEUInt16	ZEDepthStencilBuffer::TotalCount = 0;

ZEDepthStencilBuffer::ZEDepthStencilBuffer()
{
	Width = 0;
	Height = 0;
	PixelFormat = ZE_DSPF_NOTSET;
}

ZEDepthStencilBuffer::~ZEDepthStencilBuffer()
{
	TotalSize -= CalculateBufferSize(Width, Height, PixelFormat);
	TotalCount--;
}

ZEDepthStencilBuffer* ZEDepthStencilBuffer::CreateInstance()
{
	return zeGraphics->CreateDepthStencilBuffer();
}
