//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATSun.cpp
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

#include "ZEATSun.h"

#include "ZEATPeriodicTerms.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
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

#define ZEAT_SDF_SHADERS				1
#define ZEAT_SDF_RENDER_STATES			2
#define ZEAT_SDF_CONSTANT_BUFFERS		4

bool ZEATSun::UpdateShaders()
{
	if (!DirtyFlags.GetFlags(ZEAT_SDF_SHADERS))
		return true;

	ZEGRShaderCompileOptions Options;

	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNSun.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNSun_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);
	zeCheckError(VertexShader == NULL, false, "Can not compile vertex shader");

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNSun_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);
	zeCheckError(PixelShader == NULL, false, "Can not compile pixel shader");

	DirtyFlags.UnraiseFlags(ZEAT_SDF_SHADERS);
	DirtyFlags.RaiseFlags(ZEAT_SDF_RENDER_STATES);

	return true;
}

bool ZEATSun::UpdateRenderStates()
{
	if (!DirtyFlags.GetFlags(ZEAT_SDF_RENDER_STATES))
		return true;

	ZEGRRenderState RenderState = ZERNStageAtmosphere::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);
	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);
	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, false, "Cannot set sun render state.");

	DirtyFlags.UnraiseFlags(ZEAT_SDF_RENDER_STATES);

	return true;
}

bool ZEATSun::UpdateConstantBuffers()
{
	if (!DirtyFlags.GetFlags(ZEAT_SDF_CONSTANT_BUFFERS))
		return true;

	ConstantBuffer->SetData(&Constants);

	DirtyFlags.UnraiseFlags(ZEAT_SDF_CONSTANT_BUFFERS);

	return true;
}

bool ZEATSun::Update()
{
	if (!UpdateShaders())
		return false;

	if (!UpdateRenderStates())
		return false;

	if (!UpdateConstantBuffers())
		return false;

	return true;
}

bool ZEATSun::CalculateSunPositionScreen(const ZERNView& View, ZEVector2& OutVector)
{
	ZEVector3 SunDirectionView;
	Direction.NormalizeSelf();
	ZEMatrix4x4::Transform3x3(SunDirectionView, View.ViewTransform, Direction);
	float SunPositionScreenX = (SunDirectionView.x * View.ProjectionTransform.M11 + SunDirectionView.z * View.ProjectionTransform.M13) / SunDirectionView.z;
	float SunPositionScreenY = (SunDirectionView.y * View.ProjectionTransform.M22 + SunDirectionView.z * View.ProjectionTransform.M23) / SunDirectionView.z;

	if (//SunPositionScreenX >= -1.0f && SunPositionScreenX <= 1.0f &&
		//SunPositionScreenY >= -1.0f && SunPositionScreenY <= 1.0f &&
		ZEVector3::DotProduct(View.N, Direction) > 0.0f)
	{
		OutVector.x = SunPositionScreenX;
		OutVector.y = SunPositionScreenY;
		
		return true;
	}

	return false;
}

ZEEntityResult ZEATSun::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);
	
	ConstantBuffer = ZEGRBuffer::CreateResource(ZEGR_BT_CONSTANT_BUFFER, sizeof(Constants), 0, ZEGR_RU_DYNAMIC, ZEGR_RBF_CONSTANT_BUFFER);
	zeCheckError(ConstantBuffer == NULL, ZE_ER_FAILED_CLEANUP, "Cannot create constant buffer.");

	if (!Update())
		return ZE_ER_FAILED_CLEANUP;

	return ZE_ER_DONE;
}

ZEEntityResult ZEATSun::UnloadInternal()
{
	DirtyFlags.RaiseAll();

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();

	ConstantBuffer.Release();

	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);
	return ZE_ER_DONE;
}

ZEATSun::ZEATSun()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_ATMOSPHERE;
	Command.Priority = 2;

	Direction = ZEVector3(0.0f, 1.0f, 0.0f);
	DiskRadius = 0.266f;

	SunLight = ZELightDirectional::CreateInstance();
	SunLight->SetName("SunLight");
	SunLight->SetVisible(true);
	SunLight->SetEnabled(true);
	SunLight->SetCastsShadow(true);
	SunLight->SetRange(500.0f);
	SunLight->SetCascadeCount(4);
	SunLight->SetCascadeDistanceFactor(0.8f);
	SunLight->SetCascadeDepthBias(0, 0.00005f);
	SunLight->SetCascadeDepthBias(1, 0.00005f);
	SunLight->SetCascadeDepthBias(2, 0.00005f);
	SunLight->SetCascadeDepthBias(3, 0.0004f);
	SunLight->SetCascadeNormalBias(0, 0.1f);
	SunLight->SetCascadeNormalBias(1, 0.1f);
	SunLight->SetCascadeNormalBias(2, 0.2f);
	SunLight->SetCascadeNormalBias(3, 0.4f);
	SunLight->SetShadowSampleCount(ZERN_LSC_HIGH);
	SunLight->SetShadowSampleLength(2.0f);
	SunLight->SetIntensity(10.0f);
	SunLight->SetColor(ZEVector3(1.0f, 1.0f, 1.0f));
	SunLight->SetIsTerrestrial(true);
	AddComponent(SunLight);

	Constants.Color = ZEVector3::One;

	SetEntityFlags(ZE_EF_RENDERABLE);
}

ZEATSun::~ZEATSun()
{
	ClearComponents();
}

void ZEATSun::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATSun::GetDirection() const
{
	return Direction;
}

void ZEATSun::SetDiskRadius(float DiskRadius)
{
	this->DiskRadius = DiskRadius;
}

float ZEATSun::GetDiskRadius() const
{
	return DiskRadius;
}

void ZEATSun::SetColor(const ZEVector3& Color)
{
	if (Constants.Color == Color)
		return;

	Constants.Color = Color;

	DirtyFlags.RaiseFlags(ZEAT_SDF_CONSTANT_BUFFERS);
}

const ZEVector3& ZEATSun::GetColor() const
{
	return Constants.Color;
}

ZELightDirectional* ZEATSun::GetSunLight() const
{
	return SunLight;
}

bool ZEATSun::PreRender(const ZERNPreRenderParameters* Parameters)
{
	if (!ZEEntity::PreRender(Parameters))
		return false;

	if (Parameters->Type == ZERN_RT_SHADOW)
		return false;

	const ZERNView& View = *Parameters->View;

	ZEVector2 SunPositionScreen;
	if (!CalculateSunPositionScreen(View, SunPositionScreen))
		return false;

	ZEVector2 SunSizeScreen = DiskRadius * ZEVector2(View.ProjectionTransform.M11, View.ProjectionTransform.M22);

	if (Constants.PositionScreen != SunPositionScreen)
	{
		Constants.PositionScreen = SunPositionScreen;
		DirtyFlags.RaiseFlags(ZEAT_SDF_CONSTANT_BUFFERS);
	}
	if (Constants.SizeScreen != SunSizeScreen)
	{
		Constants.SizeScreen = SunSizeScreen;
		DirtyFlags.RaiseFlags(ZEAT_SDF_CONSTANT_BUFFERS);
	}

	//Parameters->Renderer->AddCommand(&Command);
	Command.Reset();
	Parameters->CommandList->AddCommand(&Command);

	return true;
}

void ZEATSun::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	const ZERNStage* Stage = Parameters->Stage;

	Context->SetConstantBuffer(ZEGR_ST_VERTEX, 9, ConstantBuffer);
	Context->SetConstantBuffer(ZEGR_ST_PIXEL, 9, ConstantBuffer);
	Context->SetRenderState(RenderStateData);

	Context->Draw(4, 0);
}

ZEATSun* ZEATSun::CreateInstance()
{
	return new ZEATSun();
}
