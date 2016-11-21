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
#include "ZEGraphics\ZEGRGraphicsModule.h"
#include "ZEGraphics\ZEGRWindow.h"

#include "ZED11Texture.h"
#include "ZED11RenderTarget.h"
#include "ZED11Adapter.h"

#include <dxgi1_2.h>

bool ZED11Output::Initialize(const ZEGRWindow* Window, ZEGRFormat Format)
{
	if (!ZEGROutput::Initialize(Window, Format))
		return false;

	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	SwapChainDesc.Width = Window->GetWidth();
	SwapChainDesc.Height = Window->GetHeight();
	SwapChainDesc.Format = ZED11ComponentBase::ConvertFormat(Format);
	SwapChainDesc.Stereo = FALSE;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = 0;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIDevice1* DXGIDevice;
	HRESULT Result = GetDevice()->QueryInterface(__uuidof(IDXGIDevice1), (void**)&DXGIDevice);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot query IDXGIDevice1. Error: %d", Result);
		DXGIDevice->Release();
		return false;
	}

	IDXGIAdapter1* DXGIAdapter;
	Result = DXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void**)&DXGIAdapter);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIAdapter1. Error: %d", Result);
		DXGIAdapter->Release();
		return false;
	}

	IDXGIFactory2* DXGIFactory;
	Result = DXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIFactory2. Error: %d", Result);
		DXGIFactory->Release();
		return false;
	}

	Result = DXGIFactory->CreateSwapChainForHwnd(GetDevice(), (HWND)Window->GetHandle(), &SwapChainDesc, NULL, NULL, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		DXGIDevice->Release();
		DXGIAdapter->Release();
		DXGIFactory->Release();
		return false;
	}

	DXGIFactory->MakeWindowAssociation((HWND)Window->GetHandle(), DXGI_MWA_NO_WINDOW_CHANGES);

	DXGIDevice->Release();
	DXGIAdapter->Release();
	DXGIFactory->Release();

	Resize(SwapChainDesc.Width, SwapChainDesc.Height);

	return true;
}

void ZED11Output::Deinitialize()
{
	SwapChain->SetFullscreenState(FALSE, NULL);
	ZEGR_RELEASE(SwapChain);
}

ZED11Output::ZED11Output()
{
	SwapChain = NULL;
}

ZED11Output::~ZED11Output()
{
	Deinitialize();
}

void ZED11Output::SetFullScreen(bool FullScreen)
{
	SwapChain->SetFullscreenState((BOOL)FullScreen, NULL);
}

void ZED11Output::Resize(ZEUInt Width, ZEUInt Height)
{
	if (SwapChain == NULL)
		return;

	if (Width == 0 || Height == 0)
		return;

	if (Texture != NULL)
	{
		if (Texture->GetWidth() == Width && Texture->GetHeight() == Height)
			return;

		ZED11Texture* D11Texture = static_cast<ZED11Texture*>(Texture.GetPointer());
		const_cast<ZED11RenderTarget*>(static_cast<const ZED11RenderTarget*>(D11Texture->GetRenderTarget()))->ForcedRelease();
		ZEGR_RELEASE(D11Texture->Resource);
	}

	HRESULT Result = SwapChain->ResizeBuffers(1, Width, Height, ConvertFormat(GetFormat()), 0);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot resize swapchain buffers. Error: 0x%X.", Result);
		return;
	}

	ID3D11Texture2D* BackBuffer;
	Result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&BackBuffer));
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get swapchain buffers. Error: 0x%X.", Result);
		return;
	}

	Texture = new ZED11Texture();
	ZED11Texture* D11Texture = static_cast<ZED11Texture*>(Texture.GetPointer());
	D11Texture->Type = ZEGR_TT_2D;
	D11Texture->Width = Width;
	D11Texture->Height = Height;
	D11Texture->DepthOrArrayCount = 1;
	D11Texture->SampleCount = 1;
	D11Texture->LevelCount = 1;
	D11Texture->SetResourceBindFlags(ZEGR_RBF_RENDER_TARGET);
	D11Texture->SetResourceUsage(ZEGR_RU_STATIC);
	D11Texture->SetFormat(GetFormat());
	D11Texture->Resource = BackBuffer;

	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(GetFormat());
	SetSize(Width * Height * FormatDefinition->BlockSize);
}

void ZED11Output::Present()
{
	LockContext();
	SwapChain->Present(0, 0);
	UnlockContext();
}
