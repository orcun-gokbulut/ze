//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TextureCube.cpp
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
#include "ZED3D9Module.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9CommonTools.h"


ZED3D9TextureCube::ZED3D9TextureCube()
{
	CubeTexture = NULL;
	for (ZESize I = 0; I < 6; I++)
	{
		ViewPorts[I].FrameBuffer = NULL;
		ViewPorts[I].ZBuffer = NULL;
	}
}

ZED3D9TextureCube::~ZED3D9TextureCube()
{
	Release();
}

bool ZED3D9TextureCube::IsEmpty() const
{
	return CubeTexture == NULL;
}

void ZED3D9TextureCube::DeviceLost()
{
	if (RenderTarget)
	{
		for (ZESize I = 0; I < 6; I++)
			ZED3D_RELEASE(ViewPorts[I].FrameBuffer);

		ZED3D_RELEASE(CubeTexture);
	}
}

bool ZED3D9TextureCube::DeviceRestored()
{
	if (RenderTarget)
	{
		if (!Create(EdgeLength, LevelCount, PixelFormat, true))
			return false;

		for (ZESize I = 0; I < 6; I++)
		{
			CubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)I, 0, &ViewPorts[I].FrameBuffer);
			ViewPorts[I].ZBuffer = NULL;
		}

	}

	return true;
}

ZEViewPort* ZED3D9TextureCube::GetViewPort(ZETextureCubeFace Face)
{
	if (!RenderTarget)
		return NULL;
	else
		return &ViewPorts[Face];
}

bool ZED3D9TextureCube::Create(ZEUInt EdgeLength, ZEUInt Levels, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (CubeTexture != NULL)
	{
		if (this->EdgeLength == EdgeLength)
		{
			return true;
		}
		else
		{
			CubeTexture->Release();
			CubeTexture = NULL;
		}
	}

	DWORD Usage			= (RenderTarget ? D3DUSAGE_RENDERTARGET : 0);
	DWORD MipCount		= (RenderTarget ? 1 : Levels);
	D3DPOOL Pool		= (RenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED);
	D3DFORMAT Format	= ZED3D9CommonTools::ConvertPixelFormat(PixelFormat);

	HRESULT Hr;
	Hr = GetDevice()->CreateCubeTexture(EdgeLength, MipCount, Usage, Format, Pool, &CubeTexture, NULL);  
	if (Hr != D3D_OK)
	{
		zeError("Can not create cube texture.");
		return false;
	}

	this->LevelCount	= Levels;
	this->PixelFormat	= PixelFormat;
	this->EdgeLength	= EdgeLength;
	this->RenderTarget	= RenderTarget;

	if (RenderTarget)
	{
		for (ZESize I = 0; I < 6; I++)
		{
			CubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)I, 0, &ViewPorts[I].FrameBuffer);
			ViewPorts[I].ZBuffer = NULL;
		}
	}

	return true;
}

bool ZED3D9TextureCube::Lock(ZETextureCubeFace Face, ZEUInt Level, void** Buffer, ZESize* Pitch)
{
	D3DLOCKED_RECT Rect;
	HRESULT hr = CubeTexture->LockRect((D3DCUBEMAP_FACES)Face, (UINT)Level, &Rect, NULL, 0);
	if (hr != D3D_OK)
		return false;
	
	*Buffer = Rect.pBits;
	*Pitch = (ZESize)Rect.Pitch;

	return true;
}

void ZED3D9TextureCube::Unlock(ZETextureCubeFace Face, ZEUInt Level)
{
	CubeTexture->UnlockRect((D3DCUBEMAP_FACES)Face, (UINT)Level);
}

void ZED3D9TextureCube::Release()
{
	if (CubeTexture != NULL)
	{
		CubeTexture->Release();
		CubeTexture = NULL;
	}

	if (RenderTarget)
	{
		for (ZESize I = 0; I < 6; I++)
		{
			ZED3D_RELEASE(ViewPorts[I].FrameBuffer);
			ViewPorts[I].ZBuffer = NULL;
		}
	}

	EdgeLength		= 0;
	LevelCount		= 0;
	PixelFormat		= ZE_TPF_NOTSET;
	RenderTarget	= false; 
}


void ZED3D9TextureCube::Destroy()
{
	GetModule()->TextureCubes.RemoveValue((ZED3D9TextureCube*)this);
	delete this;
}
