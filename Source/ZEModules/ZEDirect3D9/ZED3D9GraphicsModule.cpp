//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9GraphicsModule.cpp
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

#ifdef ZE_DEBUG_ENABLE
	#define D3D_DEBUG_INFO
#endif

#include "ZEError.h"
#include "ZECore/ZEWindow.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Texture3D.h"
#include "ZECore/ZEConsole.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9IndexBuffer.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9ShaderManager.h"
#include "ZED3D9GraphicsModule.h"
#include "ZECore/ZEOptionManager.h"
#include "ZED3D9VertexDeclaration.h"

#include <d3dx9.h>
#include "ZED3D9Shader.h"
#include "ZED3D9GraphicsDevice.h"
#include "ZED3D9EventTracer.h"
#include "ZED3D9ShaderCompiler.h"

#pragma warning(disable:4267)

ZE_MODULE_DESCRIPTION(ZED3D9GraphicsModule, ZEGraphicsModule, NULL)


bool ZED3D9GraphicsModule::RestoreDevice(bool ForceReset)
{
	IsDeviceLost = true;

	// Release non D3DPOOL_MANAGED resources
	for (ZESize I = 0; I < RenderTarget2Ds.GetCount(); ++I)
		RenderTarget2Ds[I]->DeviceLost();

	for (ZESize I = 0; I < RenderTargetCubes.GetCount(); ++I)
		RenderTargetCubes[I]->DeviceLost();

	// Release back buffer
	ZED3D_RELEASE(FrameBuffer.Surface);
	ZED3D_RELEASE(DepthStencilBuffer.DepthSurface);

	// ForceReset device
	HRESULT DeviceState, Hr;
	do
	{
		DeviceState = Device->TestCooperativeLevel();

		if (DeviceState == D3DERR_DRIVERINTERNALERROR)
			zeCriticalError("Can not restore Direct3D Device. Internal driver error.");

		if (DeviceState == D3DERR_DEVICENOTRESET || ForceReset)
		{
			Hr = Device->Reset(&D3DPP);
			if (Hr == D3D_OK)
			{
				zeLog("Direct3D Device Restored.");
				break;
			}
			else if (Hr == D3DERR_DEVICELOST)
			{
				Sleep(100);
				continue;
			}
			else
			{
				zeCriticalError("Can not restore Direct3D Device.");
			}
		}
	}
	while (DeviceState == D3DERR_DEVICELOST);
	
	IsDeviceLost = false;
	zeLog("Recreating resources.");

	// Create back buffer
	Hr = Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &FrameBuffer.Surface);
	if (FAILED(Hr))
	{
		zeCriticalError("Cannot create back buffer");
		return false;
	}

	Hr = Device->GetDepthStencilSurface(&DepthStencilBuffer.DepthSurface);
	if (FAILED(Hr))
	{
		zeCriticalError("Cannot create depth stencil surface");
		return false;
	}
	
	// Create non D3DPOOL_MANAGED resources
	for (ZESize I = 0; I < RenderTarget2Ds.GetCount(); ++I)
		RenderTarget2Ds[I]->DeviceRestored();

	for (ZESize I = 0; I < RenderTargetCubes.GetCount(); ++I)
		RenderTargetCubes[I]->DeviceRestored();
	
	return true;
}

bool ZED3D9GraphicsModule::GetEnabled()
{
	return Enabled;
}

void ZED3D9GraphicsModule::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZED3D9GraphicsModule::GetDeviceLost()
{
	return IsDeviceLost;
}

bool ZED3D9GraphicsModule::Initialize()
{
	zeLog("Initializing Direct3D 9 module.");

	#ifdef ZE_DEBUG_GRAPHICS_EVENTS
		D3DPERF_SetOptions(0);
	#else
		D3DPERF_SetOptions(1);
	#endif

	// Read options
	ScreenWidth = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ScreenHeight = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	Fullscreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean();
	FarZ = ZEOptionManager::GetInstance()->GetOption("Graphics", "FarZ")->GetValue().GetFloat();
	NearZ = ZEOptionManager::GetInstance()->GetOption("Graphics", "NearZ")->GetValue().GetFloat();
	VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	AnisotropicFilter = ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();

	HRESULT Result;
	
	// Create direct 3d device creating parameters
	ZeroMemory(&D3DPP, sizeof(D3DPRESENT_PARAMETERS));
	D3DPP.BackBufferCount = 1;
	D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	D3DPP.BackBufferHeight = (Fullscreen ?  ScreenHeight : 0);
	D3DPP.BackBufferWidth  = (Fullscreen ?  ScreenWidth : 0);

	D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	D3DPP.MultiSampleQuality = 0;

	D3DPP.EnableAutoDepthStencil = true;
	D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;

	D3DPP.Windowed = !Fullscreen;
	D3DPP.hDeviceWindow = (HWND)zeWindow->GetHandle();
	D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3DPP.Flags = 0;

	D3DPP.PresentationInterval = (VerticalSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE);
	D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	
	// Create Direct 3d
	D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!D3D)
	{
		zeCriticalError("Can not create Direct3D.");
		Destroy();
		return false;
	}
	
	// Check device type;
	if (FAILED(D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, Fullscreen)))
	{
		zeCriticalError("Device does not support Device Type.");
		Destroy();
		return false;
	}

	UINT AdapterToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
	
	// If PerfHUD debugging enabled (by defining ZE_DEBUG_D3D9_PERFHUD) replace default adapter with PerfHUD adapter
#ifdef ZE_DEBUG_D3D9_ENABLE_PERFHUD
	for (UINT Adapter=0; Adapter < D3D->GetAdapterCount(); Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res;
		Res = D3D->GetAdapterIdentifier(Adapter, 0, &Identifier);
		if (strstr(Identifier.Description,"PerfHUD") != 0)
		{
			AdapterToUse=Adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}
#endif
	
	// Create Device
	Result = D3D->CreateDevice(AdapterToUse, DeviceType, (HWND)zeWindow->GetHandle(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPP, &Device);
	if(FAILED(Result))
	{
		zeCriticalError("Can not create Direct3D Device.");
		Destroy();
		return false;
	}

	zeLog("Device Created.");

	// Check hardware capabilities
	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);

	zeLog("Vertex Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(Caps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(Caps.VertexShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(Caps.VertexShaderVersion))
	{
		case 1:
		case 2:
			zeCriticalError("Vertex shader 1.x and 2.x are not supported.");
			Deinitialize();
			return false;
			break;
		case 3:
			break;
		default:
			zeCriticalError("No vertex shader support present.");
			Deinitialize();
			return false;
			break;
	};

	// Check pixel shader version
	zeLog("Pixel Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(Caps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(Caps.PixelShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(Caps.PixelShaderVersion))
	{
		case 1:
		case 2:
			zeCriticalError("Pixel Shader 1.x and 2.x are not supported.");
			Deinitialize();
			return false;
			break;
		case 3:
			break;
		default:
			zeCriticalError("No pixel shader support present.");
			Deinitialize();
			return false;
			break;
	};

	// Get screen's back buffer
	Result = Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &FrameBuffer.Surface);
	if(FAILED(Result)) 
	{
		zeCriticalError("Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	// Get screen's z buffer
	Result = Device->GetDepthStencilSurface(&DepthStencilBuffer.DepthSurface);
	if(FAILED(Result))
	{
		zeCriticalError("Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	// Initialize component base
	if (!ZED3D9ComponentBase::BaseInitialize(this))
	{
		zeCriticalError("Can not initialize D3D9 component base.");
		return false;
	}

	return true;
}

// OK
void ZED3D9GraphicsModule::Deinitialize()
{
	zeLog("Destroying Direct3D.\r\n");

	RenderTarget2Ds.Clear(false);
	RenderTargetCubes.Clear(false);

	ZED3D_RELEASE(FrameBuffer.Surface);
	ZED3D_RELEASE(Device);
	ZED3D_RELEASE(D3D);
}

ZEGraphicsDevice* ZED3D9GraphicsModule::GetDevice()
{
	static ZED3D9GraphicsDevice Device;
	return &Device;
}

ZEGraphicsEventTracer* ZED3D9GraphicsModule::GetEventTracer()
{
	static ZED3D9EventTracer EventTracer;
	return &EventTracer;
}

ZEShaderCompiler* ZED3D9GraphicsModule::GetShaderCompiler()
{
	static ZED3D9ShaderCompiler Compiler;
	return &Compiler;
}

ZERenderTarget* ZED3D9GraphicsModule::GetFrameBuffer()
{
	return &FrameBuffer;
}

ZEDepthBuffer* ZED3D9GraphicsModule::GetDepthStencilBuffer()
{
	return &DepthStencilBuffer;
}

ZED3D9ShaderManager* ZED3D9GraphicsModule::GetShaderManager()
{
	static ZED3D9ShaderManager ShaderManager;
	return &ShaderManager;
}

void ZED3D9GraphicsModule::RegisterRenderTarget(ZED3D9Texture2D* RenderTarget)
{
	RenderTarget2Ds.Add(RenderTarget);
}

void ZED3D9GraphicsModule::RegisterRenderTarget(ZED3D9TextureCube* RenderTarget)
{
	RenderTargetCubes.Add(RenderTarget);
}

void ZED3D9GraphicsModule::UnregisterRenderTarget(ZED3D9Texture2D* RenderTarget)
{
	RenderTarget2Ds.DeleteValue(RenderTarget);
}

void ZED3D9GraphicsModule::UnregisterRenderTarget(ZED3D9TextureCube* RenderTarget)
{
	RenderTargetCubes.DeleteValue(RenderTarget);
}

void ZED3D9GraphicsModule::SetScreenSize(ZEUInt Width, ZEUInt Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
	if (Device != NULL)
	{
		D3DPP.BackBufferWidth = Width;
		D3DPP.BackBufferHeight = Height;
		RestoreDevice(true);
	}
}

void ZED3D9GraphicsModule::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
	if (Device != NULL)
	{
		D3DPP.PresentationInterval = (VerticalSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE);
		RestoreDevice(true);
	}
}

void ZED3D9GraphicsModule::SetAntiAliasing(ZEUInt Level)
{
	AntiAliasing = Level;
	if (Device != NULL)
	{
		D3DPP.MultiSampleType = (D3DMULTISAMPLE_TYPE)(AntiAliasing);
		RestoreDevice(true);
	}
}

void ZED3D9GraphicsModule::SetAnisotropicFilter(ZEUInt Level)
{
	AnisotropicFilter = Level;
	for (DWORD I = 0; I < Caps.MaxSimultaneousTextures; I++)
	{
		/*
		DWORD Mag, Min, Mip;
		Device->GetSamplerState(I, D3DSAMP_MAGFILTER, &Mag);
		Device->GetSamplerState(I, D3DSAMP_MINFILTER, &Min);
		Device->GetSamplerState(I, D3DSAMP_MIPFILTER, &Mip);
		*/
		Device->SetSamplerState(I, D3DSAMP_MAGFILTER, (AnisotropicFilter > 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		Device->SetSamplerState(I, D3DSAMP_MINFILTER, (AnisotropicFilter > 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		Device->SetSamplerState(I, D3DSAMP_MIPFILTER, (AnisotropicFilter > 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		if (AnisotropicFilter > 0)
		{
			Device->SetSamplerState(I, D3DSAMP_MAXANISOTROPY, (AnisotropicFilter > Caps.MaxAnisotropy ? Caps.MaxAnisotropy : AnisotropicFilter));
		}
	}
}

ZEShader* ZED3D9GraphicsModule::CreatePixelShader()
{
	return new ZED3D9PixelShader();
}

ZEShader* ZED3D9GraphicsModule::CreateVertexShader()
{
	return new ZED3D9VertexShader();
}

ZERenderTarget* ZED3D9GraphicsModule::CreateRenderTarget()
{
	return new ZED3D9RenderTarget();
}

ZEVertexDeclaration* ZED3D9GraphicsModule::CreateVertexDeclaration()
{
	return new ZED3D9VertexDeclaration();
}

ZEStaticIndexBuffer* ZED3D9GraphicsModule::CreateStaticIndexBuffer()
{
	return new ZED3D9StaticIndexBuffer();
}

ZEStaticVertexBuffer* ZED3D9GraphicsModule::CreateStaticVertexBuffer()
{
	return new ZED3D9StaticVertexBuffer();
}

ZETexture2D* ZED3D9GraphicsModule::CreateTexture2D()
{
	return new ZED3D9Texture2D();
}

ZETexture3D* ZED3D9GraphicsModule::CreateTexture3D()
{
	return new ZED3D9Texture3D();
}

ZETextureCube* ZED3D9GraphicsModule::CreateTextureCube()
{
	return new ZED3D9TextureCube();
}

ZED3D9GraphicsModule::ZED3D9GraphicsModule()
{
	Enabled = true;
	IsDeviceLost = false;
	D3D = NULL;
	Device = NULL;
}

ZED3D9GraphicsModule::~ZED3D9GraphicsModule()
{
	Deinitialize();
}

LPDIRECT3D9 ZED3D9GraphicsModule::GetDirect3D()
{
	return D3D;
}

LPDIRECT3DDEVICE9 ZED3D9GraphicsModule::GetDirect3DDevice()
{
	return Device;
}



