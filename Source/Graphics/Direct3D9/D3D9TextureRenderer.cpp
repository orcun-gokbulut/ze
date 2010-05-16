//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9TextureRenderer.cpp
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

#include "D3D9TextureRenderer.h"
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



bool ZED3D9TextureRenderer::Initialize()
{
	return true;
}

void ZED3D9TextureRenderer::Deinitialize()
{
	ZED3D_RELEASE(DepthRenderTarget);
	ZED3D_RELEASE(ColorRenderTarget);
}


void ZED3D9TextureRenderer::Destroy()
{
	// Remove renderer from modules renderer list
	GetModule()->TextureRenderers.DeleteValue((ZED3D9TextureRenderer*)this);
	ZERenderer::Destroy();
}

void ZED3D9TextureRenderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZED3D9TextureRenderer::GetCamera()
{
	return Camera;
}

void ZED3D9TextureRenderer::ClearList()
{
	//Clear render lists
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9TextureRenderer::AddToRenderList(ZERenderOrder* RenderOrder)
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


ZEArray<ZEPostProcessor*>& ZED3D9TextureRenderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9TextureRenderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9TextureRenderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}

void ZED3D9TextureRenderer::Render(float ElaspedTime)
{
	// Check render module is enabled and the device is not lost
	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost())
		return;

	// Check if there is a post process attached to render order
	if (PostProcessors.GetCount() != NULL)
	{
		// If post process is attached then create offscreen render targets

	}
	else
	{
		// If there is no post process attached then use directly frame buffer
		GetDevice()->SetRenderTarget(0, GetModule()->GetFrameColorBuffer());
		GetDevice()->SetDepthStencilSurface(GetModule()->GetFrameZBuffer());
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

	// Finish Drawing
	GetDevice()->EndScene();
}

bool ZED3D9TextureRenderer::SetOutput(ZETexture2D* Texture)
{
	// Check whether texture is render target or not
	if (!Texture->IsRenderTarget())
	{
		// If not rendere can't use it
		zeError("D3D9 Texture Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	LPDIRECT3DTEXTURE9 D3DTexture = ((ZED3D9Texture2D*)Texture)->Texture;

	// Create depth buffer according to texture dimensions
	ZED3D9CommonTools::CreateDepthRenderTarget(&DepthRenderTarget, Texture->GetWidth(), Texture->GetHeight());

	// Release previous color render target
	if (ColorRenderTarget != NULL)
		ColorRenderTarget->Release();

	// Get texture's surface for using as a render target
	D3DTexture->GetSurfaceLevel(0, &ColorRenderTarget);

	// Store texture in case of device lost (Check out device lost function
	OutputTexture = Texture;
	OutputCubeTexture = NULL; 

	return true;
}

bool ZED3D9TextureRenderer::SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face)
{
	// Check whether texture is render target or not
	if (!Texture->IsRenderTarget())
	{
		// If not rendere can't use it
		zeError("D3D9 Texture Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	LPDIRECT3DCUBETEXTURE9 D3DTexture = ((ZED3D9TextureCube*)Texture)->CubeTexture;

	// Create depth buffer according to texture dimensions
	ZED3D9CommonTools::CreateDepthRenderTarget(&DepthRenderTarget, Texture->GetEdgeLenght(), Texture->GetEdgeLenght());

	// Release previous color render target
	if (ColorRenderTarget != NULL)
		ColorRenderTarget->Release();
	
	// Get texture's surface for using as a render target
	D3DTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)Face, 0, &ColorRenderTarget);

	// Store texture in case of device lost (Check out device lost function
	OutputCubeTexture = Texture;
	OutputTexture = NULL; 

	return true;
}

void ZED3D9TextureRenderer::DeviceLost()
{
	// If device is lost deinitialize render target
	Deinitialize();
}

bool ZED3D9TextureRenderer::DeviceRestored()
{
	// When device is restored recreate depth and color render targets

	if (OutputCubeTexture != NULL)
		return SetOutput(OutputCubeTexture, OutputCubeTextureFace);
	else if (OutputTexture != NULL)
		return SetOutput(OutputTexture);
	else
		return true;
}

ZED3D9TextureRenderer::ZED3D9TextureRenderer()
{
	OutputTexture = NULL;
	OutputCubeTexture = NULL;
	ColorRenderTarget = NULL;
	DepthRenderTarget = NULL;
}

ZED3D9TextureRenderer::~ZED3D9TextureRenderer()
{
	// Release depth and color render targets
	ZED3D_RELEASE(ColorRenderTarget);
	ZED3D_RELEASE(DepthRenderTarget);
}
