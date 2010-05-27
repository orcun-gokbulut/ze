//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9FrameBufferRenderer.cpp
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

#include "D3D9FrameBufferRenderer.h"
#include "D3D9RendererBase.h"
#include "D3D9VertexBuffer.h"
#include "D3D9Texture2D.h"
#include "D3D9Texture3D.h"
#include "D3D9TextureCube.h"
#include "D3D9Module.h"
#include "D3D9CommonTools.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9FixedMaterial.h"
#include "Graphics/RenderOrder.h"
#include "Core/Error.h"

#pragma warning(disable:4267)

ZED3D9FrameBufferRenderer::ZED3D9FrameBufferRenderer()
{
	RenderColorTexture = false;
	RenderDepthTexture = false;
	RenderVelocityTexture = false;
	RenderObjectTexture = true;
	HDREnabled = true;

	HDRRenderTarget = NULL;
	ColorRenderTarget = NULL;
	ColorTexture = NULL;
	DepthTexture = NULL;
	VelocityTexture = NULL;
	ObjectTexture = NULL;
}

ZED3D9FrameBufferRenderer::~ZED3D9FrameBufferRenderer()
{
	// Deinitialize renderer
	Deinitialize();
}

bool ZED3D9FrameBufferRenderer::Initialize() 
{ 
	if (HDREnabled)
	{
		// HDR Rendering
		ZED3D9CommonTools::CreateRenderTarget(&HDRRenderTarget, GetModule()->GetScreenWidth(), GetModule()->GetScreenHeight(), ZE_TPF_RGBA_HDR);
		HDRRenderTarget->GetSurfaceLevel(0, &ColorRenderTarget);
		HDRProcessor.Input = HDRRenderTarget;
		HDRProcessor.Target = GetModule()->GetFrameColorBuffer();
		HDRProcessor.Initialize();
	}
	else
	{
		// LDR Rendering
		ColorRenderTarget = GetModule()->GetFrameColorBuffer();
	}
	return true; 
} 

void ZED3D9FrameBufferRenderer::Deinitialize()
{
	HDRProcessor.Deinitialize();

	if (HDRRenderTarget != NULL)
	{
		HDRRenderTarget->Release();
		HDRRenderTarget = NULL;
	}

	if (ColorRenderTarget != NULL)
	{
		ColorRenderTarget->Release();
		ColorRenderTarget = NULL;
	}

	// If color texture is created destroy it
	if (ColorTexture != NULL)
	{
		ColorTexture->Destroy();
		ColorTexture = NULL;
	}

	// If depth texture is created destroy it
	if (DepthTexture != NULL)
	{
		DepthTexture->Destroy();
		DepthTexture = NULL;
	}

	// If velocity texture is created destroy it
	if (VelocityTexture != NULL)
	{
		VelocityTexture->Destroy();
		VelocityTexture = NULL;
	}

	// If object texture is created destroy it
	if (ObjectTexture != NULL)
	{
		ObjectTexture->Destroy();
		ObjectTexture = NULL;
	}
}

void ZED3D9FrameBufferRenderer::SetRenderColorTexture(bool Enable)
{
	RenderColorTexture = Enable;
}

bool ZED3D9FrameBufferRenderer::GetRenderColorTexture()
{
	return RenderColorTexture;
}

void ZED3D9FrameBufferRenderer::SetRenderDepthTexture(bool Enable)
{
	RenderDepthTexture = Enable;
}

bool ZED3D9FrameBufferRenderer::GetRenderDepthTexture()
{
	return RenderDepthTexture;
}

void ZED3D9FrameBufferRenderer::SetRenderVelocityTexture(bool Enable)
{
	RenderVelocityTexture = Enable;
}

bool ZED3D9FrameBufferRenderer::GetRenderVelocityTexture()
{
	return RenderVelocityTexture;
}

void ZED3D9FrameBufferRenderer::SetRenderObjectTexture(bool Enable)
{
	RenderObjectTexture = Enable;
}

bool ZED3D9FrameBufferRenderer::GetRenderObjectTexture()
{
	return RenderObjectTexture;
}

ZETexture2D* ZED3D9FrameBufferRenderer::GetColorTexture()
{
	return ColorTexture;
}

ZETexture2D* ZED3D9FrameBufferRenderer::GetDepthTexture()
{
	return DepthTexture;
}

ZETexture2D* ZED3D9FrameBufferRenderer::GetVelociyTexture()
{
	return VelocityTexture;
}

ZETexture2D* ZED3D9FrameBufferRenderer::GetObjectTexture()
{
	return ObjectTexture;
}

void ZED3D9FrameBufferRenderer::DeviceLost()
{
	// GetDevice() is lost so recreate hardware resources
}

bool ZED3D9FrameBufferRenderer::DeviceRestored()
{
	return true;
}

void ZED3D9FrameBufferRenderer::Destroy()
{
	// Remove renderer from modules renderer list
	GetModule()->Renderers.DeleteValue((ZED3D9FrameBufferRenderer*)this);
	delete this;
}

void ZED3D9FrameBufferRenderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZED3D9FrameBufferRenderer::GetCamera()
{
	return Camera;
}

ZEArray<ZEPostProcessor*>& ZED3D9FrameBufferRenderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9FrameBufferRenderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9FrameBufferRenderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}

void ZED3D9FrameBufferRenderer::AddToRenderList(ZERenderOrder* RenderOrder)
{
	#ifdef ZE_DEBUG_ENABLED
		// Check render order is valid
		if (!ZED3D9RendererBase::CheckRenderOrder(RenderOrder))
			return;
	#endif

	// Add render orders to render order lists according to their properties
	if (RenderOrder->Flags & ZE_ROF_IMPOSTER)
		Imposter.Add(*RenderOrder);
	if (RenderOrder->Flags & ZE_ROF_TRANSPARENT)
		Transparent.Add(*RenderOrder);
	else
		NonTransparent.Add(*RenderOrder);
}

void ZED3D9FrameBufferRenderer::ClearList()
{
	//Clear render lists
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9FrameBufferRenderer::Render(float ElaspedTime)
{
	unsigned int ScreenWidth = zeGraphics->GetScreenWidth();
	unsigned int ScreenHeight = zeGraphics->GetScreenHeight();

	// Check render module is enabled and the device is not lost
	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost())
		return;

	// Check if there is a post process attached to render order
	if (PostProcessors.GetCount() != NULL)
	{
		// If post process is attached then create off screen render targets

	}
	else
	{
		// If there is no post process attached then use directly frame buffer
		GetDevice()->SetRenderTarget(0, GetModule()->GetFrameColorBuffer());
		GetDevice()->SetDepthStencilSurface(GetModule()->GetFrameZBuffer());
	}

	GetDevice()->SetRenderTarget(0, ColorRenderTarget);
	GetDevice()->SetDepthStencilSurface(GetModule()->GetFrameZBuffer());


	// If renderer will give output to other mechanisms like post processors. It will enable render color buffer.
	// Creating color texture if Render Color Texture enabled.
	if (RenderColorTexture)
		// Checking older texture available
		if (ColorTexture != NULL)
		{
			// Checking older texture match the size of the screen. If it matches dont do anything
			if (ColorTexture->GetHeight() != ScreenWidth && ColorTexture->GetHeight() != ScreenHeight)
			{
				// If not release it and recreate it with the correct size
				ColorTexture->Release();
				if (ColorTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32))
				{
					zeCriticalError("D3D9 Frame Buffer Renderer", "Can not create color texture.");
					return;
				}
			}
		}
		else
		{
			// If there is no older texture then create brand new one
			ColorTexture = ZETexture2D::CreateInstance();
			ColorTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32);
		}
	else
		// If render color buffer is not enabled check color texture is available or not
		if (ColorTexture != NULL)
		{
			// If available then destroy it in order to save memory
			ColorTexture->Destroy();
			ColorTexture = NULL;
		}

	// Create depth render texture if render depth texture enabled. Same process with color texture creation
	if (RenderDepthTexture)
		if (DepthTexture != NULL)
		{
			if (DepthTexture->GetHeight() != ScreenWidth && DepthTexture->GetHeight() != ScreenHeight)
			{
				DepthTexture->Release();
				if (!DepthTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32))
				{
					zeCriticalError("D3D9 Frame Buffer Renderer", "Can not create depth texture.");
					return;
				}
			}
		}
		else
		{
			DepthTexture = ZETexture2D::CreateInstance();
			DepthTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32);
		}
	else
		if (DepthTexture != NULL)
		{
			DepthTexture->Destroy();
			DepthTexture = NULL;
		}

	// Create velocity render texture if render depth texture enabled. Same process with color texture creation
	if (RenderVelocityTexture)
		if (VelocityTexture != NULL)
		{
			if (VelocityTexture->GetHeight() != ScreenWidth && VelocityTexture->GetHeight() != ScreenHeight)
			{
				VelocityTexture->Release();
				if (!VelocityTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32))
				{
					zeCriticalError("D3D9 Frame Buffer Renderer", "Can not create velocity texture.");
					return;
				}
			}
		}
		else
		{
			VelocityTexture = ZETexture2D::CreateInstance();
			VelocityTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32);
		}
	else
		if (VelocityTexture != NULL)
		{
			VelocityTexture->Destroy();
			VelocityTexture = NULL;
		}

	// Set z-buffer options
	GetDevice()->SetRenderState(D3DRS_DEPTHBIAS, 0);
	GetDevice()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	// Clear render targets
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0x00000000);

	// Setup Texture sampling properties (Will be changed)
	for (int I = 0; I < 10; I++)
	{
		if (I < 8)
		{
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		} 
		else
		{
			GetDevice()->SetSamplerState(I, D3DSAMP_BORDERCOLOR, 0x0f);
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			GetDevice()->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		}

		if (I == 8)
		{
			GetDevice()->SetSamplerState(I, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			GetDevice()->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
			GetDevice()->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
		else
		{
			GetDevice()->SetSamplerState(I, D3DSAMP_MINFILTER, (GetModule()->GetAnisotropicFilter() ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
			GetDevice()->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			GetDevice()->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}


	}

	// Enable color output
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// Start Drawing
	GetDevice()->BeginScene();

		// Draw non-transparent object first
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&NonTransparent[I], Camera);

		// Draw transparent objects
		for (size_t I = 0; I < Transparent.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&Transparent[I], Camera);

		// Draw imposters last
		for (size_t I = 0; I < Imposter.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&Imposter[I], Camera);
		
		if (HDREnabled)
		{
			HDRProcessor.DoHDR();
		}

	// Finish Drawing
	GetDevice()->EndScene();



}
