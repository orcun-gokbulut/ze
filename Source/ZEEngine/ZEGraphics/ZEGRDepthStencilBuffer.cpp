//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRDepthStencilBuffer.cpp
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

#include "ZEGRDepthStencilBuffer.h"
#include "ZEGRGraphicsModule.h"

inline static ZESize GetPixelSize(ZEGRDepthStencilPixelFormat Format)
{
	ZESize Size = 0;
	switch(Format)
	{
		case ZEGR_DSPF_DEPTH16:
			Size = 2;
			break;
		case ZEGR_DSPF_DEPTH24_STENCIL8:
			Size = 4;
			break;
		case ZEGR_DSPF_DEPTHD32_FLOAT:
			Size = 4;
			break;
	}
	return Size;
}

inline static ZESize CalculateBufferSize(ZEUInt Width, ZEUInt Height, ZEGRDepthStencilPixelFormat Format)
{
	return Width * Height * GetPixelSize(Format);
}

ZEUInt ZEGRDepthStencilBuffer::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRDepthStencilBuffer::GetHeight() const
{
	return Height;
}

ZEGRDepthStencilPixelFormat ZEGRDepthStencilBuffer::GetFormat() const
{
	return Format;
}

bool ZEGRDepthStencilBuffer::Create(ZEUInt Width, ZEUInt Height, ZEGRDepthStencilPixelFormat PixelFormat)
{
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(PixelFormat == ZEGR_DSPF_NONE, "PixelFormat must be valid");
	zeDebugCheck(Width > 8192 || Height > 8192, "Depth stencil buffer dimensions exceeds the limits.");

	this->Format = PixelFormat;
	this->Height = Height;
	this->Width = Width;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil buffer created. Width: %u, Height: %u, PixelFormat: %u.", 
			Width, Height, PixelFormat);
#endif

	return true;
}

ZEGRDepthStencilBuffer::ZEGRDepthStencilBuffer()
{
	Width = 0;
	Height = 0;
	Format = ZEGR_DSPF_NONE;
}

ZEGRDepthStencilBuffer::~ZEGRDepthStencilBuffer()
{

}

ZEGRDepthStencilBuffer* ZEGRDepthStencilBuffer::CreateInstance()
{
	return ZEGRGraphicsModule::GetInstance()->CreateDepthStencilBuffer();
}
