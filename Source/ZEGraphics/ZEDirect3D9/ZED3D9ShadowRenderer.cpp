//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9ShadowRenderer.cpp
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

#include "ZED3D9Module.h"
#include "ZED3D9ShadowRenderer.h"
#include "ZED3D9Renderer.h"
#include "ZED3D9CommonTools.h"
#include "ZECore\ZEResourceFile.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Texture2D.h"
#include "ZECore\ZEError.h"
#include "ZEGraphics\ZERenderOrder.h"

ZED3D9VertexShader* OmniLightVS = NULL;
ZED3D9PixelShader* OmniLightPS = NULL;
ZED3D9VertexShader* DirectionalLightVS = NULL;
ZED3D9PixelShader* DirectionalLightPS = NULL;
ZED3D9VertexShader* ProjectiveLightVS = NULL;
ZED3D9PixelShader* ProjectiveLightPS = NULL;
	
bool ZED3D9ShadowRenderer::InitializeShaders()
{
	OmniLightVS = ZED3D9VertexShader::CreateShader("Shadows.hlsl", "OmniVSMain", 0, "vs_3_0");
	OmniLightPS = ZED3D9PixelShader::CreateShader("Shadows.hlsl", "OmniPSMain", 0, "ps_3_0");

	DirectionalLightVS = ZED3D9VertexShader::CreateShader("Shadows.hlsl", "DirectionalVSMain", 0, "vs_3_0");
	DirectionalLightPS = ZED3D9PixelShader::CreateShader("Shadows.hlsl", "DirectionalPSMain", 0, "ps_3_0");

	ProjectiveLightVS = ZED3D9VertexShader::CreateShader("Shadows.hlsl", "ProjectiveVSMain", 0, "vs_3_0");
	ProjectiveLightPS = ZED3D9PixelShader::CreateShader("Shadows.hlsl", "ProjectivePSMain", 0, "ps_3_0");

	return true;
}

void ZED3D9ShadowRenderer::DeinitializeShaders()
{
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(DirectionalLightVS);
	ZED3D_RELEASE(DirectionalLightPS);
	ZED3D_RELEASE(ProjectiveLightVS);
	ZED3D_RELEASE(ProjectiveLightVS);
}

void ZED3D9ShadowRenderer::DrawRenderOrder(ZERenderOrder* RenderOrder)
{
	//const ZEMaterial* Material = RenderOrder->Material;

	//if (RenderOrder->Flags & ZE_ROF_SKINNED)
	//	GetDevice()->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);

	//if (RenderOrder->IndexBuffer != NULL)
	//	if (RenderOrder->IndexBuffer IsStaticIndexBuffer())
	//		GetDevice()->SetIndices(StaticIndexBuffers[RenderOrder->GetStaticIndexBufferId()]);	*/

	//if (RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	//{
	//	ZEMatrix4x4 WorldViewProjMatrix;
	//	ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, ViewPoint.ViewProjMatrix);
	//	GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	//}
	//else
	//	GetDevice()->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);


	//GetDevice()->SetVertexShaderConstantF(4, (float*)&RenderOrder->WorldMatrix, 4);
	//GetDevice()->SetVertexShaderConstantF(8, (float*)&RenderOrder->WorldMatrix, 4);
	//GetDevice()->SetVertexShaderConstantF(16, (float*)&ZEVector4(ViewPoint.ViewPosition, 1.0f), 1);

	//if (RenderOrder->Flags & ZE_ROF_ENABLE_Z_CULLING)
	//{
	//	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	//	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//}
	//else
	//	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);*/
	//
	//if (Material->TwoSided)
	//	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//else
	//	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//if (Material->Wireframe)
	//	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//else
	//	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//GetDevice()->SetVertexDeclaration(VertexDeclarations[RenderOrder->VertexType]);
	//PumpStreams(RenderOrder);
}

void ZED3D9ShadowRenderer::SetLight(ZELight* Light)
{
}

void ZED3D9ShadowRenderer::SetViewPort(ZEViewPort* ViewPort)
{
	this->ViewPort = (ZED3D9ViewPort*)ViewPort;
}

ZEViewPort* ZED3D9ShadowRenderer::GetViewPort()
{
	return ViewPort;
}

ZEArray<ZEPostProcessor*>& ZED3D9ShadowRenderer::GetPostProcessors()
{
	return PostProcessors;
}

void ZED3D9ShadowRenderer::AddPostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.Add(PostProcessor);
}

void ZED3D9ShadowRenderer::RemovePostProcessor(ZEPostProcessor* PostProcessor)
{
	PostProcessors.DeleteValue(PostProcessor);
}


void ZED3D9ShadowRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9ShadowRenderer::DeviceRestored()
{
	return true;
}

bool ZED3D9ShadowRenderer::Initialize()
{
	return true;
}

void ZED3D9ShadowRenderer::Deinitialize()
{

}

void ZED3D9ShadowRenderer::Destroy()
{
	// Remove renderer from modules renderer list
	GetModule()->ShadowRenderers.DeleteValue((ZED3D9ShadowRenderer*)this);
	ZERenderer::Destroy();
}


void ZED3D9ShadowRenderer::ClearList()
{
	//Clear render lists
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9ShadowRenderer::AddToRenderList(ZERenderOrder* RenderOrder)
{
	#ifdef ZE_DEBUG_ENABLED
		// Check render order is valid
		if (!ZED3D9Renderer::CheckRenderOrder(RenderOrder))
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

void ZED3D9ShadowRenderer::Render(float ElaspedTime)
{

	switch(Light->GetLightType())
	{
		case ZE_LT_POINT:
		case ZE_LT_OMNIPROJECTIVE:
		case ZE_LT_DIRECTIONAL:
			return;

		case ZE_LT_PROJECTIVE:
			GetDevice()->SetPixelShader(ProjectiveLightPS->)
	}

/*	if (!GetModule()->IsEnabled() || GetModule()->IsDeviceLost)
		return;

	GetDevice()->SetRenderTarget(0, ColorRenderTarget);
	GetDevice()->SetDepthStencilSurface(DepthRenderTarget);
	
	GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0x00000000);

	float BiasValue;
	BiasValue = 0.0001f;//2.0f / 16777215.0f;;
	GetDevice()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&BiasValue);

	BiasValue = 0.002f;//2.0f / 16777215.0f;;
	GetDevice()->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&BiasValue);

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	GetDevice()->SetVertexShader(ShadowMapVS);
	GetDevice()->SetPixelShader(ShadowMapPS);

	GetDevice()->BeginScene();
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			DrawSM2(&NonTransparent[I]);

		if (Transparent.GetCount() != 0)
		{
			GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x01);

			GetDevice()->SetVertexShader(ShadowMapTextVS);
			GetDevice()->SetPixelShader(ShadowMapOpasityPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
				if (((ZEFixedMaterial*)Transparent[I].Material)->OpacityMap != NULL)
					DrawRenderOrder(&Transparent[I]);


			GetDevice()->SetPixelShader(ShadowMapDiffuseAlphaPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
				if (((ZEFixedMaterial*)Transparent[I].Material)->OpacityMap == NULL)
					DrawRenderOrder(&Transparent[I]);
		}
	GetDevice()->EndScene();
//	GetModule()->CurrentFrameId++;*/
}


ZED3D9ShadowRenderer::ZED3D9ShadowRenderer()
{

}

ZED3D9ShadowRenderer::~ZED3D9ShadowRenderer()
{

}



