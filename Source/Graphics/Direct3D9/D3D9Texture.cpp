//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Texture.cpp
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

#include "D3D9Texture.h"
#include "Direct3D9Module.h"
#include "Core/Core.h"
#include "D3D9CommonTools.h"
/*
D3DFORMAT ConvertPixelFormat(ZETexturePixelFormat Format)
{
	switch(Format)
	{
		case ZE_TPF_ARGB32:
			return D3DFMT_A8R8G8B8;
		case ZE_TPF_DEPTH:
		case ZE_TPF_SHADOWMAP:
			return D3DFMT_D32;
		case ZE_TPF_HDR:
			return D3DFMT_A16B16G16R16F;
		default:
			ZEASSERT(true, "Wrong enum value ZETexturePixelFormat. Value : %d.", Format);
	}
}*/

ZED3D9Texture::ZED3D9Texture()
{
	Texture = NULL;
}

ZED3D9Texture::~ZED3D9Texture()
{
	Release();
}

bool ZED3D9Texture::IsEmpty() const
{
	return Texture == NULL;
}

void ZED3D9Texture::DeviceLost()
{
	if (RenderTarget)
		if (Texture != NULL)
		{
			Texture->Release();
			Texture = NULL;
		}
}

bool ZED3D9Texture::DeviceRestored()
{
	if (RenderTarget)
		return Create(Width, Height, PixelFormat, true);
}

bool ZED3D9Texture::Create(int Width, int Height, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (Texture != NULL)
		Texture->Release();
	
	DWORD Usage, MipMap;
	D3DFORMAT Format;
	D3DPOOL Pool;

	switch(PixelFormat)
	{
		case ZE_TPF_ARGB32:
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
		case ZE_TPF_HDR:
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
				Usage = D3DUSAGE_DEPTHSTENCIL;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;

			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_D24X8;
			break;
		case ZE_TPF_SHADOWMAP:
			if (RenderTarget)
			{
				Usage = D3DUSAGE_DEPTHSTENCIL;
				Pool = D3DPOOL_DEFAULT;
				MipMap = 1;
			}
			else
			{
				Usage = D3DUSAGE_AUTOGENMIPMAP;
				Pool = D3DPOOL_MANAGED;
				MipMap = 0;
			}
			Format = D3DFMT_D24X8;
			break;
	}

	HRESULT Hr;
	Hr = Device->CreateTexture(Width, Height, MipMap, Usage, Format, Pool, &Texture, NULL); 
	if (Hr != D3D_OK)
	{
		zeError("Direct3D Module", "Can not create texture resource.");
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

void ZED3D9Texture::Lock(void** Buffer, int* Pitch)
{
	D3DLOCKED_RECT Rect;
	Texture->LockRect(0, &Rect, NULL, NULL);
	*Buffer = Rect.pBits;
	*Pitch = Rect.Pitch;
}

void ZED3D9Texture::Unlock()
{
	Texture->UnlockRect(0);
}

void ZED3D9Texture::Release()
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

void ZED3D9Texture::Destroy()
{
	Module->Textures.DeleteValue((ZED3D9Texture*)this);
	delete this;
}






ZED3D9VolumeTexture::ZED3D9VolumeTexture()
{
	VolumeTexture = NULL;
}

ZED3D9VolumeTexture::~ZED3D9VolumeTexture()
{
	Release();
}

bool ZED3D9VolumeTexture::IsEmpty() const
{
	return VolumeTexture == NULL;
}

void ZED3D9VolumeTexture::DeviceLost()
{
}

bool ZED3D9VolumeTexture::DeviceRestored()
{
	return true;
}

bool ZED3D9VolumeTexture::Create(int Width, int Height, int Depth, ZETexturePixelFormat PixelFormat)
{
	if (VolumeTexture != NULL)
		VolumeTexture->Release();

	HRESULT Hr;
	Hr = Device->CreateVolumeTexture(Width, Height, Depth, 0, D3DUSAGE_AUTOGENMIPMAP, ZED3D9CommonTools::ConvertPixelFormat(PixelFormat), D3DPOOL_MANAGED, &VolumeTexture, NULL);  
	if (Hr != D3D_OK)
	{
		zeError("Direct3D Module", "Can not create volume texture resource.");
		return false;
	}

	this->PixelFormat = PixelFormat;
	this->Width = Width;
	this->Height = Height;
	this->Depth = Depth;

	return true;
}

void ZED3D9VolumeTexture::Lock(void** Buffer, int* RowPitch, int* SlicePitch)
{
	D3DLOCKED_BOX Box;
	VolumeTexture->LockBox(0, &Box, NULL, D3DLOCK_DISCARD);
	*Buffer = Box.pBits;
	*RowPitch = Box.RowPitch;
	*SlicePitch = Box.SlicePitch;
}

void ZED3D9VolumeTexture::Unlock()
{
	VolumeTexture->UnlockBox(0);
}

void ZED3D9VolumeTexture::Release()
{
	if (VolumeTexture != NULL)
		VolumeTexture->Release();
}

void ZED3D9VolumeTexture::Destroy()
{
	Module->VolumeTextures.DeleteValue((ZED3D9VolumeTexture*)this);
	delete this;
}





ZED3D9CubeTexture::ZED3D9CubeTexture()
{
	CubeTexture = NULL;
}

ZED3D9CubeTexture::~ZED3D9CubeTexture()
{
	Release();
}

bool ZED3D9CubeTexture::IsEmpty() const
{
	return CubeTexture == NULL;
}

void ZED3D9CubeTexture::DeviceLost()
{
	if (RenderTarget)
		if (CubeTexture != NULL)
		{
			CubeTexture->Release();
			CubeTexture = NULL;
		}
}

bool ZED3D9CubeTexture::DeviceRestored()
{
	if (RenderTarget)
		return Create(EdgeLength, PixelFormat, true);
	return true;
}

bool ZED3D9CubeTexture::Create(int EdgeLength, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (CubeTexture != NULL)
		CubeTexture->Release();

	HRESULT Hr;
	Hr = Device->CreateCubeTexture(EdgeLength, 0, 
		(RenderTarget ? D3DUSAGE_RENDERTARGET : D3DUSAGE_AUTOGENMIPMAP), ZED3D9CommonTools::ConvertPixelFormat(PixelFormat), 
		(RenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED), &CubeTexture, NULL);  
	if (Hr != D3D_OK)
	{
		zeError("Direct3D Module", "Can not create volume texture resource.");
		return false;
	}

	this->PixelFormat = PixelFormat;
	this->EdgeLength = EdgeLength;
	this->RenderTarget = RenderTarget;

	return true;
}

void ZED3D9CubeTexture::Lock(ZETextureCubeFace Face, void** Buffer, int* Pitch)
{
	D3DLOCKED_RECT Rect;
	CubeTexture->LockRect((D3DCUBEMAP_FACES)Face, 0, &Rect, NULL, D3DLOCK_DISCARD);
	*Buffer = Rect.pBits;
	*Pitch = Rect.Pitch;
}

void ZED3D9CubeTexture::Unlock(ZETextureCubeFace Face)
{
	CubeTexture->UnlockRect((D3DCUBEMAP_FACES)Face, 0);
}

void ZED3D9CubeTexture::Release()
{
	if (CubeTexture != NULL)
		CubeTexture->Release();
	CubeTexture = NULL;
}


void ZED3D9CubeTexture::Destroy()
{
	Module->CubeTextures.DeleteValue((ZED3D9CubeTexture*)this);
	delete this;
}
