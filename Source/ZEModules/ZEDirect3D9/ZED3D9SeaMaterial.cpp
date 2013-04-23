//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SeaMaterial.cpp
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

#include "ZED3D9SeaMaterial.h"
#include "ZEGraphics/ZEFrameRenderer.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZED3D9Shader.h"
#include "ZED3D9CommonTools.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9Texture2D.h"


void ZED3D9SeaMaterial::CreateShaders()
{
	ReleaseShaders();

	ForwardPassVertexShader = ZED3D9VertexShader::CreateShader("SeaMaterial.hlsl", "VSMainForwardPass", 0, "vs_3_0");
	ForwardPassPixelShader = ZED3D9PixelShader::CreateShader("SeaMaterial.hlsl", "PSMainForwardPass", 0, "ps_3_0");

	GPassVertexShader = ZED3D9VertexShader::CreateShader("SeaMaterial.hlsl", "VSMainGPass", 0, "vs_3_0");
	GPassPixelShader = ZED3D9PixelShader::CreateShader("SeaMaterial.hlsl", "PSMainGPass", 0, "ps_3_0");
}

void ZED3D9SeaMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(GPassVertexShader);
	ZED3D_RELEASE(ForwardPassVertexShader);
	ZED3D_RELEASE(ForwardPassPixelShader);
	ZED3D_RELEASE(GPassPixelShader);
}

ZED3D9SeaMaterial::ZED3D9SeaMaterial()
{
	GPassVertexShader = NULL;
	ForwardPassVertexShader = NULL;
	ForwardPassPixelShader = NULL;
	GPassPixelShader = NULL;

	CreateShaders();
}

 ZED3D9SeaMaterial::~ZED3D9SeaMaterial()
{
	ReleaseShaders();
}

bool ZED3D9SeaMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	GetDevice()->SetTexture(0, DiffuseTexture == NULL ? NULL : ((ZED3D9Texture2D*)DiffuseTexture)->Texture);
	GetDevice()->SetPixelShader(ForwardPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(ForwardPassVertexShader->GetVertexShader());
	GetDevice()->SetVertexShaderConstantF(8, ZEVector4(EntityXZScale.x, EntityXZScale.y, DiffuseTextureTile.x, DiffuseTextureTile.y).M, 1);

	GetDevice()->SetPixelShaderConstantF(0, ZEVector4(DiffuseTextureOffset.x, DiffuseTextureOffset.y, NormalTextureOffset.x, NormalTextureOffset.y).M, 1);
	GetDevice()->SetPixelShaderConstantF(1, DiffuseColor.M, 1);
	GetDevice()->SetPixelShaderConstantF(2, SpecularColor.M, 1);
	GetDevice()->SetPixelShaderConstantF(5, AmbientColor.M, 1);
	GetDevice()->SetPixelShaderConstantF(3, ZEVector4(DiffuseFactor, SpecularFactor, AmbientFactor, 0.0f).M, 1);
	GetDevice()->SetPixelShaderConstantF(8, ZEVector4(EntityXZScale.x, EntityXZScale.y, DiffuseTextureTile.x, DiffuseTextureTile.y).M, 1);

	ZEMatrix4x4 WorldViewProjection;
	ZEMatrix4x4::Multiply(WorldViewProjection, zeScene->GetActiveCamera()->GetViewProjectionTransform(), RenderCommand->WorldMatrix);
	GetDevice()->SetVertexShaderConstantF(0, WorldViewProjection.MA, 4);

	float Width = (float)Renderer->GetViewPort()->GetWidth();
	float Height = (float)Renderer->GetViewPort()->GetHeight();
	ZEVector2 PixelSize = ZEVector2::One / ZEVector2(Width, Height);
	ZEVector2 HalfPixelSize = PixelSize / 2.0f;

	ZEVector4 PixelParameters = ZEVector4(PixelSize.x, PixelSize.y, HalfPixelSize.x, HalfPixelSize.y);
	GetDevice()->SetPixelShaderConstantF(4, PixelParameters.M, 1);	

	return true;
}

bool ZED3D9SeaMaterial::SetupGBufferPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	D3DPERF_BeginEvent(0, L"Sea GBufferPass");

	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	GetDevice()->SetTexture(0, NormalTexture == NULL ? NULL : ((ZED3D9Texture2D*)NormalTexture)->Texture);
	GetDevice()->SetPixelShader(GPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(GPassVertexShader->GetVertexShader());
	GetDevice()->SetVertexShaderConstantF(8, ZEVector4(EntityXZScale.x, EntityXZScale.y, NormalTextureTile.x, NormalTextureTile.y).M, 1);

	GetDevice()->SetPixelShaderConstantF(0, ZEVector4(DiffuseTextureOffset.x, DiffuseTextureOffset.y, NormalTextureOffset.x, NormalTextureOffset.y).M, 1);
	GetDevice()->SetPixelShaderConstantF(1, ZEVector4(SpecularShineness, 0.0f, 0.0f, 0.0f).M, 1);
	GetDevice()->SetPixelShaderConstantF(8, ZEVector4(EntityXZScale.x, EntityXZScale.y, NormalTextureTile.x, NormalTextureTile.y).M, 1);

	ZEMatrix4x4 WorldViewProjection;
	ZEMatrix4x4::Multiply(WorldViewProjection, zeScene->GetActiveCamera()->GetViewProjectionTransform(), RenderCommand->WorldMatrix);
	GetDevice()->SetVertexShaderConstantF(0, WorldViewProjection.MA, 4);

	ZEMatrix4x4 WorldView;
	ZEMatrix4x4::Multiply(WorldView, zeScene->GetActiveCamera()->GetViewTransform(), RenderCommand->WorldMatrix);
	GetDevice()->SetVertexShaderConstantF(4, WorldView.MA, 4);

	D3DPERF_EndEvent();

	return true;
}

void ZED3D9SeaMaterial::UpdateMaterial()
{

}

void ZED3D9SeaMaterial::Release()
{
	ReleaseShaders();
}
