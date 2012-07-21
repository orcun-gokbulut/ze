//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Texture3D.h
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
#ifndef __ZE_D3D9_TEXTURE_3D_H__
#define __ZE_D3D9_TEXTURE_3D_H__

#include "ZETypes.h"
#include "ZEGraphics/ZETexture3D.h"
#include "ZED3D9ComponentBase.h"
#include <d3d9.h>

class ZED3D9Texture3D : public ZETexture3D, public ZED3D9ComponentBase
{
	friend class ZED3D9Module;
	private:
										ZED3D9Texture3D();
		virtual							~ZED3D9Texture3D();

	public:
		LPDIRECT3DVOLUMETEXTURE9		VolumeTexture;

		virtual void*					GetTextureHandle() const;
		virtual bool					IsEmpty() const;

		virtual void					DeviceLost();
		virtual bool					DeviceRestored();

		virtual bool					Create(ZEUInt Width, ZEUInt Height, ZEUInt Depth, ZEUInt MipLevel, ZETexturePixelFormat PixelFormat);
		virtual void					Lock(void** Buffer, ZESize* RowPitch, ZESize* SlicePitch, ZEUInt Level);
		virtual void					Unlock(ZEUInt Level);
		virtual void					Release();

		virtual void					Destroy();
};
#endif




