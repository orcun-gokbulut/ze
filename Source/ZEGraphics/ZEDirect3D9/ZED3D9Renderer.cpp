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
#include "ZED3D9Material.h"
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

bool ZED3D9Renderer::CheckRenderOrder(ZERenderOrder* RenderOrder)
{
	#ifdef ZE_DEBUG_ENABLED
		// Check render order material is available
		if (RenderOrder->Material == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid material.");
			return false;
		}

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
		if (RenderOrder->VertexBuffer == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex buffer is not valid.");
			return false;
		}

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

void ZED3D9Renderer::DoPreZPass()
{
	GetDevice()->SetRenderTarget(0, ViewPort->FrameBuffer);
	GetDevice()->SetRenderTarget(1, NULL);
	GetDevice()->SetDepthStencilSurface(ViewPort->ZBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];
		ZED3D9Material* Material = (ZED3D9Material*)RenderOrder->Material;
		Material->SetupPreZPass();
		PumpStreams(RenderOrder);
	}
}

void ZED3D9Renderer::DoGBufferPass()
{
	ZED3D9CommonTools::SetRenderTarget(0, GBuffer1);
	ZED3D9CommonTools::SetRenderTarget(1, GBuffer2);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);

	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];
		ZED3D9Material* Material = (ZED3D9Material*)RenderOrder->Material;
		Material->SetupGBufferPass();
		PumpStreams(RenderOrder);
	}
}

void ZED3D9Renderer::DoLightningPass()
{
	ZED3D9CommonTools::SetRenderTarget(0, LBuffer1);
	ZED3D9CommonTools::SetRenderTarget(1, LBuffer2);

	// Do lighting passes
}


void ZED3D9Renderer::DoForwardPass()
{
	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];
		ZED3D9Material* Material = (ZED3D9Material*)RenderOrder->Material;
		Material->SetupForwardPass();
		PumpStreams(RenderOrder);
	}	
}

void ZED3D9Renderer::CreateRenderTargets()
{
	ZED3D9CommonTools::CreateRenderTarget(&GBuffer1, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_INT32);
	ZED3D9CommonTools::CreateRenderTarget(&GBuffer2, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_INT32);
	ZED3D9CommonTools::CreateRenderTarget(&LBuffer1, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&LBuffer2, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&AccumulationBuffer, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
}


void ZED3D9Renderer::DestroyRenderTargets()
{
	ZED3D_RELEASE(GBuffer1);
	ZED3D_RELEASE(GBuffer2);
	ZED3D_RELEASE(LBuffer1);
	ZED3D_RELEASE(LBuffer2);
	ZED3D_RELEASE(AccumulationBuffer);
}


ZED3D9Renderer::ZED3D9Renderer()
{
	GBuffer1 = NULL;
	GBuffer2 = NULL;
	LBuffer1 = NULL;
	LBuffer2 = NULL;
	AccumulationBuffer = NULL;
	SetViewPort(zeGraphics->GetFrameBufferViewPort());
}


ZED3D9Renderer::~ZED3D9Renderer()
{
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
	CreateRenderTargets();
	SSAOProcessor.Renderer = this;
	HDRProcessor.Initialize();
	SSAOProcessor.Initialize();

	return true; 
} 


void ZED3D9Renderer::Deinitialize()
{
	HDRProcessor.Deinitialize();
	SSAOProcessor.Deinitialize();
	DestroyRenderTargets();
}


void ZED3D9Renderer::DeviceLost()
{
	HDRProcessor.OnDeviceLost();
	SSAOProcessor.OnDeviceLost();
	DestroyRenderTargets();
}


bool ZED3D9Renderer::DeviceRestored()
{
	HDRProcessor.OnDeviceRestored();
	SSAOProcessor.OnDeviceRestored();
	CreateRenderTargets();

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
	// Do Pre-Z
	DoPreZPass();

	// Do GBuffer
	// Do SSAO
	// Do Light Passes
	// Do Deferred Forward
	// Do Forward
	// Do Post Processes

/*	unsigned int ScreenWidth = zeGraphics->GetScreenWidth();
	unsigned int ScreenHeight = zeGraphics->GetScreenHeight();

	// Check render module is enabled and the device is not lost
	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost())
		return;

	// Set z-buffer options
	GetDevice()->SetRenderState(D3DRS_DEPTHBIAS, 0);
	GetDevice()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	
	// Enable color output
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// Start Drawing
	GetDevice()->BeginScene();
		// Select Render Targets
		GetDevice()->SetRenderTarget(0, ViewPort->FrameBuffer);
	//	ZED3D9CommonTools::SetRenderTarget(0, AmbientColorRenderTarget);
		ZED3D9CommonTools::SetRenderTarget(1, NormalDepthRenderTarget);
		ZED3D9CommonTools::SetRenderTarget(2, PositionRenderTarget);
		GetDevice()->SetDepthStencilSurface(ViewPort->ZBuffer);

		// Clear Render Targets
		GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0x0);

		// Set z-buffer options
		GetDevice()->SetRenderState(D3DRS_DEPTHBIAS, 0);
		GetDevice()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

		// Draw ambient
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)	
			DrawAmbientPass(&NonTransparent[I], Camera);

		GetDevice()->SetRenderTarget(1, NULL);
		GetDevice()->SetRenderTarget(2, NULL);

		SSAOProcessor.SetIntensity(2.0f);
		SSAOProcessor.SetSampleRadius(0.5f);
		SSAOProcessor.SetSampleScale(0.5f);
		SSAOProcessor.SetSampleBias(0.1f);
		SSAOProcessor.InputColor = AmbientColorRenderTarget;
		SSAOProcessor.InputDepth = NormalDepthRenderTarget;
		SSAOProcessor.InputPosition = PositionRenderTarget;
		SSAOProcessor.Output = ViewPort->FrameBuffer;
		//SSAOProcessor.Process();
		/*LPDIRECT3DSURFACE9 TempRenderTargetSurface;
		TempRenderTarget->GetSurfaceLevel(0, &TempRenderTargetSurface);
		SSAOProcessor.Output = TempRenderTargetSurface;
		SSAOProcessor.Process();

		//ZED3D9CommonTools::SetRenderTarget(0, Fr);

//		GetDevice()->SetRenderTarget(0, AmbientColorRenderTarget);
		//GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0x0);
		//TempRenderTargetSurface->Release();

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

		// Draw non-transparent object first
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			DrawLightPasses(&NonTransparent[I], Camera);

		// Draw transparent objects
		for (size_t I = 0; I < Transparent.GetCount(); I++)
			DrawAmbientPass(&Transparent[I], Camera);

		// Draw imposers last
		for (size_t I = 0; I < Imposter.GetCount(); I++)
			DrawAmbientPass(&Imposter[I], Camera);
		
		/*HDRProcessor.Input = TempRenderTarget;
		HDRProcessor.Target = ViewPort->FrameBuffer;
		HDRProcessor.DoHDR();

	// Finish Drawing
	GetDevice()->EndScene();*/
}
