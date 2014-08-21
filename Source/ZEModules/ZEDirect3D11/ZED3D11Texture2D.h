//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11Texture2D.h
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

#ifndef __ZE_D3D11_TEXTURE_2D_H__
#define __ZE_D3D11_TEXTURE_2D_H__

#include <d3d11.h>

#include "ZETypes.h"
#include "ZED3D11ComponentBase.h"
#include "ZEGraphics/ZETexture2D.h"

class ZETextureData;
class ZERenderTarget;

class ZED3D11Texture2D : public ZETexture2D, public ZED3D11ComponentBase
{
	friend class ZED3D11GraphicsModule;
	friend class ZED3D11GraphicsDevice;
	friend class ZED3D11GraphicsWindow;

	protected:
		ID3D11Texture2D*					D3D10Texture2D;
		ID3D11ShaderResourceView*			D3D10ShaderResourceView;

		bool								UpdateWith(ZEUInt ShadowIndex);

											ZED3D11Texture2D();
		virtual								~ZED3D11Texture2D();

	public:
		const ID3D11Texture2D*				GetD3D10Texture() const;
		const ID3D11ShaderResourceView*		GetD3D10ResourceView() const;
		
		ZERenderTarget*						CreateRenderTarget(ZEUInt MipLevel = 0) const;
		
		bool								Create(ID3D11Texture2D* D3DTexture);

		bool								CreateDynamic(ZEUInt Width, ZEUInt Height, ZETexturePixelFormat PixelFormat, ZETextureData* Data = NULL);
		bool								CreateStatic(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZETexturePixelFormat PixelFormat, bool RenderTarget = false, ZETextureData* Data = NULL);
};

#endif




