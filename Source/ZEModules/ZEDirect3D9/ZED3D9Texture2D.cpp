//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Texture2D.cpp
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

#include "ZED3D9Texture2D.h"
#include "ZED3D9GraphicsModule.h"
#include "ZED3D9CommonTools.h"
#include "ZEError.h"

ZED3D9Texture2D::ZED3D9Texture2D()
{
	Texture = NULL;
	ViewPort.Surface = NULL;
}

ZED3D9Texture2D::~ZED3D9Texture2D()
{
	Release();
}

bool ZED3D9Texture2D::IsEmpty() const
{
	return Texture == NULL;
}

void ZED3D9Texture2D::DeviceLost()
{
	if (RenderTarget)
	{
		ZED3D_RELEASE(ViewPort.Surface);
		ZED3D_RELEASE(Texture);	
	}
}

bool ZED3D9Texture2D::DeviceRestored()
{
	if (RenderTarget)
	{
		if (!Create(Width, Height, LevelCount, PixelFormat, true))
			return false;

		if (FAILED(Texture->GetSurfaceLevel(0, &ViewPort.Surface)))
		{
			zeError("Cannot get texture surface.");
			Release();
			return false;
		}
	}

	return true;
}

ZERenderTarget* ZED3D9Texture2D::GetViewPort()
{
	return &ViewPort;
}

bool ZED3D9Texture2D::Create(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (!IsEmpty())
		Release();

	DWORD Usage = (RenderTarget ? D3DUSAGE_RENDERTARGET : 0);
	DWORD MipMap = (RenderTarget ? 1 : LevelCount);
	D3DPOOL Pool = (RenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED);
	D3DFORMAT Format = ZED3D9CommonTools::ConvertPixelFormat(PixelFormat);

	if (FAILED(Device->CreateTexture(Width, Height, MipMap, Usage, Format, Pool, &Texture, NULL)))
	{
		zeError("Can not create 2D texture.");
		Release();
		return false;
	}

	this->Width = Width;
	this->Height = Height;
	this->LevelCount = LevelCount;
	this->PixelFormat = PixelFormat;
	this->RenderTarget = RenderTarget;
	
	if (RenderTarget)
	{
		if (FAILED(Texture->GetSurfaceLevel(0, &ViewPort.Surface)))
		{
			zeError("Cannot get texture surface.");
			Release();
			return false;
		}

		Module->RegisterRenderTarget(this);
		ViewPort.SetTexture(this);
	}

	return true;
}

bool ZED3D9Texture2D::Lock(void** Buffer, ZESize* Pitch, ZEUInt Level)
{
	D3DLOCKED_RECT Rect;
	if (FAILED(Texture->LockRect(Level, &Rect, NULL, NULL)))
	{
		zeError("Cannot lock texture 2d.");
		return false;
	}

	*Buffer = Rect.pBits;
	*Pitch = (ZESize)Rect.Pitch;

	return true;
}

bool ZED3D9Texture2D::Unlock(ZEUInt Level)
{
	if (FAILED(Texture->UnlockRect(Level)))
	{
		zeError("Cannot unlock texture 2d.");
		return false;
	}

	return true;
}

void ZED3D9Texture2D::Release()
{
	if (RenderTarget)
	{
		Module->UnregisterRenderTarget(this);
		ZED3D_RELEASE(ViewPort.Surface);
		ViewPort.SetTexture(NULL);
	}
	
	ZED3D_RELEASE(Texture);

	Width = 0;
	Height = 0;
	RenderTarget = false;
	PixelFormat = ZE_TPF_NOTSET;
}

void ZED3D9Texture2D::Destroy()
{
	delete this;
}
