//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9FixedMaterial.cpp
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

#include "ZED3D9FixedMaterial.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Shader.h"
#include "ZED3D9CommonTools.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERenderCommand.h"
#include "ZERenderer/ZEMaterialComponents.h"
#include "ZEGame/ZEScene.h"

void ZED3D9FixedMaterial::CreateShaders()
{
	ReleaseShaders();

	if (TransparancyMode == ZE_MTM_NONE)
	{
		GBufferPassVertexShader = ZED3D9VertexShader::CreateShader("FixedMaterial.hlsl", "ZEFixedMaterial_GBuffer_VertexShader", MaterialComponents, "vs_3_0");
		GBufferPassPixelShader = ZED3D9PixelShader::CreateShader("FixedMaterial.hlsl", "ZEFixedMaterial_GBuffer_PixelShader", MaterialComponents, "ps_3_0");
	}
	else
	{
		GBufferPassVertexShader = NULL;
		GBufferPassPixelShader = NULL;
	}
	
	ForwardPassVertexShader = ZED3D9VertexShader::CreateShader("FixedMaterial.hlsl", "ZEFixedMaterial_ForwardPass_VertexShader", MaterialComponents, "vs_3_0");
	ForwardPassPixelShader = ZED3D9PixelShader::CreateShader("FixedMaterial.hlsl", "ZEFixedMaterial_ForwardPass_PixelShader", MaterialComponents, "ps_3_0");
}

void ZED3D9FixedMaterial::ReleaseShaders()
{

	ZED3D_RELEASE(GBufferPassVertexShader);
	ZED3D_RELEASE(GBufferPassPixelShader);

	ZED3D_RELEASE(ForwardPassVertexShader);
	ZED3D_RELEASE(ForwardPassPixelShader);

	ZED3D_RELEASE(ShadowPassVertexShader);
	ZED3D_RELEASE(ShadowPassPixelShader);
}

ZED3D9FixedMaterial::~ZED3D9FixedMaterial()
{
	Release();
}

ZED3D9FixedMaterial::ZED3D9FixedMaterial()
{
	GBufferPassVertexShader = NULL;
	GBufferPassPixelShader = NULL;
	ForwardPassVertexShader = NULL;
	ForwardPassPixelShader = NULL;
	ShadowPassVertexShader = NULL;
	ShadowPassPixelShader = NULL;
}


void ZED3D9FixedMaterial::SetTextureStage(ZEUInt Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV) const
{
	GetDevice()->SetSamplerState(Id, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(Id, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(Id, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	switch(AddressU)
	{
		case ZE_TAM_WRAP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressV)
	{
		case ZE_TAM_WRAP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			break;
	}

}

void ZED3D9FixedMaterial::SetTextureStage(ZEUInt Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV, ZETextureAddressMode AddressW) const
{
	switch(AddressU)
	{
		case ZE_TAM_WRAP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressV)
	{
		case ZE_TAM_WRAP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressW)
	{
		case ZE_TAM_WRAP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			GetDevice()->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
			break;
	}

}

bool ZED3D9FixedMaterial::SetupGBufferPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9FixedMaterial*)this)->UpdateMaterial();

	ZEUInt32 MaterialComponents = this->MaterialComponents & MaterialComponentMask;

	ZECamera* Camera = Renderer->GetCamera();

	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}

	ZEMatrix4x4 WorldViewProjMatrix;
	ZEMatrix4x4 WorldViewMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
		ZEMatrix4x4::Multiply(WorldViewMatrix, Camera->GetViewTransform(), RenderCommand->WorldMatrix);
	}
	else
	{
		WorldViewProjMatrix = ViewProjMatrix;
		WorldViewMatrix = Camera->GetViewTransform();
	}

	GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(8, (float*)&WorldViewMatrix, 4);

	/*
	if (GetAlphaCullEnabled())
	{
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		GetDevice()->SetRenderState(D3DRS_ALPHAREF, (ZEInt)(256.0f * AlphaCullLimit));
		GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		//GetDevice()->SetRenderState(D3DRS_ADAPTIVETESS_Y, (D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C'));
	}
	else
	*/
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// Setup ZCulling
	if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
	{
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
	}
	else
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	// Setup Back Face Culling
	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Setup Wireframe
	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// Setup Bone Transforms
	BOOL SkinEnabled = false;
	if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
	{
		GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
		SkinEnabled = true;
	}

	GetDevice()->SetVertexShaderConstantB(0, &SkinEnabled, 1);

	// Setup Material Properties
	//GetDevice()->SetVertexShaderConstantF(14, (const float*)VertexShaderConstants, sizeof(VertexShaderConstants) / 16);
	GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4(1.0f / (float)Renderer->GetViewPort()->GetWidth(), 1.0f / (float)Renderer->GetViewPort()->GetHeight(), 0.5f / (float)Renderer->GetViewPort()->GetWidth(), 0.5f / (float)Renderer->GetViewPort()->GetHeight()), 1);
	GetDevice()->SetPixelShaderConstantF(10, (const float*)PixelShaderConstants, sizeof(PixelShaderConstants) / 16);
	
	// Setup Textures

	if ((GetAlphaCullEnabled() || TransparancyMode != ZE_MTM_NONE) && OpacityComponent == ZE_MOC_OPACITY_MAP && MaterialComponents & ZE_SHADER_OPACITY_MAP && Opacity != NULL && OpacityMap != NULL)
	{
		SetTextureStage(9, OpacityMapAddressModeU, OpacityMapAddressModeV);
		GetDevice()->SetTexture(9, ((ZED3D9Texture2D*)OpacityMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_NORMAL_MAP && NormalMap != NULL)
	{
		SetTextureStage(6, NormalMapAddressModeU, NormalMapAddressModeV);
		GetDevice()->SetTexture(6, ((ZED3D9Texture2D*)NormalMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_PARALLAX_MAP && ParallaxMap != NULL)
	{
		SetTextureStage(7, ParallaxMapAddressModeU, ParallaxMapAddressModeV);
		GetDevice()->SetTexture(7, ((ZED3D9Texture2D*)ParallaxMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_SPECULAR_MAP && SpecularMap != NULL)
	{
		SetTextureStage(8, SpecularMapAddressModeU, SpecularMapAddressModeV);
		GetDevice()->SetTexture(8, ((ZED3D9Texture2D*)SpecularMap)->Texture);
	}

	if ((GetAlphaCullEnabled() || TransparancyMode != ZE_MTM_NONE) && OpacityComponent == ZE_MOC_BASE_MAP_ALPHA && MaterialComponents & ZE_SHADER_BASE_MAP && BaseMap != NULL)
	{
		SetTextureStage(5, BaseMapAddressModeU, BaseMapAddressModeV);
		GetDevice()->SetTexture(5, ((ZED3D9Texture2D*)BaseMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_DETAIL_NORMAL_MAP && DetailNormalMap != NULL)
	{
		SetTextureStage(15, DetailNormalMapAddressModeU, DetailNormalMapAddressModeV);
		GetDevice()->SetTexture(15, ((ZED3D9Texture2D*)DetailNormalMap)->Texture);
	}

	GetDevice()->SetPixelShader(GBufferPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(GBufferPassVertexShader->GetVertexShader());

	return true;
}

bool ZED3D9FixedMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9FixedMaterial*)this)->UpdateMaterial();

	ZEUInt32 MaterialComponents = this->MaterialComponents & MaterialComponentMask;

	ZECamera* Camera = Renderer->GetCamera();

	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}

	ZEMatrix4x4 WorldViewMatrix;
	ZEMatrix4x4 WorldViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
		ZEMatrix4x4::Multiply(WorldViewMatrix, Camera->GetViewTransform(), RenderCommand->WorldMatrix);
	}
	else
	{
		WorldViewProjMatrix = ViewProjMatrix;
		WorldViewMatrix = Camera->GetViewTransform();
	}

	GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&WorldViewMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(8, (float*)&WorldViewMatrix, 4);

	// Setup ZCulling
	if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
	{
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
	}
	else
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	// Setup Backface Culling
	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	/*
	if (GetAlphaCullEnabled())
	{
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		GetDevice()->SetRenderState(D3DRS_ALPHAREF, (ZEInt)(256.0f * AlphaCullLimit));
		GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		GetDevice()->SetRenderState(D3DRS_ADAPTIVETESS_Y, (D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C'));
	}
	else
	*/
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// Setup Wireframe
	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// Setup Transparancy
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Setup Bone Transforms
	BOOL SkinEnabled = false;
	if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
	{
		GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
		SkinEnabled = true;
	}
	
	GetDevice()->SetVertexShaderConstantB(0, &SkinEnabled, 1);

	// Setup Transparancy
	if (TransparancyMode != ZE_MTM_NONE)
	{
		switch(TransparancyMode)
		{
			case ZE_MTM_ADDAPTIVE:
				GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_MTM_SUBTRACTIVE:
				GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);
				GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_MTM_REGULAR:
				GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
		}

	}
	else
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Setup Material Properties
	//GetDevice()->SetVertexShaderConstantF(14, (const float*)VertexShaderConstants, sizeof(VertexShaderConstants) / 16);
	GetDevice()->SetPixelShaderConstantF(9, (const float*)&ZEVector4(1.0f / (float)Renderer->GetViewPort()->GetWidth(), 1.0f / (float)Renderer->GetViewPort()->GetHeight(), 0.5f / (float)Renderer->GetViewPort()->GetWidth(), 0.5f / (float)Renderer->GetViewPort()->GetHeight()), 1);
	GetDevice()->SetPixelShaderConstantF(10, (const float*)PixelShaderConstants, sizeof(PixelShaderConstants) / 16);

	// Setup Textures
	if (MaterialComponents & ZE_SHADER_BASE_MAP && BaseMap != NULL)
	{
		SetTextureStage(5, BaseMapAddressModeU, BaseMapAddressModeV);
		GetDevice()->SetTexture(5, ((ZED3D9Texture2D*)BaseMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_NORMAL_MAP && NormalMap != NULL)
	{
		SetTextureStage(6, NormalMapAddressModeU, NormalMapAddressModeV);
		GetDevice()->SetTexture(6, ((ZED3D9Texture2D*)NormalMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_PARALLAX_MAP && ParallaxMap != NULL)
	{
		SetTextureStage(7, ParallaxMapAddressModeU, ParallaxMapAddressModeV);
		GetDevice()->SetTexture(7, ((ZED3D9Texture2D*)ParallaxMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_SPECULAR_MAP && SpecularMap != NULL)
	{
		SetTextureStage(8, SpecularMapAddressModeU, SpecularMapAddressModeV);
		GetDevice()->SetTexture(8, ((ZED3D9Texture2D*)SpecularMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_OPACITY_MAP && Opacity != NULL && OpacityMap != NULL)
	{
		SetTextureStage(9, OpacityMapAddressModeU, OpacityMapAddressModeV);
		GetDevice()->SetTexture(9, ((ZED3D9Texture2D*)OpacityMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_EMMISIVE_MAP && EmmisiveMap != NULL)
	{
		SetTextureStage(10, EmmisiveMapAddressModeU, EmmisiveMapAddressModeV);
		GetDevice()->SetTexture(10, ((ZED3D9Texture2D*)EmmisiveMap)->Texture);
	}

	if ((MaterialComponents & ZE_SHADER_REFLECTION || MaterialComponents & ZE_SHADER_REFRACTION) && EnvironmentMap != NULL)
	{
		SetTextureStage(11, EnvironmentMapAddressModeU, EnvironmentMapAddressModeV, EnvironmentMapAddressModeW);
		GetDevice()->SetTexture(11, ((ZED3D9TextureCube*)EnvironmentMap)->CubeTexture);
	}

	if (MaterialComponents & ZE_SHADER_LIGHT_MAP && LightMap != NULL)
	{
		SetTextureStage(12, LightMapAddressModeU, LightMapAddressModeV);
		GetDevice()->SetTexture(12, ((ZED3D9Texture2D*)LightMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_DISTORTION_MAP && DistortionMap != NULL)
	{
		SetTextureStage(13, DistortionMapAddressModeU, DistortionMapAddressModeV);
		GetDevice()->SetTexture(13, ((ZED3D9Texture2D*)DistortionMap)->Texture);
	}
	
	if (MaterialComponents & ZE_SHADER_DETAIL_BASE_MAP && DetailBaseMap != NULL)
	{
		SetTextureStage(14, DetailBaseMapAddressModeU, DetailBaseMapAddressModeV);
		GetDevice()->SetTexture(14, ((ZED3D9Texture2D*)DetailBaseMap)->Texture);
	}

	if (MaterialComponents & ZE_SHADER_DETAIL_NORMAL_MAP && DetailNormalMap != NULL)
	{
		SetTextureStage(15, DetailNormalMapAddressModeU, DetailNormalMapAddressModeV);
		GetDevice()->SetTexture(15, ((ZED3D9Texture2D*)DetailNormalMap)->Texture);
	}

	GetDevice()->SetPixelShader(ForwardPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(ForwardPassVertexShader->GetVertexShader());
	
	return true;
}

bool ZED3D9FixedMaterial::SetupShadowPass() const
{
	return false;
}

void ZED3D9FixedMaterial::UpdateMaterial()
{
	if(GetGlobalAmbientEnabled())
	{
		SetAmbientEnabled(true);
		SetAmbientFactor(zeScene->GetAmbientFactor());
		SetAmbientColor(zeScene->GetAmbientColor());
	}

	if ((MaterialComponents & MaterialComponentMask) != OldMaterialComponents)
	{
		CreateShaders();
		OldMaterialComponents = MaterialComponents & MaterialComponentMask;
	}
}

void ZED3D9FixedMaterial::Release()
{
	ReleaseShaders();
}
