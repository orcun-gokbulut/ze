//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11GraphicsWindow.cpp
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

#include "ZED3D11GraphicsWindow.h"
#include "ZED3D11GraphicsModule.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZED3D11DepthStencilBuffer.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZED3D11RenderTarget.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZED3D11Texture2D.h"

void ZED3D11GraphicsWindow::ReleaseSwapChain()
{
	ZED3D_RELEASE(SwapChain);
}

bool ZED3D11GraphicsWindow::CreateSwapChain()
{
	ZED3D11GraphicsModule* Module = (ZED3D11GraphicsModule*)zeGraphics;

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
	SwapChainDesc.Width = Width;
	SwapChainDesc.Height = Height;
	SwapChainDesc.Format = Module->GetDXGIDisplayFormat();
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
	// Override for better performance
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
#endif

#ifdef ZE_GRAPHICS_CONTENT_PROTECTION
	SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;
#endif

	// Always create windowed and then switch to full screen if needed
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullScreenDesc;
	FullScreenDesc.Windowed = TRUE;
	FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	FullScreenDesc.RefreshRate.Numerator = 0;
	FullScreenDesc.RefreshRate.Denominator = 0;

//#ifdef ZE_GRAPHICS_CONTENT_PROTECTION
//	Output = Adapters[ActiveAdapter].Outputs[I].Output;
//#endif

	IDXGIOutput* Output = NULL;
	ID3D11Device* D3DDevice = Module->GetD3D10Device();
	IDXGIFactory2* DXGIFactory = Module->GetDXGIFactory();
	
	HRESULT Result = DXGIFactory->CreateSwapChainForHwnd(D3DDevice, (HWND)Handle, &SwapChainDesc, &FullScreenDesc, Output, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		return false;
	}

	zeLog("Swap chain created");

	return true;
}

void ZED3D11GraphicsWindow::ReleaseBackBuffer()
{
	ZE_DESTROY(BackBuffer);
	ZE_DESTROY(BackBufferTexture);
	ZE_DESTROY(DepthBuffer);

	zeLog("Buffers released.");
}

bool ZED3D11GraphicsWindow::CreateBackBuffer()
{
	HRESULT Result = S_FALSE;
	ID3D11Device* Device = ((ZED3D11GraphicsModule*)zeGraphics)->GetD3D10Device();

	ID3D11Texture2D* D3DBackBuffer = NULL;
	Result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&D3DBackBuffer);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get back buffer pointer. Error: %d", Result);
		return false;
	}

	BackBufferTexture = new ZED3D11Texture2D();
	BackBufferTexture->Create(D3DBackBuffer);
	BackBufferTexture->SetDebugName("SwapChainBackBufferTexture");

	BackBuffer = (ZED3D11RenderTarget*)BackBufferTexture->CreateRenderTarget();
	BackBuffer->SetDebugName("SwapChainBackBuffer");

	DepthBuffer = (ZED3D11DepthStencilBuffer*)ZEDepthStencilBuffer::CreateInstance();
	DepthBuffer->Create(Width, Height, ZE_DSPF_DEPTH24_STENCIL8);
	DepthBuffer->SetDebugName("SwapChainDepthBuffer");

	zeLog("Buffers created");

	return true;
}

bool ZED3D11GraphicsWindow::ManageFlagCreated()
{
	if (!CreateSwapChain())
		return false;

	if (!CreateBackBuffer())
		return false;

	return true;
}

bool ZED3D11GraphicsWindow::ManageFlagDestroyed()
{
	ReleaseBackBuffer();
	ReleaseSwapChain();

	return true;
}

#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZECamera.h"

bool ZED3D11GraphicsWindow::ManageFlagResized()
{
	ReleaseBackBuffer();

	DXGI_FORMAT Format =  DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT SwapShainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	HRESULT Result = SwapChain->ResizeBuffers(5, Width, Height, Format, SwapShainFlags);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot resize swap chain buffers");
		return false;
	}

	zeLog("Swap chain buffers resized");

	if (!CreateBackBuffer())
		return false;

	zeScene->GetActiveCamera()->SetWidth(Width);
	zeScene->GetActiveCamera()->SetHeight(Height);

	return true;
}

bool ZED3D11GraphicsWindow::ManageFlagWindowed()
{
	HRESULT Result = SwapChain->SetFullscreenState(false, NULL);
	if (FAILED(Result))
	{
		zeError("Cannot set full screen state.");
		return false;
	}

	zeLog("Full screen state changed to false");

	return true;
}

#include "ZED3D11GraphicsMonitor.h"

bool ZED3D11GraphicsWindow::ManageFlagFullScreen()
{
	zeLog("ManageFlagFullScreen CALLEDDDDDDD.");

	HRESULT Result = S_FALSE;

	ZED3D11GraphicsModule* Module = (ZED3D11GraphicsModule*)zeGraphics;
	IDXGIOutput1* DXGIOutput = ((const ZED3D11GraphicsMonitor*)FullScreenMonitor)->GetDXGIOutput();

	DXGI_MODE_DESC1 SearchMode = {0};
	SearchMode.Width = Width;
	SearchMode.Height = Height;
	SearchMode.Format = Module->GetDXGIDisplayFormat();
	SearchMode.RefreshRate.Numerator = 0;
	SearchMode.RefreshRate.Denominator = 0;
	SearchMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SearchMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SearchMode.Stereo = FALSE;

	DXGI_MODE_DESC1 MatchMode = {0};
	Result = DXGIOutput->FindClosestMatchingMode1(&SearchMode, &MatchMode, Module->GetD3D10Device());
	if (FAILED(Result))
	{
		zeError("Cannot find matching display mode. Error: %d", Result);
		return false;
	}

	zeLog("Matching full screen mode found.");

	Result = SwapChain->SetFullscreenState(true, FullScreen ?  NULL : DXGIOutput);
	if (FAILED(Result))
	{
		zeError("Cannot set full screen state.");
		return false;
	}

	zeLog("Full screen state changed to true");

	// Also resize front buffer to full screen resolution
	Result = SwapChain->ResizeTarget((DXGI_MODE_DESC*)&MatchMode);
	if (FAILED(Result))
	{
		zeError("Cannot resize front buffers.");
		return false;
	}

	zeLog("Resized front buffer");

	return true;
}

IDXGISwapChain1* ZED3D11GraphicsWindow::GetDXGISwapChain() const
{
	return SwapChain;
}

bool ZED3D11GraphicsWindow::Update()
{
	if (!ZEGRWindow::Update())
		return false;

	FlagLock.Lock();
	ZEUInt32 TempFlags = Flags;
	Flags = 0;
	FlagLock.Unlock();

	ZEUInt I = 0;
	bool Result = true;
	while (TempFlags != ZE_GWF_NONE)
	{
		ZEUInt32 CurrentFlag = 1 << I;

		Result = true;
		switch (TempFlags & CurrentFlag)
		{
			case ZE_GWF_CREATED:
				Result = ManageFlagCreated();
				break;
			case ZE_GWF_DESTROYED:
				Result = ManageFlagDestroyed();
				break;
			case ZE_GWF_RESIZED:
				Result = ManageFlagResized();
				break;
			case ZE_GWF_WINDOWED:
				Result = ManageFlagWindowed();
				break;
			case ZE_GWF_FULLSCREEN:
				Result = ManageFlagFullScreen();
				break;
		};

		if (!Result)
			break;

		TempFlags &= ~CurrentFlag;
		I++;
	}

	return Result;
}

bool ZED3D11GraphicsWindow::Present()
{
	//! Handle possible errors
	HRESULT Result = SwapChain->Present(VSynchEnabed ? 1 : 0, 0);
	if (Result == DXGI_ERROR_DEVICE_RESET  || Result == DXGI_ERROR_DEVICE_REMOVED)
		return false;

	return true;
}

const ZEGRTexture2D* ZED3D11GraphicsWindow::GetBackBufferTexture()
{
	return NULL; //BackBufferTexture;
}

const ZEGRRenderTarget* ZED3D11GraphicsWindow::GetBackBuffer()
{
	return BackBuffer;
}

const ZEDepthStencilBuffer* ZED3D11GraphicsWindow::GetDepthBuffer()
{
	return DepthBuffer;
}

ZED3D11GraphicsWindow::ZED3D11GraphicsWindow()
{
	SwapChain = NULL;
	BackBuffer = NULL;
	//BackBufferTexture = NULL;
	DepthBuffer = NULL;
}

ZED3D11GraphicsWindow::~ZED3D11GraphicsWindow()
{
	
}
