//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Output.cpp
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

#include "ZED11Output.h"

#include "ZEGraphics\ZEGRDefinitions.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZEGraphics\ZEGRDepthStencilBuffer.h"
#include "ZEGraphics\ZEGRAdapter.h"
#include "ZED11Texture2D.h"

#include "ZEError.h"

#include <d3d11.h>
#include <dxgi1_2.h>

void ZED11Output::SwitchToFullscreen()
{
	DXGI_MODE_DESC Description;
	Description.Width = Mode->GetWidth();
	Description.Height = Mode->GetWidth();
	Description.RefreshRate.Numerator = Mode->GetRefreshRate().Numerator;
	Description.RefreshRate.Denominator = Mode->GetRefreshRate().Denominator;
	Description.Format = ConvertFormat(Mode->GetFormat());
	Description.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Description.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain->ResizeTarget(&Description);
	SwapChain->SetFullscreenState(TRUE, Output);
}

void ZED11Output::UpdateRenderTarget(ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11Texture2D* OutputTexture;
	HRESULT Result = SwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D), (void**)&OutputTexture);
	Result = GetDevice()->CreateRenderTargetView(OutputTexture, NULL, &RenderTargetView);
	OutputTexture->Release();
	RenderTarget = new ZED11RenderTarget(Width, Height, Format, RenderTargetView);

	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(Format);
	SetSize(Width * Height * FormatDefinition->BlockSize);
}

bool ZED11Output::Initialize(void* Handle, ZEGRMonitorMode* Mode, ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	zeDebugCheck(Handle == NULL, "Handle parameter cannot be null.");
	zeDebugCheck(Width == 0 || Height == 0, "Width and Height cannot be null.");

	this->Handle = Handle;
	this->Mode = Mode;

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	memset(&SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));
	SwapChainDesc.Width = Mode != NULL ? Mode->GetWidth() : Width;
	SwapChainDesc.Height = Mode != NULL ? Mode->GetHeight() : Height;
	SwapChainDesc.Format = Mode != NULL ? ZED11ComponentBase::ConvertFormat(Mode->GetFormat()) : DXGI_FORMAT_B8G8R8A8_UNORM;
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
	memset(&FullScreenDesc, 0, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	FullScreenDesc.Windowed = Mode == NULL;
	FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	FullScreenDesc.RefreshRate.Numerator = Mode != NULL ? Mode->GetRefreshRate().Numerator : 0;
	FullScreenDesc.RefreshRate.Denominator = Mode != NULL ? Mode->GetRefreshRate().Denominator : 0;

	IDXGIFactory2* DXGIFactory = NULL;
	HRESULT Result = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create graphics interface factory. Error: %d.", Result);
		return false;
	}

	IDXGIOutput* Output = NULL;
	Result = DXGIFactory->CreateSwapChainForHwnd(GetDevice(), (HWND)Handle, &SwapChainDesc, &FullScreenDesc, Output, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		return false;
	}

	UpdateRenderTarget(SwapChainDesc.Width, SwapChainDesc.Height, Mode != NULL ? Mode->GetFormat() : Format);
	DXGIFactory->Release();

	return true;
}

void ZED11Output::Deinitialize()
{
	SwapChain->SetFullscreenState(FALSE, Output);
	ZEGR_RELEASE(SwapChain);
	ZEGR_RELEASE(Output);
}

ZED11Output::ZED11Output()
{
	Handle = NULL;
	Mode = NULL;
}

void* ZED11Output::GetHandle()
{
	return Handle;
}

ZEGRRenderTarget* ZED11Output::GetRenderTarget()
{
	return RenderTarget;
}

void ZED11Output::SetMonitorMode(ZEGRMonitorMode* Mode)
{
	if (this->Mode == Mode)
		return;
	
	this->Mode = Mode;
	
	if (Fullscreen)
		SwitchToFullscreen();
}

ZEGRMonitorMode* ZED11Output::GetMonitorMode()
{
	return Mode;
}

void ZED11Output::SetFullscreen(bool Enabled)
{
	if (Fullscreen == Enabled)
		return;

	Fullscreen = Enabled;

	if (Enabled)
		SwitchToFullscreen();
	else
		SwapChain->SetFullscreenState(FALSE, Output);
}

bool ZED11Output::GetFullscreen()
{
	return Fullscreen;
}

void ZED11Output::Resize(ZEUInt Width, ZEUInt Height)
{
	if (SwapChain == NULL)
		return;

	if (Width == 0 || Height == 0)
		return;

	if (RenderTarget->GetWidth() == Width && RenderTarget->GetHeight() == Height)
		return;

	ZEGR_RELEASE(RenderTarget->View);

	HRESULT Result = SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	UpdateRenderTarget(Width, Height, RenderTarget->GetFormat());
}

void ZED11Output::Present()
{
	SwapChain->Present(0, 0);
}
