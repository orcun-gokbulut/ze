//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSunMaterial.cpp
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

#include "ZERNSunMaterial.h"

#include "ZERNStageID.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZERNStagePreProcess.h"

#define ZERN_SMDF_CONSTANT_BUFFER		1
#define ZERN_SMDF_RENDER_STATE			2
#define ZERN_SMDF_SHADERS				4

bool ZERNSunMaterial::UpdateShaders() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSunMaterial.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSunMaterial_RenderSun_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Can not compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSunMaterial_RenderSun_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Can not compile pixel shader");

	DirtyFlags.UnraiseFlags(ZERN_SMDF_SHADERS);
	DirtyFlags.RaiseFlags(ZERN_SMDF_RENDER_STATE);

	return true;
}

bool ZERNSunMaterial::UpdateRenderState() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_RENDER_STATE))
		return true;

	ZEGRRenderState RenderState = ZERNStagePreProcess::GetRenderState();

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);

	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);

	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set sun render state.");

	DirtyFlags.UnraiseFlags(ZERN_SMDF_RENDER_STATE);

	return true;
}

bool ZERNSunMaterial::UpdateConstantBuffer() const
{
	if (!DirtyFlags.GetFlags(ZERN_SMDF_CONSTANT_BUFFER))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZERN_SMDF_CONSTANT_BUFFER);

	return true;
}

bool ZERNSunMaterial::InitializeSelf()
{
	if (!ZERNMaterial::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if (!UpdateConstantBuffer())
		return false;

	return true;
}

void ZERNSunMaterial::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();

	ConstantBuffer.Release();

	ZERNMaterial::DeinitializeSelf();
}

ZERNSunMaterial::ZERNSunMaterial()
{
	DirtyFlags.RaiseAll();
}

ZEUInt ZERNSunMaterial::GetStageMask() const
{
	return ZERN_STAGE_POST_EFFECT;
}

void ZERNSunMaterial::SetSunPositionScreen(const ZEVector2& SunPositionScreen)
{
	if (Constants.SunPositionScreen == SunPositionScreen)
		return;

	Constants.SunPositionScreen = SunPositionScreen;

	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNSunMaterial::GetSunPositionScreen() const
{
	return Constants.SunPositionScreen;
}

void ZERNSunMaterial::SetSunSizeScreen(const ZEVector2& SunSizeScreen)
{
	if (Constants.SunSizeScreen == SunSizeScreen)
		return;

	Constants.SunSizeScreen = SunSizeScreen;

	DirtyFlags.RaiseFlags(ZERN_SMDF_CONSTANT_BUFFER);
}

const ZEVector2& ZERNSunMaterial::GetSunSizeScreen() const
{
	return Constants.SunSizeScreen;
}

bool ZERNSunMaterial::SetupMaterial(ZEGRContext* Context, ZERNStage* Stage) const
{
	if (!ZERNMaterial::SetupMaterial(Context, Stage))
		return false;

	if (!Update())
		return false;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, ConstantBuffer);
	Context->SetRenderState(RenderStateData);

	return true;
}

void ZERNSunMaterial::CleanupMaterial(ZEGRContext* Context, ZERNStage* Stage) const
{
	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 8, NULL);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 8, NULL);

	ZERNMaterial::CleanupMaterial(Context, Stage);
}

bool ZERNSunMaterial::Update() const
{
	if (!IsInitialized())
		return false;

	if (!UpdateShaders())
		return false;

	if (!UpdateRenderState())
		return false;

	if(!UpdateConstantBuffer())
		return false;

	return true;
}

ZEHolder<ZERNSunMaterial> ZERNSunMaterial::CreateInstance()
{
	return new ZERNSunMaterial();
}
