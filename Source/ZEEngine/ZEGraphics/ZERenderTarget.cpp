//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderTarget.cpp
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

#include "ZEError.h"
#include "ZERenderTarget.h"

ZEUInt ZERenderTarget::GetWidth() const
{
	return Width;
}
		
ZEUInt ZERenderTarget::GetHeight() const
{
	return Height;
}

ZEUInt ZERenderTarget::GetDepth() const
{
	return Depth;
}

const ZEVector3& ZERenderTarget::GetPixelSize() const
{
	return PixelSize;
}

ZETexturePixelFormat ZERenderTarget::GetPixelFormat() const
{
	return PixelFormat;
}

ZERenderTargetType ZERenderTarget::GetRenderTargetType() const
{
	return RenderTargetType;
}

void ZERenderTarget::SetDebugName(const char* String)
{
#ifdef ZE_DEBUG_ENABLE
	DebugName = String;
#endif
}

const char* ZERenderTarget::GetDebugName() const
{
#ifdef ZE_DEBUG_ENABLE
	return DebugName.ToCString();
#else
	return NULL;
#endif
}

void ZERenderTarget::Destroy()
{
	delete this;
}

ZEUInt16	ZERenderTarget::TotalCount = 0;

ZERenderTarget::ZERenderTarget(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZETexturePixelFormat PixelFormat, ZERenderTargetType RenderTargetType)
{
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(Depth == 0, "Depth cannot be zero");
	zeDebugCheck(PixelFormat == ZE_TT_NONE, "RenderTargetType must be set");
	zeDebugCheck(PixelFormat == ZE_TPF_NOTSET, "PixelFormat must be set");

	this->Width = Width;
	this->Height = Height;
	this->Depth = Depth;
	this->PixelSize = ZEVector3(1.0f / Width, 1.0f / Height, 1.0f / Depth);
	this->PixelFormat = PixelFormat;
	this->RenderTargetType = RenderTargetType;

	TotalCount++;
}

ZERenderTarget::~ZERenderTarget()
{
	TotalCount--;
}
