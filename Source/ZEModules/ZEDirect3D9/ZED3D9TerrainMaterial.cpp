//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9TerrainMaterial.cpp
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

#include "ZED3D9TerrainMaterial.h"
#include "ZED3D9FrameRenderer.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9Shader.h"
#include "ZED3D9CommonTools.h"
#include "ZEGraphics/ZECamera.h"
#include "ZEGraphics/ZERenderCommand.h"
#include "ZEGraphics/ZEMaterialComponents.h"
#include "ZETerrain/ZETRTerrain.h"
#include "ZETerrain/ZETRLevel.h"
#include "ZETerrain/ZETRLayer.h"
#include "ZEMath/ZEMath.h"

void ZED3D9TerrainMaterial::CreateShaders()
{
	if (GBufferPassVertexShader != NULL)
		return;

	GBufferPassVertexShader = ZED3D9VertexShader::CreateShader("TerrainMaterial.hlsl", "ZETerrainMaterial_GBuffer_VertexShader", 0, "vs_3_0");
	GBufferPassPixelShader = ZED3D9PixelShader::CreateShader("TerrainMaterial.hlsl", "ZETerrainMaterial_GBuffer_PixelShader", 0, "ps_3_0");

	ForwardPassVertexShader = ZED3D9VertexShader::CreateShader("TerrainMaterial.hlsl", "ZETerrainMaterial_ForwardPass_VertexShader", 0, "vs_3_0");
	ForwardPassPixelShader = ZED3D9PixelShader::CreateShader("TerrainMaterial.hlsl", "ZETerrainMaterial_ForwardPass_PixelShader", 0, "ps_3_0");
}

void ZED3D9TerrainMaterial::ReleaseShaders()
{
	ZED3D_RELEASE(GBufferPassVertexShader);
	ZED3D_RELEASE(GBufferPassPixelShader);

	ZED3D_RELEASE(ForwardPassVertexShader);
	ZED3D_RELEASE(ForwardPassPixelShader);

	ZED3D_RELEASE(ShadowPassVertexShader);
	ZED3D_RELEASE(ShadowPassPixelShader);
}

ZED3D9TerrainMaterial::~ZED3D9TerrainMaterial()
{
	Release();
}

ZED3D9TerrainMaterial::ZED3D9TerrainMaterial()
{
	GBufferPassVertexShader = NULL;
	GBufferPassPixelShader = NULL;
	ForwardPassVertexShader = NULL;
	ForwardPassPixelShader = NULL;
	ShadowPassVertexShader = NULL;
	ShadowPassPixelShader = NULL;
}

ZEVector4 ZED3D9TerrainMaterial::GetTexcoordTransform(ZETRLevel* Level, const ZEVector3& Position) const
{
	ZEVector4 Transform; // xy: scale, zw: offset

	float Scale = Level->GetLevelBlockSize() * Level->GetBlockCount();
	Transform.x = 1.0f / Scale;
	Transform.y = Transform.x;

	Transform.z = 0.0f;
	Transform.w = Transform.z;

	return Transform;
}

void ZED3D9TerrainMaterial::SetupLevel(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	float ElevationScale = 0.001f;
	float ElevationOffset = 0.0f;

	ZEInt Level = (ZEInt)RenderCommand->InstanceData;

	ZEVector3 Position = Renderer->GetCamera()->GetWorldPosition();
	Position.x = ZEMath::Floor(Position.x);
	Position.z = ZEMath::Floor(Position.z);

	bool HasElevation = false;
	if (ElevationLayer != NULL)
	{
		ZETRLevel* ElevationLevel = NULL;
		if (Level >= ElevationLayer->GetMinLevel())
			ElevationLevel = ElevationLayer->GetLevel(Level);
		else
			ElevationLevel = ElevationLayer->GetLevel(ElevationLayer->GetMinLevel());
		
		ZETRLevel* ElevationLevelLow = NULL;
		if (Level + 1 < ElevationLayer->GetMaxLevel())
			ElevationLevelLow = ElevationLayer->GetLevel(Level + 1);
		else
			ElevationLevelLow = ElevationLevel;

		if (ElevationLevel != NULL)
		{
			float TextureSize = ElevationLevel->GetBlockCount() * ElevationLayer->GetBlockSize();

			ZEVector4 ElevationTexcoordTransform = GetTexcoordTransform(ElevationLevel, Position);
			ZEVector4 ElevationTexcoordTransformLow = GetTexcoordTransform(ElevationLevelLow, Position);

			GetDevice()->SetVertexShaderConstantF(16, (float*)&ElevationTexcoordTransform, 1);
			GetDevice()->SetVertexShaderConstantF(17, (float*)&ElevationTexcoordTransformLow, 1);

			GetDevice()->SetVertexShaderConstantF(18, (float*)&ZEVector4(TextureSize, 1.0f / TextureSize, ElevationScale, ElevationOffset), 1);
		
			ZED3D9CommonTools::SetTexture(D3DVERTEXTEXTURESAMPLER0, ElevationLevel->GetTexture(), D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_WRAP);
			ZED3D9CommonTools::SetTexture(D3DVERTEXTEXTURESAMPLER1, ElevationLevelLow->GetTexture(), D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_WRAP);

			HasElevation = true;
		}
	}
	
	if (!HasElevation)
	{
		ZED3D9CommonTools::SetTexture(D3DVERTEXTEXTURESAMPLER0, (ZED3D9Texture2D*)NULL, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_WRAP);
		ZED3D9CommonTools::SetTexture(D3DVERTEXTEXTURESAMPLER1, (ZED3D9Texture2D*)NULL, D3DTEXF_POINT, D3DTEXF_POINT, D3DTADDRESS_WRAP);

	}
	
	bool HasColor = false;
	if (ColorLayer != NULL)
	{
		ZETRLevel* ColorLevel = NULL;
		if (Level >= ColorLayer->GetMinLevel())
			ColorLevel = ColorLayer->GetLevel(Level);
		else
			ColorLevel = ColorLayer->GetLevel(ColorLayer->GetMinLevel());

		ZETRLevel* ColorLevelLow = NULL;
		if (Level + 1 < ColorLayer->GetMaxLevel())
			ColorLevelLow = ColorLayer->GetLevel(Level + 1);
		else
			ColorLevelLow = ColorLevel;

		if (ColorLevel != NULL)
		{
			ZEVector4 ColorTexcoordTransform = GetTexcoordTransform(ColorLevel, Position);
			ZEVector4 ColorTexcoordTransformLow = GetTexcoordTransform(ColorLevelLow, Position);

			GetDevice()->SetVertexShaderConstantF(19, (float*)&ColorTexcoordTransform, 1);
			GetDevice()->SetVertexShaderConstantF(20, (float*)&ColorTexcoordTransformLow, 1);

			ZED3D9CommonTools::SetTexture(4, ColorLevel->GetTexture(), D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_WRAP);
			ZED3D9CommonTools::SetTexture(5, ColorLevelLow->GetTexture(), D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_WRAP);

			HasColor = true;
		}
	}
	
	if (!HasColor)
	{
		ZED3D9CommonTools::SetTexture(4, (ZED3D9Texture2D*)NULL, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_WRAP);
		ZED3D9CommonTools::SetTexture(5, (ZED3D9Texture2D*)NULL, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTADDRESS_WRAP);
	}

	float LevelScale =  ZEMath::Power(2, Level);
	float BlendPercentage = 0.00125;
	float BlendEnd = 2.0f * ChunkSize * LevelScale;
	float BlendStart = BlendEnd * (1.0f - BlendPercentage);
	float BlendScale = 1.0f / (BlendEnd - BlendStart);

	GetDevice()->SetVertexShaderConstantF(21, (float*)&ZEVector4(BlendStart, BlendScale, 0.0f, 0.0f), 1);
}

bool ZED3D9TerrainMaterial::SetupGBufferPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9TerrainMaterial*)this)->UpdateMaterial();

	// Setup Transformations
	ZECamera* Camera = Renderer->GetCamera();
	GetDevice()->SetVertexShaderConstantF(0, (float*)&Camera->GetViewProjectionTransform(), 4);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&RenderCommand->WorldMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(8, (float*)&Camera->GetViewTransform(), 4);
	GetDevice()->SetVertexShaderConstantF(12, (float*)&RenderCommand->LocalMatrix, 4);

	SetupLevel(Renderer, RenderCommand);

	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
	{
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
	}
	else
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Setup Wireframe
	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);


	// Setup Material Properties
	//GetDevice()->SetPixelShaderConstantF(1, (const float*)PixelShaderConstants, sizeof(PixelShaderConstants) / 16);
	
	// Setup Shaders
	GetDevice()->SetPixelShader(GBufferPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(GBufferPassVertexShader->GetVertexShader());

	return true;
}

bool ZED3D9TerrainMaterial::SetupForwardPass(ZEFrameRenderer* Renderer, ZERenderCommand* RenderCommand) const
{
	// Update material if its changed. (Recompile shaders, etc.)
	((ZED3D9TerrainMaterial*)this)->UpdateMaterial();

	// Setup Transformations
	ZECamera* Camera = Renderer->GetCamera();
	GetDevice()->SetVertexShaderConstantF(0, (float*)&Camera->GetViewProjectionTransform(), 4);
	GetDevice()->SetVertexShaderConstantF(4, (float*)&RenderCommand->WorldMatrix, 4);
	GetDevice()->SetVertexShaderConstantF(8, (float*)&Camera->GetViewTransform(), 4);
	GetDevice()->SetVertexShaderConstantF(12, (float*)&RenderCommand->LocalMatrix, 4);

	SetupLevel(Renderer, RenderCommand);

	// Setup ZCulling
	if (RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING)
	{
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		GetDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE ? FALSE : TRUE);
	}
	else
		GetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	// Setup Backface Culling
	if (TwoSided)
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Setup Wireframe
	if (Wireframe)
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// Setup Material Properties
	GetDevice()->SetPixelShaderConstantF(0, (const float*)&ZEVector4(1.0f / (float)Renderer->GetViewPort()->GetWidth(), 1.0f / (float)Renderer->GetViewPort()->GetHeight(), 0.5f / (float)Renderer->GetViewPort()->GetWidth(), 0.5f / (float)Renderer->GetViewPort()->GetHeight()), 1);
	GetDevice()->SetPixelShaderConstantF(1, (const float*)&ZEVector4(AmbientFactor * AmbientColor, 1.0f), 1);
	GetDevice()->SetPixelShaderConstantF(2, (const float*)&ZEVector4(DiffuseFactor * DiffuseColor, 1.0f), 1);

	GetDevice()->SetSamplerState(4, D3DSAMP_MAXANISOTROPY, 4);

	GetDevice()->SetPixelShader(ForwardPassPixelShader->GetPixelShader());
	GetDevice()->SetVertexShader(ForwardPassVertexShader->GetVertexShader());
	
	return true;
}

bool ZED3D9TerrainMaterial::SetupShadowPass() const
{
	return false;
}

void ZED3D9TerrainMaterial::UpdateMaterial()
{
	CreateShaders();
}

void ZED3D9TerrainMaterial::Release()
{
	ReleaseShaders();
}
