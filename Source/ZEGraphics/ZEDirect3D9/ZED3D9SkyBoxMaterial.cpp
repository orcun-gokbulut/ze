//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9SkyBoxMaterial.cpp
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
#include "ZED3D9TextureCube.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZECamera.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9SkyBoxMaterial.h"
#include "ZEGraphics/ZERenderOrder.h"


#include <D3D9.h>

ZED3D9SkyBoxMaterial::ZED3D9SkyBoxMaterial()
{
	VertexShader = NULL;
	PixelShader = NULL;
}

ZED3D9SkyBoxMaterial::~ZED3D9SkyBoxMaterial()
{
	ReleaseShaders();
}


void ZED3D9SkyBoxMaterial::CreateShaders()
{
	ReleaseShaders();

	VertexShader = ZED3D9VertexShader::CreateShader("SkyBoxMaterial.hlsl", "VSMain", 0, "vs_3_0");
	PixelShader = ZED3D9PixelShader::CreateShader("SkyBoxMaterial.hlsl", "PSMain", 0, "ps_3_0");
}

void ZED3D9SkyBoxMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(VertexShader);
	ZED3D_RELEASE(PixelShader);
}

bool ZED3D9SkyBoxMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderOrder* RenderOrder) const 
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9SkyBoxMaterial*)this)->UpdateMaterial();

	ZECamera* Camera = Renderer->GetCamera();

	// Setup Transformations
	ZEMatrix4x4 WorldViewProjMatrix;
	ZEMatrix4x4 WorldViewMatrix;
	ZEMatrix4x4::Multiply(WorldViewProjMatrix, Camera->GetViewProjectionTransform(), RenderOrder->WorldMatrix);
	GetDevice()->SetVertexShaderConstantF(0, (float*)&RenderOrder->WorldMatrix, 4);

	// ZBuffer
	GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Setup Transparancy
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// Setup Shaders
	GetDevice()->SetVertexShader(VertexShader->GetVertexShader());
	GetDevice()->SetPixelShader(PixelShader->GetPixelShader());

	GetDevice()->SetPixelShaderConstantF(10, (float*)&ZEVector4(Color.x * Brightness, Color.y * Brightness, Color.z * Brightness, 1.0f), 1);
	
	// Setup Texture
	GetDevice()->SetTexture(5, Texture != NULL ? ((ZED3D9TextureCube*)Texture)->CubeTexture : NULL);

	return true;
}

void ZED3D9SkyBoxMaterial::UpdateMaterial()
{
	if (VertexShader == NULL)
		CreateShaders();
}

void ZED3D9SkyBoxMaterial::Release()
{
	ReleaseShaders();
}




