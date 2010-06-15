//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9Renderer.cpp
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

#include "ZED3D9Renderer.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Texture3D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Module.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9VertexDeclaration.h"
#include "ZEGraphics\ZEMaterial.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZEPointLight.h"
#include "ZEGraphics\ZEDirectionalLight.h"
#include "ZEGraphics\ZEProjectiveLight.h"
#include "ZEGraphics\ZEOmniProjectiveLight.h"
#include "ZECore\ZEError.h"

#pragma warning(disable:4267)

void ZED3D9Renderer::PumpStreams(ZERenderOrder* RenderOrder)
{
	ZEVertexBuffer* VertexBuffer = RenderOrder->VertexBuffer;

	// Figure out primitive type
	D3DPRIMITIVETYPE PrimitiveType;
	switch(RenderOrder->PrimitiveType)
	{
	default:
	case ZE_ROPT_POINT:
		PrimitiveType = D3DPT_POINTLIST;
		break;
	case ZE_ROPT_LINE:
		PrimitiveType = D3DPT_LINELIST;
		break;
	case ZE_ROPT_TRIANGLE:
		PrimitiveType = D3DPT_TRIANGLELIST;
		break;
	}

	// Make draw call
	// Check wheater render order is indexed or not
	if (RenderOrder->IndexBuffer != NULL)
	{
		/* NOT SUPPORTED !!! */
		/*
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
		GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexBuffer->GetVertexCount(), 0, VertexBuffer->GetPolygonCount());
		else
		GetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VertexBuffer->GetVertexCount(), RenderOrder->PrimitiveCount, RenderOrder->IndexBuffer, D3DFMT_INDEX32, VertexBuffer->GetVertexBuffer(), RenderOrder->VertexDeclaration->GetVertexSize());
		*/
	} 
	else
	{
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
			GetDevice()->DrawPrimitive(PrimitiveType, RenderOrder->VertexBufferOffset, RenderOrder->PrimitiveCount);
		else
			GetDevice()->DrawPrimitiveUP(PrimitiveType, RenderOrder->PrimitiveCount, 
			(char*)((ZEDynamicVertexBuffer*)VertexBuffer)->GetVertexBuffer() + RenderOrder->VertexBufferOffset * RenderOrder->VertexDeclaration->GetVertexSize(),  
			RenderOrder->VertexDeclaration->GetVertexSize());
	}
}

void ZED3D9Renderer::DrawRenderOrder(ZERenderOrder* RenderOrder, ZECamera* Camera)
{
	// Get Vertex buffer of render order
	ZED3D9StaticVertexBuffer* VertexBuffer = (ZED3D9StaticVertexBuffer*)RenderOrder->VertexBuffer;

	// Check render order has vertex buffer and one or more primitives.
	if (RenderOrder->VertexBuffer == NULL || RenderOrder->PrimitiveCount == 0)
		return;

	// Get material of the render order
	const ZEMaterial* Material = RenderOrder->Material;

	// Setup vertex declaration
	RenderOrder->VertexDeclaration->SetupVertexDeclaration();

	/* NO INDEX BUFFERS SUPPORTED ANYMORE */
	/*
	// Setup index buffer if available
	if (RenderOrder->IndexBuffer != NULL)
	GetDevice()->SetIndices(RenderOrder->IndexBuffer);
	*/

	// If vertex buffer is static, setup vertex buffer
	if (VertexBuffer->IsStatic())
		GetDevice()->SetStreamSource(0, VertexBuffer->StaticBuffer, 0, RenderOrder->VertexDeclaration->GetVertexSize());

	// Setup Material
	Material->SetupMaterial(RenderOrder, Camera);

	// Do prelightning pass
	if (Material->SetupPreLightning())
	{
		size_t PassCount;
		do
		{
			PassCount = Material->DoPreLightningPass();
			PumpStreams(RenderOrder);
		}
		while(PassCount != 1);
	}

	// Do light passes
	if (RenderOrder->Lights.GetCount() != 0 && Material->SetupLightning())
	{
		// Sort the lights
		ZEPointLight* PointLightList[64];
		size_t PointLightCount = 0;

		ZEDirectionalLight* DirectionalLightList[64];
		size_t DirectionalLightCount = 0;

		ZEProjectiveLight* ProjectiveLightList[64];
		size_t ProjectiveLightCount = 0;

		ZEOmniProjectiveLight* OmniProjectiveLightList[64];
		size_t OmniProjectiveLightCount = 0;

		ZEPointLight* ShadowedPointLightList[64];
		size_t ShadowedPointLightCount = 0;

		ZEDirectionalLight* ShadowedDirectionalLightList[64];
		size_t ShadowedDirectionalLightCount = 0;

		ZEProjectiveLight* ShadowedProjectiveLightList[64];
		size_t ShadowedProjectiveLightCount = 0;

		ZEOmniProjectiveLight* ShadowedOmniProjectiveLightList[64];
		size_t ShadowedOmniProjectiveLightCount = 0;

		for (size_t I = 0; I < RenderOrder->Lights.GetCount(); I++)
		{			
			ZELight* CurrentLight = RenderOrder->Lights[I];
			switch (CurrentLight->GetLightType())
			{
			case ZE_LT_POINT:
				if (CurrentLight->GetCastsShadows())
				{
					ShadowedPointLightList[ShadowedPointLightCount] = (ZEPointLight*)CurrentLight;
					ShadowedPointLightCount++;
				}
				else
				{
					PointLightList[PointLightCount] = (ZEPointLight*)CurrentLight;
					PointLightCount++;
				}
				break;

			case ZE_LT_DIRECTIONAL:
				if (CurrentLight->GetCastsShadows() == NULL)
				{
					DirectionalLightList[DirectionalLightCount] = (ZEDirectionalLight*)CurrentLight;
					DirectionalLightCount++;
				}
				else
				{
					ShadowedDirectionalLightList[ShadowedDirectionalLightCount] = (ZEDirectionalLight*)CurrentLight;
					ShadowedDirectionalLightCount++;
				}
				break;

			case ZE_LT_PROJECTIVE:
				if (CurrentLight->GetCastsShadows() == NULL)
				{
					ProjectiveLightList[PointLightCount] = (ZEProjectiveLight*)CurrentLight;
					ProjectiveLightCount++;
				}
				else
				{
					ShadowedProjectiveLightList[ShadowedPointLightCount] = (ZEProjectiveLight*)CurrentLight;
					ShadowedProjectiveLightCount++;
				}
				break;

			case ZE_LT_OMNIPROJECTIVE:
				if (CurrentLight->GetCastsShadows() == NULL)
				{
					OmniProjectiveLightList[OmniProjectiveLightCount] = (ZEOmniProjectiveLight*)CurrentLight;
					OmniProjectiveLightCount++;

				}
				else
				{
					ShadowedOmniProjectiveLightList[OmniProjectiveLightCount] = (ZEOmniProjectiveLight*)CurrentLight;
					ShadowedOmniProjectiveLightCount++;
				}
				break;
			}
		}

		// Draw point light passes
		if (PointLightCount != 0 && Material->SetupPointLightPass(false))
		{
			// Setup material for point light passes

			while (PointLightCount != 0)
			{
				// Setup point light pass and decrease processed light coun t
				//		Materials can have multiple lights per pass or multiple pass per light so DoPointLightPass 
				//		function returns how many light will be processed in a particular pass. If its returns zero
				//		it means renderer should do multiple passes per lights. On multiple pass per light situation
				//		DoPointLightPass will return 0 until light has been successfully processed. Then it will return
				//		one. (In multiple lights per multiple pass it will return more than zero) 
				// Decrease number of light count.

				PointLightCount -= Material->DoPointLightPass(PointLightList, PointLightCount);

				// Draw primitives
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed point light passes
		if (ShadowedPointLightCount != 0 && Material->SetupPointLightPass(true))
		{
			while (PointLightCount != 0)
			{
				ShadowedPointLightCount -= Material->DoPointLightPass(ShadowedPointLightList, PointLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw directional light passes
		if (DirectionalLightCount != 0 && Material->SetupDirectionalLightPass(false))
		{

			while (DirectionalLightCount != 0)
			{
				DirectionalLightCount -= Material->DoDirectionalLightPass(DirectionalLightList, DirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed directional light passes
		if (ShadowedDirectionalLightCount != 0 && Material->SetupDirectionalLightPass(true))
		{
			while (ShadowedDirectionalLightCount != 0)
			{
				ShadowedDirectionalLightCount -= Material->DoDirectionalLightPass(ShadowedDirectionalLightList, ShadowedDirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw projective light passes
		if (ProjectiveLightCount != 0 && Material->SetupProjectiveLightPass(false))
		{
			while (ProjectiveLightCount != 0)
			{
				ProjectiveLightCount -= Material->DoProjectiveLightPass(ProjectiveLightList, ProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed projective light passes		
		if (ShadowedProjectiveLightCount != 0 && Material->SetupProjectiveLightPass(true))
		{
			while (ShadowedProjectiveLightCount != 0)
			{
				ShadowedProjectiveLightCount -= Material->DoProjectiveLightPass(ShadowedProjectiveLightList, ShadowedProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw omni projective light passes
		if (OmniProjectiveLightCount != 0 && Material->SetupOmniProjectiveLightPass(false))
		{
			while (OmniProjectiveLightCount != 0)
			{
				OmniProjectiveLightCount -= Material->DoOmniProjectivePass(OmniProjectiveLightList, OmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed omni projective light passes
		if (ShadowedOmniProjectiveLightCount != 0 && Material->SetupOmniProjectiveLightPass(true))
		{
			while (ShadowedOmniProjectiveLightCount != 0)
			{
				ShadowedOmniProjectiveLightCount -= Material->DoOmniProjectivePass(ShadowedOmniProjectiveLightList, ShadowedOmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
	}
}



bool ZED3D9Renderer::CheckRenderOrder(ZERenderOrder* RenderOrder)
{
#ifdef ZE_DEBUG_ENABLED
	// Check render order material is available
	if (RenderOrder->Material == NULL)
	{
		zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid material.");
		return false;
	}

	// Check render order material has shader
	/*if (RenderOrder->Material->GetShader() == NULL)
	{
	zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid shader.");
	return false;
	}*/

	// Check vertex declaration is available
	if (RenderOrder->VertexDeclaration == NULL)
	{
		zeError("Direct3D9 Renderer", "RenderOrder's vertex declaration is invalid.");
		return false;
	}

	// Check render order has valid primitive type
	if (RenderOrder->PrimitiveType != ZE_ROPT_POINT &&
		RenderOrder->PrimitiveType != ZE_ROPT_LINE &&
		RenderOrder->PrimitiveType != ZE_ROPT_TRIANGLE &&
		RenderOrder->PrimitiveType != ZE_ROPT_TRIANGLESTRIPT)
	{
		zeError("Direct3D9 Renderer", "RenderOrder's primitive type is not valid.");
		return false;
	}

	// Check render order has vertex buffer
	/*if (RenderOrder->VertexBuffer == NULL)
	{
	zeError("Direct3D9 Renderer", "RenderOrder's vertex buffer is not valid.");
	return false;
	}*/

	// Check render order has one or more primitive
	/*if (RenderOrder->PrimitiveCount == 0)
	{
	zeError("Direct3D9 Renderer", "RenderOrder's primitive count is zero.");
	return false;
	}*/
#else
	if (RenderOrder->PrimitiveCount == 0)
		return false;
#endif

	return true;
}
ZED3D9Renderer::ZED3D9Renderer()
{
	RenderColorTexture = false;
	RenderNormalDepthTexture = false;
	RenderVelocityTexture = false;
	RenderObjectTexture = true;
	HDREnabled = true;

	HDRRenderTarget = NULL;
	ColorRenderTarget = NULL;
	ColorTexture = NULL;
	NormalDepthTexture = NULL;
	VelocityTexture = NULL;
	ObjectTexture = NULL;

	SetViewPort(zeGraphics->GetFrameBufferViewPort());
}

ZED3D9Renderer::~ZED3D9Renderer()
{
	// Deinitialize renderer
	Deinitialize();
}

void ZED3D9Renderer::SetViewPort(ZEViewPort* ViewPort)
{
	this->ViewPort = (ZED3D9ViewPort*)ViewPort;
}

ZEViewPort* ZED3D9Renderer::GetViewPort()
{
	return ViewPort;
}

bool ZED3D9Renderer::Initialize() 
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

/*	if (SSAOEnabled)
	{
		ZED3D9CommonTools::CreateDepthRenderTarget(DepthBuffer, GetModule()->GetScreenWidth(), GetModule()->GetScreenHeight());
	}
	
	if (!HDREnabled && !SSAOEnabled)
	{
		// LDR Rendering
		ColorRenderTarget = GetModule()->GetFrameColorBuffer();
	}*/

	SSAOProcessor.Initialize();

	return true; 
} 

void ZED3D9Renderer::Deinitialize()
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
	if (NormalDepthTexture != NULL)
	{
		NormalDepthTexture->Destroy();
		NormalDepthTexture = NULL;
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

void ZED3D9Renderer::SetRenderColorTexture(bool Enable)
{
	RenderColorTexture = Enable;
}

bool ZED3D9Renderer::GetRenderColorTexture()
{
	return RenderColorTexture;
}

void ZED3D9Renderer::SetRenderDepthTexture(bool Enable)
{
	RenderNormalDepthTexture = Enable;
}

bool ZED3D9Renderer::GetRenderDepthTexture()
{
	return RenderNormalDepthTexture;
}

void ZED3D9Renderer::SetRenderVelocityTexture(bool Enable)
{
	RenderVelocityTexture = Enable;
}

bool ZED3D9Renderer::GetRenderVelocityTexture()
{
	return RenderVelocityTexture;
}

void ZED3D9Renderer::SetRenderObjectTexture(bool Enable)
{
	RenderObjectTexture = Enable;
}

bool ZED3D9Renderer::GetRenderObjectTexture()
{
	return RenderObjectTexture;
}

ZETexture2D* ZED3D9Renderer::GetColorTexture()
{
	return ColorTexture;
}

ZETexture2D* ZED3D9Renderer::GetDepthTexture()
{
	return NormalDepthTexture;
}

ZETexture2D* ZED3D9Renderer::GetVelociyTexture()
{
	return VelocityTexture;
}

ZETexture2D* ZED3D9Renderer::GetObjectTexture()
{
	return ObjectTexture;
}

void ZED3D9Renderer::DeviceLost()
{
	// GetDevice() is lost so recreate hardware resources
	HDRProcessor.OnDeviceLost();
}

bool ZED3D9Renderer::DeviceRestored()
{
	HDRProcessor.OnDeviceRestored();
	return true;
}

void ZED3D9Renderer::Destroy()
{
	// Remove renderer from modules renderer list
	GetModule()->Renderers.DeleteValue((ZED3D9Renderer*)this);
	delete this;
}

void ZED3D9Renderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZED3D9Renderer::GetCamera()
{
	return Camera;
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
	#ifdef ZE_DEBUG_ENABLED
		// Check render order is valid
		if (!CheckRenderOrder(RenderOrder))
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

void ZED3D9Renderer::ClearList()
{
	//Clear render lists
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9Renderer::Render(float ElaspedTime)
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
	if (RenderNormalDepthTexture)
		if (NormalDepthTexture != NULL)
		{
			if (NormalDepthTexture->GetHeight() != ScreenWidth && NormalDepthTexture->GetHeight() != ScreenHeight)
			{
				NormalDepthTexture->Release();
				if (!NormalDepthTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32))
				{
					zeCriticalError("D3D9 Frame Buffer Renderer", "Can not create depth texture.");
					return;
				}
			}
		}
		else
		{
			NormalDepthTexture = ZETexture2D::CreateInstance();
			NormalDepthTexture->Create(ScreenWidth, ScreenHeight, ZE_TPF_RGBA_INT32);
		}
	else
		if (NormalDepthTexture != NULL)
		{
			NormalDepthTexture->Destroy();
			NormalDepthTexture = NULL;
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
			DrawRenderOrder(&NonTransparent[I], Camera);

		// Draw transparent objects
		for (size_t I = 0; I < Transparent.GetCount(); I++)
			DrawRenderOrder(&Transparent[I], Camera);

		// Draw imposers last
		for (size_t I = 0; I < Imposter.GetCount(); I++)
			DrawRenderOrder(&Imposter[I], Camera);
		
		if (HDREnabled)
		{
			HDRProcessor.DoHDR();
		}

	// Finish Drawing
	GetDevice()->EndScene();



}


