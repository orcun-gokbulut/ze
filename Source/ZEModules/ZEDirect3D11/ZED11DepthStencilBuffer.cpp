//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11DepthStencilBuffer.cpp
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
#include "ZED11Module.h"
#include "ZED11DepthStencilBuffer.h"

inline static DXGI_FORMAT ConvertDepthStencilFormat(ZEGRDepthStencilFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_DSF_NONE:
			return DXGI_FORMAT_UNKNOWN;

		case ZEGR_DSF_DEPTH16:
			return DXGI_FORMAT_D16_UNORM;

		case ZEGR_DSF_DEPTH24_STENCIL8:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;

		case ZEGR_DSF_DEPTHD32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
	}
}

ID3D11Texture2D* ZED11DepthStencilBuffer::GetTexture()
{
	return Texture;
}

ID3D11DepthStencilView* ZED11DepthStencilBuffer::GetView()
{
	return View;
}

bool ZED11DepthStencilBuffer::Initialize(ZEUInt Width, ZEUInt Height, ZEGRDepthStencilFormat Format)
{
	zeDebugCheck(Width == 0, "Width cannot be zero.");
	zeDebugCheck(Height == 0, "Height cannot be zero.");
	zeDebugCheck(ConvertDepthStencilFormat(Format) == DXGI_FORMAT_UNKNOWN, "Unknown depth stencil format.");
	zeDebugCheck(Width > 8191 || Height > 8191, "Depth stencil buffer dimensions exceeds the limits, 0-8191.");

	D3D11_TEXTURE2D_DESC DepthStencilDesc;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.Width = Width;
	DepthStencilDesc.Height = Height;
	DepthStencilDesc.Format = ConvertDepthStencilFormat(Format);
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilDesc.CPUAccessFlags = 0;
	DepthStencilDesc.MiscFlags = 0;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;

	HRESULT Result = GetDevice()->CreateTexture2D(&DepthStencilDesc, NULL, &Texture);
	if(FAILED(Result))
	{
		zeError("Depth stencil texture creation failed. ErrorCode: %d.", Result);
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	DepthStencilViewDesc.Format = DepthStencilDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;
	DepthStencilViewDesc.Flags = 0;

	Result = GetDevice()->CreateDepthStencilView(Texture, &DepthStencilViewDesc, &View);
	if (FAILED(Result))
	{	
		zeError("Depth stencil view creation failed. ErrorCode: %d.", Result);	
		ZEGR_RELEASE(Texture);
		return false;
	}

	return ZEGRDepthStencilBuffer::Initialize(Width, Height, Format);
}

void ZED11DepthStencilBuffer::Deinitialize()
{
	ZEGR_RELEASE(View);
	ZEGR_RELEASE(Texture);
	ZEGRDepthStencilBuffer::Deinitialize();
}

ZED11DepthStencilBuffer::ZED11DepthStencilBuffer()
{
	Texture = NULL;
	View = NULL;
}
