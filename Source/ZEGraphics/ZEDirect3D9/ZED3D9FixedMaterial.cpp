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
#include "ZEGraphics\ZERenderOrder.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9TextureCube.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEGraphics\ZEMaterialComponents.h"
#include "ZED3D9Shader.h"
#include "ZED3D9CommonTools.h"

void ZED3D9FixedMaterial::CreateShaders()
{
	ReleaseShaders();

	PreZPassVertexShader = ZED3D9VertexShader::CreateShader("FixedMaterial.hlsl", "PreZVSMain", MaterialComponents, "vs_2_0");
	PreZPassPixelShader = ZED3D9PixelShader::CreateShader("FixedMaterial.hlsl", "PreZPSMain", MaterialComponents, "ps_2_0");

	GBufferPassVertexShader = ZED3D9VertexShader::CreateShader("FixedMaterial.hlsl", "GBVSMain", MaterialComponents, "vs_2_0");
	GBufferPassPixelShader = ZED3D9PixelShader::CreateShader("FixedMaterial.hlsl", "GBPSMain", MaterialComponents, "ps_2_0");

	/*ForwardPassVertexShader = ZED3D9VertexShader::CreateShader("FixedMaterial.hlsl", "ForwardVSMain", MaterialComponents, "vs_2_0");
	ForwardPassPixelShader = ZED3D9PixelShader::CreateShader("FixedMaterial.hlsl", "ForwardPSMain", MaterialComponents, "ps_2_0");*/
}

void ZED3D9FixedMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(PreZPassVertexShader);
	ZED3D_RELEASE(PreZPassPixelShader);

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
	PreZPassPixelShader = NULL;
	PreZPassVertexShader = NULL;
	PreZPassPixelShader = NULL;
	GBufferPassVertexShader = NULL;
	GBufferPassPixelShader = NULL;
	ForwardPassVertexShader = NULL;
	ForwardPassPixelShader = NULL;
	ShadowPassVertexShader = NULL;
	ShadowPassPixelShader = NULL;
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

bool ZED3D9FixedMaterial::SetupPreZPass() const
{
	GetDevice()->SetPixelShader(PreZPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(PreZPassVertexShader->GetVertexShader());

	return true;
}

bool ZED3D9FixedMaterial::SetupGBufferPass() const
{
	GetDevice()->SetPixelShader(GBufferPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(GBufferPassVertexShader->GetVertexShader());

	return true;
}

bool ZED3D9FixedMaterial::SetupForwardPass() const
{
	GetDevice()->SetPixelShader(ForwardPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(ForwardPassVertexShader->GetVertexShader());
	
	return false;
}

bool ZED3D9FixedMaterial::SetupShadowPass() const
{
	return false;
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

void ZED3D9FixedMaterial::UpdateMaterial()
{
	if (MaterialComponents != OldMaterialComponents)
	{
		CreateShaders();
		OldMaterialComponents = MaterialComponents;
	}
}

void ZED3D9FixedMaterial::Release()
{
	ReleaseShaders();
}




