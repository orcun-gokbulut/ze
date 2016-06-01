//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATCloud.cpp
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

#include "ZEATCloud.h"

#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStageAtmosphere.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNFilter.h"
#include "ZETexture/ZETexture2DResource.h"

#define ZE_CDF_SHADERS			1
#define ZE_CDF_RENDER_STATES	2
#define ZE_CDF_CONSTANT_BUFFER	4

ZEGRVertexLayout GetPositionTexcoordVertexLayout()
{
	static ZEGRVertexLayout VertexLayout;
	if (VertexLayout.GetElementCount() == 0)
	{
		ZEGRVertexElement ElementArray[] = 
		{
			{ZEGR_VES_POSITION, 0, ZEGR_VET_FLOAT3, 0, 0, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD, 0, ZEGR_VET_FLOAT2, 0, 12, ZEGR_VU_PER_VERTEX, 0}
		};

		VertexLayout.SetElements(ElementArray, 2);
	}

	return VertexLayout;
}

void ZEATCloud::CreatePlane()
{
	struct Vertex
	{
		ZEVector3 Position;
		ZEVector2 Texcoord;
	};

	float Height = 0.5f;

	Vertex Vertices[16] = 
	{
		{ ZEVector3(-10.0f, 0.0f, 10.0f), ZEVector2(0.0f, 0.0f) },
		{ ZEVector3(-5.0f, 0.0f, 10.0f), ZEVector2(0.25f, 0.0f) },
		{ ZEVector3(5.0f, 0.0f, 10.0f), ZEVector2(0.75f, 0.0f) },
		{ ZEVector3(10.0f, 0.0f, 10.0f), ZEVector2(1.0f, 0.0f) },

		{ ZEVector3(-10.0f, 0.0f, 5.0f), ZEVector2(0.0f, 0.25f) },
		{ ZEVector3(-5.0f, Height, 5.0f), ZEVector2(0.25f, 0.25f) },
		{ ZEVector3(5.0f, Height, 5.0f), ZEVector2(0.75f, 0.25f) },
		{ ZEVector3(10.0f, 0.0f, 5.0f), ZEVector2(1.0f, 0.25f) },

		{ ZEVector3(-10.0f, 0.0f, -5.0f), ZEVector2(0.0f, 0.5f) },
		{ ZEVector3(-5.0f, Height, -5.0f), ZEVector2(0.25f, 0.5f) },
		{ ZEVector3(5.0f, Height, -5.0f), ZEVector2(0.75f, 0.5f) },
		{ ZEVector3(10.0f, 0.0f, -5.0f), ZEVector2(1.0f, 0.5f) },

		{ ZEVector3(-10.0f, 0.0f, -10.0f), ZEVector2(0.0f, 1.0f) },
		{ ZEVector3(-5.0f, 0.0f, -10.0f), ZEVector2(0.25f, 1.0f) },
		{ ZEVector3(5.0f, 0.0f, -10.0f), ZEVector2(0.75f, 1.0f) },
		{ ZEVector3(10.0f, 0.0f, -10.0f), ZEVector2(1.0f, 1.0f) }
	};

	PlaneVertexBuffer = ZEGRVertexBuffer::Create(16, sizeof(Vertex));

	void* Data;
	PlaneVertexBuffer->Lock(&Data);
	memcpy(Data, &Vertices, sizeof(Vertex) * 16);
	PlaneVertexBuffer->Unlock();
}

bool ZEATCloud::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNCloud.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNCloud_Plane_VertexShader_Main";
	PlaneVertexShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneVertexShader == NULL, false, "Cloud plane vertex shader cannot compile");

	Options.Type = ZEGR_ST_HULL;
	Options.EntryPoint = "ZERNCloud_Plane_HullShader_Main";
	PlaneHullShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneHullShader == NULL, false, "Cloud plane hull shader cannot compile");

	Options.Type = ZEGR_ST_DOMAIN;
	Options.EntryPoint = "ZERNCloud_Plane_DomainShader_Main";
	PlaneDomainShader = ZEGRShader::Compile(Options);
	zeCheckError(PlaneDomainShader == NULL, false, "Cloud plane domain shader cannot compile");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNCloud_Plane_PixelShader_Main";
	PlanePixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PlanePixelShader == NULL, false, "Cloud plane pixel shader cannot compile");

	DirtyFlags.UnraiseFlags(ZE_CDF_SHADERS);
	DirtyFlags.RaiseFlags(ZE_CDF_RENDER_STATES);

	return true;
}

bool ZEATCloud::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStageAtmosphere::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_16_CONTROL_POINT_PATCHLIST);
	RenderState.SetVertexLayout(GetPositionTexcoordVertexLayout());

	ZEGRRasterizerState RasterizerStateFrontCCW;
	RasterizerStateFrontCCW.SetFrontIsCounterClockwise(true);

	RenderState.SetRasterizerState(RasterizerStateFrontCCW);

	ZEGRBlendState BlendState;
	BlendState.SetBlendEnable(true);
	ZEGRBlendRenderTarget RenderTarget0_AlphaBlending = BlendState.GetRenderTarget(0);
	RenderTarget0_AlphaBlending.SetBlendEnable(true);
	RenderTarget0_AlphaBlending.SetSource(ZEGRBlend::ZEGR_BO_SRC_ALPHA);
	RenderTarget0_AlphaBlending.SetDestination(ZEGRBlend::ZEGR_BO_INV_SRC_ALPHA);
	RenderTarget0_AlphaBlending.SetOperation(ZEGRBlendOperation::ZEGR_BE_ADD);
	BlendState.SetRenderTargetBlend(0, RenderTarget0_AlphaBlending);

	RenderState.SetBlendState(BlendState);

	RenderState.SetShader(ZEGR_ST_VERTEX, PlaneVertexShader);
	RenderState.SetShader(ZEGR_ST_HULL, PlaneHullShader);
	RenderState.SetShader(ZEGR_ST_DOMAIN, PlaneDomainShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PlanePixelShader);

	PlaneRenderStateData = RenderState.Compile();
	zeCheckError(PlaneRenderStateData == NULL, false, "Cloud plane render state cannot compile");

	DirtyFlags.UnraiseFlags(ZE_CDF_RENDER_STATES);

	return true;
}

bool ZEATCloud::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZE_CDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZE_CDF_CONSTANT_BUFFER);

	return true;
}

bool ZEATCloud::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

bool ZEATCloud::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	CreatePlane();

	PlaneTransformConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return Update();
}

bool ZEATCloud::DeinitializeSelf()
{
	PlaneVertexShader.Release();
	PlaneHullShader.Release();
	PlaneDomainShader.Release();
	PlanePixelShader.Release();
	PlaneRenderStateData.Release();
	PlaneVertexBuffer.Release();
	PlaneTransformConstantBuffer.Release();
	
	ConstantBuffer.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEATCloud::ZEATCloud()
{
	DirtyFlags.RaiseAll();

	RenderCommand.Entity = this;
	RenderCommand.Priority = 4;
	RenderCommand.StageMask = ZERN_STAGE_ATMOSPHERE;

	CloudTexture = NULL;

	Constants.PlaneSubdivision = 10.0f;
	Constants.CloudCoverage = 0.0f;
	Constants.CloudDensity = 0.0f;
	Constants.LightColor = ZEVector3::One;
	Constants.Inscattering = 0.05f;
	Constants.LightDirection = ZEVector3::One;
	Constants.Translation = ZEVector2::Zero;
}

ZEATCloud::~ZEATCloud()
{

}

ZEDrawFlags ZEATCloud::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATCloud::SetCloudTexture(const ZEString& FileName)
{
	ZETextureOptions TextureOption = 
	{
		ZE_TCT_NONE,
		ZE_TCQ_LOW,
		ZE_TDS_NONE,
		ZE_TFC_DISABLED,
		ZE_TMM_DISABLED,
		1
	};

	CloudTexture = ZETexture2DResource::LoadResource(FileName, &TextureOption);
}

const ZEString& ZEATCloud::GetCloudTexture() const
{
	return CloudTexture == NULL ? ZEString::Empty : CloudTexture->GetFileName();
}

void ZEATCloud::SetCloudCoverage(float CloudCoverage)
{
	CloudCoverage = ZEMath::Max(0.0f, CloudCoverage);

	if (Constants.CloudCoverage == CloudCoverage)
		return;

	Constants.CloudCoverage = CloudCoverage;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

float ZEATCloud::GetCloudCoverage() const
{
	return Constants.CloudCoverage;
}

void ZEATCloud::SetCloudDensity(float CloudDensity)
{
	CloudDensity = ZEMath::Max(0.0f, CloudDensity);

	if (Constants.CloudDensity == CloudDensity)
		return;

	Constants.CloudDensity = CloudDensity;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

float ZEATCloud::GetCloudDensity() const
{
	return Constants.CloudDensity;
}

void ZEATCloud::SetLightColor(const ZEVector3& LightColor)
{
	if (Constants.LightColor == LightColor)
		return;

	Constants.LightColor = LightColor;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATCloud::GetLightColor() const
{
	return Constants.LightColor;
}

void ZEATCloud::SetInscattering(float Inscattering)
{
	if (Constants.Inscattering == Inscattering)
		return;

	Constants.Inscattering = Inscattering;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

float ZEATCloud::GetInscattering() const
{
	return Constants.Inscattering;
}

void ZEATCloud::SetLightDirection(const ZEVector3& LightDirection)
{
	if (Constants.LightDirection == LightDirection)
		return;

	Constants.LightDirection = LightDirection;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATCloud::GetLightDirection() const
{
	return Constants.LightDirection;
}

void ZEATCloud::SetTranslation(const ZEVector2& Translation)
{
	if (Constants.Translation == Translation)
		return;

	Constants.Translation = Translation;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

const ZEVector2& ZEATCloud::GetTranslation() const
{
	return Constants.Translation;
}

void ZEATCloud::Tick(float Time)
{
	static ZEVector2 Translation = ZEVector2(0.0f, 0.0f);
	Translation += Time * ZEVector2(0.0001f, 0.0003f);

	if (Translation.x > 1.0f)
		Translation.x = 0.0f;

	if (Translation.y > 1.0f)
		Translation.y = 0.0f;

	SetTranslation(Translation);
}

bool ZEATCloud::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!ZEEntity::PreRender(CullParameters))
		return false;

	CullParameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEATCloud::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, Parameters->View->Position, ZEVector3::One);

	PlaneTransformConstantBuffer->SetData(&WorldMatrix);

	ZEGRContext* Context = Parameters->Context;	
	ZERNStage* Stage = Parameters->Stage;

	Context->SetConstantBuffers(ZEGR_ST_DOMAIN, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, 1, PlaneTransformConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_ALL, 9, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetRenderState(PlaneRenderStateData);
	ZEGRTexture* Texture = CloudTexture->GetTexture();
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, &Texture);
	Context->SetVertexBuffers(0, 1, PlaneVertexBuffer.GetPointerToPointer());

	Context->Draw(PlaneVertexBuffer->GetVertexCount(), 0);
}

ZEATCloud* ZEATCloud::CreateInstance()
{
	return new ZEATCloud();
}
