//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATSkyBox.cpp
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

#include "ZEATSkyBox.h"

#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZELightDirectional.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNShaderSlots.h"
#include "ZERenderer/ZERNStageAtmosphere.h"

#define	ZEAT_SBDF_SHADERS			1
#define	ZEAT_SBDF_RENDER_STATE		2
#define	ZEAT_SBDF_CONSTANT_BUFFER	4

ZEDrawFlags ZEATSkyBox::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATSkyBox::SetTextureFile(const ZEString& FileName)
{
	ZEGRTextureCubeOptions TextureOptions;
	TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
	TextureOptions.GenerateMipMaps = false;
	TextureOptions.MaximumMipmapLevel = 0;
	TextureOptions.sRGB = true;

	SkyTexture = ZEGRTextureCube::LoadResourceShared(FileName, TextureOptions);
}

const ZEString& ZEATSkyBox::GetTextureFile() const
{
	return SkyTexture != NULL ? SkyTexture->GetName() : ZEString::Empty;
}

void ZEATSkyBox::SetBrightness(float Brightness)
{
	if (this->Brightness == Brightness)
		return;

	this->Brightness = Brightness;

	DirtyFlags.RaiseFlags(ZEAT_SBDF_CONSTANT_BUFFER);
}

float ZEATSkyBox::GetBrightness() const
{
	return Brightness;
}

void ZEATSkyBox::SetColor(const ZEVector3& Color)
{
	if (this->Color == Color)
		return;

	this->Color = Color;

	DirtyFlags.RaiseFlags(ZEAT_SBDF_CONSTANT_BUFFER);
}

const ZEVector3& ZEATSkyBox::GetColor() const
{
	return Constants.Color;
}

bool ZEATSkyBox::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	ZECanvas SkyBox;
	SkyBox.AddBox(2.0f, 2.0, 2.0f);
	VertexBuffer = SkyBox.CreateVertexBuffer();

	ConstantBuffer = ZEGRConstantBuffer::CreateResource(sizeof(Constants));
	ConstantBufferTransform = ZEGRConstantBuffer::CreateResource(sizeof(ZEMatrix4x4));

	return Update();
}

bool ZEATSkyBox::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	VertexBuffer.Release();
	ConstantBuffer.Release();
	ConstantBufferTransform.Release();
	SkyTexture.Release();

	return ZEEntity::DeinitializeSelf();
}

bool ZEATSkyBox::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_SBDF_SHADERS))
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

	DirtyFlags.UnraiseFlags(ZEAT_SBDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_SBDF_RENDER_STATE);
	
	return true;
}

bool ZEATSkyBox::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZEAT_SBDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStageAtmosphere::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_LIST);
	RenderState.SetVertexLayout(*ZECanvasVertex::GetVertexLayout());

	ZEGRRasterizerState RasterizerStateFrontCCW;
	RasterizerStateFrontCCW.SetFrontIsCounterClockwise(true);

	RenderState.SetRasterizerState(RasterizerStateFrontCCW);
	
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL,false, "Skybox render state cannot compile");

	DirtyFlags.UnraiseFlags(ZEAT_SBDF_RENDER_STATE);

	return true;
}

bool ZEATSkyBox::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZEAT_SBDF_CONSTANT_BUFFER))
		return true;

	Constants.Color = Color * Brightness;
	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_SBDF_CONSTANT_BUFFER);

	return true;
}

bool ZEATSkyBox::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

ZEATSkyBox::ZEATSkyBox()
{
	DirtyFlags.RaiseAll();

	SkyRenderCommand.Entity = this;
	SkyRenderCommand.Priority = 1;
	SkyRenderCommand.StageMask = ZERN_STAGE_ATMOSPHERE;

	SkyTexture = NULL;

	Brightness = 1.0f;

	Constants.Color = ZEVector3::One;
}

ZEATSkyBox::~ZEATSkyBox()
{

}

bool ZEATSkyBox::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	Parameters->Renderer->AddCommand(&SkyRenderCommand);

	return true;
}

void ZEATSkyBox::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEVector3 CameraPositionWorld = Parameters->View->Position;
	ZEMatrix4x4 WorldMatrix;
	ZEMatrix4x4::CreateOrientation(WorldMatrix, CameraPositionWorld, ZEVector3::One);

	ConstantBufferTransform->SetData(&WorldMatrix);

	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, ZERN_SHADER_CONSTANT_DRAW_TRANSFORM, ConstantBufferTransform);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(RenderStateData);
	const ZEGRTexture* Texture = SkyTexture;
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, &Texture);
	Context->SetVertexBuffer(0, VertexBuffer);

	Context->Draw(VertexBuffer->GetVertexCount(), 0);
}

ZEATSkyBox* ZEATSkyBox::CreateInstance()
{
	return new ZEATSkyBox();
}
