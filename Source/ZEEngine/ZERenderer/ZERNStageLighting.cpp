//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageLighting.cpp
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

// Copyright 2010 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#include "ZERNStageLighting.h"
#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRShaderCompileOptions.h"
#include "ZEError.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZERNStageGBuffer.h"
#include "ZEModules/ZEDirect3D11/ZED11Context.h"
#include "ZEModules/ZEDirect3D11/ZED11DepthStencilBuffer.h"
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics\ZEGRConstantBuffer.h"
#include "ZERandom.h"
#include "ZEGraphics\ZEGRTexture2D.h"
#include "ZEGraphics\ZEGRStructuredBuffer.h"
#include "ZECanvas.h"
#include "ZEGraphics\ZEGRTextureCube.h"
#include "ZEMath\ZEMath.h"

#define ZERN_SLDF_LIGHT_BUFFER	1
#define ZERN_SLDF_TILE_BUFFER	2
#define ZERN_SLDF_SHADERS		4
#define ZERN_SLDF_RENDER_STATE	8

ZEInt ZERNStageLighting::GetId()
{
	return ZERN_STAGE_LIGHTING;
}

const ZEString& ZERNStageLighting::GetName()
{
	static ZEString Name = "Lighting";
	return Name;
}

void ZERNStageLighting::SetOutput(ZEGRRenderTarget* Output)
{
	if(OutputRenderTarget == Output)
		return;

	OutputRenderTarget = Output;
}

ZEGRRenderTarget* ZERNStageLighting::GetOutput() const
{
	return OutputRenderTarget;
}

void ZERNStageLighting::SetIsTiledDeferred(bool Value)
{
	if(SetTiledDeferred == Value)
		return;

	SetTiledDeferred = Value;
}

bool ZERNStageLighting::GetIsTiledDefferred() const
{
	return SetTiledDeferred;
}

bool ZERNStageLighting::InitializeSelf()
{
	PrevWidth = 0;
	PrevHeight = 0;
	SetTiledDeferred = true;
	LightBuffer = NULL;
	TileInfoBuffer = NULL;
	OutputRenderTarget = NULL;

	CreateLights();
	CreateLightGeometries();

	LightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZERNLightConstants));

	DirtyFlags.RaiseAll();

	return true;
}

void ZERNStageLighting::DeinitializeSelf()
{
	TiledDeferredVertexShader.Release();
	TiledDeferredPixelShader.Release();
	TiledDeferredRenderState.Release();
	DeferredVertexShader.Release();
	DeferredPixelShader.Release();
	DeferredRenderState.Release();
	LightBuffer.Release();
	TileInfoBuffer.Release();
	LightConstantBuffer.Release();
	LightVertexBuffer.Release();

	Lights.Clear();
	TileInfos.Clear();
}

bool ZERNStageLighting::SetupTiledDeferred(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	if(PrevWidth != CurrentWidth || PrevHeight != CurrentHeight)
	{
		DirtyFlags.RaiseFlags(ZERN_SLDF_TILE_BUFFER);
		PrevWidth = CurrentWidth;
		PrevHeight = CurrentHeight;
	}

	ZERNView View = Renderer->GetView();
	AssingLightsToTiles(Renderer, Lights, View.ProjectionTransform.M11, View.ProjectionTransform.M22, View.NearZ);

	if(!Update())
		return false;

	Context->SetRenderState(TiledDeferredRenderState);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 5, LightBuffer);
	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 6, TileInfoBuffer);

	Context->Draw(3, 0);

	return true;
}

bool ZERNStageLighting::SetupDeferred(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	if(!Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, LightConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, LightConstantBuffer);

	Context->SetRenderState(DeferredRenderState);
	Context->SetVertexBuffers(0, 1, &LightVertexBuffer);

	ZESize LightCount = Lights.GetCount();
	for(ZESize I = 0; I < LightCount; ++I)
	{
		switch(Lights[I]->GetLightType())
		{
			case ZE_LT_POINT:
				DrawPointLight((ZELightPoint*)Lights[I], Renderer, Context);
				break;
			case ZE_LT_DIRECTIONAL:
				DrawDirectionalLight((ZELightDirectional*)Lights[I], Renderer, Context);
				break;
			case ZE_LT_PROJECTIVE:
				DrawProjectiveLight((ZELightProjective*)Lights[I], Renderer, Context);
				break;
			case ZE_LT_OMNIPROJECTIVE:
				DrawOmniProjectiveLight((ZELightOmniProjective*)Lights[I], Renderer, Context);
				break;
		}
	}

	return true;
}

bool ZERNStageLighting::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZERNStageGBuffer* StageGbuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	ZEGROutput* Output = Renderer->GetOutput();
	if(StageGbuffer == NULL || Output == NULL)
		return false;

	OutputRenderTarget = StageGbuffer->GetAccumulationMap()->GetRenderTarget();

	Context->SetViewports(1, &Output->GetViewport());
	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);

	SetupGbufferResources(Renderer, Context);

	if(SetTiledDeferred)
		return SetupTiledDeferred(Renderer, Context);
	else
		return SetupDeferred(Renderer, Context);
}

void ZERNStageLighting::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 6, NULL);

	Context->SetTexture(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 4, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 7, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
}

void ZERNStageLighting::UpdateClipRegionRoot(float nc,          // Tangent plane x/y normal coordinate (view space)
                          float lc,								// Light x/y coordinate (view space)
                          float lz,								// Light z coordinate (view space)
                          float lightRadius,
                          float cameraScale,					// Project scale for coordinate (_11 or _22 for x/y respectively)
                          float& clipMin,
                          float& clipMax)
{
    float nz = (lightRadius - nc * lc) / lz;
    float pz = (lc * lc + lz * lz - lightRadius * lightRadius) /
               (lz - (nz / nc) * lc);

	if (pz > 0.0f) 
	{
        float c = -nz * cameraScale / nc;
        if (nc > 0.0f) 
		{	// Left side boundary
            clipMin = max(clipMin, c);
        } else 
		{   // Right side boundary
            clipMax = min(clipMax, c);
        }
    }
}

void ZERNStageLighting::UpdateClipRegion(float lc,          // Light x/y coordinate (view space)
                      float lz,								// Light z coordinate (view space)
                      float lightRadius,
                      float cameraScale,					// Project scale for coordinate (_11 or _22 for x/y respectively)
                      float& clipMin,
                      float& clipMax)
{
    float rSq = lightRadius * lightRadius;
    float lcSqPluslzSq = lc * lc + lz * lz;
	float d = rSq * lc * lc - lcSqPluslzSq * (rSq - lz * lz);

    if (d > 0.0f) 
	{
        float a = lightRadius * lc;
        float b = sqrt(d);
        float nx0 = (a + b) / lcSqPluslzSq;
        float nx1 = (a - b) / lcSqPluslzSq;
        
        UpdateClipRegionRoot(nx0, lc, lz, lightRadius, cameraScale, clipMin, clipMax);
        UpdateClipRegionRoot(nx1, lc, lz, lightRadius, cameraScale, clipMin, clipMax);
    }
}

// Returns bounding box [min.xy, max.xy] in clip [-1, 1] space.
ZEVector4 ZERNStageLighting::ComputeClipRegion(const ZEVector3& lightPosView, float lightRadius, float CameraScaleX, float CameraScaleY, float CameraNear)
{
    // Early out with empty rectangle if the light is too far behind the view frustum
    ZEVector4 clipRegion = ZEVector4(1, 1, 0, 0);
    if (lightPosView.z + lightRadius >= CameraNear) 
	{
        ZEVector2 clipMin = ZEVector2(-1.0f, -1.0f);
        ZEVector2 clipMax = ZEVector2( 1.0f,  1.0f);
    
        UpdateClipRegion(lightPosView.x, lightPosView.z, lightRadius, CameraScaleX, clipMin.x, clipMax.x);
        UpdateClipRegion(lightPosView.y, lightPosView.z, lightRadius, CameraScaleY, clipMin.y, clipMax.y);

		clipRegion = ZEVector4(clipMin.x, clipMin.y, clipMax.x, clipMax.y);
    }

    return clipRegion;
}

void ZERNStageLighting::AssingLightsToTiles(ZERNRenderer* Renderer, const ZEArray<ZELight*>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear)
{
	ZEUInt Width = OutputRenderTarget->GetWidth();
	ZEUInt Height = OutputRenderTarget->GetHeight();

	ZEUInt TileCountX = (Width + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (Height + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	ZEUInt TileCount = TileCountX * TileCountY;
	ZEUInt LightCount = (ZEUInt)Lights.GetCount();

	if(TileInfos.GetCount() != TileCount)
		TileInfos.Resize(TileCount);
	
	memset(&TileInfos[0], 0, sizeof(ZERNTileInfo) * TileCount);

	for(ZEUInt I = 0; I < LightCount; ++I)
	{
		ZELight* Light = Lights[I];
		if(Light->GetLightType() == ZELightType::ZE_LT_POINT)
		{
			ZEVector3 LightPositionView = Renderer->GetView().ViewTransform * Light->GetPosition();
			ZEVector4 ClipSpaceQuad = ComputeClipRegion(LightPositionView, Light->GetRange(), CameraScaleX, CameraScaleY, CameraNear);

			float TempMaxY = ClipSpaceQuad.y;
			ClipSpaceQuad.y = ClipSpaceQuad.w;
			ClipSpaceQuad.w = TempMaxY;

			ClipSpaceQuad *= ZEVector4(0.5f, -0.5f, 0.5f, -0.5f);
			ClipSpaceQuad += ZEVector4(0.5f, 0.5f, 0.5f, 0.5f);

			ZEVector4 ScreenSpaceQuad = ZEVector4(ClipSpaceQuad.x * Width, ClipSpaceQuad.y * Height, ClipSpaceQuad.z * Width, ClipSpaceQuad.w * Height);

			if(ScreenSpaceQuad.z < ScreenSpaceQuad.x || ScreenSpaceQuad.w < ScreenSpaceQuad.y)
				continue;

			ZEUInt StartTileX = (ZEUInt)ZEMath::Max(ScreenSpaceQuad.x / TILE_SIZE_IN_PIXELS, 0.0f);
			ZEUInt StartTileY = (ZEUInt)ZEMath::Max(ScreenSpaceQuad.y / TILE_SIZE_IN_PIXELS, 0.0f);

			ZEUInt EndTileX = (ZEUInt)ZEMath::Min(ScreenSpaceQuad.z / TILE_SIZE_IN_PIXELS, TileCountX - 1.0f);
			ZEUInt EndTileY = (ZEUInt) ZEMath::Min(ScreenSpaceQuad.w / TILE_SIZE_IN_PIXELS, TileCountY - 1.0f);

			for(ZEUInt Y = StartTileY; Y <= EndTileY; ++Y)
			{
				for(ZEUInt X = StartTileX; X <= EndTileX; ++X)
				{
					ZEUInt TileId = Y * TileCountX + X;
					ZERNTileInfo& Info = TileInfos[TileId];
					Info.LightIndices[Info.LightCount] = I;
					Info.LightCount = (Info.LightCount >= (MAX_LIGHTS)) ? (MAX_LIGHTS) : ++Info.LightCount;
				}
			}
		}
		else if(Light->GetLightType() == ZELightType::ZE_LT_DIRECTIONAL)
		{
			for(ZEUInt Y = 0; Y < TileCountY; ++Y)
			{
				for(ZEUInt X = 0; X < TileCountX; ++X)
				{
					ZEUInt TileId = Y * TileCountX + X;
					ZERNTileInfo& Info = TileInfos[TileId];
					Info.LightIndices[Info.LightCount] = I;
					Info.LightCount = (Info.LightCount >= (MAX_LIGHTS)) ? (MAX_LIGHTS) : ++Info.LightCount;
				}
			}
		}
	}
}

void ZERNStageLighting::CreateLights()
{
	ZERandom::Reset();

	for(ZEInt I = 0; I < 20; ++I)
	{
		ZELightPoint* PointLight = ZELightPoint::CreateInstance();
		PointLight->SetIntensity(0.3f);
		PointLight->SetColor(ZEVector3(ZERandom::GetFloatPositive(), ZERandom::GetFloatPositive(), ZERandom::GetFloatPositive()));
		PointLight->SetAttenuation(1.0f, 0.1f, 0.05f);
		PointLight->SetPosition(ZEVector3(ZERandom::GetFloat() * 10.0f, 5.0f, 0.0f));
		PointLight->SetRange(10);

		Lights.Add(PointLight);
	}
	
	ZELightDirectional* DirectionalLight = ZELightDirectional::CreateInstance();
	DirectionalLight->SetIntensity(0.5f);
	DirectionalLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	DirectionalLight->SetPosition(ZEVector3(0.0f, 50.0f, 10.0f));
	ZEQuaternion Rotation;
	ZEQuaternion::CreateFromDirection(Rotation, ZEVector3(1.0f, 1.0f, 1.0f));
	DirectionalLight->SetRotation(Rotation);

	Lights.Add(DirectionalLight);

	/*ZELightOmniProjective* OmniProjectiveLight = ZELightOmniProjective::CreateInstance();
	OmniProjectiveLight->SetProjectionTextureFile("#R:/ZEEngine/ZEAtmosphere/Textures/c.tga");
	OmniProjectiveLight->SetIntensity(5.0f);
	OmniProjectiveLight->SetAttenuation(1.0f, 0.5f, 0.4f);
	OmniProjectiveLight->SetRange(10.0f);
	OmniProjectiveLight->SetPosition(ZEVector3(0.0f, 10.0f, 0.0f));

	Lights.Add(OmniProjectiveLight);

	ZELightProjective* ProjectiveLight = ZELightProjective::CreateInstance();
	ProjectiveLight->SetPosition(ZEVector3(20.0f, 10.0f, 0.0f));
	ZEQuaternion::CreateFromDirection(Rotation, ZEVector3(0.0001f, -1.0f, 0.0001f));
	ProjectiveLight->SetRotation(Rotation);
	ProjectiveLight->SetProjectionTextureFile("#R:/ZEEngine/ZEPostEffects/Textures/BlurMask.png");
	ProjectiveLight->SetIntensity(5.0f);
	ProjectiveLight->SetAspectRatio(1.0f);
	ProjectiveLight->SetFOV(ZE_PI / 3.0f);
	ProjectiveLight->SetAttenuation(1.0f, 0.0f, 0.0f);
	ProjectiveLight->SetRange(10.0f);

	Lights.Add(ProjectiveLight);*/
}

void ZERNStageLighting::CreateLightGeometries()
{
	/*
	Primitive	Start		Start(Byte)	Vertex Count	Prim. Count		Size (Bytes)
	Quad		0			0			6				2				288
	Sphere12	6			288			936				312				44928
	Sphere24	942			45216		3600			1200			172800
	Cone12		4542		218016		18				6				864
	End			4650				
	*/

	ZECanvas Canvas;
	// Directional
	Canvas.AddQuad(
		ZEVector3(-1.0f, -1.0f, 0.0f),
		ZEVector3(-1.0f, 1.0f, 0.0f),
		ZEVector3(1.0f, 1.0f, 0.0f),
		ZEVector3(1.0f, -1.0f, 0.0f));

	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Projective
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

	LightVertexBuffer = Canvas.CreateVertexBuffer();
}

bool ZERNStageLighting::UpdateBuffers()
{
	if(DirtyFlags.GetFlags(ZERN_SLDF_LIGHT_BUFFER))
	{
		LightBuffer.Release();
		LightBuffer = ZEGRStructuredBuffer::Create(MAX_LIGHTS, sizeof(ZERNGPULight));

		DirtyFlags.UnraiseFlags(ZERN_SLDF_LIGHT_BUFFER);
	}

	ZERNGPULight* GPULight;
	LightBuffer->Lock((void**)&GPULight);

	ZESize Count = Lights.GetCount();
	for(ZESize I = 0; I < Count; ++I)
	{
		GPULight[I].PositionWorld = Lights[I]->GetPosition();
		GPULight[I].Range = Lights[I]->GetRange();
		GPULight[I].Color = Lights[I]->GetColor();
		GPULight[I].Intensity = Lights[I]->GetIntensity();
		GPULight[I].Attenuation = Lights[I]->GetAttenuation();
		GPULight[I].Type = Lights[I]->GetLightType();
		ZEVector3 Direction;
		ZEQuaternion::ConvertToEulerAngles(Direction, Lights[I]->GetRotation());
		Direction.NormalizeSelf();
		GPULight[I].Direction = Direction;
	}

	LightBuffer->Unlock();

	if(DirtyFlags.GetFlags(ZERN_SLDF_TILE_BUFFER))
	{
		ZEUInt TileCountX = (OutputRenderTarget->GetWidth() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		ZEUInt TileCountY = (OutputRenderTarget->GetHeight() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		
		TileInfoBuffer.Release();
		TileInfoBuffer = ZEGRStructuredBuffer::Create(TileCountX * TileCountY, sizeof(ZERNTileInfo));

		DirtyFlags.UnraiseFlags(ZERN_SLDF_TILE_BUFFER);
	}

	ZERNTileInfo* TileInfo;
	TileInfoBuffer->Lock((void**)&TileInfo);
		memcpy(TileInfo, &TileInfos[0], TileInfos.GetCount() * sizeof(ZERNTileInfo));
	TileInfoBuffer->Unlock();

	return true;
}

bool ZERNStageLighting::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGRPrimitiveType::ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetRenderTargetFormat(0, OutputRenderTarget->GetFormat());

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTarget = BlendState.GetRenderTarget(0);
	BlendRenderTarget.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTarget.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTarget.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTarget.SetBlendEnable(true);
	BlendState.SetRenderTargetBlend(0, BlendRenderTarget);

	RenderState.SetBlendState(BlendState);

	RenderState.SetShader(ZEGR_ST_VERTEX, TiledDeferredVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, TiledDeferredPixelShader);

	ZEGRDepthStencilState TiledDeferredDepthStencilState;
	TiledDeferredDepthStencilState.SetDepthTestEnable(false);
	TiledDeferredDepthStencilState.SetDepthWriteEnable(false);
	TiledDeferredDepthStencilState.SetStencilTestEnable(false);

	RenderState.SetDepthStencilState(TiledDeferredDepthStencilState);

	TiledDeferredRenderState = RenderState.Compile();
	zeCheckError(TiledDeferredRenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_VERTEX, DeferredVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, DeferredPixelShader);

	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	DeferredRenderState = RenderState.Compile();
	zeCheckError(DeferredRenderState == NULL, false, "Cannot set render state.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::UpdateShaders()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.Model = ZEGR_SM_5_0;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShading.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNScreenCover_VertexShader_Position";
	TiledDeferredVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNTiledDeferredShading_PixelShader_LightingStage";
	TiledDeferredPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredPixelShader == NULL, false, "Cannot set pixel shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDeferredShading.hlsl";

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNDeferredShading_VertexShader_LightingStage";
	DeferredVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredVertexShader == NULL, false, "Cannot set vertex shader.");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_PixelShader_LightingStage";
	DeferredPixelShader = ZEGRShader::Compile(Options);
	zeCheckError(DeferredPixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

void ZERNStageLighting::SetupGbufferResources(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Renderer->GetStage(ZERN_STAGE_GBUFFER));
	if(StageGBuffer == NULL)
		return;

	Context->SetTexture(ZEGR_ST_PIXEL, 2, StageGBuffer->GetNormalMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 3, StageGBuffer->GetDiffuseColorMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 4, StageGBuffer->GetSpecularColorMap());

	ZED11DepthStencilBuffer* D11DepthStencilBuffer = (ZED11DepthStencilBuffer*)StageGBuffer->GetDepthStencilBuffer();
	ID3D11ShaderResourceView* NativeTexture =  D11DepthStencilBuffer->GetResourceView();
	ID3D11DeviceContext1* D11Context = static_cast<ZED11Context*>(Context)->GetContext();
	D11Context->PSSetShaderResources(0, 1, &NativeTexture);
}

bool ZERNStageLighting::Update()
{
	if(!UpdateShaders())
		return false;

	if(!UpdateRenderState())
		return false;

	if(SetTiledDeferred && !UpdateBuffers())
			return false;

	return true;
}
 
void ZERNStageLighting::DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZERNLightConstants* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		ZERNGPULight& GPULight = LightConstants->Light;
		GPULight.PositionWorld = DirectionalLight->GetPosition();
		GPULight.Range = DirectionalLight->GetRange();
		GPULight.Color = DirectionalLight->GetColor();
		GPULight.Intensity = DirectionalLight->GetIntensity();
		GPULight.Attenuation = DirectionalLight->GetAttenuation();
		GPULight.Type = DirectionalLight->GetLightType();
		ZEVector3 Direction;
		ZEQuaternion::ConvertToEulerAngles(Direction, DirectionalLight->GetRotation());
		Direction.NormalizeSelf();
		GPULight.Direction = Direction;
	LightConstantBuffer->Unlock();

	Context->Draw(6, 0);
}

void ZERNStageLighting::DrawPointLight(ZELightPoint* PointLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZERNLightConstants* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		ZERNGPULight& GPULight = LightConstants->Light;
		GPULight.PositionWorld = PointLight->GetPosition();
		GPULight.Range = PointLight->GetRange();
		GPULight.Color = PointLight->GetColor();
		GPULight.Intensity = PointLight->GetIntensity();
		GPULight.Attenuation = PointLight->GetAttenuation();
		GPULight.Type = PointLight->GetLightType();
		ZEVector3 Direction;
		ZEQuaternion::ConvertToEulerAngles(Direction, PointLight->GetRotation());
		Direction.NormalizeSelf();
		GPULight.Direction = Direction;

		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, PointLight->GetPosition(), ZEQuaternion::Identity,
			ZEVector3(GPULight.Range, GPULight.Range, GPULight.Range));
	LightConstantBuffer->Unlock();

	Context->Draw(3600, 942);
}

void ZERNStageLighting::DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEMatrix4x4 LightViewMatrix;
	ZEMatrix4x4::CreateViewTransform(LightViewMatrix, ProjectiveLight->GetWorldPosition(), ProjectiveLight->GetWorldRotation());

	ZEMatrix4x4 LightProjectionMatrix;
	ZEMatrix4x4::CreatePerspectiveProjection(LightProjectionMatrix, ProjectiveLight->GetFOV(), ProjectiveLight->GetAspectRatio(), Renderer->GetView().NearZ, ProjectiveLight->GetRange());

	ZEMatrix4x4 LightViewProjectionMatrix;
	ZEMatrix4x4::Multiply(LightViewProjectionMatrix, LightProjectionMatrix, LightViewMatrix);

	ZEMatrix4x4 TextureMatrix;
	ZEMatrix4x4::Create(TextureMatrix,
						0.5f, 0.0f, 0.0f, 0.5f,
						0.0f, -0.5f, 0.0f, 0.5f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);

	ZEMatrix4x4 InvCameraViewMatrix;
	ZEMatrix4x4::Inverse(InvCameraViewMatrix, Renderer->GetView().ViewTransform);

	ZEMatrix4x4 LightSpaceMatrix;
	ZEMatrix4x4::Multiply(LightSpaceMatrix, LightViewProjectionMatrix, InvCameraViewMatrix);
	
	ZEMatrix4x4 ProjectionMatrix;
	ZEMatrix4x4::Multiply(ProjectionMatrix, TextureMatrix, LightSpaceMatrix);

	ZERNLightConstants* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		ZERNGPULight& GPULight = LightConstants->Light;
		GPULight.PositionWorld = ProjectiveLight->GetPosition();
		GPULight.Range = ProjectiveLight->GetRange();
		GPULight.Color = ProjectiveLight->GetColor();
		GPULight.Intensity = ProjectiveLight->GetIntensity();
		GPULight.Attenuation = ProjectiveLight->GetAttenuation();
		GPULight.Type = ProjectiveLight->GetLightType();
		ZEVector3 Direction;
		ZEQuaternion::ConvertToEulerAngles(Direction, ProjectiveLight->GetRotation());
		Direction.NormalizeSelf();
		GPULight.Direction = Direction;

		LightConstants->ProjectionMatrix = ProjectionMatrix;
		float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, ProjectiveLight->GetPosition(), ProjectiveLight->GetWorldRotation(),
			ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));
	LightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, (ZEGRTexture*)ProjectiveLight->GetProjectionTexture());

	Context->Draw(18, 4542);
}

void ZERNStageLighting::DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEQuaternion ProjectionRotation;
	ZEQuaternion::Product(ProjectionRotation, OmniProjectiveLight->GetWorldRotation().Conjugate(), Renderer->GetView().Rotation);
	
	ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);

	ZEMatrix3x3 RotationMatrix;
	ZEMatrix3x3::CreateRotation(RotationMatrix, ProjectionRotation);

	ZERNLightConstants* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		ZERNGPULight& GPULight = LightConstants->Light;
		GPULight.PositionWorld = OmniProjectiveLight->GetPosition();
		GPULight.Range = OmniProjectiveLight->GetRange();
		GPULight.Color = OmniProjectiveLight->GetColor();
		GPULight.Intensity = OmniProjectiveLight->GetIntensity();
		GPULight.Attenuation = OmniProjectiveLight->GetAttenuation();
		GPULight.Type = OmniProjectiveLight->GetLightType();
		
		LightConstants->RotationMatrix = RotationMatrix;
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, OmniProjectiveLight->GetPosition(), ZEQuaternion::Identity,
			ZEVector3(GPULight.Range, GPULight.Range, GPULight.Range));
	LightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearClamp);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, (ZEGRTextureCube*)OmniProjectiveLight->GetProjectionTexture());

	Context->Draw(936, 6);
}
