//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Texture2D.cpp
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

#include "D3D9Texture2D.h"
#include "D3D9Module.h"
#include "D3D9CommonTools.h"
#include "Core/Error.h"

ZED3D9Texture2D::ZED3D9Texture2D()
{
	Texture = NULL;
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
		if (Texture != NULL)
		{
			Texture->Release();
			Texture = NULL;
		}
}

bool ZED3D9Texture2D::DeviceRestored()
{
	if (RenderTarget)
		return Create(Width, Height, PixelFormat, true);

	return true;
}

bool ZED3D9Texture2D::Create(int Width, int Height, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (Texture != NULL)
		if (this->Width == Width || this->Height == Height || this->PixelFormat == PixelFormat || this->RenderTarget == RenderTarget)
			return true;
		else
			Texture->Release();

	
	DWORD Usage, MipMap;
	D3DFORMAT Format;
	D3DPOOL Pool;

	switch(PixelFormat)
	{
		case ZE_TPF_RGBA_INT32:
			if (RenderTarget)
			{
				Usage = D3DUSAGE_RENDERTARGET;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;
			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_A8R8G8B8;
			break;

		case ZE_TPF_LUM_HDR:
			if (RenderTarget)
			{
				Usage = D3DUSAGE_RENDERTARGET;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;

			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_R16F;
			break;

		case ZE_TPF_RGBA_HDR:
			if (RenderTarget)
			{
				Usage = D3DUSAGE_RENDERTARGET;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;

			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_A16B16G16R16F;
			break;

		case ZE_TPF_DEPTH:
			if (RenderTarget)
			{
				Usage = D3DUSAGE_RENDERTARGET;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;

			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_R32F;
			break;
	}

	HRESULT Hr;
	Hr = GetDevice()->CreateTexture(Width, Height, MipMap, Usage, Format, Pool, &Texture, NULL); 
	if (Hr != D3D_OK)
	{
		zeError("D3D9 GetModule()", "Can not create texture resource.");
		return false;
	}

	if (MipMap != 1)
		Texture->GenerateMipSubLevels();
	
	this->Width = Width;
	this->Height = Height;
	this->PixelFormat = PixelFormat;
	this->RenderTarget = RenderTarget;

	return true;

}

void ZED3D9Texture2D::Lock(void** Buffer, int* Pitch)
{
	D3DLOCKED_RECT Rect;
	Texture->LockRect(0, &Rect, NULL, NULL);
	*Buffer = Rect.pBits;
	*Pitch = Rect.Pitch;
}

void ZED3D9Texture2D::Unlock()
{
	Texture->UnlockRect(0);
}

void ZED3D9Texture2D::Release()
{
	
	Width = 0;
	Height = 0;
	PixelFormat = ZE_TPF_NOTSET;;
	RenderTarget = false;

	if (Texture != NULL)
	{
		Texture->Release();
		Texture = NULL;
	}
}

void ZED3D9Texture2D::Destroy()
{
	GetModule()->Texture2Ds.DeleteValue((ZED3D9Texture2D*)this);
	delete this;
}
