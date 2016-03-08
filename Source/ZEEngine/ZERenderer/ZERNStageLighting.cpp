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

#include "ZEError.h"
#include "ZERandom.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZERNStageID.h"
#include "ZERNRenderer.h"
#include "ZERNStageGBuffer.h"
#include "ZECanvas.h"
#include "ZELightPoint.h"
#include "ZELightDirectional.h"
#include "ZELightProjective.h"
#include "ZELightOmniProjective.h"
#include "ZEGame/ZEScene.h"
#include "ZEGame/ZEEntity.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"

#define ZERN_SLDF_LIGHT_BUFFER	1
#define ZERN_SLDF_TILE_BUFFER	2
#define ZERN_SLDF_SHADERS		4
#define ZERN_SLDF_RENDER_STATE	8

void ZERNStageLighting::CreateRandomVectors()
{
	const ZEUInt Size = 128 * 128 * 2;
	ZEUInt8 Vectors[Size];
	for(ZEUInt I = 0; I < Size; I += 2)
	{
		Vectors[I]		= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
		Vectors[I + 1]	= (ZEUInt8)(ZERandom::GetFloatPositive() * 255.0f + 0.5f);
	}

	RandomVectorsTexture.Release();
	RandomVectorsTexture = ZEGRTexture2D::CreateInstance(128, 128, 1, 1, 1, ZEGR_TF_R8G8_UNORM);
	RandomVectorsTexture->UpdateSubResource(0, 0, &Vectors[0], 128 * 2);
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
		ZEVector3(-1.0f, 1.0f, 0.0f),
		ZEVector3(-1.0f, -1.0f, 0.0f),
		ZEVector3(1.0f, -1.0f, 0.0f),
		ZEVector3(1.0f, 1.0f, 0.0f));

	// Point/OmniProjective
	Canvas.AddSphere(1.0f, 12, 12);
	Canvas.AddSphere(1.0f, 24, 24);

	// Projective
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

	DeferredLightVertexBuffer = Canvas.CreateVertexBuffer();
}

void ZERNStageLighting::CreateSamplers()
{
	ZEGRSamplerDescription SamplerLinearBorderDescription;
	SamplerLinearBorderDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerLinearBorderDescription.BorderColor = ZEVector4::Zero;
	SamplerLinearBorder = ZEGRSampler::GetSampler(SamplerLinearBorderDescription);

	ZEGRSamplerDescription SamplerComparisonLinearPointClampDescription;
	SamplerComparisonLinearPointClampDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerComparisonLinearPointClampDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerComparisonLinearPointClampDescription.AddressU = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.AddressV = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.AddressW = ZEGR_TAM_CLAMP;
	SamplerComparisonLinearPointClampDescription.ComparisonFunction = ZEGR_CF_LESS_EQUAL;
	SamplerComparisonLinearPointClamp = ZEGRSampler::GetSampler(SamplerComparisonLinearPointClampDescription);

	ZEGRSamplerDescription SamplerPointWrapDescription;
	SamplerPointWrapDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointWrapDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerPointWrapDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerPointWrap = ZEGRSampler::GetSampler(SamplerPointWrapDescription);

	ZEGRSamplerDescription SamplerPointBorderDescription;
	SamplerPointBorderDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerPointBorderDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerPointBorderDescription.BorderColor = ZEVector4::One;
	SamplerPointBorder = ZEGRSampler::GetSampler(SamplerPointBorderDescription);
}

bool ZERNStageLighting::UpdateRenderState()
{
	if(!DirtyFlags.GetFlags(ZERN_SLDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState;
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetRenderTargetFormat(0, OutputRenderTarget->GetFormat());

	ZEGRBlendState BlendStateAdditive;
	BlendStateAdditive.SetBlendEnable(true);
	ZEGRBlendRenderTarget BlendRenderTargetAdditive = BlendStateAdditive.GetRenderTarget(0);
	BlendRenderTargetAdditive.SetSource(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetDestination(ZEGRBlend::ZEGR_BO_ONE);
	BlendRenderTargetAdditive.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendRenderTargetAdditive.SetBlendEnable(true);
	BlendStateAdditive.SetRenderTargetBlend(0, BlendRenderTargetAdditive);

	RenderState.SetBlendState(BlendStateAdditive);

	ZEGRDepthStencilState DepthStencilStateNoTestWrite;
	DepthStencilStateNoTestWrite.SetDepthTestEnable(false);
	DepthStencilStateNoTestWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateNoTestWrite);

	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, TiledDeferredPixelShader);

	TiledDeferredRenderState = RenderState.Compile();
	zeCheckError(TiledDeferredRenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_VERTEX, DeferredVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, DeferredPixelShader);

	ZEGRRasterizerState RasterizerStateCullFront;
	RasterizerStateCullFront.SetCullMode(ZEGR_CMD_FRONT);

	RenderState.SetRasterizerState(RasterizerStateCullFront);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	DeferredRenderState = RenderState.Compile();
	zeCheckError(DeferredRenderState == NULL, false, "Cannot set render state.");

	RenderState = ZEGRRenderState();

	RenderState.SetShader(ZEGR_ST_COMPUTE, TiledDeferredComputeShader);
	TiledDeferredComputeRenderState = RenderState.Compile();
	zeCheckError(TiledDeferredComputeRenderState == NULL, false, "Cannot set render state.");

	RenderState = ZEGRRenderState();

	RenderState.SetBlendState(BlendStateAdditive);
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetShader(ZEGR_ST_VERTEX, ScreenCoverVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, AccumulatePixelShader);

	AccumulateRenderState = RenderState.Compile();
	zeCheckError(AccumulateRenderState == NULL, false, "Cannot set render state.");

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
	ScreenCoverVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(ScreenCoverVertexShader == NULL, false, "Cannot set vertex shader.");

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

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNTiledDeferredShadingCompute.hlsl";

	Options.Type = ZEGR_ST_COMPUTE;
	Options.EntryPoint = "ZERNTiledDeferredShadingCompute_ComputeShader_Main";
	TiledDeferredComputeShader = ZEGRShader::Compile(Options);
	zeCheckError(TiledDeferredComputeShader == NULL, false, "Cannot set compute shader.");

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNDeferredShading.hlsl";

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNDeferredShading_Accumulate_PixelShader_Main";
	AccumulatePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(AccumulatePixelShader == NULL, false, "Cannot set pixel shader.");

	DirtyFlags.UnraiseFlags(ZERN_SLDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SLDF_RENDER_STATE);

	return true;
}

bool ZERNStageLighting::Update()
{
	if(!UpdateShaders())
		return false;

	if(!UpdateRenderState())
		return false;

	return true;
}

void ZERNStageLighting::UpdateClipRegionRoot(float nc,          // Tangent plane x/y normal coordinate (view space)
											 float lc,			// Light x/y coordinate (view space)
											 float lz,			// Light z coordinate (view space)
											 float lightRadius,
											 float cameraScale,	// Project scale for coordinate (_11 or _22 for x/y respectively)
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
			clipMin = ZEMath::Max(clipMin, c);
		} else 
		{   // Right side boundary
			clipMax = ZEMath::Min(clipMax, c);
		}
	}
}

void ZERNStageLighting::UpdateClipRegion(float lc,          // Light x/y coordinate (view space)
										 float lz,			// Light z coordinate (view space)
										 float lightRadius,
										 float cameraScale,	// Project scale for coordinate (_11 or _22 for x/y respectively)
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

void ZERNStageLighting::AssignLightsToTiles(ZERNRenderer* Renderer, const ZEList2<ZELight>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear)
{
	ZEUInt Widthh = OutputRenderTarget->GetWidth();
	ZEUInt Heightt = OutputRenderTarget->GetHeight();

	ZEUInt TileCountX = (Widthh + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (Heightt + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	ZEUInt TileCount = TileCountX * TileCountY;
	ZESize LightCount = Lights.GetCount();

	if(Tiles.GetCount() != TileCount)
		Tiles.Resize(TileCount);

	memset(&Tiles[0], 0, sizeof(TileStruct) * TileCount);

	ZEUInt LightIndex = 0;
	const ZELink<ZELight>* Link = Lights.GetFirst();
	while(Link != NULL)
	{
		const ZELight* Light = Link->GetItem();
		if(Light->GetLightType() == ZELightType::ZE_LT_POINT)
		{
			ZEVector3 LightPositionView = Renderer->GetView().ViewTransform * Light->GetWorldPosition();
			ZEVector4 ClipSpaceQuad = ComputeClipRegion(LightPositionView, Light->GetRange(), CameraScaleX, CameraScaleY, CameraNear);

			float TempMaxY = ClipSpaceQuad.y;
			ClipSpaceQuad.y = ClipSpaceQuad.w;
			ClipSpaceQuad.w = TempMaxY;

			ClipSpaceQuad *= ZEVector4(0.5f, -0.5f, 0.5f, -0.5f);
			ClipSpaceQuad += ZEVector4(0.5f, 0.5f, 0.5f, 0.5f);

			ZEVector4 ScreenSpaceQuad = ZEVector4(ClipSpaceQuad.x * Widthh, ClipSpaceQuad.y * Heightt, ClipSpaceQuad.z * Widthh, ClipSpaceQuad.w * Heightt);

			if(ScreenSpaceQuad.z > ScreenSpaceQuad.x && ScreenSpaceQuad.w > ScreenSpaceQuad.y)
			{
				ZEUInt StartTileX = (ZEUInt)ZEMath::Max(ScreenSpaceQuad.x / TILE_SIZE_IN_PIXELS, 0.0f);
				ZEUInt StartTileY = (ZEUInt)ZEMath::Max(ScreenSpaceQuad.y / TILE_SIZE_IN_PIXELS, 0.0f);

				ZEUInt EndTileX = (ZEUInt)ZEMath::Min(ScreenSpaceQuad.z / TILE_SIZE_IN_PIXELS, TileCountX - 1.0f);
				ZEUInt EndTileY = (ZEUInt) ZEMath::Min(ScreenSpaceQuad.w / TILE_SIZE_IN_PIXELS, TileCountY - 1.0f);

				for(ZEUInt Y = StartTileY; Y <= EndTileY; Y++)
				{
					for(ZEUInt X = StartTileX; X <= EndTileX; X++)
					{
						ZEUInt TileId = Y * TileCountX + X;
						TileStruct& Tile = Tiles[TileId];
						Tile.LightIndices[Tile.LightCount] = LightIndex;
						Tile.LightCount = (Tile.LightCount >= (MAX_LIGHT)) ? (MAX_LIGHT) : ++Tile.LightCount;
					}
				}
			}
		}

		LightIndex++;
		Link = Link->GetNext();
	}
}

bool ZERNStageLighting::SetupDeferred(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	if(!Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, DeferredLightConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, DeferredLightConstantBuffer);

	Context->SetRenderState(DeferredRenderState);
	Context->SetVertexBuffers(0, 1, DeferredLightVertexBuffer.GetPointerToPointer());

	ZELink<ZELight>* Link = DeferredLightList.GetFirst();
	while(Link != NULL)
	{
		ZELight* Light = Link->GetItem();
		switch(Light->GetLightType())
		{
		case ZE_LT_POINT:
			DrawPointLight(static_cast<ZELightPoint*>(Light), Renderer, Context);
			break;
		case ZE_LT_DIRECTIONAL:
			DrawDirectionalLight(static_cast<ZELightDirectional*>(Light), Renderer, Context);
			break;
		case ZE_LT_PROJECTIVE:
			DrawProjectiveLight(static_cast<ZELightProjective*>(Light), Renderer, Context);
			break;
		case ZE_LT_OMNIPROJECTIVE:
			DrawOmniProjectiveLight(static_cast<ZELightOmniProjective*>(Light), Renderer, Context);
			break;
		}

		Link = Link->GetNext();
	}

	DeferredLightList.Clean();

	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	return true;
}

bool ZERNStageLighting::SetupTiledDeferred(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	if(!Update())
		return false;

	const ZERNView& View = Renderer->GetView();
	AssignLightsToTiles(Renderer, TiledDeferredLightList, View.ProjectionTransform.M11, View.ProjectionTransform.M22, View.NearZ);

	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	if(Width != CurrentWidth || Height != CurrentHeight)
	{
		Width = CurrentWidth;
		Height = CurrentHeight;

		ZEUInt TileCountX = (Width + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		ZEUInt TileCountY = (Height + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

		TiledDeferredTileStructuredBuffer.Release();
		TiledDeferredTileStructuredBuffer = ZEGRStructuredBuffer::Create(TileCountX * TileCountY, sizeof(TileStruct));

		TileStruct* Tile;
		TiledDeferredTileStructuredBuffer->Lock((void**)&Tile);
		memcpy(Tile, &Tiles[0], Tiles.GetCount() * sizeof(TileStruct));
		TiledDeferredTileStructuredBuffer->Unlock();

	}

	ZEUInt LightIndex = 0;
	ZELink<ZELight>* Link = TiledDeferredLightList.GetFirst();
	while(Link != NULL)
	{
		ZELight* SrcLight = Link->GetItem();
		LightStruct& DestLight = TiledDeferredLightConstants.Lights[LightIndex];

		ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, SrcLight->GetWorldPosition());
		DestLight.Range = SrcLight->GetRange();
		DestLight.Color = SrcLight->GetColor();
		DestLight.Intensity = SrcLight->GetIntensity();
		DestLight.Attenuation = SrcLight->GetAttenuation();
		DestLight.Type = SrcLight->GetLightType();
		DestLight.DirectionView = ZEVector3::Zero;

		LightIndex++;
		Link = Link->GetNext();
	}

	TiledDeferredLightConstants.LightCount = TiledDeferredLightList.GetCount();

	TiledDeferredLightList.Clean();

	TiledDeferredLightConstantBuffer->SetData(&TiledDeferredLightConstants);

	Context->SetRenderState(TiledDeferredRenderState);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, TiledDeferredLightConstantBuffer);
	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 5, TiledDeferredTileStructuredBuffer);
	Context->SetVertexBuffers(0, 0, NULL);

	Context->Draw(3, 0);

	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	return true;
}

bool ZERNStageLighting::SetupComputeTiledDeferred(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEUInt CurrentWidth = OutputRenderTarget->GetWidth();
	ZEUInt CurrentHeight = OutputRenderTarget->GetHeight();

	if(TiledDeferredComputeOutputTexture.IsNull() ||
		TiledDeferredComputeOutputTexture->GetWidth() != CurrentWidth || 
		TiledDeferredComputeOutputTexture->GetHeight() != CurrentHeight)
	{
		TiledDeferredComputeOutputTexture.Release();
		TiledDeferredComputeOutputTexture = ZEGRTexture2D::CreateInstance(CurrentWidth, CurrentHeight, 1, 1, 1, ZEGR_TF_R11G11B10_FLOAT, false, false, true);
	}

	if(!Update())
		return false;

	const ZERNView& View = Renderer->GetView();
	ZEUInt LightIndex = 0;
	ZELink<ZELight>* Link = TiledDeferredLightList.GetFirst();
	while(Link != NULL)
	{
		ZELight* SrcLight = Link->GetItem();
		LightStruct& DestLight = TiledDeferredLightConstants.Lights[LightIndex];

		ZEMatrix4x4::Transform(DestLight.PositionView, View.ViewTransform, SrcLight->GetWorldPosition());
		DestLight.Range = SrcLight->GetRange();
		DestLight.Color = SrcLight->GetColor();
		DestLight.Intensity = SrcLight->GetIntensity();
		DestLight.Attenuation = SrcLight->GetAttenuation();
		DestLight.Type = SrcLight->GetLightType();
		DestLight.DirectionView = ZEVector3::Zero;

		LightIndex++;
		Link = Link->GetNext();
	}

	TiledDeferredLightConstants.LightCount = TiledDeferredLightList.GetCount();

	TiledDeferredLightList.Clean();

	TiledDeferredLightConstantBuffer->SetData(&TiledDeferredLightConstants);

	Context->SetRenderState(TiledDeferredComputeRenderState);

	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, TiledDeferredLightConstantBuffer);
	Context->SetUnorderedAccessView(ZEGR_ST_COMPUTE, 0, TiledDeferredComputeOutputTexture);

	ZEUInt TileCountX = (CurrentWidth + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (CurrentHeight + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	Context->Dispatch(TileCountX, TileCountY, 1);

	Context->SetUnorderedAccessView(ZEGR_ST_COMPUTE, 0, NULL);
	Context->SetConstantBuffer(ZEGR_ST_COMPUTE, 8, NULL);
}

void ZERNStageLighting::DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		Light.Color = DirectionalLight->GetColor();
		Light.Intensity = DirectionalLight->GetIntensity();
		Light.Type = DirectionalLight->GetLightType();
		ZEVector3 Direction = DirectionalLight->GetWorldRotation() * ZEVector3::UnitZ;
 		ZEMatrix4x4::Transform3x3(Light.DirectionView, Renderer->GetView().ViewTransform, -Direction);
		Light.DirectionView.NormalizeSelf();

		LightConstants->CastShadow = static_cast<ZEBool32>(DirectionalLight->GetCastsShadow());
		LightConstants->ShowCascades = static_cast<ZEBool32>(ShowCascades);
		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
		LightConstants->SampleLengthOffset = DirectionalLight->GetShadowSampleLengthOffset();
	DeferredLightConstantBuffer->Unlock();

	DirectionalLight->BindCascades(Renderer, Context);	//TODO: Another way to do this?

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerComparisonLinearPointClamp);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointWrap);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerPointBorder);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, DirectionalLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(6, 0);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 3, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
}

void ZERNStageLighting::DrawPointLight(ZELightPoint* PointLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		ZEMatrix4x4::Transform(Light.PositionView, Renderer->GetView().ViewTransform, PointLight->GetPosition());
		Light.Range = PointLight->GetRange();
		Light.Color = PointLight->GetColor();
		Light.Intensity = PointLight->GetIntensity();
		Light.Attenuation = PointLight->GetAttenuation();
		Light.Type = PointLight->GetLightType();
		Light.DirectionView = ZEVector3::Zero;

		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, PointLight->GetPosition(), ZEQuaternion::Identity,
										ZEVector3(Light.Range, Light.Range, Light.Range));
	DeferredLightConstantBuffer->Unlock();

	Context->Draw(3600, 942);
}

void ZERNStageLighting::DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		ZEMatrix4x4::Transform(Light.PositionView, Renderer->GetView().ViewTransform, ProjectiveLight->GetWorldPosition());
		Light.Range = ProjectiveLight->GetRange();
		Light.Color = ProjectiveLight->GetColor();
		Light.Intensity = ProjectiveLight->GetIntensity();
		Light.Attenuation = ProjectiveLight->GetAttenuation();
		Light.Type = ProjectiveLight->GetLightType();
		Light.DirectionView = ZEVector3::Zero;

		LightConstants->CastShadow = static_cast<ZEBool32>(ProjectiveLight->GetCastsShadow());
		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
		LightConstants->SampleLengthOffset = ProjectiveLight->GetShadowSampleLengthOffset();

		LightConstants->ProjectionMatrix = ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * Renderer->GetView().InvViewTransform;

		float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, ProjectiveLight->GetWorldPosition(), ProjectiveLight->GetWorldRotation(),
			ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));
	DeferredLightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerComparisonLinearPointClamp);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointWrap);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, ProjectiveLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 6, ProjectiveLight->GetProjectionTexture());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(18, 4542);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 1, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 2, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 8, NULL);
}

void ZERNStageLighting::DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEQuaternion ProjectionRotation;
	ZEQuaternion::Product(ProjectionRotation, OmniProjectiveLight->GetWorldRotation().Conjugate(), Renderer->GetView().Rotation);
	
	ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);

	ZEMatrix4x4 RotationMatrix;
	ZEMatrix4x4::CreateRotation(RotationMatrix, ProjectionRotation);

	DeferredLightConstantsStruct* LightConstants;
	DeferredLightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		ZEMatrix4x4::Transform(Light.PositionView, Renderer->GetView().ViewTransform, OmniProjectiveLight->GetPosition());
		Light.Range = OmniProjectiveLight->GetRange();
		Light.Color = OmniProjectiveLight->GetColor();
		Light.Intensity = OmniProjectiveLight->GetIntensity();
		Light.Attenuation = OmniProjectiveLight->GetAttenuation();
		Light.Type = OmniProjectiveLight->GetLightType();
		Light.DirectionView = ZEVector3::Zero;

		LightConstants->RotationMatrix = RotationMatrix;
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, OmniProjectiveLight->GetPosition(), ZEQuaternion::Identity,
			ZEVector3(Light.Range, Light.Range, Light.Range));
	DeferredLightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder);
	Context->SetTexture(ZEGR_ST_PIXEL, 6, OmniProjectiveLight->GetProjectionTexture());

	Context->Draw(936, 6);

	Context->SetTexture(ZEGR_ST_PIXEL, 6, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
}

bool ZERNStageLighting::InitializeSelf()
{
	if (!ZERNStage::InitializeSelf())
		return false;

	CreateRandomVectors();
	CreateLightGeometries();
	CreateSamplers();

	DeferredLightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(DeferredLightConstantsStruct));
	TiledDeferredLightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(TiledDeferredLightConstantsStruct));

	return true;
}

void ZERNStageLighting::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	ScreenCoverVertexShader.Release();
	DeferredVertexShader.Release();
	DeferredPixelShader.Release();
	TiledDeferredPixelShader.Release();
	TiledDeferredComputeShader.Release();
	AccumulatePixelShader.Release();

	DeferredRenderState.Release();
	TiledDeferredRenderState.Release();
	TiledDeferredComputeRenderState.Release();
	AccumulateRenderState.Release();

	DeferredLightConstantBuffer.Release();
	DeferredLightVertexBuffer.Release();
	TiledDeferredTileStructuredBuffer.Release();
	TiledDeferredLightConstantBuffer.Release();

	TiledDeferredComputeOutputTexture.Release();

	Tiles.Clear();
	DeferredLightList.Clean();
	TiledDeferredLightList.Clean();

	RandomVectorsTexture.Release();

	OutputRenderTarget = NULL;

	SamplerLinearBorder.Release();
	SamplerComparisonLinearPointClamp.Release();
	SamplerPointWrap.Release();
	SamplerPointBorder.Release();

	ZERNStage::DeinitializeSelf();
}

ZEInt ZERNStageLighting::GetId() const
{
	return ZERN_STAGE_LIGHTING;
}

const ZEString& ZERNStageLighting::GetName() const
{
	static ZEString Name = "Lighting";
	return Name;
}

void ZERNStageLighting::SetRenderModel(ZERNRenderModel RenderModel)
{
	this->RenderModel = RenderModel;
}

ZERNRenderModel ZERNStageLighting::GetRenderModel() const
{
	return RenderModel;
}

void ZERNStageLighting::SetShowCascades(bool ShowCascades)
{
	this->ShowCascades = ShowCascades;
}

bool ZERNStageLighting::GetShowCascades() const
{
	return ShowCascades;
}

ZEGRTexture2D* ZERNStageLighting::GetOutputTexture() const
{
	return TiledDeferredComputeOutputTexture;
}

bool ZERNStageLighting::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	if (!ZERNStage::Setup(Renderer, Context, Commands))
		return false;

	ZERNStageGBuffer* StageGBuffer = static_cast<ZERNStageGBuffer*>(Renderer->GetStage(ZERN_STAGE_GBUFFER));
	if (StageGBuffer == NULL)
		return false;

	ZEGRTexture2D* AccumulationMap = StageGBuffer->GetAccumulationMap();
	if (AccumulationMap == NULL)
		return false;

	ZELink<ZERNCommand>* Link = Commands.GetFirst();
	while (Link != NULL)
	{
		ZELight* Light = static_cast<ZELight*>(Link->GetItem()->Entity);

		if (RenderModel == ZERN_RM_DEFERRED)
		{
			DeferredLightList.AddEnd(new ZELink<ZELight>(Light));
		}
		else
		{	
			switch (Light->GetLightType())
			{
				case ZE_LT_DIRECTIONAL:
				case ZE_LT_PROJECTIVE:
				case ZE_LT_OMNIPROJECTIVE:
					DeferredLightList.AddEnd(new ZELink<ZELight>(Light));
					break;
				case ZE_LT_POINT:
					TiledDeferredLightList.AddEnd(new ZELink<ZELight>(Light));
					break;
			}
		}

		Link = Link->GetNext();
	}
	Commands.Clean();

	OutputRenderTarget = AccumulationMap->GetRenderTarget();

	Viewport.SetWidth((float)OutputRenderTarget->GetWidth());
	Viewport.SetHeight((float)OutputRenderTarget->GetHeight());

	Context->SetTexture(ZEGR_ST_ALL, 0, StageGBuffer->GetDepthMap());
	Context->SetTexture(ZEGR_ST_ALL, 2, StageGBuffer->GetNormalMap());
	Context->SetTexture(ZEGR_ST_ALL, 3, StageGBuffer->GetDiffuseColorMap());
	Context->SetTexture(ZEGR_ST_ALL, 4, StageGBuffer->GetSpecularColorMap());

	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetViewports(1, &Viewport);

	bool Result = true;

	if(DeferredLightList.GetCount() > 0)
		Result = SetupDeferred(Renderer, Context);

	if(TiledDeferredLightList.GetCount() > 0)
	{
		if(RenderModel == ZERN_RM_COMPUTE_TILED_DEFERRED)
		{
			Result = SetupComputeTiledDeferred(Renderer, Context);

			if(Result)
			{	
				Context->SetRenderState(AccumulateRenderState);
				Context->SetTexture(ZEGR_ST_PIXEL, 9, TiledDeferredComputeOutputTexture);
				Context->SetVertexBuffers(0, 0, NULL);

				Context->Draw(3, 0);

				Context->SetTexture(ZEGR_ST_PIXEL, 9, NULL);
			}

		}
		else if(RenderModel == ZERN_RM_TILED_DEFERRED)
			Result = SetupTiledDeferred(Renderer, Context);
	}

	return Result;
}

void ZERNStageLighting::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	Context->SetRenderTargets(0, NULL, NULL);

	Context->SetTexture(ZEGR_ST_ALL, 0, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 2, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 3, NULL);
	Context->SetTexture(ZEGR_ST_ALL, 4, NULL);

	ZERNStage::CleanUp(Renderer, Context);
}

ZERNStageLighting::ZERNStageLighting()
{
	DirtyFlags.RaiseAll();

	Width = 0;
	Height = 0;
	ShowCascades = false;
	OutputRenderTarget = NULL;

	RenderModel = ZERN_RM_COMPUTE_TILED_DEFERRED;
}
