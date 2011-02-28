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

#include "ZED3D9ShadowRenderer.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9Module.h"
#include "ZED3D9CommonTools.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Shader.h"
#include "ZEGraphics/ZERenderOrder.h"
#include "ZEGraphics/ZEDirectionalLight.h"
#include "ZEGraphics/ZEPointLight.h"
#include "ZEGraphics/ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZEProjectiveLight.h"
#include "ZECore/ZEError.h"
#include "ZECore/ZEResourceFile.h"
#include "ZEMath/ZEMathDefinitions.h"

ZED3D9VertexShader* OmniLightVS = NULL;
ZED3D9PixelShader* OmniLightPS = NULL;
ZED3D9VertexShader* DirectionalLightVS = NULL;
ZED3D9PixelShader* DirectionalLightPS = NULL;
ZED3D9VertexShader* ProjectiveLightVS = NULL;
ZED3D9PixelShader* ProjectiveLightPS = NULL;

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

void ZED3D9ShadowRenderer::SetViewPort(ZEViewPort* ViewPort)
{
	this->ViewPort = (ZED3D9ViewPort*)ViewPort;
}

ZEViewPort* ZED3D9ShadowRenderer::GetViewPort()
{
	return ViewPort;
}

void ZED3D9ShadowRenderer::SetFace(bool Front)
{
	Face = Front;
}

bool ZED3D9ShadowRenderer::GetFace()
{
	return Face;
}

void ZED3D9ShadowRenderer::SetLight(ZELight* Light)
{
	this->Light = Light;
}

ZELight* ZED3D9ShadowRenderer::GetLight()
{
	return Light;
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
	Initialize();
	return true;
}

bool ZED3D9ShadowRenderer::Initialize()
{
	Deinitialize();

	OmniLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "OmniSMVSMain", 0, "vs_3_0");
	OmniLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "OmniSMPSMain", 0, "ps_3_0");

	//DirectionalLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "DirectionalVSMain", 0, "vs_3_0");
	//DirectionalLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "DirectionalPSMain", 0, "ps_3_0");

	ProjectiveLightVS = ZED3D9VertexShader::CreateShader("ShadowMaterial.hlsl", "ProjectiveSMVSMain", 0, "vs_3_0");
	ProjectiveLightPS = ZED3D9PixelShader::CreateShader("ShadowMaterial.hlsl", "ProjectiveSMPSMain", 0, "ps_3_0");

	if (ShadowMapFrameBuffer == NULL)
	{
		HRESULT hr = GetDevice()->CreateRenderTarget(512, 512, D3DFMT_R32F, D3DMULTISAMPLE_NONE, 0, FALSE, &ShadowMapFrameBuffer, NULL);
		if (hr != S_OK)
		{
			zeError("D3D9 Shadow Renderer", "Can not create shadow map frame buffer.");
			return false;
		}
	}

	if (ShadowMapZBuffer == NULL)
	{
		HRESULT hr = GetDevice()->CreateDepthStencilSurface(512, 512, D3DFMT_D24X8, D3DMULTISAMPLE_NONE, 0, FALSE, &ShadowMapZBuffer, NULL);
		if (hr != S_OK)
		{
			zeError("D3D9 Shadow Renderer", "Can not create shadow map z buffer.");
			return false;
		}
	}

	return true;
}

void ZED3D9ShadowRenderer::Deinitialize()
{
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(OmniLightVS);
	ZED3D_RELEASE(DirectionalLightVS);
	ZED3D_RELEASE(DirectionalLightPS);
	ZED3D_RELEASE(ProjectiveLightVS);
	ZED3D_RELEASE(ProjectiveLightVS);

	ZED3D_RELEASE(ShadowMapFrameBuffer);
	ZED3D_RELEASE(ShadowMapZBuffer);
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
		if (!ZED3D9FrameRenderer::CheckRenderOrder(RenderOrder))
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

void ZED3D9ShadowRenderer::RenderProjectiveLight()
{
	ZEProjectiveLight* Light = (ZEProjectiveLight*)this->Light;
	GetDevice()->SetRenderTarget(0, ShadowMapFrameBuffer);
	GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetShadowMap()->GetViewPort())->FrameBuffer);
	float MaxFloat = 1.0f;
	GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, *(int*)&MaxFloat, 1.0f, 0x00);

	GetDevice()->SetVertexShader(ProjectiveLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(ProjectiveLightPS->GetPixelShader());
	
	ZEMatrix4x4 ViewTransform, ProjectionTransform, ViewProjectionTransform;
	ZEMatrix4x4::CreateViewTransform(ViewTransform, Light->GetPosition(), Light->GetRotation());
	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, Light->GetFOV(), Light->GetAspectRatio(), zeGraphics->GetNearZ(), Light->GetRange());

	ZEMatrix4x4::Multiply(ViewProjectionTransform, ViewTransform, ProjectionTransform);
	GetDevice()->BeginScene();
	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];

		ZEMatrix4x4 ViewProjMatrix;
		if ((RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM) == ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
			ViewProjMatrix = ViewProjectionTransform;
		else if (RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
			ViewProjMatrix = ViewTransform;
		else if (RenderOrder->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
			ViewProjMatrix = ProjectionTransform;
		else
			ViewProjMatrix = ZEMatrix4x4::Identity;

		ZEMatrix4x4 WorldViewProjMatrix;
		if (RenderOrder->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
			ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, ViewProjMatrix);
		else
			WorldViewProjMatrix = ViewProjectionTransform;

		GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);

		// Setup ZCulling
		if (RenderOrder->Flags & ZE_ROF_ENABLE_Z_CULLING)
		{
			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			if (RenderOrder->Flags & (ZE_ROF_TRANSPARENT | ZE_ROF_IMPOSTER))
				GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			else
				GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		// Setup Bone Transforms
		if (RenderOrder->Flags & ZE_ROF_SKINNED && RenderOrder->BoneTransforms.GetCount() < 58)
			GetDevice()->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);

		ZED3D9FrameRenderer::PumpStreams(RenderOrder);
	}
	GetDevice()->EndScene();
}

void ZED3D9ShadowRenderer::RenderDirectionalLight()
{

}

void ZED3D9ShadowRenderer::RenderPointLight()
{
	ZEPointLight* Light = (ZEPointLight*)this->Light;

	GetDevice()->SetDepthStencilSurface(ShadowMapZBuffer);
	if (Face)
		GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetFrontShadowMap()->GetViewPort())->FrameBuffer);
	else
		GetDevice()->SetDepthStencilSurface(((ZED3D9ViewPort*)Light->GetBackShadowMap()->GetViewPort())->FrameBuffer);

	float MaxFloat = FLT_MAX;
	GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, *(int*)&MaxFloat, 1.0f, 0x00);

	GetDevice()->SetVertexShader(OmniLightVS->GetVertexShader());
	GetDevice()->SetPixelShader(OmniLightPS->GetPixelShader());
	
	ZEMatrix4x4 ViewTransform, ProjectionTransform, ViewProjectionTransform;
	ZEMatrix4x4::CreateViewTransform(ViewTransform, Light->GetPosition(), (Face ? Light->GetRotation() : Light->GetRotation() * ZEQuaternion(ZE_PI, ZEVector3(0.0f, 1.0f, 0.0f))));
//	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, Light->GetFOV(), Light->GetAspectRatio(), zeGraphics->GetNearZ(), Light->GetRange());
	ProjectionTransform = ZEMatrix4x4::Identity;

	ZEMatrix4x4::Multiply(ViewProjectionTransform, ViewTransform, ProjectionTransform);

	ZEVector4 Options = ZEVector4(Light->GetRange(), 0.0f, 0.0f, 0.0f);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&Options, 1);

	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFF);
	GetDevice()->BeginScene();
	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
	{
		ZERenderOrder* RenderOrder = &NonTransparent[I];

		ZEMatrix4x4 ViewProjMatrix;
		if ((RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM) == ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
			ViewProjMatrix = ViewProjectionTransform;
		else if (RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
			ViewProjMatrix = ViewTransform;
		else if (RenderOrder->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
			ViewProjMatrix = ProjectionTransform;
		else
			ViewProjMatrix = ZEMatrix4x4::Identity;

		ZEMatrix4x4 WorldViewProjMatrix;
		if (RenderOrder->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
			ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, ViewProjMatrix);
		else
			WorldViewProjMatrix = ViewProjectionTransform;

		GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);

		// Setup ZCulling
		if (RenderOrder->Flags & ZE_ROF_ENABLE_Z_CULLING)
		{
			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			if (RenderOrder->Flags & (ZE_ROF_TRANSPARENT | ZE_ROF_IMPOSTER))
				GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			else
				GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
			GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		// Setup Bone Transforms
		if (RenderOrder->Flags & ZE_ROF_SKINNED && RenderOrder->BoneTransforms.GetCount() < 58)
			GetDevice()->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);

		ZED3D9FrameRenderer::PumpStreams(RenderOrder);
	}
	GetDevice()->EndScene();
}


void ZED3D9ShadowRenderer::Render(float ElaspedTime)
{
	GetDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	switch(Light->GetLightType())
	{
		case ZE_LT_POINT:
			RenderPointLight();
			return;
		case ZE_LT_OMNIPROJECTIVE:
			//RenderOmniProjectiveLight();
			return;
		case ZE_LT_DIRECTIONAL:
			return;

		case ZE_LT_PROJECTIVE:

			RenderProjectiveLight();
			break;
	}
	GetDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFF);
}

ZED3D9ShadowRenderer::ZED3D9ShadowRenderer()
{
	ShadowMapFrameBuffer = NULL;
	ShadowMapZBuffer = NULL;
}

ZED3D9ShadowRenderer::~ZED3D9ShadowRenderer()
{

}



