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

void ZERNStageLighting::CreateOffsetVectors()
{
	float Tan = ZEAngle::Tan(ZEAngle::ToRadian(22.5f));

	ZEVector2 Samples[] =
	{
		//4 sample
		ZEVector2(-1.0f, 1.0f),
		ZEVector2(1.0f, 1.0f),
		ZEVector2(1.0f, -1.0f),
		ZEVector2(-1.0f, -1.0f),

		//8 sample
		ZEVector2(-1.0f, 0.0f),
		ZEVector2(1.0f, 0.0f),
		ZEVector2(0.0f, -1.0f),
		ZEVector2(0.0f, 1.0f),

		//16 sample
		ZEVector2(-1.0f, Tan),
		ZEVector2(-Tan, 1.0f),
		ZEVector2(Tan, 1.0f),
		ZEVector2(1.0f, Tan),

		ZEVector2(1.0f, -Tan),
		ZEVector2(Tan, -1.0f),
		ZEVector2(-Tan, -1.0f),
		ZEVector2(-1.0f, -Tan)
	};

	for (ZEUInt I = 0; I < 16; I++)
	{
		Samples[I].NormalizeSelf();
		OffsetVectors[I] = Samples[I] * (ZERandom::GetFloatPositive() * 0.75f + 0.25f);		//[0.25, 1.0]
	}
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

	LightVertexBuffer = Canvas.CreateVertexBuffer();
}

void ZERNStageLighting::CreateSamplers()
{
	ZEGRSamplerDescription SamplerDescription;

	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerDescription.BorderColor = ZEVector4::Zero;

	SamplerLinearBorder = ZEGRSampler::GetSampler(SamplerDescription);

	SamplerDescription.SetToDefault();
	SamplerDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerDescription.BorderColor = ZEVector4::One;
	SamplerDescription.ComparisonFunction = ZEGR_CF_LESS_EQUAL;

	SamplerComparisonLinearBorder = ZEGRSampler::GetSampler(SamplerDescription);

	SamplerDescription.SetToDefault();
	SamplerDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerDescription.AddressW = ZEGR_TAM_WRAP;

	SamplerPointWrap = ZEGRSampler::GetSampler(SamplerDescription);

	SamplerDescription.SetToDefault();
	SamplerDescription.MinFilter = ZEGR_TFM_POINT;
	SamplerDescription.MagFilter = ZEGR_TFM_POINT;
	SamplerDescription.MipFilter = ZEGR_TFM_POINT;
	SamplerDescription.AddressU = ZEGR_TAM_BORDER;
	SamplerDescription.AddressV = ZEGR_TAM_BORDER;
	SamplerDescription.AddressW = ZEGR_TAM_BORDER;
	SamplerDescription.BorderColor = ZEVector4::One;

	SamplerPointBorder = ZEGRSampler::GetSampler(SamplerDescription);
}

bool ZERNStageLighting::UpdateBuffers()
{
	if(DirtyFlags.GetFlags(ZERN_SLDF_LIGHT_BUFFER))
	{
		LightStructuredBuffer.Release();
		LightStructuredBuffer = ZEGRStructuredBuffer::Create(MAX_LIGHTS, sizeof(LightStruct));

		DirtyFlags.UnraiseFlags(ZERN_SLDF_LIGHT_BUFFER);
	}

	LightStruct* Light;
	LightStructuredBuffer->Lock((void**)&Light);

	ZESize Count = Lights.GetCount();
	for(ZESize I = 0; I < Count; I++)
	{
		Light[I].PositionView = Lights[I]->GetPosition();
		Light[I].Range = Lights[I]->GetRange();
		Light[I].Color = Lights[I]->GetColor();
		Light[I].Intensity = Lights[I]->GetIntensity();
		Light[I].Attenuation = Lights[I]->GetAttenuation();
		Light[I].Type = Lights[I]->GetLightType();
		ZEVector3 Direction;
		ZEQuaternion::ConvertToEulerAngles(Direction, Lights[I]->GetRotation());
		Direction.NormalizeSelf();
		Light[I].DirectionView = Direction;
	}

	LightStructuredBuffer->Unlock();

	if(DirtyFlags.GetFlags(ZERN_SLDF_TILE_BUFFER))
	{
		ZEUInt TileCountX = (OutputRenderTarget->GetWidth() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		ZEUInt TileCountY = (OutputRenderTarget->GetHeight() + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
		
		TileStructuredBuffer.Release();
		TileStructuredBuffer = ZEGRStructuredBuffer::Create(TileCountX * TileCountY, sizeof(TileStruct));

		DirtyFlags.UnraiseFlags(ZERN_SLDF_TILE_BUFFER);
	}

	TileStruct* Tile;
	TileStructuredBuffer->Lock((void**)&Tile);
		memcpy(Tile, &Tiles[0], Tiles.GetCount() * sizeof(TileStruct));
	TileStructuredBuffer->Unlock();

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

	ZEGRDepthStencilState TiledDeferredDepthStencilState;
	TiledDeferredDepthStencilState.SetDepthTestEnable(false);
	TiledDeferredDepthStencilState.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(TiledDeferredDepthStencilState);

	RenderState.SetShader(ZEGR_ST_VERTEX, TiledDeferredVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, TiledDeferredPixelShader);

	TiledDeferredRenderState = RenderState.Compile();
	zeCheckError(TiledDeferredRenderState == NULL, false, "Cannot set render state.");

	RenderState.SetShader(ZEGR_ST_VERTEX, DeferredVertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, DeferredPixelShader);

	ZEGRRasterizerState RasterizerState;
	RasterizerState.SetCullMode(ZEGR_CMD_FRONT);

	RenderState.SetRasterizerState(RasterizerState);
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

bool ZERNStageLighting::Update()
{
	if(!UpdateShaders())
		return false;

	if(!UpdateRenderState())
		return false;

	if((UseTiledDeferred) && !UpdateBuffers())
		return false;

	return true;
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
			clipMin = ZEMath::Max(clipMin, c);
		} else 
		{   // Right side boundary
			clipMax = ZEMath::Min(clipMax, c);
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

void ZERNStageLighting::AssignLightsToTiles(ZERNRenderer* Renderer, const ZEArray<ZELight*>& Lights, float CameraScaleX, float CameraScaleY, float CameraNear)
{
	ZEUInt Width = OutputRenderTarget->GetWidth();
	ZEUInt Height = OutputRenderTarget->GetHeight();

	ZEUInt TileCountX = (Width + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;
	ZEUInt TileCountY = (Height + TILE_SIZE_IN_PIXELS - 1) / TILE_SIZE_IN_PIXELS;

	ZEUInt TileCount = TileCountX * TileCountY;
	ZEUInt LightCount = (ZEUInt)Lights.GetCount();

	if(Tiles.GetCount() != TileCount)
		Tiles.Resize(TileCount);

	memset(&Tiles[0], 0, sizeof(TileStruct) * TileCount);

	for(ZEUInt I = 0; I < LightCount; I++)
	{
		ZELight* Light = Lights[I];
		if(Light->GetLightType() == ZELightType::ZE_LT_POINT)
		{
			ZEVector3 LightPositionView = Renderer->GetView().ViewTransform * Light->GetWorldPosition();
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

			for(ZEUInt Y = StartTileY; Y <= EndTileY; Y++)
			{
				for(ZEUInt X = StartTileX; X <= EndTileX; X++)
				{
					ZEUInt TileId = Y * TileCountX + X;
					TileStruct& Tile = Tiles[TileId];
					Tile.LightIndices[Tile.LightCount] = I;
					Tile.LightCount = (Tile.LightCount >= (MAX_LIGHTS)) ? (MAX_LIGHTS) : ++Tile.LightCount;
				}
			}
		}
		else if(Light->GetLightType() == ZELightType::ZE_LT_DIRECTIONAL)
		{
			for(ZEUInt Y = 0; Y < TileCountY; Y++)
			{
				for(ZEUInt X = 0; X < TileCountX; X++)
				{
					ZEUInt TileId = Y * TileCountX + X;
					TileStruct& Tile = Tiles[TileId];
					Tile.LightIndices[Tile.LightCount] = I;
					Tile.LightCount = (Tile.LightCount >= (MAX_LIGHTS)) ? (MAX_LIGHTS) : ++Tile.LightCount;
				}
			}
		}
	}
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
	AssignLightsToTiles(Renderer, Lights, View.ProjectionTransform.M11, View.ProjectionTransform.M22, View.NearZ);

	if(!Update())
		return false;

	Context->SetRenderState(TiledDeferredRenderState);

	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 5, LightStructuredBuffer);
	Context->SetStructuredBuffer(ZEGR_ST_PIXEL, 6, TileStructuredBuffer);

	Context->SetVertexBuffers(0, 0, NULL);
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
	for(ZESize I = 0; I < LightCount; I++)
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

void ZERNStageLighting::DrawDirectionalLight(ZELightDirectional* DirectionalLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	LightConstantsStruct* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		Light.Color = DirectionalLight->GetColor();
		Light.Intensity = DirectionalLight->GetIntensity();
		Light.Type = DirectionalLight->GetLightType();
		ZEVector3 Direction = DirectionalLight->GetWorldRotation() * ZEVector3::UnitZ;
 		ZEMatrix4x4::Transform3x3(Light.DirectionView, Renderer->GetView().ViewTransform, -Direction);
		Light.DirectionView.NormalizeSelf();

		LightConstants->CastShadow = (ZEBool32)DirectionalLight->GetCastsShadow();
		LightConstants->ShowCascades = (ZEBool32)ShowCascades;
		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(DirectionalLight->GetShadowSampleCount());
		LightConstants->SampleLengthOffset = DirectionalLight->GetShadowSampleLengthOffset();
		memcpy(LightConstants->OffsetVectors, OffsetVectors, sizeof(ZEVector2) * LightConstants->SampleCount);
	LightConstantBuffer->Unlock();

	DirectionalLight->BindCascades(Renderer, Context);

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerComparisonLinearBorder.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointWrap.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 3, SamplerPointBorder.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 5, DirectionalLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(6, 0);
}

void ZERNStageLighting::DrawPointLight(ZELightPoint* PointLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	LightConstantsStruct* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
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
	LightConstantBuffer->Unlock();

	Context->Draw(3600, 942);
}

void ZERNStageLighting::DrawProjectiveLight(ZELightProjective* ProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	LightConstantsStruct* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
		LightStruct& Light = LightConstants->Light;
		ZEMatrix4x4::Transform(Light.PositionView, Renderer->GetView().ViewTransform, ProjectiveLight->GetWorldPosition());
		Light.Range = ProjectiveLight->GetRange();
		Light.Color = ProjectiveLight->GetColor();
		Light.Intensity = ProjectiveLight->GetIntensity();
		Light.Attenuation = ProjectiveLight->GetAttenuation();
		Light.Type = ProjectiveLight->GetLightType();
		Light.DirectionView = ZEVector3::Zero;

		LightConstants->CastShadow = (ZEBool32)ProjectiveLight->GetCastsShadow();
		LightConstants->SampleCount = ZELight::ConvertShadowSampleCount(ProjectiveLight->GetShadowSampleCount());
		LightConstants->SampleLengthOffset = ProjectiveLight->GetShadowSampleLengthOffset();
		memcpy(LightConstants->OffsetVectors, OffsetVectors, sizeof(ZEVector2) * LightConstants->SampleCount);

		LightConstants->ProjectionMatrix = ProjectiveLight->GetProjectionTransform() * ProjectiveLight->GetViewTransform() * Renderer->GetView().InvViewTransform;

		float TanFovRange = ZEAngle::Tan(ProjectiveLight->GetFOV() * 0.5f) * ProjectiveLight->GetRange();
		ZEMatrix4x4::CreateOrientation(LightConstants->WorldMatrix, ProjectiveLight->GetWorldPosition(), ProjectiveLight->GetWorldRotation(),
			ZEVector3(TanFovRange * ProjectiveLight->GetAspectRatio() * 2.0f, TanFovRange * 2.0f, ProjectiveLight->GetRange()));
	LightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 1, SamplerComparisonLinearBorder.GetPointer());
	Context->SetSampler(ZEGR_ST_PIXEL, 2, SamplerPointWrap.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 5, ProjectiveLight->GetShadowMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 6, ProjectiveLight->GetProjectionTexture());
	Context->SetTexture(ZEGR_ST_PIXEL, 8, RandomVectorsTexture);

	Context->Draw(18, 4542);
}

void ZERNStageLighting::DrawOmniProjectiveLight(ZELightOmniProjective* OmniProjectiveLight, ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEQuaternion ProjectionRotation;
	ZEQuaternion::Product(ProjectionRotation, OmniProjectiveLight->GetWorldRotation().Conjugate(), Renderer->GetView().Rotation);
	
	ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);

	ZEMatrix4x4 RotationMatrix;
	ZEMatrix4x4::CreateRotation(RotationMatrix, ProjectionRotation);

	LightConstantsStruct* LightConstants;
	LightConstantBuffer->Lock((void**)&LightConstants);
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
	LightConstantBuffer->Unlock();

	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearBorder.GetPointer());
	Context->SetTexture(ZEGR_ST_PIXEL, 6, (ZEGRTextureCube*)OmniProjectiveLight->GetProjectionTexture());

	Context->Draw(936, 6);
}

bool ZERNStageLighting::InitializeSelf()
{
	CreateRandomVectors();
	CreateOffsetVectors();
	CreateLightGeometries();
	CreateSamplers();

	LightConstantBuffer = ZEGRConstantBuffer::Create(sizeof(LightConstantsStruct));

	DirtyFlags.RaiseAll();

	const ZESmartArray<ZEEntity*>& Entities = GetOwnerRenderer()->GetScene()->GetEntities();
	ZEUInt Count = (ZEUInt)Entities.GetCount();
	for(ZEUInt I = 0; I < Count; I++)
	{
		ZEEntity* Entity = Entities[I];
		if(Entity->GetDrawFlags().GetFlags(ZE_DF_LIGHT_SOURCE))
			Lights.Add((ZELight*)Entity);
	}

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
	LightStructuredBuffer.Release();
	TileStructuredBuffer.Release();
	LightConstantBuffer.Release();
	LightVertexBuffer.Release();

	Lights.Clear();
	Tiles.Clear();

	RandomVectorsTexture.Release();

	OutputRenderTarget = NULL;
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

void ZERNStageLighting::SetTiledDeferred(bool UseTiledDeferred)
{
	this->UseTiledDeferred = UseTiledDeferred;
}

bool ZERNStageLighting::GetTiledDefferred() const
{
	return UseTiledDeferred;
}

void ZERNStageLighting::SetShowCascades(bool ShowCascades)
{
	this->ShowCascades = ShowCascades;
}

bool ZERNStageLighting::GetShowCascades() const
{
	return ShowCascades;
}

bool ZERNStageLighting::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZERNStageGBuffer* StageGBuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	if(StageGBuffer == NULL)
		return false;

	ZEGRTexture2D* AccumulationMap = StageGBuffer->GetAccumulationMap();
	if(AccumulationMap == NULL)
		return false;

	OutputRenderTarget = AccumulationMap->GetRenderTarget();

	Context->SetTexture(ZEGR_ST_PIXEL, 0, StageGBuffer->GetDepthMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 2, StageGBuffer->GetNormalMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 3, StageGBuffer->GetDiffuseColorMap());
	Context->SetTexture(ZEGR_ST_PIXEL, 4, StageGBuffer->GetSpecularColorMap());

	Context->SetRenderTargets(1, &OutputRenderTarget, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, OutputRenderTarget->GetWidth(), OutputRenderTarget->GetHeight()));

	if(UseTiledDeferred)
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

ZERNStageLighting::ZERNStageLighting()
{
	PrevWidth = 0;
	PrevHeight = 0;
	UseTiledDeferred = false;
	ShowCascades = false;
	OutputRenderTarget = NULL;
}
