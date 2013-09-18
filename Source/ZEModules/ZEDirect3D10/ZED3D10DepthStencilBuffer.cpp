//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10DepthStencilBuffer.cpp
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

#include <d3d11.h>

#include "ZEError.h"
#include "ZED3D10GraphicsModule.h"
#include "ZED3D10DepthStencilBuffer.h"

inline static DXGI_FORMAT ConvertPixelFormat(ZEDepthStencilPixelFormat Format)
{
	DXGI_FORMAT DXGIFormat = DXGI_FORMAT_UNKNOWN;
	switch(Format)
	{
		case ZE_DSPF_NOTSET:
			DXGIFormat = DXGI_FORMAT_UNKNOWN;
			break;
		case ZE_DSPF_DEPTH16:
			DXGIFormat = DXGI_FORMAT_D16_UNORM;
			break;
		case ZE_DSPF_DEPTH24_STENCIL8:
			DXGIFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		case ZE_DSPF_DEPTHD32_FLOAT:
			DXGIFormat = DXGI_FORMAT_D32_FLOAT;
			break;
	}
	return DXGIFormat;
}

const ID3D11DepthStencilView* ZED3D10DepthStencilBuffer::GetD3D10DepthStencilView() const
{
	return D3D10DepthStencilView;
}

bool ZED3D10DepthStencilBuffer::IsEmpty() const
{
	return D3D10DepthStencilView == NULL;
}

bool ZED3D10DepthStencilBuffer::Create(ZEUInt Width, ZEUInt Height, ZEDepthStencilPixelFormat PixelFormat)
{
	zeDebugCheck(Width == 0, "Width cannot be zero");
	zeDebugCheck(Height == 0, "Height cannot be zero");
	zeDebugCheck(!IsEmpty(), "Depth stencil buffer alread created");
	zeDebugCheck(PixelFormat == ZE_DSPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(Width > 8191 || Height > 8191, "Depth stencil buffer dimensions exceeds the limits, 0-8191.");

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.Width = Width;
	DepthStencilDesc.Height = Height;
	DepthStencilDesc.Format = ConvertPixelFormat(PixelFormat);
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;

	// Create depth stencil texture
	ID3D11Texture2D* DepthTexture = NULL;
	HRESULT Result = D3DDevices[0]->CreateTexture2D(&DepthStencilDesc, NULL, &DepthTexture);
	if(FAILED(Result))
	{
		zeError("D3D10 depth stencil texture creation failed. ErrorCode: %d.", Result);
		return false;
	}

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = DepthStencilDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	DepthStencilViewDesc.Flags = 0;

	Result = D3DDevices[0]->CreateDepthStencilView(DepthTexture, &DepthStencilViewDesc, &D3D10DepthStencilView);
	if (FAILED(Result))
	{	
		zeError("D3D10 depth stencil view creation failed. ErrorCode: %d.", Result);
		ZED3D_RELEASE(DepthTexture);
		return false;
	}
	
	ZED3D_RELEASE(DepthTexture);

	return ZEDepthStencilBuffer::Create(Width, Height, PixelFormat);
}

ZED3D10DepthStencilBuffer::ZED3D10DepthStencilBuffer()
{
	D3D10DepthStencilView = NULL;
}

ZED3D10DepthStencilBuffer::~ZED3D10DepthStencilBuffer()
{
	ZED3D_RELEASE(D3D10DepthStencilView);
}
