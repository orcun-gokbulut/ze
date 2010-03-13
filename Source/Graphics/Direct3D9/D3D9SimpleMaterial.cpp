//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9SimpleMaterial.cpp
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

#include "D3D9SimpleMaterial.h"
#include "D3D9CommonTools.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/Camera.h"
#include "Core/ResourceFile.h"
#include <D3D9.h>

LPDIRECT3DVERTEXSHADER9 ZED3D9SimpleMaterial::VertexShader = NULL;
LPDIRECT3DPIXELSHADER9 ZED3D9SimpleMaterial::PixelShader = NULL;

const char* ZED3D9SimpleMaterial::GetMaterialUID() const 
{
	return "";
}

unsigned int ZED3D9SimpleMaterial::GetMaterialFlags() const 
{
	return NULL;
}

ZEMaterialType ZED3D9SimpleMaterial::GetMaterialType() const 
{
	return ZE_MT_FIXED;
}

bool ZED3D9SimpleMaterial::SetupMaterial(ZERenderOrder* RenderOrder, ZECamera* Camera) const 
{
	((ZED3D9SimpleMaterial*)this)->UpdateMaterial();

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
	GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
	
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

	GetDevice()->SetVertexShader(VertexShader);
	GetDevice()->SetPixelShader(PixelShader);

	return true;
}

bool ZED3D9SimpleMaterial::SetupPreLightning() const 
{
	return true;
}

size_t ZED3D9SimpleMaterial::DoPreLightningPass() const 
{
	return 1;
}

bool ZED3D9SimpleMaterial::SetupLightning() const 
{
		return false;
}

bool ZED3D9SimpleMaterial::SetupPointLightPass(bool Shadowed) const 
{
	return false;
}

size_t ZED3D9SimpleMaterial::DoPointLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 0;
}


bool ZED3D9SimpleMaterial::SetupDirectionalLightPass(bool Shadowed) const 
{
	return false;
}

size_t ZED3D9SimpleMaterial::DoDirectionalLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9SimpleMaterial::SetupProjectiveLightPass(bool Shadowed) const 
{
	return true;
}

size_t ZED3D9SimpleMaterial::DoProjectiveLightPass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9SimpleMaterial::SetupOmniProjectiveLightPass(bool Shadowed) const 
{
	return true;
}

size_t ZED3D9SimpleMaterial::DoOmniProjectivePass(const ZERLLight** Lights, size_t Count) const 
{
	return 1;
}


bool ZED3D9SimpleMaterial::SetupCustomPass(unsigned int CustomPassId) const 
{
	return false;
}

bool ZED3D9SimpleMaterial::DoCustomPass(unsigned int CustomPassId, void* CustomData) const 
{
	return false;
}

bool ZED3D9SimpleMaterial::SetupShadowPass() const 
{
	return false;
}

size_t ZED3D9SimpleMaterial::DoShadowPass() const 
{
	return 1;
}

void ZED3D9SimpleMaterial::EndOfPasses() const 
{

}

void ZED3D9SimpleMaterial::UpdateMaterial()
{
	if (VertexShader == NULL)
	{
		char SourceBuffer[65536];
		ZEResourceFile::ReadTextFile("Shaders\\SimpleVertexShader.vs", SourceBuffer, 65536);
		ZED3D9CommonTools::CompileVertexShader(&VertexShader, SourceBuffer, "Simple Material Vertex Shader", "vs_2_0", NULL);

		ZEResourceFile::ReadTextFile("Shaders\\SimplePixelShader.ps", SourceBuffer, 65536);
		ZED3D9CommonTools::CompilePixelShader(&PixelShader, SourceBuffer, "Simple Material Solid Pixel Shader", "ps_2_0", NULL);
	}
}

void ZED3D9SimpleMaterial::Release()
{

}
