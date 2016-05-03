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

#include "ZEError.h"

#include "ZEGraphics\ZEGRDefinitions.h"
#include "ZEGraphics\ZEGRRenderTarget.h"
#include "ZEGraphics\ZEGRDepthStencilBuffer.h"
#include "ZEGraphics\ZEGRGraphicsModule.h"

#include "ZED11Texture2D.h"
#include "ZED11RenderTarget.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11Adapter.h"

#include <d3d11.h>
#include <dxgi1_2.h>

void ZED11Output::SwitchToFullscreen()
{
	/*DXGI_MODE_DESC Description;
	Description.Width = Mode->GetWidth();
	Description.Height = Mode->GetWidth();
	Description.RefreshRate.Numerator = Mode->GetRefreshRate().Numerator;
	Description.RefreshRate.Denominator = Mode->GetRefreshRate().Denominator;
	Description.Format = ConvertFormat(Mode->GetFormat());
	Description.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Description.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain->ResizeTarget(&Description);*/

	IDXGIOutput1* Output = NULL;
	if(RestrictedToMonitor)
		Output = static_cast<ZED11Monitor*>(Monitor)->GetOutput();

	SwapChain->SetFullscreenState(TRUE, Output);
}

void ZED11Output::UpdateRenderTarget(ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	if (RenderTarget != NULL)
	{
		RenderTarget.Cast<ZED11RenderTarget>()->ForcedRelease();
		RenderTarget.Release();
	}

	HRESULT Result = SwapChain->ResizeBuffers(1, Width, Height, ConvertFormat(Format), 0);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot resize swapchain buffers. Error: %d.", Result);
		return;
	}

	ID3D11RenderTargetView* RenderTargetView;
	ID3D11Texture2D* BackBuffer;
	Result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get swapchain buffers. Error: %d.", Result);
		return;
	}

	Result = GetDevice()->CreateRenderTargetView(BackBuffer, NULL, &RenderTargetView);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create render target view. Error: %d.", Result);
		return;
	}
	ZEGR_RELEASE(BackBuffer);

	RenderTarget = new ZED11RenderTarget(Width, Height, Format, RenderTargetView);

	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(Format);
	SetSize(Width * Height * FormatDefinition->BlockSize);
}

bool ZED11Output::Initialize(void* Handle, ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	zeDebugCheck(Handle == NULL, "Handle parameter cannot be null.");
	zeDebugCheck(Width == 0 || Height == 0, "Width and Height cannot be null.");

	this->Handle = Handle;

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	memset(&SwapChainDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));
	SwapChainDesc.Width = Width;
	SwapChainDesc.Height = Height;
	SwapChainDesc.Format = ZED11ComponentBase::ConvertFormat(Format);
	SwapChainDesc.Stereo = FALSE;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullScreenDesc;
	memset(&FullScreenDesc, 0, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	FullScreenDesc.Windowed = TRUE;
	FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	FullScreenDesc.RefreshRate.Numerator = 0;
	FullScreenDesc.RefreshRate.Denominator = 0;

	IDXGIDevice1* DXGIDevice;
	HRESULT Result = GetDevice()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot query IDXGIDevice1. Error: %d", Result);
		DXGIDevice->Release();
		return false;
	}

	IDXGIAdapter1* DXGIAdapter;
	Result = DXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void**)&DXGIAdapter);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIAdapter1. Error: %d", Result);
		DXGIAdapter->Release();
		return false;
	}

	IDXGIFactory2* DXGIFactory;
	Result = DXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIFactory2. Error: %d", Result);
		DXGIFactory->Release();
		return false;
	}

	Result = DXGIFactory->CreateSwapChainForHwnd(GetDevice(), (HWND)Handle, &SwapChainDesc, &FullScreenDesc, NULL, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		DXGIDevice->Release();
		DXGIAdapter->Release();
		DXGIFactory->Release();
		return false;
	}

	DXGIDevice->Release();
	DXGIAdapter->Release();
	DXGIFactory->Release();

	UpdateRenderTarget(SwapChainDesc.Width, SwapChainDesc.Height, Format);

	return true;
}

void ZED11Output::Deinitialize()
{
	Handle = NULL;
	Monitor = NULL;

	SwapChain->SetFullscreenState(FALSE, NULL);
	ZEGR_RELEASE(SwapChain);
}

ZED11Output::ZED11Output()
{
	Handle = NULL;
	Monitor = NULL;
	SwapChain = NULL;

	Fullscreen = false;
	RestrictedToMonitor = false;
}

ZED11Output::~ZED11Output()
{
	Deinitialize();
}

void* ZED11Output::GetHandle() const
{
	return Handle;
}

ZEGRRenderTarget* ZED11Output::GetRenderTarget() const
{
	return RenderTarget;
}

void ZED11Output::SetMonitor(ZEGRMonitor* Monitor, bool RestrictToMonitor)
{
	if (this->Monitor == Monitor && RestrictedToMonitor == RestrictToMonitor)
		return;
	
	this->Monitor = Monitor;
	
	if (Fullscreen)
		SwitchToFullscreen();
}

ZEGRMonitor* ZED11Output::GetMonitor() const
{
	if (RestrictedToMonitor)
		return Monitor;

	IDXGIOutput* CurrentOutputMonitor;
	SwapChain->GetContainingOutput(&CurrentOutputMonitor);

	DXGI_OUTPUT_DESC CurrentOutputDescription;
	CurrentOutputMonitor->GetDesc(&CurrentOutputDescription);

	if (Monitor == NULL)
	{
		ZEGRAdapter* CurrentAdapter = ZEGRGraphicsModule::GetInstance()->GetCurrentAdapter();

		const ZEArray<ZEGRMonitor*>& Monitors = CurrentAdapter->GetMonitors();
		for (ZEUInt I = 0; I < Monitors.GetCount(); I++)
		{
			if (Monitors[I]->GetHandle() == CurrentOutputDescription.Monitor)
				Monitor = Monitors[I];
		}
	}
	else
	{
		if (Monitor->GetHandle() != CurrentOutputDescription.Monitor)
		{
			const ZEArray<ZEGRMonitor*>& Monitors = Monitor->GetAdapter()->GetMonitors();
			for (ZEUInt I = 0; I < Monitors.GetCount(); I++)
			{
				if (Monitors[I]->GetHandle() == CurrentOutputDescription.Monitor)
					Monitor = Monitors[I];
			}
		}
	}

	CurrentOutputMonitor->Release();

	return Monitor;
}

void ZED11Output::SetFullscreen(bool Enabled)
{
	if (Fullscreen == Enabled)
		return;

	Fullscreen = Enabled;

	if (Enabled)
		SwitchToFullscreen();
	else
		SwapChain->SetFullscreenState(FALSE, NULL);
}

bool ZED11Output::GetFullscreen() const
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

	UpdateRenderTarget(Width, Height, RenderTarget->GetFormat());
}

void ZED11Output::Present()
{
	SwapChain->Present(0, 0);
}
