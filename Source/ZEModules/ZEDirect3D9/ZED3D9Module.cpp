//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Module.cpp
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
#include "ZED3D9Module.h"
#include "ZECore/ZEWindow.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEOptionManager.h"
#include "ZED3D9FixedMaterial.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Texture3D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9VertexDeclaration.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9ShadowRenderer.h"
#include "ZED3D9UIMaterial.h"
#include "ZED3D9SimpleMaterial.h"
#include "ZED3D9SkyBoxMaterial.h"
#include "ZED3D9MoonMaterial.h"
#include "ZED3D9ShaderManager.h"
#include "ZED3D9TerrainMaterial.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9IndexBuffer.h"

#include <d3dx9.h>
#include "ZED3D9SkyDomeMaterial.h"
#include "ZED3D9CloudMaterial.h"
#include "ZED3D9SeaMaterial.h"
#include "ZEGraphics/ZESeaMaterial.h"
#include "Logo_png.h"


LPDIRECT3DDEVICE9 D3D9Device;
ZED3D9Module* D3D9Module;
D3DPRESENT_PARAMETERS D3DPP;

#pragma warning(disable:4267)

ZE_MODULE_DESCRIPTION(ZED3D9Module, ZEGraphicsModule, NULL)

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

bool ZED3D9Module::GetEnabled()
{
	return Enabled;
}

void ZED3D9Module::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

static void DrawRect(LPDIRECT3DDEVICE9 Device, float Left, float Right, float Top, float Bottom, LPDIRECT3DTEXTURE9 Texture, D3DCOLOR Color)
{
	Left -= 0.5f;
	Right -= 0.5f;
	Top -= 0.5f;
	Bottom -= 0.5f;

	struct Vertex
	{
		float x, y, z, w;
		D3DCOLOR Color;
		float u,v;
	} 
	Vertices[6] =
	{
		{Left,	Bottom,	1.0f, 1.0f, Color, 0.0f, 1.0f},
		{Left,	Top,	1.0f, 1.0f, Color, 0.0f, 0.0f},
		{Right, Top,	1.0f, 1.0f, Color, 1.0f, 0.0f},
		{Left,	Bottom,	1.0f, 1.0f, Color, 0.0f, 1.0f},
		{Right, Top,	1.0f, 1.0f, Color, 1.0f, 0.0f},
		{Right, Bottom,	1.0f, 1.0f, Color, 1.0f, 1.0f}
	};

	Device->SetTexture(0, Texture);
	Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	Device->SetTextureStageState(0 ,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	Device->SetTextureStageState(0, D3DTSS_CONSTANT, Color);
	
	Device->SetVertexShader(NULL);
	Device->SetPixelShader(NULL);
	Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, Vertices, sizeof(Vertex));
}

void ZED3D9Module::DrawLogo()
{
	Logo_png Data;
	LPDIRECT3DTEXTURE9 Logo;
	D3DXCreateTextureFromFileInMemoryEx(GetDevice(), Data.GetData(), Data.GetSize(),
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_DEFAULT , D3DX_DEFAULT , 0, NULL, NULL, &Logo);
	
	D3DSURFACE_DESC LogoDesc;
	Logo->GetLevelDesc(0, &LogoDesc);

	float Left = (FrameBufferViewPort.GetWidth() - LogoDesc.Width) / 2;
	float Right = Left + LogoDesc.Width;
	float Top = (FrameBufferViewPort.GetHeight() - LogoDesc.Height) / 2;
	float Bottom = Top + LogoDesc.Height;

	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	GetDevice()->Present(NULL, NULL, NULL, NULL);
	Sleep(200);

	for (int I = 0; I < 256; I+=8)
	{
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		GetDevice()->BeginScene();
		DrawRect(GetDevice(), Left, Right, Top, Bottom, Logo, 0x01010101 * I);
		GetDevice()->EndScene();
		GetDevice()->Present(NULL, NULL, NULL, NULL);
		Sleep(20);
	}

	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	GetDevice()->BeginScene();
	DrawRect(GetDevice(), Left, Right, Top, Bottom, Logo, 0xFFFFFFFF);
	GetDevice()->EndScene();
	GetDevice()->Present(NULL, NULL, NULL, NULL);

	Logo->Release();
}

bool ZED3D9Module::InitializeSelf()
{
	zeLog("Initializing Direct3D 9 module.");

	if (!ZEGraphicsModule::InitializeSelf())
		return false;

	// Read options
	ScreenWidth = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenWidth")->GetValue().GetInt32();
	ScreenHeight = ZEOptionManager::GetInstance()->GetOption("Graphics", "ScreenHeight")->GetValue().GetInt32();
	Fullscreen = ZEOptionManager::GetInstance()->GetOption("Graphics", "Fullscreen")->GetValue().GetBoolean();
	FarZ = ZEOptionManager::GetInstance()->GetOption("Graphics", "FarZ")->GetValue().GetFloat();
	NearZ = ZEOptionManager::GetInstance()->GetOption("Graphics", "NearZ")->GetValue().GetFloat();
	VerticalSync = ZEOptionManager::GetInstance()->GetOption("Graphics", "VerticalSync")->GetValue().GetBoolean();
	AnisotropicFilter = ZEOptionManager::GetInstance()->GetOption("Graphics", "AnisotropicFilter")->GetValue().GetInt32();
	TextureQuality = (ZETextureQuality)ZEOptionManager::GetInstance()->GetOption("Graphics", "TextureQuality")->GetValue().GetInt32();

	HRESULT Result;
	
	// Create direct 3d device creating parameters
	ZeroMemory(&D3DPP, sizeof(D3DPRESENT_PARAMETERS));
	D3DPP.BackBufferCount = 1;
	D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	D3DPP.BackBufferHeight = (Fullscreen ?  ScreenHeight : 0);
	D3DPP.BackBufferWidth  = (Fullscreen ?  ScreenWidth : 0);

	D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	D3DPP.MultiSampleQuality = 0;

	// D3DPP.EnableAutoDepthStencil = true;
	D3DPP.EnableAutoDepthStencil = false;
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
	
	// If perfhud debugging enabled (by defining ZE_DEBUG_D3D9_PERFHUD) replace default adapter with perfhud adapter
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

	D3D9Device = Device;

	// Initialize component base
	if (!ZED3D9ComponentBase::BaseInitialize(this))
	{
		zeCriticalError("Can not initialize D3D9 component base.");
		return false;
	}

	zeLog("Device Created.");

	// Check hardware capabilities
	D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps);
		
	// Check vertex shader versions.
	PipelineType = ZE_D3D9_PT_PROGRAMABLE;

	zeLog("Vertex Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion), D3DSHADER_VERSION_MINOR(DeviceCaps.VertexShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(DeviceCaps.VertexShaderVersion))
	{
		case 1:
			zeWarning("Vertex shader model 1.x is not supported.");
			break;
		case 2:
			if (strcmp(D3DXGetVertexShaderProfile(Device), "vs_2_a") == 0)
			{
				zeLog("Vertex shader profile 2.0a found.");
				VertexShaderVersion = ZE_D3D9_VSV_2_0_A;
			}
			else
				VertexShaderVersion = ZE_D3D9_VSV_2_0_0;
			break;
		case 3:
			VertexShaderVersion = ZE_D3D9_VSV_3_0_0;
			break;
		default:
			zeWarning("No pixel shader support present.");
			VertexShaderVersion = ZE_D3D9_VSV_NONE;
			break;
	};

	// Check pixel shader version
	zeLog("Pixel Shader Version : %d.%d.", D3DSHADER_VERSION_MAJOR(DeviceCaps.PixelShaderVersion), D3DSHADER_VERSION_MINOR(DeviceCaps.PixelShaderVersion));
	switch(D3DSHADER_VERSION_MAJOR(DeviceCaps.PixelShaderVersion))
	{
		case 1:
			zeWarning("Pixel shader model 1.x is not supported.");
			break;
		case 2:
			if (strcmp(D3DXGetPixelShaderProfile(Device), "ps_2_a") == 0)
			{
				zeLog("Pixel shader profile 2.0a found.");
				PixelShaderVersion = ZE_D3D9_PSV_2_0_A;
			}
			else if (strcmp(D3DXGetPixelShaderProfile(Device), "ps_2_b") == 0)
			{
				zeLog("Pixel shader profile 2.0a found.");
				PixelShaderVersion = ZE_D3D9_PSV_2_0_A;
			}
			else
				PixelShaderVersion = ZE_D3D9_PSV_2_0_0;
			break;
		case 3:
			PixelShaderVersion = ZE_D3D9_PSV_3_0_0;
			break;
		default:
			zeWarning("No pixel shader support present.");
			PixelShaderVersion = ZE_D3D9_PSV_NONE;
			break;
	};

	// If pixel shader or vertex shader is not available fall back to fixed pipeline
	if (PixelShaderVersion == ZE_D3D9_PSV_NONE || VertexShaderVersion == ZE_D3D9_VSV_NONE)
		PipelineType = ZE_D3D9_PT_FIXED_FUNCTION;

	// If falled back to fixed pipeline give warning to the user
	if (PipelineType == ZE_D3D9_PT_FIXED_FUNCTION)
	{
		zeWarning("Programable shaders are not supported. Falling back to fixed pipeline.");
		zeWarning("Fixed pipeline does not have necessery capabilities to render most of the visual so visual quality will be disimprove dramatically.");
	}

	// Get screen's back buffer
	Result = Device->GetBackBuffer(0, 0,D3DBACKBUFFER_TYPE_MONO, &FrameBufferViewPort.FrameBuffer);
	if(FAILED(Result)) 
	{
		zeCriticalError("Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	// Get screen's z buffer
	Result = Device->CreateDepthStencilSurface(ScreenWidth, ScreenHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &FrameBufferViewPort.ZBuffer, NULL);
	if(FAILED(Result))
	{
		zeCriticalError("Can not create Direct3D Backbuffer.");
		Destroy();
		return false;
	}

	DrawLogo();
	
	ShaderManager = new ZED3D9ShaderManager();

	return true;
}

bool ZED3D9Module::DeinitializeSelf()
{
	zeLog("Destroying Direct3D.\r\n");

	D3D9Device = NULL;
	D3D9Module = NULL;

	if (ShaderManager != NULL)
	{
		delete ShaderManager;
		ShaderManager = NULL;
	}

	ZED3D_RELEASE(FrameBufferViewPort.FrameBuffer);
	ZED3D_RELEASE(FrameBufferViewPort.ZBuffer);
	ZED3D_RELEASE(Device);
	ZED3D_RELEASE(D3D);

	return ZEGraphicsModule::DeinitializeSelf();
}

bool ZED3D9Module::IsDeviceLost()
{
	return DeviceLostState;
}

void ZED3D9Module::DeviceLost()
{
	if (IsDeviceLost())
		return;

	for (ZESize I = 0; I < Texture2Ds.GetCount(); I++)
		Texture2Ds[I]->DeviceLost();	

	for (ZESize I = 0; I < Texture3Ds.GetCount(); I++)
		Texture3Ds[I]->DeviceLost();

	for (ZESize I = 0; I < TextureCubes.GetCount(); I++)
		TextureCubes[I]->DeviceLost();

/*	for (ZESize I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceLost();

	for (ZESize I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceLost();
	
	for (ZESize I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceRestored();*/

	for (ZESize I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceLost();
	
	for (ZESize I = 0; I < ShadowRenderers.GetCount(); I++)
		ShadowRenderers[I]->DeviceLost();


	ZED3D_RELEASE(FrameBufferViewPort.FrameBuffer);
	ZED3D_RELEASE(FrameBufferViewPort.ZBuffer);

	DeviceLostState = true;
}

void ZED3D9Module::DeviceRestored()
{
	HRESULT Result = Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &FrameBufferViewPort.FrameBuffer);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot retrive Direct3D backbuffer.");
		return;
	}
	
	// Get screen's z buffer
	Result = Device->CreateDepthStencilSurface(ScreenWidth, ScreenHeight, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &FrameBufferViewPort.ZBuffer, NULL);
	if (FAILED(Result))
	{
		zeCriticalError("Cannot create Direct3D depth stencil buffer.");
		return;
	}

	for (ZESize I = 0; I < Texture2Ds.GetCount(); I++)
		Texture2Ds[I]->DeviceRestored();

	for (ZESize I = 0; I < Texture3Ds.GetCount(); I++)
		Texture3Ds[I]->DeviceRestored();

	for (ZESize I = 0; I < TextureCubes.GetCount(); I++)
		TextureCubes[I]->DeviceRestored();

	/*
	for (ZESize I = 0; I < VertexBuffers.GetCount(); I++)
		VertexBuffers[I]->DeviceRestored();

	for (ZESize I = 0; I < Shaders.GetCount(); I++)
		Shaders[I]->DeviceRestored();

	for (ZESize I = 0; I < PostProcessors.GetCount(); I++)
		PostProcessors[I]->DeviceRestored();
	*/

	for (ZESize I = 0; I < Renderers.GetCount(); I++)
		Renderers[I]->DeviceRestored();

	for (ZESize I = 0; I < ShadowRenderers.GetCount(); I++)
		ShadowRenderers[I]->DeviceRestored();

	DeviceLostState = false;
}

void ZED3D9Module::ResetDevice()
{
	DeviceLost();
	HRESULT Result = Device->Reset(&D3DPP);
	if (Result == D3D_OK)
		DeviceRestored();
}

void ZED3D9Module::CheckAndRestoreDevice()
{
	HRESULT Result = Device->TestCooperativeLevel();
	if (Result == D3DERR_DEVICELOST)
	{
		DeviceLost();
	}
	else if (Result == D3DERR_DEVICENOTRESET)
	{
		if (IsIconic(D3DPP.hDeviceWindow) || !IsWindowVisible(D3DPP.hDeviceWindow))
			return;

		Result = Device->Reset(&D3DPP);
		if (Result == D3D_OK)
			DeviceRestored();
	}
}

void ZED3D9Module::SetScreenSize(ZEInt Width, ZEInt Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
	if (Device != NULL)
	{
		D3DPP.BackBufferWidth = Width;
		D3DPP.BackBufferHeight = Height;
		ResetDevice();
	}
}

void ZED3D9Module::SetVerticalSync(bool Enabled)
{
	VerticalSync = Enabled;
	if (Device != NULL)
	{
		D3DPP.PresentationInterval = (VerticalSync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE);
		ResetDevice();
	}
}

void ZED3D9Module::SetShaderQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetTextureQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetModelQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetShadowQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetPostEffectQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetHDRQuality(ZEInt Quality)
{
}

void ZED3D9Module::SetAntiAliasing(ZEInt Level)
{
	AntiAliasing = Level;
	if (Device != NULL)
	{
		D3DPP.MultiSampleType = (D3DMULTISAMPLE_TYPE)(AnisotropicFilter * 2);
		ResetDevice();
	}
}

void ZED3D9Module::SetAnisotropicFilter(ZEUInt Level)
{
	AnisotropicFilter = Level;
	for (DWORD I = 0; I < DeviceCaps.MaxSimultaneousTextures; I++)
	{
		Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(I, D3DSAMP_MINFILTER, (AnisotropicFilter != 1 ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
		Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		if (AnisotropicFilter != 0)
		{
			Device->SetSamplerState(I, D3DSAMP_MAXANISOTROPY, (AnisotropicFilter * 2 > DeviceCaps.MaxAnisotropy ? DeviceCaps.MaxAnisotropy : 2 * AnisotropicFilter));
		}
	}
}

void ZED3D9Module::SetMaterialComponentMask(ZEUInt Mask)
{
	#pragma message("Task : Implament ZED3D9Module::SetMaterialComponentMask()")
}

ZEUInt ZED3D9Module::GetMaterialComponentMask()
{
	#pragma message("Task : Implament ZED3D9Module::GetMaterialComponentMask()")
	return 0xFFFFFFFF;
}

ZEViewPort* ZED3D9Module::GetFrameBufferViewPort()
{
	return &FrameBufferViewPort;
}

ZED3D9ShaderManager* ZED3D9Module::GetShaderManager()
{
	return ShaderManager;
}

ZEFrameRenderer* ZED3D9Module::CreateFrameRenderer()
{
	ZED3D9FrameRenderer* Renderer = new ZED3D9FrameRenderer();
	Renderers.Add(Renderer);
	return Renderer;
}

ZEShadowRenderer* ZED3D9Module::CreateShadowRenderer()
{
	ZED3D9ShadowRenderer* Renderer = new ZED3D9ShadowRenderer();
	ShadowRenderers.Add(Renderer);
	return Renderer; 
}

ZEPostProcessor* ZED3D9Module::CreatePostProcessor()
{
	return NULL;//new  ZED3D9PostProcessor();
}

ZEVertexDeclaration* ZED3D9Module::CreateVertexDeclaration()
{
	return new ZED3D9VertexDeclaration();
}

void ZED3D9Module::UpdateScreen()
{
	CheckAndRestoreDevice();
	if (IsDeviceLost())
		return;

	Device->Present(NULL, NULL, NULL, NULL);
}
 
void ZED3D9Module::ClearFrameBuffer()
{
 	if (IsDeviceLost())
 		return;
 
 
}

ZEStaticIndexBuffer* ZED3D9Module::CreateStaticIndexBuffer()
{
	ZED3D9StaticIndexBuffer* IndexBuffer = new ZED3D9StaticIndexBuffer();
	IndexBuffers.Add(IndexBuffer);
	return (ZEStaticIndexBuffer*)IndexBuffer;
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

ZEMoonMaterial* ZED3D9Module::CreateMoonMaterial()
{
	return new ZED3D9MoonMaterial();
}

ZEUIMaterial* ZED3D9Module::CreateUIMaterial()
{
	return new ZED3D9UIMaterial();
}

ZECloudMaterial* ZED3D9Module::CreateCloudMaterial()
{
	return new ZED3D9CloudMaterial();
}

ZESimpleMaterial* ZED3D9Module::CreateSimpleMaterial()
{
	return new ZED3D9SimpleMaterial();
}

ZETerrainMaterial* ZED3D9Module::CreateTerrainMaterial()
{
	return new ZED3D9TerrainMaterial();
}

ZESkyBoxMaterial* ZED3D9Module::CreateSkyBoxMaterial()
{
	return new ZED3D9SkyBoxMaterial();
}

ZESkyDomeMaterial* ZED3D9Module::CreateSkyDomeMaterial()
{
	return new ZED3D9SkyDomeMaterial();
}

ZEFixedMaterial* ZED3D9Module::CreateCustomMaterial()
{
	zeError("Custom Materials are not implamented.");
	return NULL;
}

ZEFixedMaterial* ZED3D9Module::CreateCGFXMaterial()
{
	zeError("CGFX Materials are not implamented.");
	return NULL;
}

ZESeaMaterial* ZED3D9Module::CreateSeaMaterial()
{
	return new ZED3D9SeaMaterial();
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
	Enabled = true;
	DeviceLostState = false;
	D3D9Device = NULL;
	D3D = NULL;
	Device = NULL;
	FrameBufferViewPort.FrameBuffer = NULL;
	FrameBufferViewPort.ZBuffer = NULL;
	ShaderManager = NULL;
}

ZED3D9Module::~ZED3D9Module()
{
	Deinitialize();
}
