//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SimpleMaterial.cpp
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

#include "ZED3D9Shader.h"
#include "ZEFile/ZEFile.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9SimpleMaterial.h"
#include "ZEGraphics/ZERenderCommand.h"

#include <D3D9.h>

ZED3D9SimpleMaterial::ZED3D9SimpleMaterial()
{
	VertexShader = NULL;
	PixelShader = NULL;
}

ZED3D9SimpleMaterial::~ZED3D9SimpleMaterial()
{
	ReleaseShaders();
}


void ZED3D9SimpleMaterial::CreateShaders()
{
	ReleaseShaders();

	VertexShader = ZED3D9VertexShader::CreateShader("SimpleMaterial.hlsl", "VSMain", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("SimpleMaterial.hlsl", "PSMain", 0, "ps_3_0");
}

void ZED3D9SimpleMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
}

bool ZED3D9SimpleMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderOrder) const 
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9SimpleMaterial*)this)->UpdateMaterial();

	ZECamera* Camera = Renderer->GetCamera();

	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if ((RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM) == ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	else if (RenderOrder->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
		ViewProjMatrix = Camera->GetViewTransform();
	else if (RenderOrder->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
		ViewProjMatrix = Camera->GetProjectionTransform();
	else
		ViewProjMatrix = ZEMatrix4x4::Identity;

	ZEMatrix4x4 WorldViewProjMatrix;
	ZEMatrix4x4 WorldViewMatrix;
	if (RenderOrder->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderOrder->WorldMatrix);
	else
		WorldViewProjMatrix = ViewProjMatrix;

	GetDevice()->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);


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

	// Setup Backface Culling
	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Setup Wireframe
	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	// Setup Transparancy
	if (TransparancyMode != ZE_MTM_NONE)
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
		}
	}
	else
	{
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	// Setup Shaders
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());

	GetDevice()->SetPixelShaderConstantF(10, (float*)&MaterialColor, 1);

	// Setup Constants
	BOOL Options[] = {Texture != NULL, VertexColorEnabled};
	GetDevice()->SetPixelShaderConstantB(0, Options, 2);
	
	// Setup Texture
	GetDevice()->SetTexture(5, Texture != NULL ? ((ZED3D9Texture2D*)Texture)->Texture : NULL);

	return true;
}

void ZED3D9SimpleMaterial::UpdateMaterial()
{
	if (VertexShader == NULL)
		CreateShaders();
}

void ZED3D9SimpleMaterial::Release()
{
	ReleaseShaders();
}




