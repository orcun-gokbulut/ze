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

#include "ZEError.h"
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
#include "ZED11StructuredBuffer.h"
#include "ZED11Adapter.h"
#include "ZED11DepthStencilBuffer.h"
#include "ZED11RenderStateData.h"
#include "ZED11Output.h"
#include "ZED11Sampler.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEOptionManager.h"
#include "ZECore/ZEModule.h"
#include "ZECore/ZEWindow.h"

#include <d3d11_1.h>
#include <dxgi1_2.h>

#pragma warning(disable:4267)

static IDXGIFactory2* GetFactoryOfDevice(ID3D11Device* Device)
{
	IDXGIDevice* DXGIDevice;
	HRESULT Result = Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&DXGIDevice);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot query IDXGIDevice. Error: %d", Result);
		DXGIDevice->Release();
		return NULL;
	}

	IDXGIAdapter1* DXGIAdapter;
	Result = DXGIDevice->GetParent(__uuidof(IDXGIAdapter1), (void**)&DXGIAdapter);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIAdapter1. Error: %d", Result);
		DXGIAdapter->Release();
		return NULL;
	}

	IDXGIFactory2* DXGIFactory;
	Result = DXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&DXGIFactory);
	if(FAILED(Result))
	{
		zeCriticalError("Cannot get IDXGIFactory2. Error: %d", Result);
		DXGIFactory->Release();
		return NULL;
	}

	DXGIDevice->Release();
	DXGIAdapter->Release();

	return DXGIFactory;
}

bool ZED11Module::InitializeInternal()
{
	if (!ZEGRGraphicsModule::InitializeInternal())
		return false;

	#ifdef ZE_DEBUG_ENABLE
		ZEGRTracer::GetInstance()->SetEnabled(true);
	#else
		ZEGRTracer::GetInstance()->SetEnabled(false);
	#endif
	
	IDXGIFactory2* Factory;
	HRESULT Result = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&Factory);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create factory. Error: %d", Result);
		return false;
	}

	D3D_FEATURE_LEVEL FeatureLevelArr[] = 
	{
		D3D_FEATURE_LEVEL_11_0	
	};

	IDXGIAdapter1* Adapter;
	UINT AdapterIndex = 0;
	bool AdapterFound = false;
	while (Factory->EnumAdapters1(AdapterIndex++, &Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 AdapterDesc;
		Adapter->GetDesc1(&AdapterDesc);

		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			continue;

		HRESULT Result = D3D11CreateDevice(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, FeatureLevelArr, _countof(FeatureLevelArr), D3D11_SDK_VERSION, NULL, NULL, NULL);
		if (SUCCEEDED(Result))
		{
			AdapterFound = true;
			break;
		}
	}

	Factory->Release();

	if (!AdapterFound)
	{
		zeCriticalError("There is no hardware that supports Directx 11.");
		Adapter->Release();
		return false;
	}

	CurrentAdapter = new ZED11Adapter(Adapter);

	UINT DeviceFlags = 0;
	#ifdef _DEBUG
		//DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	#else
		DeviceFlags = D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY;
	#endif

	Result = D3D11CreateDevice(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, DeviceFlags, FeatureLevelArr, _countof(FeatureLevelArr), D3D11_SDK_VERSION, &Device, NULL, NULL);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create device. Error: %d", Result);
		return false;
	}

	ID3D11DeviceContext* NativeContext;
	Device->GetImmediateContext(&NativeContext);

	ZED11ComponentBase::Module = this;
	ZED11ComponentBase::Device = Device;
	ZED11ComponentBase::Context = NativeContext;

	Context.Initialize(NativeContext);

	return true;
}

bool ZED11Module::DeinitializeInternal()
{
	ZEGR_RELEASE(Device);

	delete (ZED11Adapter*)CurrentAdapter;

	Context.Deinitialize();

	for (ZESize I = 0; I < Adapters.GetCount(); I++)
		delete (ZED11Adapter*)Adapters[I];

	Adapters.Clear();

	return ZEGRGraphicsModule::DeinitializeInternal();
}

ZED11Module::ZED11Module()
{
	Device = NULL;
}

ZED11StatePool* ZED11Module::GetStatePool()
{
	return &StatePool;
}

ZEGRAdapter* ZED11Module::GetCurrentAdapter()
{
	return CurrentAdapter;
}

ZEGRTracer* ZED11Module::GetTracer()
{
	return &Tracer;
}

const ZEArray<ZEGRAdapter*>& ZED11Module::GetAdapters()
{
	if (Adapters.GetCount() == 0)
	{
		IDXGIFactory2* Factory = GetFactoryOfDevice(Device);

		IDXGIAdapter1* DXGIAdapter;
		ZEUInt AdapterId = 0;
		while (Factory->EnumAdapters1(AdapterId++, &DXGIAdapter) != DXGI_ERROR_NOT_FOUND)
			Adapters.Add(new ZED11Adapter(DXGIAdapter));

		Factory->Release();
	}
	
	return Adapters;
}

ZEGRContext*  ZED11Module::GetMainContext()
{
	return &Context;
}

ZEGRSampler* ZED11Module::CreateSamplerState()
{
	return new ZED11Sampler();
}

ZEGROutput* ZED11Module::CreateOutput()
{
	return new ZED11Output();
}

ZEGRContext* ZED11Module::CreateContext()
{
	ZED11Context* Context = new ZED11Context();
	if (FAILED(Device->CreateDeferredContext(0, &Context->Context)))
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

ZEGRStructuredBuffer* ZED11Module::CreateStructuredBuffer()
{
	return new ZED11StructuredBuffer();
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

ZEGRRenderStateData* ZED11Module::CreateRenderStateData()
{
	return new ZED11RenderStateData();
}

ZEGRComputeRenderStateData* ZED11Module::CreateComputeRenderStateData()
{
	return new ZED11ComputeRenderStateData();
}

ZEGRShaderCompiler* ZED11Module::CreateShaderCompiler()
{
	return new ZED11ShaderCompiler();
}

ZED11Module* ZED11Module::CreateInstance()
{
	return new ZED11Module();
}
