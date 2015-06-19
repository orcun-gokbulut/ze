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
#include "ZEGRCounter.h"

ZESize SizeOfPixel(ZEGRDepthStencilFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_DSF_NONE:
			return 0;

		case ZEGR_DSF_DEPTH16:
			return 2;

		case ZEGR_DSF_DEPTH24_STENCIL8:
			return 4;

		case ZEGR_DSF_DEPTHD32_FLOAT:
			return 4;
	}
}

ZEUInt ZEGRDepthStencilBuffer::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRDepthStencilBuffer::GetHeight() const
{
	return Height;
}

ZEGRDepthStencilFormat ZEGRDepthStencilBuffer::GetFormat() const
{
	return Format;
}

bool ZEGRDepthStencilBuffer::Initialize(ZEUInt Width, ZEUInt Height, ZEGRDepthStencilFormat Format)
{
	this->Format = Format;
	this->Height = Height;
	this->Width = Width;
	
	#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil buffer created. Width: %u, Height: %u, PixelFormat: %u.", 	Width, Height, Format);
	#endif

	SetSize(Width * Height * SizeOfPixel(Format));

	ZEGR_COUNTER_RESOURCE_INCREASE(this, DepthStencilBuffer, Texture);

	return true;
}

void ZEGRDepthStencilBuffer::Deinitialize()
{
	Width = 0;
	Height = 0;
	Format = ZEGR_DSF_NONE;
	
	SetSize(0);
	
	ZEGR_COUNTER_RESOURCE_INCREASE(this, DepthStencilBuffer, Texture);
}

ZEGRDepthStencilBuffer::ZEGRDepthStencilBuffer()
{
	Width = 0;
	Height = 0;
	Format = ZEGR_DSF_NONE;

}

ZEGRDepthStencilBuffer::~ZEGRDepthStencilBuffer()
{
	Deinitialize();
}

ZEGRDepthStencilBuffer* ZEGRDepthStencilBuffer::Create(ZEUInt Width, ZEUInt Height, ZEGRDepthStencilFormat Format)
{
	ZEGRDepthStencilBuffer* DepthStencilBuffer = ZEGRGraphicsModule::GetInstance()->CreateDepthStencilBuffer();
	if (DepthStencilBuffer == NULL)
		return NULL;

	if (!DepthStencilBuffer->Initialize(Width, Height, Format))
	{
		DepthStencilBuffer->Destroy();
		return NULL;
	}

	return DepthStencilBuffer;
}
