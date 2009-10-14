//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Renderer.cpp
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

#include "D3D9Renderer.h"
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

ZED3D9Renderer::ZED3D9Renderer()
{

}

ZED3D9Renderer::~ZED3D9Renderer()
{
	// Deinitialize renderer
	Deinitialize();
}

bool ZED3D9Renderer::Initialize() 
{ 
	// No initialization core necessary
	return true; 
} 

void ZED3D9Renderer::Deinitialize()
{

}

void ZED3D9Renderer::DeviceLost()
{
	
}

bool ZED3D9Renderer::DeviceRestored()
{
	return true;
}

void ZED3D9Renderer::Destroy()
{
	// Remove renderer from modules renderer list
	Module->Renderers.DeleteValue((ZED3D9Renderer*)this);
	delete this;
}

void ZED3D9Renderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZEArray<ZEPostProcessor*>& ZED3D9Renderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9Renderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9Renderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}

void ZED3D9Renderer::AddToRenderList(ZERenderOrder* RenderOrder)
{
	#ifdef ZEDEBUG_ENABLED
		// Check render order is valid
		if (ZED3D9RendererBase::CheckRenderOrder(RenderOrder))
			return;
	#endif

	// Add render orders to render order lists according to their properties
	if (RenderOrder->Flags & ZE_RLF_IMPOSTER)
		Imposter.Add(*RenderOrder);
	if (RenderOrder->Flags & ZE_RLF_TRANSPARENT)
		Transparent.Add(*RenderOrder);
	else
		NonTransparent.Add(*RenderOrder);
}

void ZED3D9Renderer::ClearList()
{
	//Clear render lists
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9Renderer::Render(float ElaspedTime)
{
	// Check render module is enabled and the device is not lost
	if (!Module->IsEnabled() || Module->IsDeviceLost)
		return;

	// Check if there is a post process attached to render order
	if (PostProcessors.GetCount() != NULL)
	{
		// If post process is attached then create offscreen render targets

	}
	else
	{
		// If there is no post process attached then use directly frame buffer
		Device->SetRenderTarget(0, Module->FrameColorBuffer);
		Device->SetDepthStencilSurface(Module->FrameZBuffer);
	}

	// Set z-buffer options
	Device->SetRenderState(D3DRS_DEPTHBIAS, 0);
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	// Clear render targets
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0x00000000);

	// Setup Texture sampling properties (Will be changed)
	for (int I = 0; I < 10; I++)
	{
		if (I < 8)
		{
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		} 
		else
		{
			Device->SetSamplerState(I, D3DSAMP_BORDERCOLOR, 0x0f);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		}

		if (I == 8)
		{
			Device->SetSamplerState(I, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
			Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
		else
		{
			Device->SetSamplerState(I, D3DSAMP_MINFILTER, (Module->GetAnisotropicFilter() ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
			Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}


	}

	// Enable color output
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// Start Drawing
	Device->BeginScene();

		// Draw non-transparent object first
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&NonTransparent[I], Camera);

		// Draw transparent objects
		for (size_t I = 0; I < Transparent.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&Transparent[I], Camera);

		// Draw imposters last
		for (size_t I = 0; I < Imposter.GetCount(); I++)
			ZED3D9RendererBase::DrawRenderOrder(&Imposter[I], Camera);

	// Finish Drawing
	Device->EndScene();
}
