//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Direct3D11Module.cpp
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

#include "ZED11Direct3D11Module.h"

#include "ZED11Shader.h"
#include "ZED11StatePool.h"
#include "ZED11Texture2D.h"
#include "ZED11Texture3D.h"
#include "ZED11Tracer.h"
#include "ZED11TextureCube.h"
#include "ZED11Tracer.h"
#include "ZED11IndexBuffer.h"
#include "ZED11VertexBuffer.h"
#include "ZED11RenderTarget.h"
#include "ZED11ComponentBase.h"
#include "ZED11Context.h"
#include "ZED11ShaderCompiler.h"
#include "ZED11ConstantBuffer.h"
#include "ZED11Adapter.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11RenderStateData.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEOptionManager.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEWindow.h"
#include "ZEError.h"

#include <d3d11.h>
#include <dxgi1_2.h>

#pragma warning(disable:4267)

ZE_MODULE_DESCRIPTION(ZED11Direct3D11Module, ZEGRGraphicsModule, NULL)

bool ZED11Direct3D11Module::CreateDevice(ZEGRAdapter* Adapter)
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

		HRESULT Result = D3D11CreateDevice(((ZED11Adapter*)Adapter)->GetAdapter(), 
			D3D_DRIVER_TYPE_UNKNOWN, NULL, DeviceFlags, 
			FeatureLevelArr, _countof(FeatureLevelArr), D3D11_SDK_VERSION, 
			&Device, &FeatureLevel, &D3DContext);
		
		if(FAILED(Result))
		{
			zeCriticalError("Cannot create device. Error: %d", Result);
			Deinitialize();
			return false;
		}

		D3DDevice->GetImmediateContext(&MainContext.Context);
	}
	
	return true;
}

bool ZED11Direct3D11Module::InitializeSelf()
{
	if (!ZEGRGraphicsModule::InitializeSelf())
		return false;

	#ifdef ZE_DEBUG_ENABLE
		ZEGRTracer::GetInstance()->SetEnabled(true);
	#else
		ZEGRTracer::GetInstance()->SetEnabled(false);
	#endif

	if (!CreateDevice(Adapters[0]))
		return false;
	
	ZED11ComponentBase::Module = this;
	ZED11ComponentBase::Device = Device;

	// Read options
	TextureQuality = (ZETextureQuality)ZEOptionManager::GetInstance()->GetOption("Graphics", "TextureQuality")->GetValue().GetInt32();
	ZEUInt Anisotropy = (ZEUInt)ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();
	ZEInt Width = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ZEInt Height = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	bool FullScreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "FullScreen")->GetValue().GetBoolean();
	bool VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();

	return true;
}

bool ZED11Direct3D11Module::DeinitializeSelf()
{
	Adapters.Clear();
	for (ZESize I = 0; I < Adapters.GetCount(); I++)
		delete (ZED11Adapter*)Adapters[I];

	return ZEGRGraphicsModule::DeinitializeSelf();
}

ZED11StatePool* ZED11Direct3D11Module::GetStatePool()
{
	return &StatePool;
}

ZEGRTracer* ZED11Direct3D11Module::GetTracer()
{
	return &Tracer;
}

const ZEArray<ZEGRAdapter*>& ZED11Direct3D11Module::GetAdapters()
{
	if (Adapters.GetCount() == 0)
	{
		IDXGIFactory2* DXGIFactory = NULL;
		HRESULT Result = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);

		IDXGIAdapter2* DXGIAdapter = NULL;
		ZEUInt AdapterId = 0;
		while (DXGIFactory->EnumAdapters1(AdapterId++, (IDXGIAdapter1**)&DXGIAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			ZED11Adapter* Adapter = new ZED11Adapter(DXGIAdapter);
			Adapters.Add(Adapter);
		}

		if (Adapters.GetCount() == 0)
		{
			zeCriticalError("Cannot enumerate adapters. Error: %d", DXGI_ERROR_NOT_FOUND);
			Deinitialize();
			return Adapters;
		}

		#ifdef ZE_GRAPHIC_LOG_ENABLE
		zeLog("Adaptors are enumerated.");
		#endif
	}
	
	return Adapters;
}

ZEGRContext*  ZED11Direct3D11Module::GetMainContext()
{
	return &MainContext;
}


ZEGRContext* ZED11Direct3D11Module::CreateContext()
{
	ZED11Context* Context = new ZED11Context();
	if (FAILED(Device->CreateDeferredContext(0, &Context->Context)))
	{
		zeError("Cannot create deffered device context.");
		return NULL;
	}

	return Context;
}

ZEGRIndexBuffer* ZED11Direct3D11Module::CreateIndexBuffer()
{
	return new ZED11IndexBuffer();
}

ZEGRConstantBuffer* ZED11Direct3D11Module::CreateConstantBuffer()
{
	return new ZED11ConstantBuffer();
}

ZEGRVertexBuffer* ZED11Direct3D11Module::CreateVertexBuffer()
{
	return new ZED11VertexBuffer();
}

ZEGRTexture2D* ZED11Direct3D11Module::CreateTexture2D()
{
	return new ZED11Texture2D();
}

ZEGRTexture3D* ZED11Direct3D11Module::CreateTexture3D()
{
	return new ZED11Texture3D();
}

ZEGRTextureCube* ZED11Direct3D11Module::CreateTextureCube()
{
	return new ZED11TextureCube();
}

ZEGRDepthStencilBuffer* ZED11Direct3D11Module::CreateDepthStencilBuffer()
{
	return new ZED11DepthStencilBuffer();
}

ZEGRRenderStateData* ZED11Direct3D11Module::CreateRenderStateData()
{
	return new ZED11RenderStateData();
}

ZEGRShaderCompiler* ZED11Direct3D11Module::CreateShaderCompiler()
{
	return new ZED11ShaderCompiler();
}

ZED11Direct3D11Module::ZED11Direct3D11Module()
{
}

ZED11Direct3D11Module::~ZED11Direct3D11Module()
{
	Deinitialize();
}
