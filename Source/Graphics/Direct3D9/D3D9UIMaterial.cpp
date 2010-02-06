//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9UIMaterial.cpp
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

#include "D3D9UIMaterial.h"
#include "D3D9Texture2D.h"
#include "D3D9CommonTools.h"
#include "Graphics/RenderOrder.h"
#include "Core/ResourceFile.h"
#include <D3D9.h>

LPDIRECT3DVERTEXSHADER9 ZED3D9UIMaterial::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9UIMaterial::SolidPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9UIMaterial::TexturedPixelShader = NULL;

const char* ZED3D9UIMaterial::GetMaterialUID() const 
{
	return "";
}

unsigned int ZED3D9UIMaterial::GetMaterialFlags() const 
{
	return NULL;
}

ZEMaterialType ZED3D9UIMaterial::GetMaterialType() const 
{
	return ZE_MT_FIXED;
}

bool ZED3D9UIMaterial::SetupMaterial(ZERenderOrder* RenderOrder, ZECamera* Camera) const 
{
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
	
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (WireFrame)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);

	/*GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x0);
	GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);*/

	GetDevice()->SetVertexShader(VertexShader);
	GetDevice()->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);

	if (Texture != NULL)
	{
		ZEVector4 TextureSize(0.5f / Texture->GetWidth(), 0.5f / Texture->GetHeight(), 0.0f, 0.0f);
		GetDevice()->SetVertexShaderConstantF(5, (const float*)&TextureSize, 1);
		GetDevice()->SetTexture(0, ((ZED3D9Texture2D*)Texture)->Texture);
		GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		GetDevice()->SetPixelShader(TexturedPixelShader);
	}
	else
		GetDevice()->SetPixelShader(SolidPixelShader);

	return true;
}

bool ZED3D9UIMaterial::SetupPreLightning() const 
{
	return true;
}

size_t ZED3D9UIMaterial::DoPreLightningPass() const 
{
	return 1;
}

bool ZED3D9UIMaterial::SetupLightning() const 
{
		return false;
}

bool ZED3D9UIMaterial::SetupPointLightPass(bool Shadowed) const 
{
	return false;
}

size_t ZED3D9UIMaterial::DoPointLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 0;
}


bool ZED3D9UIMaterial::SetupDirectionalLightPass(bool Shadowed) const 
{
	return false;
}

size_t ZED3D9UIMaterial::DoDirectionalLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9UIMaterial::SetupProjectiveLightPass(bool Shadowed) const 
{
	return true;
}

size_t ZED3D9UIMaterial::DoProjectiveLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9UIMaterial::SetupOmniProjectiveLightPass(bool Shadowed) const 
{
	return true;
}

size_t ZED3D9UIMaterial::DoOmniProjectivePass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9UIMaterial::SetupCustomPass(unsigned int CustomPassId) const 
{
	return false;
}

bool ZED3D9UIMaterial::DoCustomPass(unsigned int CustomPassId, void* CustomData) const 
{
	return false;
}

bool ZED3D9UIMaterial::SetupShadowPass() const 
{
	return false;
}

size_t ZED3D9UIMaterial::DoShadowPass() const 
{
	return 1;
}

void ZED3D9UIMaterial::EndOfPasses() const 
{

}

void ZED3D9UIMaterial::UpdateMaterial()
{
	if (VertexShader == NULL)
	{
		char SourceBuffer[65536];
		ZEResourceFile::ReadTextFile("Shaders\\UIVertexShader.vs", SourceBuffer, 65536);
		ZED3D9CommonTools::CompileVertexShader(&VertexShader, SourceBuffer, "UI Material Vertex Shader", "vs_2_0", NULL);

		ZEResourceFile::ReadTextFile("Shaders\\UISolidPixelShader.ps", SourceBuffer, 65536);
		ZED3D9CommonTools::CompilePixelShader(&SolidPixelShader, SourceBuffer, "UI Material Solid Pixel Shader", "ps_2_0", NULL);

		ZEResourceFile::ReadTextFile("Shaders\\UITexturedPixelShader.ps", SourceBuffer, 65536);
		ZED3D9CommonTools::CompilePixelShader(&TexturedPixelShader, SourceBuffer, "UI Material Textured Pixel Shader", "ps_2_0", NULL);
	}
}

void ZED3D9UIMaterial::Release()
{

}
