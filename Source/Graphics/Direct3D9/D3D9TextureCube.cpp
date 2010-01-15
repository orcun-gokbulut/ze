//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9TextureCube.cpp
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

#include "D3D9TextureCube.h"
#include "D3D9Module.h"
#include "D3D9CommonTools.h"
#include "Core/Error.h"

ZED3D9TextureCube::ZED3D9TextureCube()
{
	CubeTexture = NULL;
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
		if (CubeTexture != NULL)
		{
			CubeTexture->Release();
			CubeTexture = NULL;
		}
}

bool ZED3D9TextureCube::DeviceRestored()
{
	if (RenderTarget)
		return Create(EdgeLength, PixelFormat, true);
	return true;
}

bool ZED3D9TextureCube::Create(int EdgeLength, ZETexturePixelFormat PixelFormat, bool RenderTarget)
{
	if (CubeTexture != NULL)
		CubeTexture->Release();

	HRESULT Hr;
	Hr = GetDevice()->CreateCubeTexture(EdgeLength, 0, 
		(RenderTarget ? D3DUSAGE_RENDERTARGET : D3DUSAGE_AUTOGENMIPMAP), ZED3D9CommonTools::ConvertPixelFormat(PixelFormat), 
		(RenderTarget ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED), &CubeTexture, NULL);  
	if (Hr != D3D_OK)
	{
		zeError("D3D9 GetModule()", "Can not create volume texture resource.");
		return false;
	}

	this->PixelFormat = PixelFormat;
	this->EdgeLength = EdgeLength;
	this->RenderTarget = RenderTarget;

	return true;
}

void ZED3D9TextureCube::Lock(ZETextureCubeFace Face, void** Buffer, int* Pitch)
{
	D3DLOCKED_RECT Rect;
	CubeTexture->LockRect((D3DCUBEMAP_FACES)Face, 0, &Rect, NULL, D3DLOCK_DISCARD);
	*Buffer = Rect.pBits;
	*Pitch = Rect.Pitch;
}

void ZED3D9TextureCube::Unlock(ZETextureCubeFace Face)
{
	CubeTexture->UnlockRect((D3DCUBEMAP_FACES)Face, 0);
}

void ZED3D9TextureCube::Release()
{
	if (CubeTexture != NULL)
		CubeTexture->Release();
	CubeTexture = NULL;
}


void ZED3D9TextureCube::Destroy()
{
	GetModule()->TextureCubes.DeleteValue((ZED3D9TextureCube*)this);
	delete this;
}
