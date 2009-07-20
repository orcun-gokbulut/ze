//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Texture.h
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

#pragma once
#ifndef __ZE_D3D9TEXTURE_H__
#define __ZE_D3D9TEXTURE_H__

#include "Graphics/Texture.h"
#include "D3D9ComponentBase.h"
#include <d3d9.h>

class ZED3D9Texture : public ZETexture, public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DTEXTURE9				Texture;


		virtual bool					IsEmpty() const;

		virtual void					DeviceLost();
		virtual bool					DeviceRestored();

		virtual bool					Create(int Width, int Height, ZETexturePixelFormat PixelFormat, bool RenderTarget = false);
		virtual void					Lock(void** Buffer, int* Pitch);
		virtual void					Unlock();
		virtual void					Release();

		virtual void					Destroy();

										ZED3D9Texture();
										~ZED3D9Texture();
};

class ZED3D9VolumeTexture : public ZEVolumeTexture, public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DVOLUMETEXTURE9		VolumeTexture;

		virtual bool					IsEmpty() const;

		virtual void					DeviceLost();
		virtual bool					DeviceRestored();

		virtual bool					Create(int Width, int Height, int Depth, ZETexturePixelFormat PixelFormat);
		virtual void					Lock(void** Buffer, int* RowPitch, int* SlicePitch);
		virtual void					Unlock();
		virtual void					Release();

		virtual void					Destroy();

										ZED3D9VolumeTexture();
										~ZED3D9VolumeTexture();
};

class ZED3D9CubeTexture : public ZECubeTexture, public ZED3D9ComponentBase
{
	public:
		LPDIRECT3DCUBETEXTURE9			CubeTexture;

		virtual bool					IsEmpty() const;

		virtual void					DeviceLost();
		virtual bool					DeviceRestored();

		virtual bool					Create(int EdgeLenght, ZETexturePixelFormat PixelFormat, bool RenderTarget = false);
		virtual void					Lock(ZECubeTextureFace Face, void** Buffer, int* Pitch);
		virtual void					Unlock(ZECubeTextureFace Face);
		virtual void					Release();

		virtual void					Destroy();

										ZED3D9CubeTexture();
										~ZED3D9CubeTexture();
};
#endif
