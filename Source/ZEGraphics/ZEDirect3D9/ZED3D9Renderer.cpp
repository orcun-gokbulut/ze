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
#include "ZED3D9Shader.h"
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

	((ZED3D9VertexDeclaration*)RenderOrder->VertexDeclaration)->SetupVertexDeclaration();

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
		{
			GetDevice()->SetStreamSource(0, ((ZED3D9StaticVertexBuffer*)RenderOrder->VertexBuffer)->StaticBuffer, 0, RenderOrder->VertexDeclaration->GetVertexSize());
			GetDevice()->DrawPrimitive(PrimitiveType, RenderOrder->VertexBufferOffset, RenderOrder->PrimitiveCount);
		}
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

#include "ZEGraphics/ZECanvas.h"

void ZED3D9Renderer::InitializeLightning()
{
	// Create Static Vertex Buffer;
	/*
	Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
	Quad		0			0			6				2				288
	Sphere12	6			288			936				312				44928
	Sphere24	942			45216		3600			1200			172800
	Cone12		4542		218016		36				12				1728
	Cone24		4578		219744		72				24				3456
	End			4650				
	*/

	ZECanvas Canvas;
	// Directional
	Canvas.AddQuad(ZEVector3(0.0f, 0.0f, 0.0f),
		ZEVector3(0.0f, 0.0f, 0.0f),
		ZEVector3(0.0f, 0.0f, 0.0f),
		ZEVector3(0.0f, 0.0f, 0.0f));

	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Projective
	Canvas.AddCone(1.0f, 12, 1.0f);
	Canvas.AddCone(1.0f, 24, 1.0f);

	if (LightningComponents.LightMeshVB == NULL)
		LightningComponents.LightMeshVB = (ZED3D9StaticVertexBuffer*)Canvas.CreateStaticVertexBuffer();
	LightningComponents.PointLightVS = ZED3D9VertexShader::CreateShader("Lights.hlsl", "PLVSMain", 0, "vs_2_0");
	LightningComponents.PointLightPS = ZED3D9PixelShader::CreateShader("Lights.hlsl", "PLPSMain", 0, "ps_2_0");
}

void ZED3D9Renderer::DeinitializeLightning()
{
	ZED3D_RELEASE(LightningComponents.LightMeshVB);
	ZED3D_RELEASE(LightningComponents.PointLightVS);
	ZED3D_RELEASE(LightningComponents.PointLightPS);
	ZED3D_RELEASE(LightningComponents.DirectionalLightVS);
	ZED3D_RELEASE(LightningComponents.DirectionalLightPS);
	ZED3D_RELEASE(LightningComponents.ProjectiveLightVS);
	ZED3D_RELEASE(LightningComponents.ProjectiveLightPS);
	ZED3D_RELEASE(LightningComponents.OmniProjectiveLightVS);
	ZED3D_RELEASE(LightningComponents.OmniProjectiveLightPS);
}

void ZED3D9Renderer::DrawPointLight(ZEPointLight* Light)
{
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 312); // Sphere 1
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 942, 1200); // Sphere 2
}

void ZED3D9Renderer::DrawDirectionalLight(ZEDirectionalLight* Light)
{
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2); // Quad
}

void ZED3D9Renderer::DrawProjectiveLight(ZEProjectiveLight* Light)
{
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 4542, 12); // Cone 1
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 4578, 24); // Cone 2

}

void ZED3D9Renderer::DrawOmniProjectiveLight(ZEOmniProjectiveLight* Light)
{
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 312); // Sphere 1
	GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 942, 1200); // Sphere 2

}

void ZED3D9Renderer::DoPreZPass()
{
	GetDevice()->SetRenderTarget(0, ViewPort->FrameBuffer);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];
		
		if ((RenderOrder->Material->GetMaterialFlags() & ZE_MTF_PREZ_PASS) == 0)
			continue;

		if (!RenderOrder->Material->SetupPreZPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's Pre-Z pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());

		PumpStreams(RenderOrder);
	}
}

void ZED3D9Renderer::DoGBufferPass()
{
	ZED3D9CommonTools::SetRenderTarget(0, GBuffer1);
	ZED3D9CommonTools::SetRenderTarget(1, GBuffer2);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];

		if ((RenderOrder->Material->GetMaterialFlags() & ZE_MTF_G_BUFFER_PASS) == 0)
			continue;

		if (!RenderOrder->Material->SetupGBufferPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's GBuffer pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());

		PumpStreams(RenderOrder);
	}
	
	ZED3D9CommonTools::SetRenderTarget(0, NULL);
	ZED3D9CommonTools::SetRenderTarget(1, NULL);
}

void ZED3D9Renderer::DoLightningPass()
{
	// Render Targets
	ZED3D9CommonTools::SetRenderTarget(0, LBuffer1);
	ZED3D9CommonTools::SetRenderTarget(1, LBuffer2);

	// Z-Buffer
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Alpha blending
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);
	GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	// Stencil Buffer
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// GBuffers
	GetDevice()->SetTexture(0, GBuffer1);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	GetDevice()->SetTexture(1, GBuffer2);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	GetDevice()->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	GetDevice()->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	for (size_t I = 0; I < Lights.GetCount(); I++)
		switch(Lights[I]->GetLightType())
		{
		case ZE_LT_POINT:
			DrawPointLight((ZEPointLight*)Lights[I]);
		case ZE_LT_DIRECTIONAL:
			DrawDirectionalLight((ZEDirectionalLight*)Lights[I]);
			break;
		case ZE_LT_PROJECTIVE:
			DrawProjectiveLight((ZEProjectiveLight*)Lights[I]);
			break;
		case ZE_LT_OMNIPROJECTIVE:
			DrawOmniProjectiveLight((ZEOmniProjectiveLight*)Lights[I]);
			break;
		}

	// Render Targets
	ZED3D9CommonTools::SetRenderTarget(0, NULL);
	ZED3D9CommonTools::SetRenderTarget(1, NULL);
}

void ZED3D9Renderer::DoForwardPass()
{
	ZED3D9CommonTools::SetRenderTarget(0, AccumulationBuffer);

	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];
		
		if (!RenderOrder->Material->SetupForwardPass(this, RenderOrder))
			zeCriticalError("Renderer", "Can not set material's Forward pass. (Material Type : \"%s\")", RenderOrder->Material->GetClassDescription()->GetName());
		PumpStreams(RenderOrder);
	}
}

void ZED3D9Renderer::InitializeRenderTargets()
{
	ZED3D9CommonTools::CreateRenderTarget(&GBuffer1, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&GBuffer2, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&LBuffer1, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&LBuffer2, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
	ZED3D9CommonTools::CreateRenderTarget(&AccumulationBuffer, ViewPort->GetWidth(), ViewPort->GetHeight(), ZE_TPF_RGBA_HDR);
}

void ZED3D9Renderer::DeinitializeRenderTargets()
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

	LightningComponents.LightMeshVB = NULL;
	LightningComponents.PointLightVS = NULL;
	LightningComponents.PointLightPS = NULL;
	LightningComponents.DirectionalLightVS = NULL;
	LightningComponents.DirectionalLightPS = NULL;
	LightningComponents.ProjectiveLightVS = NULL;
	LightningComponents.ProjectiveLightPS = NULL;
	LightningComponents.OmniProjectiveLightVS = NULL;
	LightningComponents.OmniProjectiveLightPS = NULL;

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
	InitializeRenderTargets();
	SSAOProcessor.Renderer = this;
	HDRProcessor.Initialize();
	SSAOProcessor.Initialize();
	
	InitializeLightning();

	return true; 
} 

void ZED3D9Renderer::Deinitialize()
{
	HDRProcessor.Deinitialize();
	SSAOProcessor.Deinitialize();
	DeinitializeLightning();
}

void ZED3D9Renderer::DeviceLost()
{
	HDRProcessor.OnDeviceLost();
	SSAOProcessor.OnDeviceLost();
	DeinitializeRenderTargets();
}

bool ZED3D9Renderer::DeviceRestored()
{
	HDRProcessor.OnDeviceRestored();
	SSAOProcessor.OnDeviceRestored();
	InitializeRenderTargets();

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

void ZED3D9Renderer::SetLights(ZEArray<ZELight*>& Lights)
{

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
	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost())
		return;

	GetDevice()->SetRenderState(D3DRS_DEPTHBIAS, 0);
	GetDevice()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	/*GetDevice()->SetRenderTarget(0, GBuffer1);
	GetDevice()->SetRenderTarget(1, GBuffer2);*/

	GetDevice()->SetDepthStencilSurface(ViewPort->ZBuffer);
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0x00);

	GetDevice()->BeginScene();

		//DoPreZPass();
		DoGBufferPass();
		DoLightningPass();
		//DoForwardPass();
		SSAOProcessor.InputNormal = GBuffer1;
		SSAOProcessor.InputDepth = GBuffer2;
		SSAOProcessor.Output = ViewPort->FrameBuffer;
		SSAOProcessor.Process();

	GetDevice()->EndScene();
}
