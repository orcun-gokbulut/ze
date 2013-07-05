//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10GraphicsModule.cpp
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
#include "ZECore/ZECore.h"
#include "ZED3D10Shader.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEWindow.h"
#include "ZED3D10StatePool.h"
#include "ZED3D10Texture2D.h"
#include "ZED3D10Texture3D.h"
#include "ZED3D10EventTracer.h"
#include "ZED3D10TextureCube.h"
#include "ZED3D10EventTracer.h"
#include "ZED3D10IndexBuffer.h"
#include "ZED3D10VertexBuffer.h"
#include "ZED3D10RenderTarget.h"
#include "ZED3D10ShaderManager.h"
#include "ZED3D10ComponentBase.h"
#include "ZED3D10GraphicsModule.h"
#include "ZED3D10GraphicsDevice.h"
#include "ZED3D10ShaderCompiler.h"
#include "ZED3D10ConstantBuffer.h"
#include "ZECore/ZEOptionManager.h"
#include "ZED3D10DepthStencilBuffer.h"

#pragma warning(disable:4267)

ZE_MODULE_DESCRIPTION(ZED3D10GraphicsModule, ZEGraphicsModule, NULL)

bool ZED3D10GraphicsModule::ResizeFrontBuffer(DXGI_MODE_DESC& ModeToSwitchTo)
{
	if (FAILED(SwapChain->ResizeTarget(&ModeToSwitchTo)))
	{
		zeError("Cannot resize front buffers.");
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Front buffer is resized. Width: %u, Height: %u.", ModeToSwitchTo.Width, ModeToSwitchTo.Height);
#endif

	return true;
}

bool ZED3D10GraphicsModule::ResizeBackBuffers(ZEUInt Width, ZEUInt Height)
{
	UINT SwapShainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, SwapShainFlags)))
	{
		zeCriticalError("Cannot resize swap chain buffers");
		Deinitialize();
		return false;
	}

	GetDevice()->ResetStates();

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Back buffers(frame buffers) are resized. Width: %u, Height: %u", Width, Height);
#endif

	return true;
}

bool ZED3D10GraphicsModule::SetFullScreenState(bool Value)
{
	if (FAILED(SwapChain->SetFullscreenState(Value, Value ? AdapterOutput : NULL)))
	{
		zeError("Cannot set full screen state.");
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Full screen state changed to: %u.", (ZEUInt32)Value);
#endif

	return true;
}

DXGI_MODE_DESC ZED3D10GraphicsModule::FindMatchingDisplayMode(ZEUInt Width, ZEUInt Height)
{
	DXGI_MODE_DESC ModeFound = {0};
	DXGI_MODE_DESC ModeToSearch = {0};

	ModeToSearch.Width = Width;
	ModeToSearch.Height = Height;

	HRESULT Result = AdapterOutput->FindClosestMatchingMode(&ModeToSearch, &ModeFound, Device);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot find d3d10 matching display mode. Error: %d", Result);
		Deinitialize();
		return ModeFound;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Found matching display mode. Width: %u, Height: %u, RefreshRate: %u/%u ", 
			Width, Height, ModeFound.RefreshRate.Numerator, ModeFound.RefreshRate.Denominator);
#endif

	return ModeFound;
}

// Creates frame buffer(render target) that represents back buffer of the swap chain
bool ZED3D10GraphicsModule::CreateBuffers(ZEUInt Width, ZEUInt Height)
{
	// Get back buffer as a texture
	ID3D10Texture2D* BackBuffer = NULL;
	HRESULT Result = SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&BackBuffer);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot get back buffer pointer. Error: %d", Result);
		Deinitialize();
		return false;
	}

	// Create frame buffer render target
	ID3D10RenderTargetView* RenderTarget = NULL;
	Result = Device->CreateRenderTargetView(BackBuffer, NULL, &RenderTarget);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create frame buffer. Error: %d", Result);
		Deinitialize();
		return false;
	}

	FrameBuffers[0] = new ZED3D10RenderTarget(Width, Height, ZE_TPF_I8_4, ZE_TT_2D, RenderTarget);

	// Create default depth stencil buffer
	DepthBuffers[0] = ZEDepthStencilBuffer::CreateInstance();
	DepthBuffers[0]->Create(Width, Height, ZE_DSPF_DEPTH24_STENCIL8);

	// Create default view-ports
	Viewports[0].StateData.Width = Width;
	Viewports[0].StateData.Height = Height;
	Viewports[0].StateData.MinDepth = 0.0f;
	Viewports[0].StateData.MaxDepth = 1.0f;
	Viewports[0].StateData.TopLeftX = 0;
	Viewports[0].StateData.TopLeftY = 0;

	// Create default scissor rectangles
	ScissorRects[0].StateData.Top = 0;
	ScissorRects[0].StateData.Left = 0;
	ScissorRects[0].StateData.Right = Width;
	ScissorRects[0].StateData.Bottom = Height;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Frame buffers are created. Width: %u, Height: %u.", Width, Height);
#endif

	ZED3D_RELEASE(BackBuffer);

	return true;
}

void ZED3D10GraphicsModule::DestroyBuffers()
{
	ZE_DESTROY(FrameBuffers[0]);
	ZE_DESTROY(DepthBuffers[0]);
}

bool ZED3D10GraphicsModule::InitializeSelf()
{
	if (!ZEGraphicsModule::InitializeSelf())
		return false;

	#ifdef ZE_DEBUG_ENABLE
		ZEGraphicsEventTracer::GetInstance()->SetTracingEnabled(true);
	#else
		ZEGraphicsEventTracer::GetInstance()->SetTracingEnabled(false);
	#endif
	
	HRESULT Result;

	// Read options
	ScreenWidth = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ScreenHeight = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	FullScreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "FullScreen")->GetValue().GetBoolean();
	VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	SampleCount = ZEOptionManager::GetInstance()->GetOption("Graphics", "SampleCount")->GetValue().GetInt32();
	AnisotropicFilter = ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();
	TextureQuality = (ZETextureQuality)ZEOptionManager::GetInstance()->GetOption("Graphics", "TextureQuality")->GetValue().GetInt32();

	Viewports.SetCount(ScreenCount);
	FrameBuffers.SetCount(ScreenCount);
	DepthBuffers.SetCount(ScreenCount);
	ScissorRects.SetCount(ScreenCount);

	// Create a DirectX graphics interface factory.
	Result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create graphics interface factory. Error: %d.", Result);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("DXGI Factory created.");
#endif

	AdapterName = "Unknown Adapter.";

	// Enumerate and choose adapter
	UINT AdapterId = 0;
	IDXGIAdapter* TempAdapter = NULL;
	D3D10_DRIVER_TYPE DriverType = D3D10_DRIVER_TYPE_HARDWARE;
	while (Factory->EnumAdapters(AdapterId, &TempAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		if (TempAdapter != NULL)
		{
			AdapterId++;
			Adapter = TempAdapter;

			// Get adapter info
			DXGI_ADAPTER_DESC AdapterDesc;
			if(FAILED(Adapter->GetDesc(&AdapterDesc)))
			{
				zeWarning("Cannot get adapter descriptor.");
				continue;
			}

			ZEUInt MemoryMb = (ZEUInt)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);
			AdapterName = ZEString::FromUInt32(MemoryMb);
			AdapterName += "MB - ";
			AdapterName += ZEString::FromWCString(AdapterDesc.Description);

			#ifdef ZE_DEBUG_D3D10_ENABLE_PERFHUD
				// Is it NVIDIA PerfHUD adapter ?
				if (wcscmp(AdapterDesc.Description, L"NVIDIA PerfHUD") == 0)
				{
					DriverType = D3D10_DRIVER_TYPE_REFERENCE;
					break;
				}
			#endif
		}
	}
	if (AdapterId == 0)
	{
		zeCriticalError("Cannot enumerate adapters. Error: %d", DXGI_ERROR_NOT_FOUND);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Adaptor choosed. Name: %s", AdapterName.ToCString());
#endif

	UINT DeviceFlags = 0;
	#ifdef ZE_DEBUG_ENABLE
		//DeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
	#endif

	// Create the device
	Result = D3D10CreateDevice(Adapter, DriverType, NULL, DeviceFlags, D3D10_SDK_VERSION, &Device);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create device. Error: %d", Result);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Graphics device created.");
#endif

	// Enumerate adapter outputs
	Result = Adapter->EnumOutputs(0, &AdapterOutput);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot enumerate adapter outputs., Error: %d", Result);
		Deinitialize();
		return false;
	}

	ZEUInt DisplayModeCount = 0;
	ZEUInt DisplayModeFlags = DXGI_ENUM_MODES_INTERLACED;
	DXGI_FORMAT DisplayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	Result = AdapterOutput->GetDisplayModeList(DisplayFormat, DisplayModeFlags, &DisplayModeCount, NULL);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get display mode count. Error: %d", Result);
		Deinitialize();
		return false;
	}

	// Get all display modes
	AvailableModes.SetCount(DisplayModeCount);
	Result = AdapterOutput->GetDisplayModeList(DisplayFormat, DisplayModeFlags, &DisplayModeCount, AvailableModes.GetCArray());
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get display modes. Error: %d", Result);
		Deinitialize();
		return false;
	}
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Display mode list gathered.");
#endif

	// Initialize component base
	if (!ZED3D10ComponentBase::BaseInitialize(this))
	{
		zeCriticalError("Can not initialize D3D10 component base.");
		Deinitialize();
		return false;
	}

	// Check multi sample count
	Result = Device->CheckMultisampleQualityLevels(DisplayFormat, SampleCount, &SampleQuality);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot query sample count/quality support. Error: %d", Result);
		Deinitialize();
		return false;
	}

	if (SampleQuality == 0)
	{
		zeWarning("Sample count not supported by device. Multisampling disabled.");

		SampleCount = 1;
		SampleQuality = 0;
	}

	// Fill swap chain description
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = ScreenWidth;
	SwapChainDesc.BufferDesc.Height = ScreenHeight;
	SwapChainDesc.BufferDesc.Format = DisplayFormat;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.Windowed = true; // It is advised not to start full screen
	SwapChainDesc.BufferCount = 3;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Lets us to change front buffer resolution
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = (HWND)zeCore->GetWindow()->GetHandle();
	SwapChainDesc.SampleDesc.Count = SampleCount;
	SwapChainDesc.SampleDesc.Quality = SampleQuality != 0 ? SampleQuality - 1 : SampleQuality;

	// Create swap chain
	Result = Factory->CreateSwapChain(Device, &SwapChainDesc, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Swap chain created for matching display mode. BufferCount: %u, SampleCount: %u", SwapChainDesc.BufferCount, SampleCount);
#endif

	// Create default depth stencil buffers and frame buffers
	if (!CreateBuffers(ScreenWidth, ScreenHeight))
	{
		Deinitialize();
		return false;
	}

	SetFullScreen(FullScreen);
	
	// Disable DXGI defaults for alt + enter and print screen
	ZEUInt AssociationFlags = DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN;
	Result = Factory->MakeWindowAssociation((HWND)zeCore->GetWindow()->GetHandle(), AssociationFlags);
	if (FAILED(Result))
	{
		zeWarning("Cannot disable default DXGI Window Association .");
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Direct3D 10 module initialized.");
#endif

	return true;
}

bool ZED3D10GraphicsModule::DeinitializeSelf()
{
	AdapterName.Clear();
	AvailableModes.Clear(false);

	DestroyBuffers();
	
	ZED3D_RELEASE(Device);
	ZED3D_RELEASE(SwapChain);
	ZED3D_RELEASE(AdapterOutput);
	ZED3D_RELEASE(Adapter);
	ZED3D_RELEASE(Factory);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Direct3D 10 module destroyed.");
#endif

	return ZEGraphicsModule::DeinitializeSelf();
}

void ZED3D10GraphicsModule::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
}

void ZED3D10GraphicsModule::SetAnisotropicFilter(ZEUInt Anisotropy)
{
	AnisotropicFilter = Anisotropy;
}

bool ZED3D10GraphicsModule::SetFullScreen(bool Value)
{
	DXGI_MODE_DESC NewMode = FindMatchingDisplayMode(ScreenWidth, ScreenHeight);
	
	if (!ResizeFrontBuffer(NewMode))
		return false;
	
	if (!SetFullScreenState(Value))
		return false;

	FullScreen = Value;
	return true;
}

bool ZED3D10GraphicsModule::SetScreenSize(ZEUInt Width, ZEUInt Height)
{
	DestroyBuffers();

	if (!ResizeBackBuffers(Width, Height))
		return false;
		
	if (!CreateBuffers(Width, Height))
		return false;

	ScreenWidth = Width;
	ScreenHeight = Height;

	return true;
}

bool ZED3D10GraphicsModule::SetScreenCount(ZEUInt Count)
{
	ScreenCount = Count;

	return true;
}

bool ZED3D10GraphicsModule::SetSampleCount(ZEUInt Count)
{	
	ZEUInt Quality = 0;
	DXGI_FORMAT DisplayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	HRESULT Result = Device->CheckMultisampleQualityLevels(DisplayFormat, Count, &Quality);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot query sample count/quality support. Error: %d", Result);
		Deinitialize();
		return false;
	}

	if (Quality == 0)
	{
		zeWarning("Sample Count is not supported");
		return false;
	}

	SampleCount = Count;
	SampleQuality = Quality;

	DestroyBuffers();

	ZED3D_RELEASE(SwapChain);
	
	// Fill swap chain description
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = ScreenWidth;
	SwapChainDesc.BufferDesc.Height = ScreenWidth;
	SwapChainDesc.BufferDesc.Format = DisplayFormat;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.Windowed = true; // It is advised not to create full screen
	SwapChainDesc.BufferCount = 3;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Lets us to change front buffer resolution
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = (HWND)zeCore->GetWindow()->GetHandle();
	SwapChainDesc.SampleDesc.Count = SampleCount;
	SwapChainDesc.SampleDesc.Quality = SampleQuality != 0 ? SampleQuality - 1 : SampleQuality;
	
	// Create swap chain
	Result = Factory->CreateSwapChain(Device, &SwapChainDesc, &SwapChain);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create swap chain. Error: %d", Result);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Swap chain created for matching display mode. BufferCount: %u, SampleCount: %u", SwapChainDesc.BufferCount, SampleCount);
#endif

	if (!CreateBuffers(ScreenWidth, ScreenHeight))
	{
		zeCriticalError("Cannot create buffers");
		Deinitialize();
		return false;
	}

	return SetFullScreen(FullScreen);
}

void ZED3D10GraphicsModule::GetStatistics(ZEGraphicsStatistics& Statistics) const
{
	Statistics.BlendStateCount			= ZED3D10StatePool::BlendStateCount;
	Statistics.SamplerStateCount		= ZED3D10StatePool::SamplerStateCount;
	Statistics.DepthStencilCount		= ZED3D10StatePool::DepthStencilStateCount;
	Statistics.RasterizerCount			= ZED3D10StatePool::RasterizerStateCount;
	Statistics.VertexLayoutCount		= ZED3D10StatePool::VertexlayoutCount;
	Statistics.StatesPerSecond			= ZED3D10StatePool::StatesPerSecond;

	Statistics.ShaderCount				= ZED3D10Shader::GlobalCount;
	Statistics.ShaderSize				= ZED3D10Shader::GlobalSize;

	Statistics.Texture2DCount			= ZED3D10Texture2D::GlobalCount;
	Statistics.Texture2DSize			= ZED3D10Texture2D::GlobalSize;
	
	Statistics.Texture3DCount			= ZED3D10Texture3D::GlobalCount;
	Statistics.Texture3DSize			= ZED3D10Texture3D::GlobalSize;
	
	Statistics.TextureCubeCount			= ZED3D10TextureCube::GlobalCount;
	Statistics.TextureCubeSize			= ZED3D10TextureCube::GlobalSize;
	
	Statistics.DepthStancilBufferCount	= ZED3D10DepthStencilBuffer::GlobalCount;
	Statistics.DepthStancilBufferSize	= ZED3D10DepthStencilBuffer::GlobalSize;
	
	Statistics.IndexBufferCount			= ZED3D10IndexBuffer::GlobalCount;
	Statistics.IndexBufferSize			= ZED3D10IndexBuffer::GlobalSize;
	
	Statistics.VertexBufferCount		= ZED3D10VertexBuffer::GlobalCount;
	Statistics.VertexBufferSize			= ZED3D10VertexBuffer::GlobalSize;
	
	Statistics.ConstantBufferCount		= ZED3D10ConstantBuffer::GlobalCount;
	Statistics.ConstantBufferSize		= ZED3D10ConstantBuffer::GlobalSize;	
	
	Statistics.RenderTargetCount		= ZED3D10RenderTarget::GlobalCount;	
}

ZEGraphicsDevice* ZED3D10GraphicsModule::GetDevice() const
{
	static ZED3D10GraphicsDevice Device;
	return &Device;
}

ZEGraphicsEventTracer* ZED3D10GraphicsModule::GetEventTracer() const
{
	static ZED3D10EventTracer EventTracer;
	return &EventTracer;
}

ZEShaderCompiler* ZED3D10GraphicsModule::GetShaderCompiler() const
{
	static ZED3D10ShaderCompiler Compiler;
	return &Compiler;
}

ZEStatePool* ZED3D10GraphicsModule::GetStatePool() const
{
	static ZED3D10StatePool Pool;
	return &Pool;
}

ZEIndexBuffer* ZED3D10GraphicsModule::CreateIndexBuffer() const
{
	return new ZED3D10IndexBuffer();
}

ZEConstantBuffer* ZED3D10GraphicsModule::CreateConstantBuffer() const
{
	return new ZED3D10ConstantBuffer();
}

ZEVertexBuffer* ZED3D10GraphicsModule::CreateVertexBuffer() const
{
	return new ZED3D10VertexBuffer();
}

ZETexture2D* ZED3D10GraphicsModule::CreateTexture2D() const
{
	return new ZED3D10Texture2D();
}

ZETexture3D* ZED3D10GraphicsModule::CreateTexture3D() const
{
	return new ZED3D10Texture3D();
}

ZETextureCube* ZED3D10GraphicsModule::CreateTextureCube() const
{
	return new ZED3D10TextureCube();
}

ZEDepthStencilBuffer* ZED3D10GraphicsModule::CreateDepthStencilBuffer() const
{
	return new ZED3D10DepthStencilBuffer();
}

ZED3D10GraphicsModule::ZED3D10GraphicsModule()
{
	Enabled = true;

	Device = NULL;
	Adapter = NULL;
	Factory = NULL;
	SwapChain = NULL;
	AdapterOutput = NULL;

	ScreenCount = 1;
}

ZED3D10GraphicsModule::~ZED3D10GraphicsModule()
{
	Deinitialize();
}

ID3D10Device* ZED3D10GraphicsModule::GetD3D10Device() const
{
	return Device;
}

IDXGISwapChain* ZED3D10GraphicsModule::GetDXGISwapChain() const
{
	return SwapChain;
}
