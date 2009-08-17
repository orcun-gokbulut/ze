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
#include "Core/Core.h"
#include "D3D9Renderer.h"
#include "D3D9VertexBuffer.h"
#include "D3D9Texture.h"
#include "Direct3D9Module.h"
#include "D3D9CommonTools.h"

#pragma warning(disable:4267)

ZED3D9RendererBase::ZED3D9RendererBase()
{
	RenderColorBufferRT = NULL;
	RenderZBufferRT = NULL;	
};

ZED3D9RendererBase::~ZED3D9RendererBase()
{
	Deinitialize();
}

void ZED3D9RendererBase::PumpStreams(ZERenderOrder* RenderOrder)
{
	ZED3D9StaticVertexBuffer* VertexBuffer = (ZED3D9StaticVertexBuffer*)RenderOrder->VertexBuffer;
	D3DPRIMITIVETYPE PrimitiveType;
	switch(RenderOrder->PrimitiveType)
	{
		default:
		case ZE_RLPT_POINT:
			PrimitiveType = D3DPT_POINTLIST;
			break;
		case ZE_RLPT_LINE:
			PrimitiveType = D3DPT_LINELIST;
			break;
		case ZE_RLPT_TRIANGLE:
			PrimitiveType = D3DPT_TRIANGLELIST;
			break;
	}

	if (RenderOrder->IndexBuffer != NULL)
	{
		/*
		Device->SetIndices(RenderOrder->IndexBuffer);
		if (VertexBuffer->IsStatic())
		{
			Device->SetStreamSource(0, StaticVertexBuffers[VertexBuffer, 0, RenderL VertexBuffer->GetVertexSize());
			Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexBuffer->GetVertexCount(), 0, VertexBuffer->GetPolygonCount());
		}
		else
			Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VertexBuffer->GetVertexCount(), VertexBuffer->GetPolygonCount(), VertexBuffer->GetIndexBuffer(), D3DFMT_INDEX32, VertexBuffer->GetVertexBuffer(), VertexBuffer->GetVertexSize());
		*/
	} 
	else
	{
		if (VertexBuffer->IsStatic())
		{
			Device->SetStreamSource(0, VertexBuffer->StaticBuffer, RenderOrder->VertexBufferOffset, RenderOrder->VertexDeclaration->GetVertexSize());
			Device->DrawPrimitive(PrimitiveType, 0, RenderOrder->PrimitiveCount);
		}
		else
			Device->DrawPrimitiveUP(PrimitiveType, RenderOrder->PrimitiveCount, ((ZEDynamicVertexBuffer*)VertexBuffer)->GetVertexBuffer(),  RenderOrder->VertexDeclaration->GetVertexSize());
	}
}

void ZED3D9RendererBase::SetupRenderOrder(ZERenderOrder* RenderOrder)
{
	const ZEMaterial* Material = RenderOrder->Material;
	RenderOrder->VertexDeclaration->SetupVertexDeclaration();
	Material->SetupMaterial(RenderOrder, Camera);

	Material->SetupPreLightning();
	do
	{
		PumpStreams(RenderOrder);
	}
	while(Material->DoPreLightningPass() != 1);

	if (Material->SetupLightning() && RenderOrder->Lights.GetCount() != 0)
	{
		const ZERLLight*	PointLightList[64];
		size_t				PointLightCount = 0;
		const ZERLLight*	DirectionalLightList[64];
		size_t				DirectionalLightCount = 0;
		const ZERLLight*	ProjectiveLightList[64];
		size_t				ProjectiveLightCount = 0;
		const ZERLLight*	OmniProjectiveLightList[64];
		size_t				OmniProjectiveLightCount = 0;
		const ZERLLight*	ShadowedPointLightList[64];
		size_t				ShadowedPointLightCount = 0;
		const ZERLLight*	ShadowedDirectionalLightList[64];
		size_t				ShadowedDirectionalLightCount = 0;
		const ZERLLight*	ShadowedProjectiveLightList[64];
		size_t				ShadowedProjectiveLightCount = 0;
		const ZERLLight*	ShadowedOmniProjectiveLightList[64];
		size_t				ShadowedOmniProjectiveLightCount = 0;

		for (size_t I = 0; I < RenderOrder->Lights.GetCount(); I++)
		{			
			const ZERLLight* CurrentLight = RenderOrder->Lights[I];
			switch (CurrentLight->Type)
			{
				case ZE_RLLT_POINT:
					if (CurrentLight->ShadowMap != NULL)
					{
						PointLightList[PointLightCount] = CurrentLight;
						PointLightCount++;
					}
					else
					{
						ShadowedPointLightList[ShadowedPointLightCount] = CurrentLight;
						ShadowedPointLightCount++;
					}
					break;

				case ZE_RLLT_DIRECTIONAL:
					if (CurrentLight->ShadowMap != NULL)
					{
						DirectionalLightList[DirectionalLightCount] = CurrentLight;
						DirectionalLightCount++;
					}
					else
					{
						ShadowedDirectionalLightList[ShadowedDirectionalLightCount] = CurrentLight;
						ShadowedDirectionalLightCount++;
					}
					break;

				case ZE_RLLT_PROJECTIVE:
					if (CurrentLight->ShadowMap != NULL)
					{
						ProjectiveLightList[PointLightCount] = CurrentLight;
						ProjectiveLightCount++;
					}
					else
					{
						ShadowedProjectiveLightList[ShadowedPointLightCount] = CurrentLight;
						ShadowedProjectiveLightCount++;
					}
					break;

				case ZE_RLLT_OMNIPROJECTIVE:
					if (CurrentLight->ShadowMap != NULL)
					{
						OmniProjectiveLightList[OmniProjectiveLightCount] = CurrentLight;
						OmniProjectiveLightCount++;

					}
					else
					{
						ShadowedOmniProjectiveLightList[OmniProjectiveLightCount] = CurrentLight;
						ShadowedOmniProjectiveLightCount++;
					}
					break;
			}
		}
	
		if (PointLightCount != 0)
		{
			Material->SetupPointLightPass(false);			
			while (PointLightCount != 0)
			{
				PointLightCount -= Material->DoPointLightPass(PointLightList, PointLightCount);
				PumpStreams(RenderOrder);
			}
		}

		if (ShadowedPointLightCount != 0)
		{
			Material->SetupPointLightPass(true);			
			while (PointLightCount != 0)
			{
				ShadowedPointLightCount -= Material->DoPointLightPass(ShadowedPointLightList, PointLightCount);
				PumpStreams(RenderOrder);
			}
		}

		if (DirectionalLightCount != 0)
		{
			Material->SetupDirectionalLightPass(false);			
			while (DirectionalLightCount != 0)
			{
				DirectionalLightCount -= Material->DoDirectionalLightPass(DirectionalLightList, DirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}

		if (ShadowedDirectionalLightCount != 0)
		{
			Material->SetupDirectionalLightPass(true);			
			while (ShadowedDirectionalLightCount != 0)
			{
				ShadowedDirectionalLightCount -= Material->DoDirectionalLightPass(ShadowedDirectionalLightList, ShadowedDirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}


		if (ProjectiveLightCount != 0)
		{
			Material->SetupProjectiveLightPass(false);			
			while (ProjectiveLightCount != 0)
			{
				ProjectiveLightCount -= Material->DoProjectiveLightPass(ProjectiveLightList, ProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
		
		if (ShadowedProjectiveLightCount != 0)
		{
			Material->SetupProjectiveLightPass(true);			
			while (ShadowedProjectiveLightCount != 0)
			{
				ShadowedProjectiveLightCount -= Material->DoProjectiveLightPass(ShadowedProjectiveLightList, ShadowedProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
		
		if (OmniProjectiveLightCount != 0)
		{
			Material->SetupOmniProjectiveLightPass(false);			
			while (OmniProjectiveLightCount != 0)
			{
				OmniProjectiveLightCount -= Material->DoOmniProjectivePass(OmniProjectiveLightList, OmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		if (ShadowedOmniProjectiveLightCount != 0)
		{
			Material->SetupOmniProjectiveLightPass(true);			
			while (ShadowedOmniProjectiveLightCount != 0)
			{
				ShadowedOmniProjectiveLightCount -= Material->DoOmniProjectivePass(ShadowedOmniProjectiveLightList, ShadowedOmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
	}
}

bool ZED3D9RendererBase::Initialize() 
{ 
         return true; 
} 

void ZED3D9RendererBase::Deinitialize()
{
	ZED3D_RELEASE(RenderZBufferRT);
	ZED3D_RELEASE(RenderColorBufferRT);
}


void ZED3D9RendererBase::Destroy()
{
	Module->Renderers.DeleteValue((ZED3D9RendererBase*)this);
	delete this;
}

void ZED3D9RendererBase::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

void ZED3D9RendererBase::ClearList()
{
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9RendererBase::AddToRenderOrder(ZERenderOrder* RenderOrder)
{
	#ifdef ZEDEBUG_ENABLED
		if (RenderOrder->Material == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid material.");
			return;
		}
/*
		if (RenderOrder->Material->GetShader() == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid shader.");
			return;
		}*/

		if (RenderOrder->VertexDeclaration == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex declaration is invalid.");
			return;
		}

		if (RenderOrder->PrimitiveType != ZE_RLPT_POINT &&
			RenderOrder->PrimitiveType != ZE_RLPT_LINE &&
			RenderOrder->PrimitiveType != ZE_RLPT_TRIANGLE &&
			RenderOrder->PrimitiveType != ZE_RLPT_TRIANGLESTRIPT)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive type is not valid.");
			return;
		}

		if (RenderOrder->VertexBuffer == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex buffer is not valid.");
			return;
		}

		if (RenderOrder->PrimitiveCount == 0)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive count is zero.");
			return;
		}
	#endif

/*	if (RenderOrder->Lights.GetCount() != 0 && RenderOrder->Lights[0]->ShadowMap != NULL)
		return;*/
	if (RenderOrder->Flags & ZE_RLF_IMPOSTER)
		Imposter.Add(*RenderOrder);
	if (RenderOrder->Flags & ZE_RLF_TRANSPARENT)
		Transparent.Add(*RenderOrder);
	else
		NonTransparent.Add(*RenderOrder);
}

void ZED3D9RendererBase::Render(float ElaspedTime)
{

	if (!Module->IsEnabled() || Module->IsDeviceLost)
		return;

	Device->SetRenderTarget(0, RenderColorBufferRT);
	Device->SetDepthStencilSurface(RenderZBufferRT);

	Device->SetRenderState(D3DRS_DEPTHBIAS, 0);
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0x00000000);

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

	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	Device->BeginScene();
	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
		SetupRenderOrder(&NonTransparent[I]);

	for (size_t I = 0; I < Transparent.GetCount(); I++)
		SetupRenderOrder(&Transparent[I]);

	for (size_t I = 0; I < Imposter.GetCount(); I++)
		SetupRenderOrder(&Imposter[I]);

	Device->EndScene();

}

// Framebuffer renderer
bool ZED3D9FrameBufferRenderer::SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face)
{
	ZEASSERT(true, "You can not set output of a frame buffer renderer.");
	return false;
}

bool ZED3D9FrameBufferRenderer::SetOutput(ZETexture2D* Texture)
{
	ZEASSERT(true, "You can not set output of a frame buffer renderer.");
	return false;
}

void ZED3D9FrameBufferRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9FrameBufferRenderer::DeviceRestored()
{
	return Initialize();
}

bool ZED3D9FrameBufferRenderer::Initialize()
{
	if (Module->FrameColorBuffer == NULL || Module->FrameZBuffer == NULL)
	{
		zeError("D3D9 Renderer", "Can not intialize renderer. Direct3D9 module is not initialized.");
		return false;
	}

	Deinitialize();
	Module->FrameColorBuffer->AddRef();
	RenderColorBufferRT = Module->FrameColorBuffer;
	Module->FrameZBuffer->AddRef();
	RenderZBufferRT = Module->FrameZBuffer;
	return true;
}

// Offscreen
bool ZED3D9TextureRenderer::SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Offscreen Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	OutputCubeTexture = Texture;
	OutputCubeTextureFace = Face;
	OutputTexture = NULL;

	LPDIRECT3DCUBETEXTURE9 D3DTexture = ((ZED3D9CubeTexture*)Texture)->CubeTexture;
	ZED3D9CommonTools::CreateDepthRenderTarget(&RenderZBufferRT, Texture->GetEdgeLenght(), Texture->GetEdgeLenght());

	if (RenderColorBufferRT != NULL)
		RenderColorBufferRT->Release();
	
	D3DTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)Face, 0, &RenderColorBufferRT);
	return true;
}

bool ZED3D9TextureRenderer::SetOutput(ZETexture2D* Texture)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Offscreen Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	OutputCubeTexture = NULL;
	OutputTexture = Texture;

	LPDIRECT3DTEXTURE9 D3DTexture = ((ZED3D9Texture*)Texture)->Texture;
	ZED3D9CommonTools::CreateDepthRenderTarget(&RenderZBufferRT, Texture->GetWidth(), Texture->GetHeight());

	if (RenderColorBufferRT != NULL)
		RenderColorBufferRT->Release();

	D3DTexture->GetSurfaceLevel(0, &RenderColorBufferRT);	
	return true;
}

void ZED3D9TextureRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9TextureRenderer::DeviceRestored()
{
	if (OutputCubeTexture != NULL)
		return SetOutput(OutputCubeTexture, OutputCubeTextureFace);
	else if (OutputTexture != NULL)
		return SetOutput(OutputTexture);
}

ZED3D9TextureRenderer::ZED3D9TextureRenderer()
{
	OutputTexture = NULL;
	OutputCubeTexture = NULL;
}

// Shadow Renderer
#define ZE_MAX_SHADER_SOURCE_SIZE 32768

LPDIRECT3DVERTEXSHADER9 ShadowMapVS = NULL;
LPDIRECT3DVERTEXSHADER9 ShadowMapTextVS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapPS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapOpasityPS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapDiffuseAlphaPS = NULL;
	
bool ZED3D9ShadowRenderer::BaseInitialize()
{
	char SourceVS[ZE_MAX_SHADER_SOURCE_SIZE], SourcePS[ZE_MAX_SHADER_SOURCE_SIZE];
	ZEArray<D3DXMACRO> Macros;

	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.vs", SourceVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.ps", SourcePS, ZE_MAX_SHADER_SOURCE_SIZE);
	
	ZED3D9CommonTools::CompileVertexShader(&ShadowMapVS, SourceVS, "Shadow Renderer Shadow Pass", "vs_2_0", Macros.GetCArray());
	ZED3D9CommonTools::CompilePixelShader(&ShadowMapPS, SourcePS, "Shadow Renderer Shadow Pass", "ps_2_0", Macros.GetCArray());

	Macros.Add();
	Macros[0].Name = "ZESHADER_TRANSPARANT";
	Macros[0].Definition = "";
	Macros.Add();
	Macros[1].Name = NULL;
	Macros[1].Definition = NULL;
	ZED3D9CommonTools::CompileVertexShader(&ShadowMapTextVS, SourceVS, "Transparant Shadow Pass", "vs_2_0", Macros.GetCArray());

	Macros[1].Name = "ZESHADER_OPASITYMAP";
	Macros[1].Definition = "";
	Macros.Add();
	Macros[2].Name = NULL;
	Macros[2].Definition = NULL;
	ZED3D9CommonTools::CompilePixelShader(&ShadowMapOpasityPS, SourcePS, "Opasity Map Transparant Shadow Pass", "ps_2_0", Macros.GetCArray());

	Macros.Add();
	Macros[1].Name = "ZESHADER_DIFFUSEMAP";
	Macros[1].Definition = "";
	ZED3D9CommonTools::CompilePixelShader(&ShadowMapDiffuseAlphaPS, SourcePS, "Diffuse Map Alpha Transparant Shadow Pass", "ps_2_0", Macros.GetCArray());

	return true;
}

void ZED3D9ShadowRenderer::BaseDeinitialize()
{
	ZED3D_RELEASE(ShadowMapVS);
	ZED3D_RELEASE(ShadowMapTextVS);
	ZED3D_RELEASE(ShadowMapPS);
	ZED3D_RELEASE(ShadowMapOpasityPS);
	ZED3D_RELEASE(ShadowMapDiffuseAlphaPS);
}

void ZED3D9ShadowRenderer::DrawSM2(ZERenderOrder* RenderOrder)
{
	//const ZEMaterial* Material = RenderOrder->Material;

	//if (RenderOrder->Flags & ZE_RLF_SKINNED)
	//	Device->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);

	//if (RenderOrder->IndexBuffer != NULL)
	//	if (RenderOrder->IndexBuffer IsStaticIndexBuffer())
	//		Device->SetIndices(StaticIndexBuffers[RenderOrder->GetStaticIndexBufferId()]);	*/

	//if (RenderOrder->Flags & ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM)
	//{
	//	ZEMatrix4x4 WorldViewProjMatrix;
	//	ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, ViewPoint.ViewProjMatrix);
	//	Device->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	//}
	//else
	//	Device->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);


	//Device->SetVertexShaderConstantF(4, (float*)&RenderOrder->WorldMatrix, 4);
	//Device->SetVertexShaderConstantF(8, (float*)&RenderOrder->WorldMatrix, 4);
	//Device->SetVertexShaderConstantF(16, (float*)&ZEVector4(ViewPoint.ViewPosition, 1.0f), 1);

	//if (RenderOrder->Flags & ZE_RLF_ENABLE_ZCULLING)
	//{
	//	Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//	Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//}
	//else
	//	Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);*/
	//
	//if (Material->TwoSided)
	//	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//else
	//	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//if (Material->Wireframe)
	//	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//else
	//	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//Device->SetVertexDeclaration(VertexDeclarations[RenderOrder->VertexType]);
	//PumpStreams(RenderOrder);
}

bool ZED3D9ShadowRenderer::SetOutput(ZETextureCube* Texture, ZETextureCubeFace Face)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not render target.");
		return false;
	}

	
	OutputCubeTexture = Texture;
	OutputCubeTextureFace = Face;
	OutputTexture = NULL;

	if (Texture->GetPixelFormat() != ZE_TPF_DEPTH && Texture->GetPixelFormat() != ZE_TPF_SHADOWMAP)
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not a depth texture.");
		return false;
	}

	if (RenderZBufferRT != NULL)
	{
		RenderZBufferRT->Release();
		RenderZBufferRT = NULL;
	}

	LPDIRECT3DCUBETEXTURE9 D3DTexture = ((ZED3D9CubeTexture*)Texture)->CubeTexture;
	if (D3DTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)Face, 0, &RenderZBufferRT) != D3D_OK || RenderZBufferRT == NULL)
	{
		zeError("D3D9 Shadow Renderer", "Can not get shadow map surface.");
		return false;
	}


	if (!ZED3D9CommonTools::CreateRenderTarget(&RenderColorBufferRT, Texture->GetEdgeLenght(), Texture->GetEdgeLenght(), ZE_TPF_ARGB32))
		return false;
	
	return true;
}

bool ZED3D9ShadowRenderer::SetOutput(ZETexture2D* Texture)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not render target.");
		return false;
	}

	OutputCubeTexture = NULL;
	OutputTexture = Texture;

	if (Texture->GetPixelFormat() != ZE_TPF_DEPTH && Texture->GetPixelFormat() != ZE_TPF_SHADOWMAP)
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not a depth texture.");
		return false;
	}

	LPDIRECT3DTEXTURE9 D3DTexture = ((ZED3D9Texture*)Texture)->Texture;
	if (RenderZBufferRT != NULL)
	{
		RenderZBufferRT->Release();
		RenderZBufferRT = NULL;
	}

	if (D3DTexture->GetSurfaceLevel(0, &RenderZBufferRT) != D3D_OK || RenderZBufferRT == NULL)
	{
		zeError("D3D9 Shadow Renderer", "Can not get shadow map surface.");
		return false;
	}

	if (!ZED3D9CommonTools::CreateRenderTarget(&RenderColorBufferRT, Texture->GetWidth(), Texture->GetHeight(), ZE_TPF_ARGB32))
		return false;
	
	return true;
}

void ZED3D9ShadowRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9ShadowRenderer::DeviceRestored()
{
	if (OutputCubeTexture != NULL)
		return SetOutput(OutputCubeTexture, OutputCubeTextureFace);
	else if (OutputTexture != NULL)
		return SetOutput(OutputTexture);

	return true;
}

void ZED3D9ShadowRenderer::Render(float ElaspedTime)
{
	if (!Module->IsEnabled() || Module->IsDeviceLost)
		return;

	Device->SetRenderTarget(0, RenderColorBufferRT);
	Device->SetDepthStencilSurface(RenderZBufferRT);
	
	Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0x00000000);

	float BiasValue;
	BiasValue = 0.0001f;//2.0f / 16777215.0f;;
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&BiasValue);

	BiasValue = 0.002f;//2.0f / 16777215.0f;;
	Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&BiasValue);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	Device->SetVertexShader(ShadowMapVS);
	Device->SetPixelShader(ShadowMapPS);

	Device->BeginScene();
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			DrawSM2(&NonTransparent[I]);

		if (Transparent.GetCount() != 0)
		{
			Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			Device->SetRenderState(D3DRS_ALPHAREF, 0x01);

			Device->SetVertexShader(ShadowMapTextVS);
			Device->SetPixelShader(ShadowMapOpasityPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
			{
				if (((ZEFixedMaterial*)Transparent[I].Material)->OpacityMap != NULL)
					SetupRenderOrder(&Transparent[I]);
			}


			Device->SetPixelShader(ShadowMapDiffuseAlphaPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
			{
				if (((ZEFixedMaterial*)Transparent[I].Material)->OpacityMap == NULL)
					SetupRenderOrder(&Transparent[I]);
			}
		}
	Device->EndScene();
//	Module->CurrentFrameId++;
}

ZED3D9ShadowRenderer::ZED3D9ShadowRenderer()
{
	OutputTexture = NULL;
	OutputCubeTexture = NULL;
}
