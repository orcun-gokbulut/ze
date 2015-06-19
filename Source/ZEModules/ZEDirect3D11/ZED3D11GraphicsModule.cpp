//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11GraphicsModule.cpp
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

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include "ZEError.h"
#include "ZECore/ZECore.h"
#include "ZED3D11Shader.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEWindow.h"
#include "ZED3D11StatePool.h"
#include "ZED3D11Texture2D.h"
#include "ZED3D11Texture3D.h"
#include "ZED11Tracer.h"
#include "ZED3D11TextureCube.h"
#include "ZED11Tracer.h"
#include "ZED11IndexBuffer.h"
#include "ZED3D11VertexBuffer.h"
#include "ZED3D11RenderTarget.h"
#include "ZED3D11ShaderManager.h"
#include "ZED11ComponentBase.h"
#include "ZED3D11GraphicsWindow.h"
#include "ZED3D11GraphicsModule.h"
#include "ZED3D11GraphicsDevice.h"
#include "ZED3D11ShaderCompiler.h"
#include "ZED11ConstantBuffer.h"
#include "ZECore/ZEOptionManager.h"
#include "ZED3D11GraphicsMonitor.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZEGraphics/ZEGRWindow.h"


#pragma warning(disable:4267)


ZE_MODULE_DESCRIPTION(ZED3D11GraphicsModule, ZEGRGraphicsModule, NULL)

void ZED3D11GraphicsModule::ReleaseWindows()
{
	//! RELEASE WINDOWS HERE
	//! RELEASE WINDOWS HERE
	//! RELEASE WINDOWS HERE
	//! RELEASE WINDOWS HERE
	//! RELEASE WINDOWS HERE
}

void ZED3D11GraphicsModule::ReleaseFactory()
{
	ZEGR_RELEASE(DXGIFactory);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Factory is released.");
#endif
}

bool ZED3D11GraphicsModule::CreateFactory()
{
	HRESULT Result = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create graphics interface factory. Error: %d.", Result);
		Deinitialize();
		return false;
	}
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Factory created.");
#endif

	return true;
}

void ZED3D11GraphicsModule::ReleaseAdapters()
{
	ZESize Count = Adapters.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{	
		ZEGR_RELEASE(Adapters[I]);
	}
	Adapters.Clear(false);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Adapters are released.");
#endif
}

bool ZED3D11GraphicsModule::EnumerateAdapters()
{
	UINT AdapterId = 0;
	IDXGIAdapter2* DXGIAdapter = NULL;

	while (DXGIFactory->EnumAdapters1(AdapterId++, (IDXGIAdapter1**)&DXGIAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC2 AdapterDesc;
		HRESULT Result = DXGIAdapter->GetDesc2(&AdapterDesc);
		if(FAILED(Result))
		{
			zeCriticalError("Cannot get adapter descriptor. Error: %d", Result);
			Deinitialize();
			return false;
		}
		
		switch (AdapterDesc.Flags)
		{
			case DXGI_ADAPTER_FLAG_NONE:
				zeLog("Adapter found: \"%s\".", ZEString(AdapterDesc.Description).ToCString());
				break;
			default:
			case DXGI_ADAPTER_FLAG_REMOTE:
			case DXGI_ADAPTER_FLAG_SOFTWARE:
				zeWarning("Skipping adapter: \"%s\".", ZEString(AdapterDesc.Description).ToCString());
				continue;
				break;
		};

		Adapters.Add(DXGIAdapter);
	}

	if (Adapters.GetCount() == 0)
	{
		zeCriticalError("Cannot enumerate adapters. Error: %d", DXGI_ERROR_NOT_FOUND);
		Deinitialize();
		return false;
	}
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Adaptors are enumerated.");
#endif

	return true;
}

void ZED3D11GraphicsModule::ReleaseMonitors()
{
	for (ZESize I = 0; I < Monitors.GetCount(); ++I)
	{
		ZED3D11GraphicsMonitor* Monitor = (ZED3D11GraphicsMonitor*)Monitors[I];
		ZEGR_RELEASE(Monitor->DXGIOutput);
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Adapter outputs are released.");
#endif
}

bool ZED3D11GraphicsModule::EnumerateMonitors()
{
	ZESize Count = Adapters.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEUInt OutputId = 0;
		IDXGIOutput1* DXGIOutput = NULL;
		
		while (Adapters[I]->EnumOutputs(OutputId++, (IDXGIOutput**)&DXGIOutput) != DXGI_ERROR_NOT_FOUND)
		{
			ZED3D11GraphicsMonitor* Monitor = new ZED3D11GraphicsMonitor(DXGIOutput, Adapters[I]);
			Monitors.Add(Monitor);
		}
	}

	if (Monitors.GetCount() == 0)
	{
		zeCriticalError("Cannot enumerate monitors. Error: %d", DXGI_ERROR_NOT_FOUND);
		Deinitialize();
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Monitors are enumerated.");
#endif

	return true;
}

void ZED3D11GraphicsModule::ReleaseDevices()
{
	ZESize Count = D3DDevices.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ZEGR_RELEASE(D3DDevices[I]);
		ZEGR_RELEASE(D3DContexes[I]);
	}
	D3DContexes.Clear(false);
	D3DDevices.Clear(false);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Devices are released.");
#endif
}

bool ZED3D11GraphicsModule::CreateDevices()
{
	D3D_FEATURE_LEVEL FeatureLevelArr[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT DeviceFlags = 0;
	#ifdef ZE_GRAPHICS_DEVICE_DEBUG_LAYER_ENABLED
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif	
	
	ZESize Count = Adapters.GetCount();
	for (ZESize I = 0; I < Count; ++I)
	{
		ID3D11Device* D3DDevice = NULL;
		ID3D11DeviceContext* D3DContext = NULL;
		D3D_FEATURE_LEVEL FeatureLevel;

		HRESULT Result = D3D11CreateDevice(	Adapters[I], 
											D3D_DRIVER_TYPE_UNKNOWN, 
											NULL, 
											DeviceFlags, 
											FeatureLevelArr, 
											_countof(FeatureLevelArr), 
											D3D11_SDK_VERSION, 
											&D3DDevice, 
											&FeatureLevel,
											&D3DContext);
		if(FAILED(Result))
		{
			zeCriticalError("Cannot create device. Error: %d", Result);
			Deinitialize();
			return false;
		}

		D3DDevices.Add(D3DDevice);
		D3DContexes.Add(D3DContext);

		ZED3D11GraphicsDevice* ZEDevice = new ZED3D11GraphicsDevice(D3DDevice, I);
		Devices.Add(ZEDevice);
	}
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Graphics devices are created.");
#endif

	return true;
}

bool ZED3D11GraphicsModule::DisableAssociations()
{
	HWND Handle = (HWND)GetWindow()->GetHandle();
	ZEUInt AssociationFlags = DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN;
	HRESULT Result = DXGIFactory->MakeWindowAssociation(Handle, AssociationFlags);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot disable DXGI Window Association. Error: %d", Result);
		Deinitialize();
		return false;
	}

	return true;
}

bool ZED3D11GraphicsModule::InitializeSelf()
{
	if (!ZEGRGraphicsModule::InitializeSelf())
		return false;

	

	#ifdef ZE_DEBUG_ENABLE
		ZEGraphicsEventTracer::GetInstance()->SetTracingEnabled(true);
	#else
		ZEGraphicsEventTracer::GetInstance()->SetTracingEnabled(false);
	#endif

	if (!CreateFactory())
		return false;

	if (!EnumerateAdapters())
		return false;

	if (!EnumerateMonitors())
		return false;

	if (!CreateDevices())
		return false;
	
	if (!DisableAssociations())
		return false;

	ZED11ComponentBase::BaseInitialize(this);

	TextureQuality = (ZETextureQuality)ZEOptionManager::GetInstance()->GetOption("Graphics", "TextureQuality")->GetValue().GetInt32();
	ZEUInt Anisotropy = (ZEUInt)ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();

	// Read options
	ZEInt Width = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ZEInt Height = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	bool FullScreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "FullScreen")->GetValue().GetBoolean();
	bool VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	
	ZEGRWindow* Window = GetWindow();
	Window->SetSize(Width, Height);
	Window->SetFullScreen(FullScreen);
	Window->SetVSynchEnabed(VerticalSync);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Direct3D module initialized.");
#endif

	return true;
}

bool ZED3D11GraphicsModule::DeinitializeSelf()
{
	ReleaseWindows();
	ReleaseDevices();
	ReleaseMonitors();
	ReleaseAdapters();
	ReleaseFactory();

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Direct3D module deinitialized.");
#endif
	
	return ZEGRGraphicsModule::DeinitializeSelf();
}

ZEGraphicsEventTracer* ZED3D11GraphicsModule::GetEventTracer() const
{
	static ZED11Tracer EventTracer;
	return &EventTracer;
}

ZEGRShaderCompiler* ZED3D11GraphicsModule::GetShaderCompiler() const
{
	static ZED3D11ShaderCompiler Compiler;
	return &Compiler;
}

ZEStatePool* ZED3D11GraphicsModule::GetStatePool() const
{
	static ZED3D11StatePool Pool;
	return &Pool;
}

ZEGRIndexBuffer* ZED3D11GraphicsModule::CreateIndexBuffer()
{
	return new ZED11IndexBuffer();
}

ZEGRConstantBuffer* ZED3D11GraphicsModule::CreateConstantBuffer()
{
	return new ZED11ConstantBuffer();
}

ZEGRVertexBuffer* ZED3D11GraphicsModule::CreateVertexBuffer()
{
	return new ZED3D11VertexBuffer();
}

ZEGRWindow* ZED3D11GraphicsModule::CreateGraphicsWindow()
{
	ZED3D11GraphicsWindow* NewWindow = new ZED3D11GraphicsWindow();
	Windows.Add(NewWindow);

	return NewWindow;
}

ZEGRTexture2D* ZED3D11GraphicsModule::CreateTexture2D()
{
	return new ZED3D11Texture2D();
}

ZEGRTexture3D* ZED3D11GraphicsModule::CreateTexture3D()
{
	return new ZED3D11Texture3D();
}

ZEGRTextureCube* ZED3D11GraphicsModule::CreateTextureCube()
{
	return new ZED3D11TextureCube();
}

ZEGRDepthStencilBuffer* ZED3D11GraphicsModule::CreateDepthStencilBuffer()
{
	return new ZED11DepthStencilBuffer();
}

ID3D11Device* ZED3D11GraphicsModule::GetD3D10Device(ZESize Index) const
{
	zeDebugCheck(Index >= D3DDevices.GetCount(), "Index out of range");
	return D3DDevices[Index];
}

ID3D11DeviceContext* ZED3D11GraphicsModule::GetD3D10Context(ZESize Index) const
{
	zeDebugCheck(Index >= D3DContexes.GetCount(), "Index out of range");
	return D3DContexes[Index];
}

const ZEArray<ID3D11Device*>& ZED3D11GraphicsModule::GetD3D10Devices() const
{
	return D3DDevices;
}

const ZEArray<ID3D11DeviceContext*>& ZED3D11GraphicsModule::GetD3D10Contexes() const
{
	return D3DContexes;
}

DXGI_FORMAT ZED3D11GraphicsModule::GetDXGIDisplayFormat() const
{
	return DisplayFormat;
}

IDXGIFactory2* ZED3D11GraphicsModule::GetDXGIFactory() const
{
	return DXGIFactory;
}

ZED3D11GraphicsModule::ZED3D11GraphicsModule()
{
	DXGIFactory = NULL;
	DisplayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
}

ZED3D11GraphicsModule::~ZED3D11GraphicsModule()
{
	Deinitialize();
}
