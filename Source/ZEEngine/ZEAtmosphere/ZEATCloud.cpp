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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStageAtmosphere.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNFilter.h"

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
			{ZEGR_VES_POSITION, 0, ZEGR_VET_FLOAT_3, 0, 0, ZEGR_VU_PER_VERTEX, 0},
			{ZEGR_VES_TEXCOORD, 0, ZEGR_VET_FLOAT_2, 0, 12, ZEGR_VU_PER_VERTEX, 0}
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

	float Height1 = 0.0f;
	float Radius1 = 1.0f;
	float CurveOffset1 = 0.11f;
	float EdgePoint1 = Radius1 / 3.0f;
	Vertex Vertices[] = 
	{
		{ ZEVector3(-Radius1, Height1, Radius1), ZEVector2(0.0f, 0.0f) },
		{ ZEVector3(-EdgePoint1, Height1, Radius1), ZEVector2(0.33f, 0.0f) },
		{ ZEVector3(EdgePoint1, Height1, Radius1), ZEVector2(0.66f, 0.0f) },
		{ ZEVector3(Radius1, Height1, Radius1), ZEVector2(1.0f, 0.0f) },

		{ ZEVector3(-Radius1, Height1, EdgePoint1), ZEVector2(0.0f, 0.33f) },
		{ ZEVector3(-EdgePoint1, Height1 + CurveOffset1, EdgePoint1), ZEVector2(0.0001f, 0.0001f) },
		{ ZEVector3(EdgePoint1, Height1 + CurveOffset1, EdgePoint1), ZEVector2(0.9999f, 0.0001f) },
		{ ZEVector3(Radius1, Height1, EdgePoint1), ZEVector2(1.0f, 0.33f) },

		{ ZEVector3(-Radius1, Height1, -EdgePoint1), ZEVector2(0.0f, 0.66f) },
		{ ZEVector3(-EdgePoint1, Height1 + CurveOffset1, -EdgePoint1), ZEVector2(0.0001f, 0.9999f) },
		{ ZEVector3(EdgePoint1, Height1 + CurveOffset1, -EdgePoint1), ZEVector2(0.9999f, 0.9999f) },
		{ ZEVector3(Radius1, Height1, -EdgePoint1), ZEVector2(1.0f, 0.66f) },

		{ ZEVector3(-Radius1, Height1, -Radius1), ZEVector2(0.0f, 1.0f) },
		{ ZEVector3(-EdgePoint1, Height1, -Radius1), ZEVector2(0.33f, 1.0f) },
		{ ZEVector3(EdgePoint1, Height1, -Radius1), ZEVector2(0.66f, 1.0f) },
		{ ZEVector3(Radius1, Height1, -Radius1), ZEVector2(1.0f, 1.0f) },
	};

	PlaneVertexBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_VERTEX_BUFFER, 16 * sizeof(Vertex), sizeof(Vertex), ZEGR_RU_IMMUTABLE, ZEGR_RBF_VERTEX_BUFFER, ZEGR_TF_NONE, Vertices);
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

ZEEntityResult ZEATCloud::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	CreatePlane();

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	zeCheckError(ConstantBuffer == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");

	if (!Update())
		return ZE_ER_FAILED_CLEANUP;

	return ZE_ER_DONE;
}

ZEEntityResult ZEATCloud::UnloadInternal()
{
	DirtyFlags.RaiseAll();

	PlaneVertexShader.Release();
	PlaneHullShader.Release();
	PlaneDomainShader.Release();
	PlanePixelShader.Release();
	PlaneRenderStateData.Release();
	PlaneVertexBuffer.Release();
	ConstantBuffer.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEATCloud::ZEATCloud()
{
	DirtyFlags.RaiseAll();

	RenderCommand.Entity = this;
	RenderCommand.Priority = 4;
	RenderCommand.StageMask = ZERN_STAGE_ATMOSPHERE;

	CloudTexture = NULL;

	Constants.PlaneSubdivision = 16.0f;
	Constants.CloudCoverage = 0.0f;
	Constants.CloudDensity = 0.0f;
	Constants.LightColor = ZEVector3::One;
	Constants.Inscattering = ZEVector3::One * 0.5f;
	Constants.LightDirection = ZEVector3::One;
	Constants.Translation = ZEVector2::Zero;
	Constants.TextureTileFactor = ZEVector2(4.0f, 4.0f);

	SetEntityFlags(ZE_EF_RENDERABLE);
}

ZEATCloud::~ZEATCloud()
{

}

void ZEATCloud::SetCloudTexture(const ZEString& FileName)
{
	ZEGRTextureOptions TextureOptions;
	TextureOptions.Type = ZEGR_TT_2D;
	TextureOptions.CompressionFormat = ZEGR_TF_NONE;
	TextureOptions.GenerateMipMaps = false;
	TextureOptions.MaximumMipmapLevel = 0;
	TextureOptions.sRGB = true;

	CloudTexture = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
}

const ZEString& ZEATCloud::GetCloudTexture() const
{
	return CloudTexture == NULL ? ZEString::Empty : CloudTexture->GetName();
}

void ZEATCloud::SetCloudCoverage(float CloudCoverage)
{
	CloudCoverage = ZEMath::Clamp(CloudCoverage, 0.0f, 4.0f);

	if (Constants.CloudCoverage == (ZEUInt)CloudCoverage)
		return;

	Constants.CloudCoverage = (ZEUInt)CloudCoverage;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

float ZEATCloud::GetCloudCoverage() const
{
	return (float)Constants.CloudCoverage;
}

void ZEATCloud::SetCloudDensity(float CloudDensity)
{
	CloudDensity = ZEMath::Clamp(CloudDensity, 0.0f, 1.0f);

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

void ZEATCloud::SetInscattering(const ZEVector3& Inscattering)
{
	if (Constants.Inscattering == Inscattering)
		return;

	Constants.Inscattering = Inscattering;

	DirtyFlags.RaiseFlags(ZE_CDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATCloud::GetInscattering() const
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

bool ZEATCloud::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (CloudTexture == NULL || !CloudTexture->IsLoaded())
		return false;

	static ZEVector2 Translation = ZEVector2::Zero;
	Translation += Parameters->ElapsedTime * ZEVector2(0.0001f, 0.0003f);

	if (Translation.x > 1.0f)
		Translation.x = 0.0f;

	if (Translation.y > 1.0f)
		Translation.y = 0.0f;

	SetTranslation(Translation);

	Parameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEATCloud::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;	
	const ZERNStage* Stage = Parameters->Stage;

	Context->SetConstantBuffer(ZEGR_ST_ALL, 9, ConstantBuffer);
	Context->SetRenderState(PlaneRenderStateData);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, CloudTexture);
	Context->SetVertexBuffer(0, PlaneVertexBuffer);

	Context->Draw(PlaneVertexBuffer->GetElementCount(), 0);
}

ZEATCloud* ZEATCloud::CreateInstance()
{
	return new ZEATCloud();
}
