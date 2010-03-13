//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9FixedMaterial.cpp
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

#include "D3D9FixedMaterial.h"
#include "Graphics/RenderOrder.h"
#include "D3D9Texture2D.h"
#include "D3D9TextureCube.h"
#include "Graphics/Camera.h"
#include "Graphics/MaterialComponents.h"

ZED3D9FixedMaterial::~ZED3D9FixedMaterial()
{
	Release();
}

ZED3D9FixedMaterial::ZED3D9FixedMaterial()
{
	Shader = NULL;
}

void ZED3D9FixedMaterial::SetShaderPass(ZED3D9FixedMateriaShaderPass* Pass, bool Skinned) const 
{
	if (Skinned)
		GetDevice()->SetVertexShader(Pass->SkinnedVertexShader);
	else
		GetDevice()->SetVertexShader(Pass->VertexShader);

	GetDevice()->SetPixelShader(Pass->PixelShader);
}

void ZED3D9FixedMaterial::SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV) const
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

}

void ZED3D9FixedMaterial::SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV, ZETextureAddressMode AddressW) const
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

const char* ZED3D9FixedMaterial::GetMaterialUID() const 
{
	return "";
}

unsigned int ZED3D9FixedMaterial::GetMaterialFlags() const 
{
	return NULL;
}

ZEMaterialType ZED3D9FixedMaterial::GetMaterialType() const 
{
	return ZE_MT_FIXED;
}

bool ZED3D9FixedMaterial::SetupMaterial(ZERenderOrder* RenderOrder, ZECamera* Camera) const 
{
	((ZED3D9FixedMaterial*)this)->UpdateMaterial();
	 
	((ZED3D9FixedMaterial*)this)->RenderOrder = RenderOrder;
	((ZED3D9FixedMaterial*)this)->Camera = Camera;

	if (RenderOrder->Flags & ZE_ROF_SKINNED)
		GetDevice()->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);
	
	GetDevice()->SetVertexShaderConstantF(12, (const float*)VertexShaderConstants, sizeof(VertexShaderConstants));
	GetDevice()->SetPixelShaderConstantF(0, (const float*)PixelShaderConstants, sizeof(PixelShaderConstants));
	
	if (RenderOrder->Flags & (ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM & ZE_ROF_ENABLE_WORLD_TRANSFORM))
	{
		ZEMatrix4x4 WorldViewProjMatrix;
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, Camera->GetViewProjectionTransform());
		GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	}
	else if (RenderOrder->Flags & ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM)
		GetDevice()->SetVertexShaderConstantF(0, (float*)&Camera->GetViewProjectionTransform(), 4);
	else if (RenderOrder->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
		GetDevice()->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);
	else
		GetDevice()->SetVertexShaderConstantF(0, (float*)&ZEMatrix4x4::Identity, 4);

	GetDevice()->SetVertexShaderConstantF(4, (float*)&RenderOrder->WorldMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(8, (float*)&RenderOrder->WorldMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(16, (float*)&ZEVector4(Camera->GetWorldPosition(), 1.0f), 1);

	if (RenderOrder->Flags & ZE_ROF_ENABLE_ZCULLING)
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
	
	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	if (TransparancyMode != ZE_MTM_NOTRANSPARACY)
	{
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		GetDevice()->SetRenderState(D3DRS_ALPHAREF, TransparancyCullLimit);
		GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

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
			case ZE_MTM_ALPHACULL:
				break;
		}

	}
	else
	{
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	if (MaterialComponents & ZESHADER_DIFFUSEMAP)
	{
		SetTextureStage(0, DiffuseMapAddressModeU, DiffuseMapAddressModeV);
		GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)DiffuseMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_NORMALMAP)
	{
		SetTextureStage(1, NormalMapAddressModeU, NormalMapAddressModeV);
		GetDevice()->SetTexture(1, ((ZED3D9Texture2D*)NormalMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_PARALLAXMAP)
	{
		SetTextureStage(2, ParallaxMapAddressModeU, ParallaxMapAddressModeV);
		GetDevice()->SetTexture(2, ((ZED3D9Texture2D*)ParallaxMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_SPECULARMAP)
	{
		SetTextureStage(3, SpecularMapAddressModeU, SpecularMapAddressModeV);
		GetDevice()->SetTexture(3, ((ZED3D9Texture2D*)SpecularMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_OPACITY)
	{
		SetTextureStage(4, OpacityMapAddressModeU, OpacityMapAddressModeV);
		GetDevice()->SetTexture(4, ((ZED3D9Texture2D*)OpacityMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DETAILDIFFUSEMAP)
	{
		SetTextureStage(5, DetailDiffuseMapAddressModeU, DetailDiffuseMapAddressModeV);
		GetDevice()->SetTexture(5, ((ZED3D9Texture2D*)DetailDiffuseMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DETAILNORMALMAP)
	{
		SetTextureStage(6, DetailNormalMapAddressModeU, DetailNormalMapAddressModeV);
		GetDevice()->SetTexture(6, ((ZED3D9Texture2D*)DetailNormalMap)->Texture);
	}

	return true;
}

bool ZED3D9FixedMaterial::SetupPreLightning() const 
{
	if (MaterialComponents & ZESHADER_EMMISIVEMAP)
	{
		SetTextureStage(0, EmmisiveMapAddressModeU, EmmisiveMapAddressModeV);
		GetDevice()->SetTexture(7, ((ZED3D9Texture2D*)EmmisiveMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_REFLECTION)
	{
		SetTextureStage(8, ReflectionMapAddressModeU, ReflectionMapAddressModeV, ReflectionMapAddressModeW);
		GetDevice()->SetTexture(8, ((ZED3D9TextureCube*)ReflectionMap)->CubeTexture);
	}

	if (MaterialComponents & ZESHADER_REFRACTION)
	{
		SetTextureStage(9, RefractionMapAddressModeU, RefractionMapAddressModeV, RefractionMapAddressModeW);
		GetDevice()->SetTexture(9, ((ZED3D9TextureCube*)RefractionMap)->CubeTexture);
	}

	if (MaterialComponents & ZESHADER_LIGHTMAP)
	{
		SetTextureStage(10, LightMapAddressModeU, LightMapAddressModeV);
		GetDevice()->SetTexture(10, ((ZED3D9Texture2D*)LightMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DISTORTIONMAP)
	{
		SetTextureStage(11, DistortionMapAddressModeU, DistortionMapAddressModeV);
		GetDevice()->SetTexture(11, ((ZED3D9Texture2D*)DistortionMap)->Texture);
	}

	SetShaderPass(&Shader->PreLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);
	return true;
}

size_t ZED3D9FixedMaterial::DoPreLightningPass() const 
{
	return 1;
}


bool ZED3D9FixedMaterial::SetupLightning() const 
{
	if (LightningEnabled)
	{
		GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		return true;
	}
	else
		return false;
}

bool ZED3D9FixedMaterial::SetupPointLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedPointLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);
	else
		SetShaderPass(&Shader->PointLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoPointLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	GetDevice()->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	GetDevice()->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Attenuation, 1);

	GetDevice()->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	GetDevice()->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, Lights[0]->Range, 0.0f, 0.0f), 1);

	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		GetDevice()->SetTexture(8, ((ZED3D9TextureCube*)Lights[0]->CubeShadowMap)->CubeTexture);

	return 1;
}


bool ZED3D9FixedMaterial::SetupDirectionalLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedDirectionalLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);
	else
		SetShaderPass(&Shader->DirectionalLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoDirectionalLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	GetDevice()->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Direction, 1);

	GetDevice()->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	GetDevice()->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);
	
	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		GetDevice()->SetTexture(8, ((ZED3D9Texture2D*)Lights[0]->ShadowMap)->Texture);

	return 1;
}


bool ZED3D9FixedMaterial::SetupProjectiveLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);
	else
		SetShaderPass(&Shader->ProjectiveLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoProjectiveLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	const ZERLLight* CurrentLight = Lights[0];

	GetDevice()->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	GetDevice()->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Direction, 1);
	GetDevice()->SetVertexShaderConstantF(26, (const float*)&Lights[0]->Attenuation, 1);
	GetDevice()->SetVertexShaderConstantF(28, (float*)&Lights[0]->LightViewProjMatrix, 4);

	GetDevice()->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	GetDevice()->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);

	GetDevice()->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	GetDevice()->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	GetDevice()->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);
	if (Lights[0]->ProjectionMap != NULL)
		GetDevice()->SetTexture(9, ((ZED3D9Texture2D*)CurrentLight->ProjectionMap)->Texture);

	if (CurrentLight->ShadowMap != NULL && RecivesShadow)
	{ 
		GetDevice()->SetSamplerState(8, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		GetDevice()->SetSamplerState(8, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		GetDevice()->SetSamplerState(8, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		GetDevice()->SetSamplerState(8, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		GetDevice()->SetSamplerState(8, D3DSAMP_BORDERCOLOR, 0x00);	
		GetDevice()->SetTexture(8, ((ZED3D9Texture2D*)Lights[0]->ShadowMap)->Texture);
		GetDevice()->SetPixelShaderConstantF(14, 
			(const float*)&ZEVector4(1.0f / ((ZED3D9Texture2D*)Lights[0]->ShadowMap)->GetWidth(), 
			1.0f / ((ZED3D9Texture2D*)Lights[0]->ShadowMap)->GetHeight(), 0.0f, 0.0f),1);
	}

	return 1;
}


bool ZED3D9FixedMaterial::SetupOmniProjectiveLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);
	else
		SetShaderPass(&Shader->OmniProjectiveLightPass, RenderOrder->Flags & ZE_ROF_SKINNED);

	GetDevice()->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	GetDevice()->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	GetDevice()->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);

	return true;
}

size_t ZED3D9FixedMaterial::DoOmniProjectivePass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	GetDevice()->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	GetDevice()->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Attenuation, 1);
	GetDevice()->SetVertexShaderConstantF(28, (float*)&Lights[0]->LightRotationMatrix, 4);

	GetDevice()->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	GetDevice()->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);

	if (Lights[0]->CubeProjectionMap != NULL)
		GetDevice()->SetTexture(9, ((ZED3D9TextureCube*)Lights[0]->CubeProjectionMap)->CubeTexture);
		
	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		GetDevice()->SetTexture(8, ((ZED3D9Texture2D*)Lights[0]->ShadowMap)->Texture);
	return 1;
}


bool ZED3D9FixedMaterial::SetupCustomPass(unsigned int CustomPassId) const 
{
	return false;
}

bool ZED3D9FixedMaterial::DoCustomPass(unsigned int CustomPassId, void* CustomData) const 
{
	return false;
}

bool ZED3D9FixedMaterial::SetupShadowPass()	const 
{
	return false;
}

size_t ZED3D9FixedMaterial::DoShadowPass() const 
{
	return 1;
}

void ZED3D9FixedMaterial::EndOfPasses() const 
{

}

void ZED3D9FixedMaterial::UpdateMaterial()
{
	if (MaterialComponents != OldMaterialComponents)
	{
		if (Shader != NULL)
			Shader->Release();
		Shader = ZED3D9FixedMaterialShader::CreateInstance(MaterialComponents);
		OldMaterialComponents = MaterialComponents;
	}
}

void ZED3D9FixedMaterial::Release()
{
	if (Shader != NULL)
		Shader->Destroy();
}
