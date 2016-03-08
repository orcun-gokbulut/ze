//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESkyBrush.cpp
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

#include "ZESkyBrush.h"

#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZERenderer/ZERNStagePostProcess.h"

#define	ZERN_SBDF_SHADERS			1
#define	ZERN_SBDF_RENDER_STATE		2
#define	ZERN_SBDF_CONSTANT_BUFFER	4

ZEDrawFlags ZESkyBrush::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZESkyBrush::SetSkyBrightness(float Brightness)
{
	if (Constants.SkyBrightness == Brightness)
		return;

	Constants.SkyBrightness = Brightness;

	DirtyFlags.RaiseFlags(ZERN_SBDF_CONSTANT_BUFFER);
}

float ZESkyBrush::GetSkyBrightness() const
{
	return Constants.SkyBrightness;
}

void ZESkyBrush::SetSkyColor(const ZEVector3& Color)
{
	if (Constants.SkyColor == Color)
		return;

	Constants.SkyColor = Color;

	DirtyFlags.RaiseFlags(ZERN_SBDF_CONSTANT_BUFFER);
}

const ZEVector3& ZESkyBrush::GetSkyColor() const
{
	return Constants.SkyColor;
}

void ZESkyBrush::SetSkyTexture(const ZEString& FileName)
{
	if (SkyTexture != NULL)
	{
		SkyTexture->Release();
		SkyTexture = NULL;
	}

	ZETextureOptions Options;
	Options.CompressionQuality = ZE_TCQ_HIGH;
	Options.CompressionType = ZE_TCT_NONE;
	Options.DownSample = ZE_TDS_NONE;
	Options.FileCaching = ZE_TFC_DISABLED;
	Options.MaximumMipmapLevel = 1;
	Options.MipMapping = ZE_TMM_DISABLED;

	SkyTexture = ZETextureCubeResource::LoadResource(FileName, &Options);
}

const ZEString& ZESkyBrush::GetSkyTexture() const
{
	if (SkyTexture == NULL)
		return ZEString::Empty;
	
	return SkyTexture->GetFileName();
}

bool ZESkyBrush::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	SkyBox.AddBox(2.0f, 2.0, 2.0f);
	VertexBuffer = SkyBox.CreateVertexBuffer();

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	ConstantBufferTransform = ZEGRConstantBuffer::Create(sizeof(ZEMatrix4x4));

	ZEGRSamplerDescription SamplerDescriptionLinearWrap;
	SamplerDescriptionLinearWrap.AddressU = ZEGR_TAM_WRAP;
	SamplerDescriptionLinearWrap.AddressV = ZEGR_TAM_WRAP;
	SamplerDescriptionLinearWrap.AddressW = ZEGR_TAM_WRAP;
	SamplerLinearWrap = ZEGRSampler::GetSampler(SamplerDescriptionLinearWrap);

	return true;
}

bool ZESkyBrush::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	if (SkyTexture != NULL)
	{
		SkyTexture->Release();
		SkyTexture = NULL;
	}

	return ZEEntity::DeinitializeSelf();
}

bool ZESkyBrush::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZERN_SBDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSkyBox.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSkyBox_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Skybox vertex shader cannot compile");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSkyBox_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Skybox pixel shader cannot compile");

	DirtyFlags.UnraiseFlags(ZERN_SBDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SBDF_RENDER_STATE);
	
	return true;
}

bool ZESkyBrush::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZERN_SBDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStagePostProcess::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	ZEGRRasterizerState RasterizerStateFrontCCW;
	RasterizerStateFrontCCW.SetFrontIsCounterClockwise(true);
	RenderState.SetRasterizerState(RasterizerStateFrontCCW);
	
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL,false, "Skybox render state cannot compile");

	DirtyFlags.UnraiseFlags(ZERN_SBDF_RENDER_STATE);

	return true;
}

bool ZESkyBrush::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZERN_SBDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SBDF_CONSTANT_BUFFER);

	return true;
}

bool ZESkyBrush::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

ZESkyBrush::ZESkyBrush()
{
	DirtyFlags.RaiseAll();

	SkyTexture = NULL;
	Constants.SkyColor = ZEVector3::One;
	Constants.SkyBrightness = 1.0f;

	SkyRenderCommand.Entity = this;
	SkyRenderCommand.StageMask = ZERN_STAGE_POST_EFFECT;
}

ZESkyBrush::~ZESkyBrush()
{

}

bool ZESkyBrush::PreRender(const ZERNCullParameters* CullParameters)
{
	CullParameters->Renderer->AddCommand(&SkyRenderCommand);

	return true;
}

void ZESkyBrush::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEVector3 CameraPositionWorld = Parameters->View->Position;
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, CameraPositionWorld, ZEVector3::One);

	ConstantBufferTransform->SetData(&WorldMatrix);

	ZEGRContext* Context = Parameters->Context;
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBufferTransform);
	Context->SetRenderState(RenderStateData);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, SamplerLinearWrap);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, SkyTexture->GetTexture());
	Context->SetVertexBuffers(0, 1, VertexBuffer.GetPointerToPointer());

	Context->Draw(VertexBuffer->GetVertexCount(), 0);

	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, NULL);
	Context->SetSampler(ZEGR_ST_PIXEL, 0, NULL);
	Context->SetTexture(ZEGR_ST_PIXEL, 5, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
}

ZESkyBrush* ZESkyBrush::CreateInstance()
{
	return new ZESkyBrush();
}
