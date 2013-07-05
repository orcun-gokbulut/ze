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

#include "ZEError.h"
#include "ZED3D10DepthStencilBuffer.h"
#include "ZED3D10GraphicsModule.h"

#include <D3D10.h>

inline static ZESize GetPixelSize(ZEDepthStencilPixelFormat Format)
{
	ZESize Size = 0;
	switch(Format)
	{
		case ZE_DSPF_DEPTH16:
			Size = 2;
			break;
		case ZE_DSPF_DEPTH24_STENCIL8:
			Size = 4;
			break;
		case ZE_DSPF_DEPTHD32_FLOAT:
			Size = 4;
			break;
	}
	return Size;
}

inline static ZESize CalculateBufferSize(ZEUInt Width, ZEUInt Height, ZEDepthStencilPixelFormat Format)
{
	return Width * Height * GetPixelSize(Format);
}


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

const ID3D10DepthStencilView* ZED3D10DepthStencilBuffer::GetD3D10DepthStencilView() const
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
	zeDebugCheck(PixelFormat == ZE_DSPF_NOTSET, "PixelFormat must be valid");
	zeDebugCheck(D3D10DepthStencilView, "Depth stencil buffer alread created");
	zeDebugCheck(Width > 8191 || Height > 8191, "Depth stencil buffer dimensions exceeds the limits, 0-8191.");

	D3D10_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.Width = Width;
	DepthStencilDesc.Height = Height;
	DepthStencilDesc.Format = ConvertPixelFormat(PixelFormat);
	DepthStencilDesc.Usage = D3D10_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;

	// Create depth stencil texture
	ID3D10Texture2D* DepthTexture = NULL;
	if(FAILED(D3D10Device->CreateTexture2D(&DepthStencilDesc, NULL, &DepthTexture)))
	{
		zeError("Cannot create depth stencil texture.");
		return false;
	}

	// Create depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = DepthStencilDesc.Format;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	DepthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(D3D10Device->CreateDepthStencilView(DepthTexture, &DepthStencilViewDesc, &D3D10DepthStencilView)))
	{	
		zeCriticalError("Cannot create depth stencil view");
		ZED3D_RELEASE(DepthTexture);
		return false;
	}
	
	ZED3D_RELEASE(DepthTexture);

	this->PixelFormat = PixelFormat;
	this->Height = Height;
	this->Width = Width;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Depth stencil buffer created. Width: %u, Height: %u, PixelFormat: %u.", 
			Width, Height, PixelFormat);
#endif

	GlobalSize += CalculateBufferSize(Width, Height, PixelFormat);
	GlobalCount++;

	return true;
}

ZESize		ZED3D10DepthStencilBuffer::GlobalSize = 0;
ZEUInt16	ZED3D10DepthStencilBuffer::GlobalCount = 0;

ZED3D10DepthStencilBuffer::ZED3D10DepthStencilBuffer()
{
	D3D10DepthStencilView = NULL;
}

ZED3D10DepthStencilBuffer::~ZED3D10DepthStencilBuffer()
{
	ZED3D_RELEASE(D3D10DepthStencilView);

	GlobalSize -= CalculateBufferSize(Width, Height, PixelFormat);
	GlobalCount--;
}
