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
#include "ZEGRContext.h"

void ZEGRDepthStencilBuffer::SetOwner(const ZEGRTexture* OwnerTexture)
{
	this->Owner = OwnerTexture;
}

const ZEGRTexture* ZEGRDepthStencilBuffer::GetOwner() const
{
	return Owner;
}

void ZEGRDepthStencilBuffer::SetReadOnly(bool ReadOnly)
{
	this->ReadOnly = ReadOnly;
}

bool ZEGRDepthStencilBuffer::GetReadOnly() const
{
	return ReadOnly;
}

void ZEGRDepthStencilBuffer::SetBound(bool Bound)
{
	this->Bound = Bound;
}

bool ZEGRDepthStencilBuffer::GetBound() const
{
	return Bound;
}

ZEGRDepthStencilBuffer::ZEGRDepthStencilBuffer()
{
	Owner = NULL;
	ReadOnly = false;
	Bound = false;

	Register();
}

ZEGRDepthStencilBuffer::ZEGRDepthStencilBuffer(ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	Owner = NULL;
	ReadOnly = false;
	Bound = false;

	this->Width = Width;
	this->Height = Height;
	this->Format = Format;

	Register();
}

ZEGRDepthStencilBuffer::~ZEGRDepthStencilBuffer()
{
	ZEGRGraphicsModule* GraphicsModule = ZEGRGraphicsModule::GetInstance();
	if (GraphicsModule != NULL)
	{
		ZEGRContext* Context = GraphicsModule->GetMainContext();
		if (Context != NULL && Bound)
		{
			ZEGRRenderTarget* CurrentRenderTargets[ZEGR_MAX_RENDER_TARGET_SLOT] = {};
			Context->GetRenderTargets(ZEGR_MAX_RENDER_TARGET_SLOT, CurrentRenderTargets);

			Context->SetRenderTargets(ZEGR_MAX_RENDER_TARGET_SLOT, CurrentRenderTargets, NULL);
		}
	}

	Owner = NULL;

	Unregister();
}

ZEGRResourceType ZEGRDepthStencilBuffer::GetResourceType() const
{
	return ZEGR_RT_DEPTH_STENCIL_BUFFER;
}

ZEUInt ZEGRDepthStencilBuffer::GetWidth() const
{
	return Width;
}

ZEUInt ZEGRDepthStencilBuffer::GetHeight() const
{
	return Height;
}

ZEGRFormat ZEGRDepthStencilBuffer::GetFormat() const
{
	return Format;
}
