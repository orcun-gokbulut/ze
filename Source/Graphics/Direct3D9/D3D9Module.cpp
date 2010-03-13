//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Module.cpp
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
#ifdef ZE_DEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif

#include "D3D9Module.h"
#include "D3D9ModuleDescription.h"
#include "Core/Window.h"
#include "Core/Error.h"
#include "Core/Console.h"
#include "D3D9FixedMaterial.h"
#include "D3D9FixedMaterialShader.h"
#include "D3D9Texture2D.h"
#include "D3D9Texture3D.h"
#include "D3D9TextureCube.h"
#include "D3D9VertexBuffer.h"
#include "D3D9VertexDeclaration.h"
#include "PostProcessor/D3D9PostProcessor.h"
#include "D3D9FrameBufferRenderer.h"
#include "D3D9TextureRenderer.h"
#include "D3D9ShadowRenderer.h"
#include "D3D9UIMaterial.h"
#include "D3D9SimpleMaterial.h"

#include <d3dx9.h>

LPDIRECT3DDEVICE9 D3D9Device;
ZED3D9Module* D3D9Module;
D3DPRESENT_PARAMETERS D3DPP;

#pragma warning(disable:4267)

ZEModuleDescription* ZED3D9Module::GetModuleDescription()
{
	static ZED3D9ModuleDescription Desc;
	return &Desc;
}

LPDIRECT3D9 ZED3D9Module::GetDirect3D()
{
	return D3D;
}

LPDIRECT3DDEVICE9 ZED3D9Module::GetDevice()
{
	return Device;
}

D3DCAPS9* ZED3D9Module::GetDeviceCaps()
{
	return &DeviceCaps;
}

LPDIRECT3DSURFACE9 ZED3D9Module::GetFrameColorBuffer()
{
	return FrameColorBuffer;
}

LPDIRECT3DSURFACE9 ZED3D9Module::GetFrameZBuffer()
{
	return FrameZBuffer;
}

ZED3D9PixelShaderVersion ZED3D9Module::GetPixelShaderVersion()
{
	return PixelShaderVersion;
}

ZED3D9VertexShaderVersion ZED3D9Module::GetVertexShaderVersion()
{
	return VertexShaderVersion;
}

ZED3D9PipelineType ZED3D9Module::GetPipelineType()
{
	return PipelineType;
}

bool ZED3D9Module::IsEnabled()
{
	return Enabled;
}

void ZED3D9Module::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZED3D9Module::Initialize()
{
	zeLog("D3D9 Module", "Initializing Direct3D 9.");

	// Read options
	ScreenWidth = zeOptions->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger();
	ScreenHeight = zeOptions->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger();
	Fullscreen = zeOptions->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean();
	FarZ = zeOptions->GetOption("Graphics", "FarZ")->GetValue().GetFloat();
	NearZ = zeOptions->GetOption("Graphics", "NearZ")->GetValue().GetFloat();
	VerticalSync = zeOptions->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	AnisotropicFilter = zeOptions->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInteger();

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
	D3DPP.AutoDepthStencilFormat = D3DFMT_D24X8;

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
		zeCriticalError("D3D9 Module", "Can not create Direct3D.");
		Destroy();
		return false;
	}

	// Check device type;
	if (FAILED(D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, Fullscreen)))
	{
		zeCriticalError("D3D9 Module", "Device does not support Device Type.");
		Destroy();
		return false;
	}

	UINT AdapterToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
	
	// If perfhud debugging enabled (by defining ZE_DEBUG_D3D9_PERFHUD) replace default adapter with perfhud adapter
	#ifdef ZE_DEBUG_D3D9_PERFHUD
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
		zeCriticalError("D3D9 Module", "Can not create Direct3D Device.");
		Destroy();
		return false;
	}

	D3D9Device = Device;

	zeLog("D3D9 Module", "Device Created.");

	// Check hardware capabilities
	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps);
		
	// Check vertex shader versions.
	PipelineType = ZE_D3D9_PT_PROGRAMABLE;

	zeLog("D3D9 Module", "Vertex Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(DeviceCaps.VertexShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion))
	{
		case 1:
			zeWarning("D3D9 Module", "Vertex shader model 1.x is not supported.");
			break;
		case 2:
			if (D3DXGetVertexShaderProfile(Device) == "vs_2_a")
			{
				zeLog("D3D9 Module", "Vertex shader profile 2.0a found.");
				VertexShaderVersion = ZE_D3D9_VSV_2_0_A;
			}
			else
				VertexShaderVersion = ZE_D3D9_VSV_2_0_0;
			break;
		case 3:
			VertexShaderVersion = ZE_D3D9_VSV_3_0_0;
			break;
		default:
			zeWarning("D3D9 Module", "No pixel shader support present.");
			VertexShaderVersion = ZE_D3D9_VSV_NONE;
			break;
	};

	// Check pixel shader version
	zeLog("D3D9 Module", "Pixel Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(DeviceCaps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(DeviceCaps.PixelShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(DeviceCaps.PixelShaderVersion))
	{
		case 1:
			zeWarning("D3D9 Module", "Pixel shader model 1.x is not supported.");
			break;
		case 2:
			if (D3DXGetPixelShaderProfile(Device) == "ps_2_a")
			{
				zeLog("D3D9 Module", "Pixel shader profile 2.0a found.");
				PixelShaderVersion = ZE_D3D9_PSV_2_0_A;
			}
			else if (D3DXGetPixelShaderProfile(Device) == "ps_2_b")
			{
				zeLog("D3D9 Module", "Pixel shader profile 2.0a found.");
				PixelShaderVersion = ZE_D3D9_PSV_2_0_A;
			}
			else
				PixelShaderVersion = ZE_D3D9_PSV_2_0_0;
			break;
		case 3:
			PixelShaderVersion = ZE_D3D9_PSV_3_0_0;
			break;
		default:
			zeWarning("D3D9 Module", "No pixel shader support present.");
			PixelShaderVersion = ZE_D3D9_PSV_NONE;
			break;
	};

	// If pixel shader or vertex shader is not available fall back to fixed pipeline
	if (PixelShaderVersion == ZE_D3D9_PSV_NONE || VertexShaderVersion == ZE_D3D9_VSV_NONE)
		PipelineType = ZE_D3D9_PT_FIXED_FUNCTION;

	// If falled back to fixed pipeline give warning to the user
	if (PipelineType == ZE_D3D9_PT_FIXED_FUNCTION)
	{
		zeWarning("D3D9 Module", "Programable shaders are not supported. Falling back to fixed pipeline.");
		zeWarning("D3D9 Module", "Fixed pipeline does not have necessery capabilities to render most of the visual so visual quality will be disimprove dramatically.");
	}

	// Get screen's back buffer
	Result = Device->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &FrameColorBuffer);
	if(FAILED(Result)) 
	{
		zeCriticalError("D3D9 Module", "Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	// Get screen's z buffer
	Result = Device->GetDepthStencilSurface(&FrameZBuffer);
	if(FAILED(Result)) 
	{
		zeCriticalError("D3D9 Module", "Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	this->ClearFrameBuffer();

	// Initialize component base
	if (!ZED3D9ComponentBase::BaseInitialize(this))
	{
		zeCriticalError("D3D9 Module", "Can not initialize D3D9 component base.");
		return false;
	}

	// Initialize fixed material shader
	if (ZED3D9FixedMaterialShader::BaseInitialize() == false)
	{
		zeCriticalError("D3D9 Module", "Can not initialize shader manager.");
		Destroy();
		return false;
	}

	/*if (!ZED3D9PostProcessor::BaseInitialize())
	{
		zeCriticalError("D3D9 Module", "Can not initialize D3D9 component base.");
		return false;
	}*/

	// Initialize shadow renderer
	if (!ZED3D9ShadowRenderer::BaseInitialize())
	{
		zeCriticalError("D3D9 Module", "Can not initialize D3D9 shadow renderer base.");
		return false;
	}

	return true;
}

void ZED3D9Module::Deinitialize()
{
	zeOutput("Destroying Direct3D.\r\n");
	ZED3D9ShadowRenderer::BaseDeinitialize();
	//ZED3D9PostProcessor::BaseDeinitialize();
	ZED3D9FixedMaterialShader::BaseDeinitialize();
	D3D9Device = NULL;
	D3D9Module = NULL;

	if (FrameColorBuffer != NULL)
	{
		FrameColorBuffer->Release();
		FrameColorBuffer = NULL;
	}

	if (FrameZBuffer != NULL)
	{
		FrameZBuffer->Release();
		FrameZBuffer = NULL;
	}

	if (Device != NULL)
	{
		Device->Release();
		Device = NULL;
	}

	if (D3D != NULL)
	{
		D3D->Release();
		D3D = NULL;
	}
}

bool ZED3D9Module::IsDeviceLost()
{
	return DeviceLostState;
}

void ZED3D9Module::DeviceLost()
{
	DeviceLostState = true;
	for (size_t I = 0; I < Texture2Ds.GetCount(); I++)
		Texture2Ds[I]->DeviceLost();

	for (size_t I = 0; I < Texture3Ds.GetCount(); I++)
		Texture3Ds[I]->DeviceLost();

	for (size_t I = 0; I < TextureCubes.GetCount(); I++)
		TextureCubes[I]->DeviceLost();

/*	for (size_t I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceLost();

	for (size_t I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceLost();
	
	for (size_t I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceRestored();*/

	for (size_t I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceLost();
	
	for (size_t I = 0; I < TextureRenderers.GetCount(); I++)
		TextureRenderers[I]->DeviceLost();

	for (size_t I = 0; I < ShadowRenderers.GetCount(); I++)
		ShadowRenderers[I]->DeviceLost();

	FrameColorBuffer->Release();
	FrameColorBuffer = NULL;
	FrameZBuffer->Release();
	FrameZBuffer = NULL;
}

void ZED3D9Module::DeviceRestored()
{
	DeviceLostState = false;

	Device->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &FrameColorBuffer);
	Device->GetDepthStencilSurface(&FrameZBuffer);

	for (size_t I = 0; I < Texture2Ds.GetCount(); I++)
		Texture2Ds[I]->DeviceRestored();

	for (size_t I = 0; I < Texture3Ds.GetCount(); I++)
		Texture3Ds[I]->DeviceRestored();

	for (size_t I = 0; I < TextureCubes.GetCount(); I++)
		TextureCubes[I]->DeviceRestored();

	/*for (size_t I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceRestored();*/

	/*for (size_t I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceRestored();

	for (size_t I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceRestored();*/

	for (size_t I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceRestored();

	for (size_t I = 0; I < TextureRenderers.GetCount(); I++)
		TextureRenderers[I]->DeviceRestored();

	for (size_t I = 0; I < ShadowRenderers.GetCount(); I++)
		ShadowRenderers[I]->DeviceRestored();
}

void ZED3D9Module::RestoreDevice(bool ForceReset)
{
	DeviceLost();
	HRESULT DeviceState, Hr;
	do
	{
		Sleep(100);
		DeviceState = Device->TestCooperativeLevel();

		if (DeviceState == D3DERR_DRIVERINTERNALERROR)
			zeCriticalError("Direct3D9", "Can not restore Direct3D Device. Internal driver error.");

		if (DeviceState == D3DERR_DEVICENOTRESET || ForceReset)
		{
			HRESULT hr =Device->Reset(&D3DPP);
			if (hr == D3D_OK)
			{
				zeOutput("Direct3D Device Restored.\r\n");
				break;
			}
			else if (hr == D3DERR_DEVICELOST)
				continue;
			else
				zeCriticalError("Direct3D9", "Can not restore Direct3D Device.");

		}
	}
	while (DeviceState == D3DERR_DEVICELOST);
	DeviceRestored();
}

void ZED3D9Module::SetScreenSize(int Width, int Height)
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

void ZED3D9Module::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
	if (Device != NULL)
	{
		D3DPP.PresentationInterval = (VerticalSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE);
		RestoreDevice(true);
	}
}

void ZED3D9Module::SetShaderQuality(int Quality)
{
}

void ZED3D9Module::SetTextureQuality(int Quality)
{
}

void ZED3D9Module::SetModelQuality(int Quality)
{
}

void ZED3D9Module::SetShadowQuality(int Quality)
{
}

void ZED3D9Module::SetPostEffectQuality(int Quality)
{
}

void ZED3D9Module::SetHDRQuality(int Quality)
{
}

void ZED3D9Module::SetAntiAliasing(int Level)
{
	AntiAliasing = Level;
	if (Device != NULL)
	{
		D3DPP.MultiSampleType = (D3DMULTISAMPLE_TYPE)(AnisotropicFilter * 2);
		RestoreDevice(true);
	}
}

void ZED3D9Module::SetAnisotropicFilter(int Level)
{
	AnisotropicFilter = Level;
	for (DWORD I = 0; I < DeviceCaps.MaxSimultaneousTextures; I++)
	{
		Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(I, D3DSAMP_MINFILTER, (AnisotropicFilter != 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		if (AnisotropicFilter != 0)
			Device->SetSamplerState(I, D3DSAMP_MAXANISOTROPY, (AnisotropicFilter * 2 > DeviceCaps.MaxAnisotropy ? DeviceCaps.MaxAnisotropy : 2 * AnisotropicFilter));
	}
}

void ZED3D9Module::SetMaterialComponentMask(unsigned int Mask)
{
	#pragma message("Task : Implament ZED3D9Module::SetMaterialComponentMask()")
}

unsigned int ZED3D9Module::GetMaterialComponentMask()
{
	#pragma message("Task : Implament ZED3D9Module::GetMaterialComponentMask()")
	return 0xFFFFFFFF;
}

ZEFrameBufferRenderer* ZED3D9Module::CreateFrameBufferRenderer()
{
	ZED3D9FrameBufferRenderer* Renderer = new ZED3D9FrameBufferRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	Renderers.Add(Renderer);
	return Renderer;
}

ZETextureRenderer* ZED3D9Module::CreateTextureRenderer()
{
	ZED3D9TextureRenderer* Renderer = new ZED3D9TextureRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	TextureRenderers.Add(Renderer);
	return Renderer;
}

ZEShadowRenderer* ZED3D9Module::CreateShadowRenderer()
{
	ZED3D9ShadowRenderer* Renderer = new ZED3D9ShadowRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	ShadowRenderers.Add(Renderer);
	return  Renderer;
}

ZEPostProcessor* ZED3D9Module::CreatePostProcessor()
{
	return new  ZED3D9PostProcessor();
}

ZEVertexDeclaration* ZED3D9Module::CreateVertexDeclaration()
{
	return new ZED3D9VertexDeclaration();
}

void ZED3D9Module::UpdateScreen()
{
	if (IsDeviceLost())
		return;

	if (Device->TestCooperativeLevel() != D3D_OK)
		RestoreDevice();

	if (!IsDeviceLost())
		Device->Present(NULL, NULL, NULL, NULL);
}
 
void ZED3D9Module::ClearFrameBuffer()
{
	if (IsDeviceLost())
		return;

	if (Device->TestCooperativeLevel() != D3D_OK)
		RestoreDevice();

	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1, 0);
}

ZEStaticVertexBuffer* ZED3D9Module::CreateStaticVertexBuffer()
{
	ZED3D9StaticVertexBuffer* VertexBuffer = new ZED3D9StaticVertexBuffer();
	VertexBuffers.Add(VertexBuffer);
	return VertexBuffer;
}

ZETexture2D* ZED3D9Module::CreateTexture2D()
{
	ZED3D9Texture2D* Texture = new ZED3D9Texture2D();
	Texture2Ds.Add(Texture);
	return Texture;
}

ZETexture3D* ZED3D9Module::CreateTexture3D()
{
	ZED3D9Texture3D* Texture = new ZED3D9Texture3D();
	Texture3Ds.Add(Texture);
	return Texture;
}

ZETextureCube* ZED3D9Module::CreateTextureCube()
{
	ZED3D9TextureCube* Texture = new ZED3D9TextureCube();
	TextureCubes.Add(Texture);
	return Texture;
}

ZEFixedMaterial* ZED3D9Module::CreateFixedMaterial()
{
	return new ZED3D9FixedMaterial();
}

ZEUIMaterial* ZED3D9Module::CreateUIMaterial()
{
	return new ZED3D9UIMaterial();
}

ZESimpleMaterial* ZED3D9Module::CreateSimpleMaterial()
{
	return new ZED3D9SimpleMaterial();
}

ZEFixedMaterial* ZED3D9Module::CreateCustomMaterial()
{
	zeError("Direct3D9 Module", "Custom Materials are not implamented.");
	return NULL;
}

ZEFixedMaterial* ZED3D9Module::CreateCGFXMaterial()
{
	zeError("Direct3D9 Module", "CGFX Materials are not implamented.");
	return NULL;
}

LPDIRECT3DDEVICE9 ZED3D9Module::GetD3D9Device()
{
	return D3D9Device;
}


ZED3D9Module* ZED3D9Module::GetD3D9Module()
{
	return D3D9Module;
}

ZED3D9Module::ZED3D9Module()
{
	DeviceLostState = false;
	D3D9Device = NULL;
	D3D = NULL;
	Device = NULL;
	FrameColorBuffer = NULL;
	FrameZBuffer = NULL;
}

ZED3D9Module::~ZED3D9Module()
{
	Deinitialize();
}
