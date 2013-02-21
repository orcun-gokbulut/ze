//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageShadow.cpp
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

#include "ZELight.h"
#include "ZERenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEShader.h"
#include "ZERenderStageShadow.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEDepthStencilBuffer.h"
#include "ZEGraphics/ZEGraphicsDeviceState.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageShadow::DestroyShaders()
{
	ZE_DESTROY(Shaders.PointLightVS);
	ZE_DESTROY(Shaders.PointLightPS);
	ZE_DESTROY(Shaders.ProjectiveLightVS);
	ZE_DESTROY(Shaders.ProjectiveLightPS);
	ZE_DESTROY(Shaders.DirectionalLightVS);
	ZE_DESTROY(Shaders.DirectionalLightPS);
	ZE_DESTROY(Shaders.OmniProjectiveLightVS);
	ZE_DESTROY(Shaders.OmniProjectiveLightPS);
}

void ZERenderStageShadow::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;
	/*
	if (Shaders.PointLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_VertexShader";
		Shaders.PointLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.PointLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_PixelShader";
		Shaders.PointLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.DirectionalLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_VertexShader";
		Shaders.DirectionalLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.DirectionalLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEDirectionalLight_PixelShader";
		Shaders.DirectionalLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.ProjectiveLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_VertexShader";
		Shaders.ProjectiveLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.ProjectiveLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEProjectiveLight_PixelShader";
		Shaders.ProjectiveLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.OmniProjectiveLightVS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_VertexShader";
		Shaders.OmniProjectiveLightVS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (Shaders.OmniProjectiveLightPS == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "Lights.hlsl";
		Compileoptions.EntryPoint = "ZEOmniProjectiveLight_PixelShader";
		Shaders.OmniProjectiveLightPS = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}*/
}

void ZERenderStageShadow::UpdateBuffers()
{
	// Update parent buffers
	ZERenderStage::UpdateBuffers();

	ZEVector2 ShadowMapDimension = zeScene->GetRenderer()->GetShadowMapDimension();
	ZEUInt32 Width = (ZEUInt32)(ShadowMapDimension.x + 0.5f);
	ZEUInt32 Height = (ZEUInt32)(ShadowMapDimension.y + 0.5f);

	if (RenderTargets.DepthStencilBuffer == NULL || RenderTargets.DepthStencilBuffer->GetWidth() != Width || RenderTargets.DepthStencilBuffer->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.DepthStencilBuffer);

		RenderTargets.DepthStencilBuffer = ZEDepthStencilBuffer::CreateInstance();
		RenderTargets.DepthStencilBuffer->Create(Width, Height, ZE_DSPF_DEPTH24_STENCIL8);
	}
}

void ZERenderStageShadow::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.DepthStencilBuffer);
}

void ZERenderStageShadow::ResetStageDefaults()
{
	// Requested pipeline constants
	StageConstants.SetCount(2);
	StageConstants.Add(ZE_PC_CAMERA_WORLD_POS);
	StageConstants.Add(ZE_PC_INV_VIEWPORT_WIDTH_HEIGHT);

	// Reset parent states
	ZERenderStage::ResetStageDefaults();

	// Render targets
	DefaultStates.DepthStencilBuffer = RenderTargets.DepthStencilBuffer;

	ZEVector2 ShadowMapDimension = zeScene->GetRenderer()->GetShadowMapDimension();
	ZEUInt32 Width = (ZEUInt32)(ShadowMapDimension.x + 0.5f);
	ZEUInt32 Height = (ZEUInt32)(ShadowMapDimension.y + 0.5f);

	DefaultStates.ViewPorts[0].ViewportData.TopLeftX = 0;
	DefaultStates.ViewPorts[0].ViewportData.TopLeftY = 0;
	DefaultStates.ViewPorts[0].ViewportData.MinDepth = 0.0f;
	DefaultStates.ViewPorts[0].ViewportData.MaxDepth = 1.0f;
	DefaultStates.ViewPorts[0].ViewportData.Width = Width;
	DefaultStates.ViewPorts[0].ViewportData.Height = Height;

	DefaultStates.ScissorRectangles[0].ScissorRectangleData.Top = 0;
	DefaultStates.ScissorRectangles[0].ScissorRectangleData.Left = 0;
	DefaultStates.ScissorRectangles[0].ScissorRectangleData.Right = (ZEInt)Width;
	DefaultStates.ScissorRectangles[0].ScissorRectangleData.Bottom = (ZEInt)Height;

	// Rasterizer state
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);

	// Blend state
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_RED);

	// Depth stencil state
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStates.DepthStencilState.SetZWriteEnable(true);
}

void ZERenderStageShadow::CommitStageDefaults()
{
	// Commit parent states
	ZERenderStage::CommitStageDefaults();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);
	Device->SetRasterizerState(DefaultStates.RasterizerState);

	Device->SetViewport(0, DefaultStates.ViewPorts[0]);
	Device->SetScissorRectangle(0, DefaultStates.ScissorRectangles[0]);
}

void ZERenderStageShadow::RenderShadowPointLight(const ZELightPoint* Light)
{
	zeCriticalError("Not implemented yet");
}

void ZERenderStageShadow::RenderShadowProjectiveLight(const ZELightProjective* Light)
{
	zeCriticalError("Not implemented yet");
}

void ZERenderStageShadow::RenderShadowDirectionalLight(const ZELightDirectional* Light)
{
	zeCriticalError("Not implemented yet");
}

void ZERenderStageShadow::RenderShadowOmniProjectiveLight(const ZELightOmniProjective* Light)
{
	zeCriticalError("Not implemented yet");
}

ZEUInt32 ZERenderStageShadow::GetStageFlags() const
{
	return ZE_RENDER_STAGE_SHADOW;
}

void ZERenderStageShadow::SetLightList(ZESmartArray<ZELight*>* Lights)
{
	zeDebugCheck(Lights == NULL, "Null pointer.");
	LightList = Lights;
}

ZESmartArray<ZELight*>* ZERenderStageShadow::GetLightList() const
{
	return LightList;
}

void ZERenderStageShadow::Setup()
{
	UpdateShaders();
	UpdateBuffers();
	ResetStageDefaults();
	CommitStageDefaults();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	Device->ClearDepthStencilBuffer(RenderTargets.DepthStencilBuffer, true, true, 1.0f, 0);
}

void ZERenderStageShadow::Process(ZERenderCommand* RenderCommand)
{
	// Be sure our array is large enough
	CommandList.SetCount(zeScene->GetEntities().GetCount());

	ZESize LightCount = LightList->GetCount();
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();

	for (ZESize I = 0; I < LightCount; ++I)
	{
		ZELight* Light = LightList->GetItem(I);
		ZELightType Type = Light->GetLightType();

		if (!Light->GetShadowCaster())
			continue;

		switch(Type)
		{
			case ZE_LT_POINT:
				Tracer->StartEvent("Point Light Shadow Pass");
				RenderShadowPointLight((ZELightPoint*)Light);
				Tracer->EndEvent();
				break;
			
			case ZE_LT_DIRECTIONAL:
				Tracer->StartEvent("Directional Light Shadow Pass");
				RenderShadowDirectionalLight((ZELightDirectional*)Light);
				Tracer->EndEvent();
				break;

			case ZE_LT_PROJECTIVE:
				Tracer->StartEvent("Projective Light Shadow Pass");
				RenderShadowProjectiveLight((ZELightProjective*)Light);
				Tracer->EndEvent();
				break;

			case ZE_LT_OMNIPROJECTIVE:
				Tracer->StartEvent("Omni Projective Light Shadow Pass");
				RenderShadowOmniProjectiveLight((ZELightOmniProjective*)Light);
				Tracer->EndEvent();
				break;
		}
	}
}

ZERenderStageShadow::ZERenderStageShadow()
{
	LightList = NULL;
	CommandList.Clear(false);

	Shaders.PointLightVS = NULL;
	Shaders.PointLightPS = NULL;
	Shaders.ProjectiveLightVS = NULL;
	Shaders.ProjectiveLightPS = NULL;
	Shaders.DirectionalLightVS = NULL;
	Shaders.DirectionalLightPS = NULL;
	Shaders.OmniProjectiveLightVS = NULL;
	Shaders.OmniProjectiveLightPS = NULL;

	RenderTargets.DepthStencilBuffer = NULL;
}

ZERenderStageShadow::~ZERenderStageShadow()
{
	DestroyBuffers();
	DestroyShaders();
}
