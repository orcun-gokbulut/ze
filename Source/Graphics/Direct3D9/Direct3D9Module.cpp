//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Direct3D9Module.cpp
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
#ifdef ZEDEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif

#include "Direct3D9Module.h"
#include "Direct3D9ModuleDescription.h"
#include "Core/Window.h"
#include "Core/Error.h"
#include "Core/Console.h"
#include "D3D9FixedMaterial.h"
#include "D3D9Shader.h"
#include "D3D9Texture.h"
#include "D3D9VertexBuffer.h"
#include "D3D9PostProcessor.h"
#include <d3dx9.h>

LPDIRECT3DDEVICE9 D3D9Device;
ZEDirect3D9Module* D3D9Module;
D3DPRESENT_PARAMETERS D3DPP;

#pragma warning(disable:4267)

ZEModuleDescription* ZEDirect3D9Module::GetModuleDescription()
{
	static ZEDirect3D9ModuleDescription Desc;
	return &Desc;
}
bool ZEDirect3D9Module::IsEnabled()
{
	return Enabled;
}

void ZEDirect3D9Module::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEDirect3D9Module::Initialize()
{
	zeLog("Initializing Direct3D 9.\r\n");

	ScreenWidth = zeOptions->GetOption("Graphics", "ScreenWidth")->GetValue().GetInteger();
	ScreenHeight = zeOptions->GetOption("Graphics", "ScreenHeight")->GetValue().GetInteger();
	Fullscreen = zeOptions->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean();
	FarZ = zeOptions->GetOption("Graphics", "FarZ")->GetValue().GetFloat();
	NearZ = zeOptions->GetOption("Graphics", "NearZ")->GetValue().GetFloat();
	VerticalSync = zeOptions->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	AnisotropicFilter = zeOptions->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInteger();

	HRESULT Result;
	
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
	
	D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!D3D)
	{
		zeCriticalError("Direct3D Module", "Can not create Direct3D.");
		Destroy();
		return false;
	}

	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &D3DCaps);

	if (D3DCaps.MaxVertexShader30InstructionSlots == 0)
	{
		if (D3DCaps.VertexShaderVersion != 0)
			ShaderModel = 2;
		else
		{
			zeCriticalError("Direct3D Module", "Device does not support Shader Model 2 or 3.");
			Destroy();
			return false;
		}
	}
	else
		ShaderModel = 3;

	if (FAILED(D3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, Fullscreen)))
	{
		zeCriticalError("Direct3D Module", "Device does not support Device Type.");
		Destroy();
		return false;
	}
	UINT AdapterToUse = D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;
	
	#ifdef ZEDEBUG_PERFHUD
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
	
	Result = D3D->CreateDevice(AdapterToUse, 
				DeviceType, 
				(HWND)zeWindow->GetHandle(), 
				D3DCREATE_HARDWARE_VERTEXPROCESSING, 
				&D3DPP, 
				&Device);
	if(FAILED(Result)) 
	{
		zeCriticalError("Direct3D Module", "Can not create Direct3D Device.");
		Destroy();
		return false;
	}

	D3D9Device = Device;
	
	Result = Device->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &FrameColorBuffer);
	if(FAILED(Result)) 
	{
		zeCriticalError("Direct3D Module", "Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}


	Result = Device->GetDepthStencilSurface(&FrameZBuffer);
	if(FAILED(Result)) 
	{
		zeCriticalError("Direct3D Module", "Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	this->ClearFrameBuffer();
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	
	if (!ZED3D9ComponentBase::BaseInitialize(this))
	{
		zeCriticalError("Direct3D Module", "Can not initialize D3D9 component base.");
		return false;
	}

	if (ZED3D9Shader::BaseInitialize() == false)
	{
		zeCriticalError("Direct3D Module", "Can not initialize shader manager.");
		Destroy();
		return false;
	}

	if (!ZED3D9PostProcessor::BaseInitialize())
	{
		zeCriticalError("Direct3D Module", "Can not initialize D3D9 component base.");
		return false;
	}

	if (!ZED3D9ShadowRenderer::BaseInitialize())
	{
		zeCriticalError("Direct3D Module", "Can not initialize D3D9 shadow renderer base.");
		return false;
	}

	//ZED3D9ComponentBase::D3
	return true;
}

void ZEDirect3D9Module::Deinitialize()
{
	zeLog("Destroying Direct3D.\r\n");
	ZED3D9ShadowRenderer::BaseDeinitialize();
	ZED3D9PostProcessor::BaseDeinitialize();
	ZED3D9Shader::BaseDeinitialize();
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

void ZEDirect3D9Module::DeviceLost()
{
	IsDeviceLost = true;
	for (size_t I = 0; I < Textures.GetCount(); I++)
		Textures[I]->DeviceLost();

	for (size_t I = 0; I < VolumeTextures.GetCount(); I++)
		VolumeTextures[I]->DeviceLost();

	for (size_t I = 0; I < CubeTextures.GetCount(); I++)
		CubeTextures[I]->DeviceLost();

/*	for (size_t I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceLost();

	for (size_t I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceLost();*/

	for (size_t I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceLost();

	for (size_t I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceLost();

	FrameColorBuffer->Release();
	FrameColorBuffer = NULL;
	FrameZBuffer->Release();
	FrameZBuffer = NULL;
}

void ZEDirect3D9Module::DeviceRestored()
{
	IsDeviceLost = false;

	Device->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &FrameColorBuffer);
	Device->GetDepthStencilSurface(&FrameZBuffer);

	for (size_t I = 0; I < Textures.GetCount(); I++)
		Textures[I]->DeviceRestored();

	for (size_t I = 0; I < VolumeTextures.GetCount(); I++)
		VolumeTextures[I]->DeviceRestored();

	for (size_t I = 0; I < CubeTextures.GetCount(); I++)
		CubeTextures[I]->DeviceRestored();

/*	for (size_t I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceRestored();

	for (size_t I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceRestored();*/

	for (size_t I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceRestored();

	for (size_t I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceRestored();

}

void ZEDirect3D9Module::RestoreDevice(bool ForceReset)
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
				zeLog("Direct3D Device Restored.\r\n");
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

void ZEDirect3D9Module::SetScreenSize(int Width, int Height)
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

void ZEDirect3D9Module::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
	if (Device != NULL)
	{
		D3DPP.PresentationInterval = (VerticalSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE);
		RestoreDevice(true);
	}
}

void ZEDirect3D9Module::SetShaderQuality(int Quality)
{
}

void ZEDirect3D9Module::SetTextureQuality(int Quality)
{
}

void ZEDirect3D9Module::SetModelQuality(int Quality)
{
}

void ZEDirect3D9Module::SetShadowQuality(int Quality)
{
}

void ZEDirect3D9Module::SetPostEffectQuality(int Quality)
{
}

void ZEDirect3D9Module::SetHDRQuality(int Quality)
{
}

void ZEDirect3D9Module::SetAntiAliasing(int Level)
{
	AntiAliasing = Level;
	if (Device != NULL)
	{
		D3DPP.MultiSampleType = (D3DMULTISAMPLE_TYPE)(AnisotropicFilter * 2);
		RestoreDevice(true);
	}
}

void ZEDirect3D9Module::SetAnisotropicFilter(int Level)
{
	AnisotropicFilter = Level;
	for (DWORD I = 0; I < D3DCaps.MaxSimultaneousTextures; I++)
	{
		Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(I, D3DSAMP_MINFILTER, (AnisotropicFilter != 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		if (AnisotropicFilter != 0)
			Device->SetSamplerState(I, D3DSAMP_MAXANISOTROPY, (AnisotropicFilter * 2 > D3DCaps.MaxAnisotropy ? D3DCaps.MaxAnisotropy : 2 * AnisotropicFilter));
	}
}

void ZEDirect3D9Module::SetMaterialComponentMask(unsigned int Mask)
{
	#pragma message("Task : Implament ZEDirect3D9Module::SetMaterialComponentMask()")
}

unsigned int ZEDirect3D9Module::GetMaterialComponentMask()
{
	#pragma message("Task : Implament ZEDirect3D9Module::GetMaterialComponentMask()")
	return 0xFFFFFFFF;
}

ZERenderer* ZEDirect3D9Module::CreateFrameRenderer()
{
	ZED3D9RendererBase* Renderer = new ZED3D9FrameBufferRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	Renderers.Add(Renderer);
	return Renderer;
}

ZERenderer* ZEDirect3D9Module::CreateTextureRenderer()
{
	ZED3D9RendererBase* Renderer = new ZED3D9TextureRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	Renderers.Add(Renderer);
	return Renderer;
}

ZERenderer* ZEDirect3D9Module::CreateShadowRenderer()
{
	ZED3D9RendererBase* Renderer = new ZED3D9ShadowRenderer();
	if (!Renderer->Initialize())
	{
		Renderer->Destroy();
		return NULL;
	}

	Renderers.Add(Renderer);
	return  Renderer;
}

ZEPostProcessor* ZEDirect3D9Module::CreatePostProcessor()
{
	return new ZED3D9PostProcessor;
}

ZEVertexDeclaration* ZEDirect3D9Module::CreateVertexDeclaration()
{
	return new ZED3D9VertexDeclaration();
}

void ZEDirect3D9Module::UpdateScreen()
{
	if (IsDeviceLost)
		return;

	if (Device->TestCooperativeLevel() != D3D_OK)
		RestoreDevice();

	if (!IsDeviceLost)
		Device->Present(NULL, NULL, NULL, NULL);
}
 
void ZEDirect3D9Module::ClearFrameBuffer()
{
	if (IsDeviceLost)
		return;

	if (Device->TestCooperativeLevel() != D3D_OK)
		RestoreDevice();

	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1, 0);
}

ZEStaticVertexBuffer* ZEDirect3D9Module::CreateStaticVertexBuffer()
{
	ZED3D9StaticVertexBuffer* VertexBuffer = new ZED3D9StaticVertexBuffer();
	VertexBuffers.Add(VertexBuffer);
	return VertexBuffer;
}

ZETexture2D* ZEDirect3D9Module::CreateTexture()
{
	ZED3D9Texture* Texture = new ZED3D9Texture();
	Textures.Add(Texture);
	return Texture;
}

ZETexture3D* ZEDirect3D9Module::CreateVolumeTexture()
{
	ZED3D9VolumeTexture* Texture = new ZED3D9VolumeTexture();
	VolumeTextures.Add(Texture);
	return Texture;
}

ZETextureCube* ZEDirect3D9Module::CreateCubeTexture()
{
	ZED3D9CubeTexture* Texture = new ZED3D9CubeTexture();
	CubeTextures.Add(Texture);
	return Texture;
}

ZEFixedMaterial* ZEDirect3D9Module::CreateFixedMaterial()
{
	return new ZED3D9FixedMaterial();
}

ZEFixedMaterial* ZEDirect3D9Module::CreateCustomMaterial()
{
	zeError("Direct3D9 Module", "Custom Materials are not implamented.");
	return NULL;
}

ZEFixedMaterial* ZEDirect3D9Module::CreateCGFXMaterial()
{
	zeError("Direct3D9 Module", "CGFX Materials are not implamented.");
	return NULL;
}

LPDIRECT3DDEVICE9 ZEDirect3D9Module::GetD3D9Device()
{
	return D3D9Device;
}


ZEDirect3D9Module* ZEDirect3D9Module::GetD3D9Module()
{
	return D3D9Module;
}

ZEDirect3D9Module::ZEDirect3D9Module()
{
	IsDeviceLost = false;
	D3D9Device = NULL;
	D3D = NULL;
	Device = NULL;
	FrameColorBuffer = NULL;
	FrameZBuffer = NULL;
}

ZEDirect3D9Module::~ZEDirect3D9Module()
{
	Destroy();
}
