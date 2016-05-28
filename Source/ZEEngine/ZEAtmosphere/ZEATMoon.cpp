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
#include "ZEGame/ZEMoon.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRSampler.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStage.h"
#include "ZERenderer/ZERNStagePostProcess.h"

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

	ZEGRRenderState RenderState = ZERNStagePostProcess::GetRenderState();
	RenderState.SetPrimitiveType(ZEGR_PT_TRIANGLE_STRIPT);

	ZEGRDepthStencilState DepthStencilStateTestNoWrite;
	DepthStencilStateTestNoWrite.SetDepthTestEnable(true);
	DepthStencilStateTestNoWrite.SetDepthWriteEnable(false);

	RenderState.SetDepthStencilState(DepthStencilStateTestNoWrite);

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

	if (MoonPositionScreenX >= -1.0f && MoonPositionScreenX <= 1.0f &&
		MoonPositionScreenY >= -1.0f && MoonPositionScreenY <= 1.0f &&
		ZEVector3::DotProduct(View.N, Direction) > 0.0f)
	{
		OutVector.x = MoonPositionScreenX;
		OutVector.y = MoonPositionScreenY;

		return true;
	}

	return false;
}

bool ZEATMoon::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));

	return Update();
}

bool ZEATMoon::DeinitializeSelf()
{
	DirtyFlags.RaiseAll();

	VertexShader.Release();
	PixelShader.Release();
	RenderStateData.Release();

	ConstantBuffer.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEATMoon::ZEATMoon()
{
	DirtyFlags.RaiseAll();

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_POST_EFFECT;
	Command.Priority = 2;

	Direction = ZEVector3(0.0f, 1.0f, 0.0f);
	DiskRadius = 0.257f;

	Constants.Phase = 31.5f / 53.0f;
	Constants.Color = ZEVector3::One;
}

ZEATMoon::~ZEATMoon()
{

}

ZEDrawFlags ZEATMoon::GetDrawFlags() const
{
	return ZE_DF_DRAW;
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

void ZEATMoon::SetTextureFile(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	if (FileName.IsEmpty())
		return;

	PhaseTexture.Load3D(FileName, HorizTileCount, VertTileCount);
}

const ZEString& ZEATMoon::GetTextureFile() const
{
	return PhaseTexture.GetTextureFile();
}

bool ZEATMoon::PreRender(const ZERNCullParameters* CullParameters)
{
	if (!ZEEntity::PreRender(CullParameters))
		return false;

	const ZERNView& View = *CullParameters->View;

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

	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATMoon::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (!Update())
		return;

	ZEGRContext* Context = Parameters->Context;
	ZERNStage* Stage = Parameters->Stage;

	const ZEGRRenderTarget* RenderTarget = Stage->GetProvidedInput(ZERN_SO_COLOR);
	const ZEGRDepthStencilBuffer* DepthStencilBuffer = Stage->GetOutput(ZERN_SO_DEPTH)->GetDepthStencilBuffer(true);

	Context->SetConstantBuffers(ZEGR_ST_VERTEX, 9, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, ConstantBuffer.GetPointerToPointer());
	Context->SetRenderState(RenderStateData);
	const ZEGRTexture* Texture = PhaseTexture.GetTexture();
	Context->SetTextures(ZEGR_ST_PIXEL, 5, 1, &Texture);

	Context->SetRenderTargets(1, &RenderTarget, DepthStencilBuffer);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));
	
	Context->Draw(4, 0);
}

ZEATMoon* ZEATMoon::CreateInstance()
{
	return new ZEATMoon();
}
