//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Screen.cpp
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

#include "ZED11Screen.h"

#include "ZEGraphics\ZEGRDefinitions.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZEGraphics\ZEGRDepthStencilBuffer.h"
#include "ZEError.h"

#include <d3d11.h>
#include <dxgi1_2.h>

bool ZED11Screen::InitializeSelf()
{
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	SwapChainDesc.Width = GetWidth();
	SwapChainDesc.Height = GetHeight();
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.Stereo = FALSE;
	SwapChainDesc.BufferCount = 5;
	SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER | DXGI_CPU_ACCESS_NONE | DXGI_USAGE_SHADER_INPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	#if(WINVER >= _WIN32_WINNT_WIN8)
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	#endif

	#ifdef ZE_GRAPHICS_CONTENT_PROTECTION
	SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
	#endif

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullScreenDesc;
	FullScreenDesc.Windowed = TRUE;
	FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	FullScreenDesc.RefreshRate.Numerator = 0;
	FullScreenDesc.RefreshRate.Denominator = 0;

	//#ifdef ZE_GRAPHICS_CONTENT_PROTECTION
	//Output = Adapters[ActiveAdapter].Outputs[I].Output;
	//#endif

	IDXGIFactory2* DXGIFactory = NULL;
	HRESULT Result = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create graphics interface factory. Error: %d.", Result);
		return false;
	}

	IDXGIOutput* Output = NULL;
	Result = DXGIFactory->CreateSwapChainForHwnd(GetDevice(), (HWND)GetHandle(), &SwapChainDesc, &FullScreenDesc, Output, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		return false;
	}

	zeLog("Swap chain created");

	return true;
}

void ZED11Screen::DeinitializeSelf()
{
	ZEGR_RELEASE(RenderTarget);
	ZEGR_RELEASE(DepthStencilBuffer);
	ZEGR_RELEASE(SwapChain);
	ZEGR_RELEASE(Output);
}

ZED11Screen::ZED11Screen()
{
	RenderTarget = NULL;
	DepthStencilBuffer = NULL;
}

void ZED11Screen::SetSize(ZEUInt Width, ZEUInt Height)
{
	ZEGRScreen::SetSize(Width, Height);
}

void ZED11Screen::SetVisible(bool Visible)
{
	ZEGRScreen::SetVisible(Visible);
}

ZEGRRenderTarget* ZED11Screen::GetRenderTarget()
{
	return RenderTarget;
}

ZEGRDepthStencilBuffer* ZED11Screen::GetDepthStencilBuffer()
{
	return DepthStencilBuffer;
}

void ZED11Screen::Present()
{

}
