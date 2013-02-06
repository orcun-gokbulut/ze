//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Texture3D.cpp
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

#include "ZED3D9Texture3D.h"
#include "ZED3D9GraphicsModule.h"
#include "ZED3D9CommonTools.h"
#include "ZEError.h"


ZED3D9Texture3D::ZED3D9Texture3D()
{
	VolumeTexture = NULL;
}

ZED3D9Texture3D::~ZED3D9Texture3D()
{
	this->Release();
}

bool ZED3D9Texture3D::IsEmpty() const
{
	return VolumeTexture == NULL;
}

bool ZED3D9Texture3D::Create(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat)
{
	if (!IsEmpty())
		Release();

	if (FAILED(Device->CreateVolumeTexture(Width, Height, Depth, LevelCount, 0, ZED3D9CommonTools::ConvertPixelFormat(PixelFormat), D3DPOOL_MANAGED, &VolumeTexture, NULL)))
	{
		zeError("Can not create volume texture resource.");
		Release();
		return false;
	}

	this->PixelFormat	= PixelFormat;
	this->LevelCount	= LevelCount;
	this->Depth			= Depth;
	this->Width			= Width;
	this->Height		= Height;
	
	return true;
}

bool ZED3D9Texture3D::Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch, ZEUInt Level)
{	
	D3DLOCKED_BOX Box = {0};
	
	if (FAILED(VolumeTexture->LockBox(Level, &Box, NULL, 0)))
	{
		zeError("Can not lock 3D texture.");
		return false;
	}

	*Buffer = Box.pBits;
	*RowPitch = (ZESize)Box.RowPitch;
	*SlicePitch = (ZESize)Box.SlicePitch;

	return true;
}

bool ZED3D9Texture3D::Unlock(ZEUInt Level)
{
	if (FAILED(VolumeTexture->UnlockBox(Level)))
	{
		zeError("Can not unlock 3D texture.");
		return false;
	}

	return true;
}

void ZED3D9Texture3D::Release()
{
	ZED3D_RELEASE(VolumeTexture);
}

void ZED3D9Texture3D::Destroy()
{
	delete this;
}







