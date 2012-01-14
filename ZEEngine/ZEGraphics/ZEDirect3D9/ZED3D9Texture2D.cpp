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
#include "ZED3D9Module.h"
#include "ZED3D9CommonTools.h"
#include "ZEError.h"

ZED3D9Texture2D::ZED3D9Texture2D()
{
	Texture = NULL;
	ViewPort.FrameBuffer = NULL;
	ViewPort.ZBuffer = NULL;
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
		ZED3D_RELEASE(ViewPort.FrameBuffer);
		ZED3D_RELEASE(Texture);
	}
}

bool ZED3D9Texture2D::DeviceRestored()
{
	if (RenderTarget)
	{
		Create(Width, Height, PixelFormat, true);
	}

	return true;
}

ZEViewPort* ZED3D9Texture2D::GetViewPort()
{
	return &ViewPort;
}

bool ZED3D9Texture2D::Create(unsigned int Width, unsigned int Height, ZETexturePixelFormat PixelFormat, bool RenderTarget, unsigned int MipLevel)
{
	if (Texture != NULL)
		if (this->Width == Width && this->Height == Height && this->PixelFormat == PixelFormat && this->RenderTarget == RenderTarget)
			return true;
		else
		{
			Texture->Release();
			Texture = NULL;
		}

	DWORD Usage;
	DWORD MipMap;
	D3DPOOL Pool;
	D3DFORMAT Format;

	if (PixelFormat == ZE_TPF_SHADOW_MAP)
	{
		Usage = D3DUSAGE_DEPTHSTENCIL;
		MipMap = 1;
		Pool = D3DPOOL_DEFAULT;
		Format = D3DFMT_D24X8;
		RenderTarget = true;
	}
	else
	{
		Usage = (RenderTarget ? D3DUSAGE_RENDERTARGET : 0);
		MipMap = (RenderTarget ? 1 : MipLevel);
		Pool = (RenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED);
		Format = ZED3D9CommonTools::ConvertPixelFormat(PixelFormat);
	}

	HRESULT Hr;
	Hr = GetDevice()->CreateTexture(Width, Height, MipMap, Usage, Format, Pool, &Texture, NULL); 
	if (Hr != D3D_OK)
	{
		zeError("Can not create 2D texture.");
		return false;
	}

	this->Width = Width;
	this->Height = Height;
	this->PixelFormat = PixelFormat;
	this->RenderTarget = RenderTarget;
	this->MipLevel = Texture->GetLevelCount();

	if (RenderTarget)
	{
		ZED3D_RELEASE(ViewPort.FrameBuffer);	
		Texture->GetSurfaceLevel(0, &ViewPort.FrameBuffer);
		ViewPort.ZBuffer = NULL;
	}

	return true;
}

void ZED3D9Texture2D::Lock(void** Buffer, unsigned int* Pitch, unsigned int MipLevel)
{
	D3DLOCKED_RECT Rect;
	Texture->LockRect(MipLevel, &Rect, NULL, NULL);
	*Buffer = Rect.pBits;
	*Pitch = Rect.Pitch;
}

void ZED3D9Texture2D::Unlock(unsigned int MipLevel)
{
	Texture->UnlockRect(MipLevel);
}

void ZED3D9Texture2D::Release()
{
	
	Width = 0;
	Height = 0;
	PixelFormat = ZE_TPF_NOTSET;;
	RenderTarget = false;

	ZED3D_RELEASE(Texture);
	ZED3D_RELEASE(ViewPort.FrameBuffer);
}

void ZED3D9Texture2D::Destroy()
{
	GetModule()->Texture2Ds.DeleteValue((ZED3D9Texture2D*)this);
	delete this;
}