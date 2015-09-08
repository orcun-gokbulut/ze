//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Module.cpp
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

#include "ZED11Module.h"

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
#include "ZED11Output.h"

#include <d3d11_1.h>
#include <dxgi1_2.h>

#pragma warning(disable:4267)

ZE_MODULE_DESCRIPTION(ZED11Module, ZEGRGraphicsModule, NULL)

bool ZED11Module::InitializeSelf()
{
	if (!ZEGRGraphicsModule::InitializeSelf())
		return false;

	#ifdef ZE_DEBUG_ENABLE
		ZEGRTracer::GetInstance()->SetEnabled(true);
	#else
		ZEGRTracer::GetInstance()->SetEnabled(false);
	#endif

	D3D_FEATURE_LEVEL FeatureLevelArr[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0	
	};

	UINT DeviceFlags = 0;
	//#ifdef ZE_GRAPHICS_DEVICE_DEBUG_LAYER_ENABLED
		DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	/*#else
		DeviceFlags |= D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY;
	#endif*/
	
	ID3D11Device* DeviceTemp;
	HRESULT Result = D3D11CreateDevice(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, NULL, DeviceFlags, 
		FeatureLevelArr, _countof(FeatureLevelArr), D3D11_SDK_VERSION, 
		&DeviceTemp, NULL, NULL);
		
	if(FAILED(Result))
	{
		zeCriticalError("Cannot create device. Error: %d", Result);
		Deinitialize();
		return false;
	}

	Result = DeviceTemp->QueryInterface(__uuidof(ID3D11Device1), (void**)&Device);
	if (FAILED(Result) || Device == NULL)
	{
		zeCriticalError("Cannot query Direct3D11 Interface.");
		return false;
	}

	DeviceTemp->Release();

	ID3D11DeviceContext1* NativeContext;
	Device->GetImmediateContext1(&NativeContext);
	Context.Initialize(NativeContext);

	ZED11ComponentBase::Module = this;
	ZED11ComponentBase::Device = Device;
	ZED11ComponentBase::Context = Context.Context;

	// Read options
	/*TextureQuality = (ZETextureQuality)ZEOptionManager::GetInstance()->GetOption("Graphics", "TextureQuality")->GetValue().GetInt32();
	ZEUInt Anisotropy = (ZEUInt)ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();
	ZEInt Width = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ZEInt Height = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	bool FullScreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "FullScreen")->GetValue().GetBoolean();
	bool VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();*/

	return true;
}

bool ZED11Module::DeinitializeSelf()
{
	Context.Deinitialize();

	Adapters.Clear();
	for (ZESize I = 0; I < Adapters.GetCount(); I++)
		delete (ZED11Adapter*)Adapters[I];

	return ZEGRGraphicsModule::DeinitializeSelf();
}

ZED11StatePool* ZED11Module::GetStatePool()
{
	return &StatePool;
}

ZEGRTracer* ZED11Module::GetTracer()
{
	return &Tracer;
}

const ZEArray<ZEGRAdapter*>& ZED11Module::GetAdapters()
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

ZEGRContext*  ZED11Module::GetMainContext()
{
	return &Context;
}

ZEGROutput* ZED11Module::CreateOutput()
{
	return new ZED11Output();
}

ZEGRContext* ZED11Module::CreateContext()
{
	ZED11Context* Context = new ZED11Context();
	if (FAILED(Device->CreateDeferredContext1(0, &Context->Context)))
	{
		zeError("Cannot create deffered device context.");
		return NULL;
	}

	return Context;
}

ZEGRIndexBuffer* ZED11Module::CreateIndexBuffer()
{
	return new ZED11IndexBuffer();
}

ZEGRShader* ZED11Module::CreateShader()
{
	return new ZED11Shader();
}

ZEGRConstantBuffer* ZED11Module::CreateConstantBuffer()
{
	return new ZED11ConstantBuffer();
}

ZEGRVertexBuffer* ZED11Module::CreateVertexBuffer()
{
	return new ZED11VertexBuffer();
}

ZEGRTexture2D* ZED11Module::CreateTexture2D()
{
	return new ZED11Texture2D();
}

ZEGRTexture3D* ZED11Module::CreateTexture3D()
{
	return new ZED11Texture3D();
}

ZEGRTextureCube* ZED11Module::CreateTextureCube()
{
	return new ZED11TextureCube();
}

ZEGRDepthStencilBuffer* ZED11Module::CreateDepthStencilBuffer()
{
	return new ZED11DepthStencilBuffer();
}

ZEGRRenderStateData* ZED11Module::CreateRenderStateData()
{
	return new ZED11RenderStateData();
}

ZEGRShaderCompiler* ZED11Module::CreateShaderCompiler()
{
	return new ZED11ShaderCompiler();
}

ZED11Module::ZED11Module()
{
	Context = Context;
}

ZED11Module::~ZED11Module()
{
	Deinitialize();
}
