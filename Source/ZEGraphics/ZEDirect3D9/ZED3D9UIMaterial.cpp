//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9UIMaterial.cpp
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

#include "ZED3D9UIMaterial.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZERenderOrder.h"
#include "ZECore/ZEResourceFile.h"
#include <D3D9.h>

LPDIRECT3DVERTEXSHADER9 ZED3D9UIMaterial::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9UIMaterial::SolidPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9UIMaterial::TexturedPixelShader = NULL;

ZED3D9UIMaterial::ZED3D9UIMaterial()
{

}

ZED3D9UIMaterial::~ZED3D9UIMaterial()
{

}

bool ZED3D9UIMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderOrder* RenderOrder) const 
{
	((ZED3D9UIMaterial*)this)->UpdateMaterial();

	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (WireFrame)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

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

void ZED3D9UIMaterial::UpdateMaterial()
{
	if (VertexShader == NULL)
	{
		ZED3D9CommonTools::CompileVertexShader(&VertexShader, "UI.hlsl", "VSMain", "vs_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(&SolidPixelShader, "UI.hlsl", "PSMain", "ps_2_0", NULL);
		ZED3D9CommonTools::CompilePixelShader(&TexturedPixelShader, "UI.hlsl", "PSMainTextured", "ps_2_0", NULL);
	}
}

void ZED3D9UIMaterial::Release()
{

}




