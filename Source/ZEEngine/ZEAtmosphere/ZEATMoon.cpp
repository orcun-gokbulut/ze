//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATMoon.cpp
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

#include "ZEATMoon.h"

#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZEATPeriodicTerms.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGRTexture.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStage.h"
#include "ZERenderer/ZERNStageAtmosphere.h"
#include "ZERenderer/ZELightDirectional.h"

#define ZEAT_MDF_SHADERS				1
#define ZEAT_MDF_RENDER_STATES			2
#define ZEAT_MDF_CONSTANT_BUFFERS		4

bool ZEATMoon::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_MDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNMoon.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNMoon_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Can not compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNMoon_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Can not compile pixel shader");

	DirtyFlags.UnraiseFlags(ZEAT_MDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_MDF_RENDER_STATES);

	return true;
}

bool ZEATMoon::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZEAT_MDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStageAtmosphere::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);
	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set sun render state.");

	DirtyFlags.UnraiseFlags(ZEAT_MDF_RENDER_STATES);

	return true;
}

bool ZEATMoon::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZEAT_MDF_CONSTANT_BUFFERS))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_MDF_CONSTANT_BUFFERS);

	return true;
}

bool ZEATMoon::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

bool ZEATMoon::CalculateMoonPositionScreen(const ZERNView& View, ZEVector2& OutVector)
{
	ZEVector3 MoonDirectionView;
	Direction.NormalizeSelf();
	ZEMatrix4x4::Transform3x3(MoonDirectionView, View.ViewTransform, Direction);
	float MoonPositionScreenX = (MoonDirectionView.x * View.ProjectionTransform.M11 + MoonDirectionView.z * View.ProjectionTransform.M13) / MoonDirectionView.z;
	float MoonPositionScreenY = (MoonDirectionView.y * View.ProjectionTransform.M22 + MoonDirectionView.z * View.ProjectionTransform.M23) / MoonDirectionView.z;

	if (//MoonPositionScreenX >= -1.0f && MoonPositionScreenX <= 1.0f &&
		//MoonPositionScreenY >= -1.0f && MoonPositionScreenY <= 1.0f &&
		ZEVector3::DotProduct(View.N, Direction) > 0.0f)
	{
		OutVector.x = MoonPositionScreenX;
		OutVector.y = MoonPositionScreenY;

		return true;
	}

	return false;
}

ZEEntityResult ZEATMoon::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	zeCheckError(ConstantBuffer == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");

	if (!Update())
		return ZE_ER_FAILED_CLEANUP;

	return ZE_ER_DONE;
}

ZEEntityResult ZEATMoon::UnloadInternal()
{
	DirtyFlags.RaiseAll();
	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
	ConstantBuffer.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEATMoon::ZEATMoon()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_ATMOSPHERE;
	Command.Priority = 2;

	Direction = ZEVector3(0.0f, 1.0f, 0.0f);
	DiskRadius = 0.257f;

	MoonLight = ZELightDirectional::CreateInstance();
	MoonLight->SetName("MoonLight");
	MoonLight->SetVisible(true);
	MoonLight->SetEnabled(true);
	MoonLight->SetCastsShadow(false);
	MoonLight->SetIntensity(0.001f);
	MoonLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	MoonLight->SetIsTerrestrial(true);
	AddComponent(MoonLight);

	Constants.Phase = 31.5f / 53.0f;
	Constants.Color = ZEVector3::One;

	SetEntityFlags(ZE_EF_RENDERABLE);
}

ZEATMoon::~ZEATMoon()
{
	ClearComponents();
}

void ZEATMoon::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATMoon::GetDirection() const
{
	return Direction;
}

void ZEATMoon::SetDiskRadius(float DiskRadius)
{
	this->DiskRadius = DiskRadius;
}

float ZEATMoon::GetDiskRadius() const
{
	return DiskRadius;
}

void ZEATMoon::SetColor(const ZEVector3& Color)
{
	if (Constants.Color == Color)
		return;

	Constants.Color = Color;

	DirtyFlags.RaiseFlags(ZEAT_MDF_CONSTANT_BUFFERS);
}

const ZEVector3& ZEATMoon::GetColor() const
{
	return Constants.Color;
}

ZELightDirectional* ZEATMoon::GetMoonLight() const
{
	return MoonLight;
}

void ZEATMoon::SetTextureFile(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	ZEGRTextureOptions TextureOptions;
	TextureOptions.Type = ZEGR_TT_3D;
	TextureOptions.HorizontalTileCount = HorizTileCount;
	TextureOptions.VerticalTileCount = VertTileCount;
	TextureOptions.CompressionFormat = ZEGR_TF_BC1_UNORM_SRGB;
	TextureOptions.GenerateMipMaps = false;
	TextureOptions.MaximumMipmapLevel = 0;
	TextureOptions.sRGB = true;

	PhaseTexture = ZEGRTexture::LoadResourceShared(FileName, TextureOptions);
}

const ZEString& ZEATMoon::GetTextureFile() const
{
	return PhaseTexture != NULL ? PhaseTexture->GetName() : ZEString::Empty;
}

bool ZEATMoon::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	const ZERNView& View = *Parameters->View;

	ZEVector2 MoonPositionScreen;
	if (!CalculateMoonPositionScreen(View, MoonPositionScreen))
		return false;

	ZEVector2 MoonSizeScreen = DiskRadius * ZEVector2(View.ProjectionTransform.M11, View.ProjectionTransform.M22);

	if (Constants.PositionScreen != MoonPositionScreen)
	{
		Constants.PositionScreen = MoonPositionScreen;
		DirtyFlags.RaiseFlags(ZEAT_MDF_CONSTANT_BUFFERS);
	}
	if (Constants.SizeScreen != MoonSizeScreen)
	{
		Constants.SizeScreen = MoonSizeScreen;
		DirtyFlags.RaiseFlags(ZEAT_MDF_CONSTANT_BUFFERS);
	}

	Parameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATMoon::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 9, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(RenderStateData);
	const ZEGRTexture* Texture = PhaseTexture;
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, &Texture);
	
	Context->Draw(4, 0);
}

ZEATMoon* ZEATMoon::CreateInstance()
{
	return new ZEATMoon();
}
