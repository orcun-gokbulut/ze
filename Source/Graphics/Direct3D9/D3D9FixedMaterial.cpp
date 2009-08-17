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
#include "D3D9Texture.h"
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

void ZED3D9FixedMaterial::SetShaderPass(ZED3D9ShaderPass* Pass, bool Skinned) const 
{
	if (Skinned)
		Device->SetVertexShader(Pass->SkinnedVertexShader);
	else
		Device->SetVertexShader(Pass->VertexShader);

	Device->SetPixelShader(Pass->PixelShader);
}

void ZED3D9FixedMaterial::SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV) const
{
	switch(AddressU)
	{
		case ZE_TAM_WRAP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressV)
	{
		case ZE_TAM_WRAP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			break;
	}

}

void ZED3D9FixedMaterial::SetTextureStage(unsigned int Id, ZETextureAddressMode AddressU, ZETextureAddressMode AddressV, ZETextureAddressMode AddressW) const
{
	switch(AddressU)
	{
		case ZE_TAM_WRAP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressV)
	{
		case ZE_TAM_WRAP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			break;
	}

	switch(AddressW)
	{
		case ZE_TAM_WRAP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			break;
		case ZE_TAM_MIRROR:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_MIRROR);
			break;
		case ZE_TAM_CLAMP:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			break;
		case ZE_TAM_BORDER:
			Device->SetSamplerState(Id, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
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

	if (RenderOrder->Flags & ZE_RLF_SKINNED)
		Device->SetVertexShaderConstantF(32, (float*)RenderOrder->BoneTransforms.GetCArray(), RenderOrder->BoneTransforms.GetCount() * 4);
	
	Device->SetVertexShaderConstantF(12, (const float*)VertexShaderConstants, sizeof(VertexShaderConstants));
	Device->SetPixelShaderConstantF(0, (const float*)PixelShaderConstants, sizeof(PixelShaderConstants));
	
	if (RenderOrder->Flags & ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM)
	{
		ZEMatrix4x4 WorldViewProjMatrix;
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderOrder->WorldMatrix, Camera->GetViewProjectionTransform());
		Device->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	}
	else
		Device->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);

	Device->SetVertexShaderConstantF(4, (float*)&RenderOrder->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(8, (float*)&RenderOrder->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(16, (float*)&ZEVector4(Camera->GetWorldPosition(), 1.0f), 1);

	if (RenderOrder->Flags & ZE_RLF_ENABLE_ZCULLING)
	{
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		if (RenderOrder->Flags & (ZE_RLF_TRANSPARENT | ZE_RLF_IMPOSTER))
			Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		else
			Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	}
	else
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	
	if (TwoSided)
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (Wireframe)
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	if (TransparancyMode != ZE_MTM_NOTRANSPARACY)
	{
		Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		Device->SetRenderState(D3DRS_ALPHAREF, TransparancyCullLimit);
		Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		switch(TransparancyMode)
		{
			case ZE_MTM_ADDAPTIVE:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_MTM_SUBTRACTIVE:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_MTM_REGULAR:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			case ZE_MTM_ALPHACULL:
				break;
		}

	}
	else
	{
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	if (MaterialComponents & ZESHADER_DIFFUSEMAP)
	{
		SetTextureStage(0, DiffuseMapAddressModeU, DiffuseMapAddressModeV);
		Device->SetTexture(0, ((ZED3D9Texture*)DiffuseMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_NORMALMAP)
	{
		SetTextureStage(1, NormalMapAddressModeU, NormalMapAddressModeV);
		Device->SetTexture(1, ((ZED3D9Texture*)NormalMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_PARALLAXMAP)
	{
		SetTextureStage(2, ParallaxMapAddressModeU, ParallaxMapAddressModeV);
		Device->SetTexture(2, ((ZED3D9Texture*)ParallaxMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_SPECULARMAP)
	{
		SetTextureStage(3, SpecularMapAddressModeU, SpecularMapAddressModeV);
		Device->SetTexture(3, ((ZED3D9Texture*)SpecularMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_OPACITY)
	{
		SetTextureStage(4, OpacityMapAddressModeU, OpacityMapAddressModeV);
		Device->SetTexture(4, ((ZED3D9Texture*)OpacityMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DETAILDIFFUSEMAP)
	{
		SetTextureStage(5, DetailDiffuseMapAddressModeU, DetailDiffuseMapAddressModeV);
		Device->SetTexture(5, ((ZED3D9Texture*)DetailDiffuseMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DETAILNORMALMAP)
	{
		SetTextureStage(6, DetailNormalMapAddressModeU, DetailNormalMapAddressModeV);
		Device->SetTexture(6, ((ZED3D9Texture*)DetailNormalMap)->Texture);
	}

	return true;
}

bool ZED3D9FixedMaterial::SetupPreLightning() const 
{
	if (MaterialComponents & ZESHADER_EMMISIVEMAP)
	{
		SetTextureStage(0, EmmisiveMapAddressModeU, EmmisiveMapAddressModeV);
		Device->SetTexture(7, ((ZED3D9Texture*)EmmisiveMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_REFLECTION)
	{
		SetTextureStage(8, ReflectionMapAddressModeU, ReflectionMapAddressModeV, ReflectionMapAddressModeW);
		Device->SetTexture(8, ((ZED3D9CubeTexture*)ReflectionMap)->CubeTexture);
	}

	if (MaterialComponents & ZESHADER_REFRACTION)
	{
		SetTextureStage(9, RefractionMapAddressModeU, RefractionMapAddressModeV, RefractionMapAddressModeW);
		Device->SetTexture(9, ((ZED3D9CubeTexture*)RefractionMap)->CubeTexture);
	}

	if (MaterialComponents & ZESHADER_LIGHTMAP)
	{
		SetTextureStage(10, LightMapAddressModeU, LightMapAddressModeV);
		Device->SetTexture(10, ((ZED3D9Texture*)LightMap)->Texture);
	}

	if (MaterialComponents & ZESHADER_DISTORTIONMAP)
	{
		SetTextureStage(11, DistortionMapAddressModeU, DistortionMapAddressModeV);
		Device->SetTexture(11, ((ZED3D9Texture*)DistortionMap)->Texture);
	}

	SetShaderPass(&Shader->PreLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);
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
		Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
		Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		return true;
	}
	else
		return false;
}

bool ZED3D9FixedMaterial::SetupPointLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedPointLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);
	else
		SetShaderPass(&Shader->PointLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoPointLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	Device->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	Device->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Attenuation, 1);

	Device->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, Lights[0]->Range, 0.0f, 0.0f), 1);

	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		Device->SetTexture(8, ((ZED3D9CubeTexture*)Lights[0]->CubeShadowMap)->CubeTexture);

	return 1;
}


bool ZED3D9FixedMaterial::SetupDirectionalLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedDirectionalLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);
	else
		SetShaderPass(&Shader->DirectionalLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoDirectionalLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	Device->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Direction, 1);

	Device->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);
	
	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		Device->SetTexture(8, ((ZED3D9Texture*)Lights[0]->ShadowMap)->Texture);

	return 1;
}


bool ZED3D9FixedMaterial::SetupProjectiveLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);
	else
		SetShaderPass(&Shader->ProjectiveLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);

	return true;
}

size_t ZED3D9FixedMaterial::DoProjectiveLightPass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	const ZERLLight* CurrentLight = Lights[0];

	Device->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	Device->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Direction, 1);
	Device->SetVertexShaderConstantF(26, (const float*)&Lights[0]->Attenuation, 1);
	Device->SetVertexShaderConstantF(28, (float*)&Lights[0]->LightViewProjMatrix, 4);

	Device->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);

	Device->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	Device->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	Device->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);
	if (Lights[0]->ProjectionMap != NULL)
		Device->SetTexture(9, ((ZED3D9Texture*)CurrentLight->ProjectionMap)->Texture);

	if (CurrentLight->ShadowMap != NULL && RecivesShadow)
	{ 
		Device->SetSamplerState(8, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		Device->SetSamplerState(8, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		Device->SetSamplerState(8, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		Device->SetSamplerState(8, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		Device->SetSamplerState(8, D3DSAMP_BORDERCOLOR, 0x00);	
		Device->SetTexture(8, ((ZED3D9Texture*)Lights[0]->ShadowMap)->Texture);
		Device->SetPixelShaderConstantF(14, 
			(const float*)&ZEVector4(1.0f / ((ZED3D9Texture*)Lights[0]->ShadowMap)->GetWidth(), 
			1.0f / ((ZED3D9Texture*)Lights[0]->ShadowMap)->GetHeight(), 0.0f, 0.0f),1);
	}

	return 1;
}


bool ZED3D9FixedMaterial::SetupOmniProjectiveLightPass(bool Shadowed) const 
{
	if (Shadowed && RecivesShadow)
		SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);
	else
		SetShaderPass(&Shader->OmniProjectiveLightPass, RenderOrder->Flags & ZE_RLF_SKINNED);

	Device->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	Device->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	Device->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);

	return true;
}

size_t ZED3D9FixedMaterial::DoOmniProjectivePass(const ZERLLight** Lights, size_t Count) const 
{
	if (Count == 0)
		return 0; 

	Device->SetVertexShaderConstantF(24, (const float*)&Lights[0]->Position, 1);
	Device->SetVertexShaderConstantF(25, (const float*)&Lights[0]->Attenuation, 1);
	Device->SetVertexShaderConstantF(28, (float*)&Lights[0]->LightRotationMatrix, 4);

	Device->SetPixelShaderConstantF(12, (const float*)&Lights[0]->Color, 1);
	Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(Lights[0]->Intensity, 0.0f, 0.0f, 0.0f), 1);

	if (Lights[0]->CubeProjectionMap != NULL)
		Device->SetTexture(9, ((ZED3D9CubeTexture*)Lights[0]->CubeProjectionMap)->CubeTexture);
		
	if (Lights[0]->ShadowMap != NULL && RecivesShadow)
		Device->SetTexture(8, ((ZED3D9Texture*)Lights[0]->ShadowMap)->Texture);
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
		Shader = ZED3D9Shader::CreateFixedMaterialShader(MaterialComponents);
		OldMaterialComponents = MaterialComponents;
	}
}

void ZED3D9FixedMaterial::Release()
{
	if (Shader != NULL)
		Shader->Destroy();
}

void ZED3D9FixedMaterial::Destroy()
{
	delete this;
}


