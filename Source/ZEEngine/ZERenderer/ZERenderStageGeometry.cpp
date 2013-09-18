//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderStageGeometry.cpp
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

#include "ZEMaterial.h"
#include "ZERenderCommand.h"
#include "ZERenderStageGeometry.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsWindow.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

void ZERenderStageGeometry::UpdateBuffers()
{
	ZETexture2D* Texture = NULL;
	ZEInt Width, Height;
	zeGraphics->GetWindow()->GetSize(Width, Height);

	Texture = Textures.Depth;
	if (Texture == NULL || Texture->GetWidth() != Width || Texture->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.Depth);
		ZE_DESTROY(Textures.Depth);

		Textures.Depth = ZETexture2D::CreateInstance();
		Textures.Depth->CreateStatic(Width, Height, 1, ZE_TPF_F32, true, NULL);
		RenderTargets.Depth = Textures.Depth->CreateRenderTarget(0);
	}
	
	Texture = Textures.NormalSpecular;
	if (Texture == NULL || Texture->GetWidth() != Width || Texture->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.NormalSpecular);
		ZE_DESTROY(Textures.NormalSpecular);

		Textures.NormalSpecular = ZETexture2D::CreateInstance();
		Textures.NormalSpecular->CreateStatic(Width, Height, 1, ZE_TPF_F16_4, true, NULL);
		RenderTargets.NormalSpecular = Textures.NormalSpecular->CreateRenderTarget(0);
	}

	Texture = Textures.SSScatteringVelocity;
	if (Texture == NULL || Texture->GetWidth() != Width || Texture->GetHeight() != Height)
	{
		ZE_DESTROY(RenderTargets.SSScatteringVelocity);
		ZE_DESTROY(Textures.SSScatteringVelocity);

		Textures.SSScatteringVelocity = ZETexture2D::CreateInstance();
		Textures.SSScatteringVelocity->CreateStatic(Width, Height, 1, ZE_TPF_I8_4, true, NULL);
		RenderTargets.SSScatteringVelocity = Textures.SSScatteringVelocity->CreateRenderTarget(0);
	}
}

void ZERenderStageGeometry::DestroyBuffers()
{
	ZE_DESTROY(RenderTargets.Depth);
	ZE_DESTROY(RenderTargets.NormalSpecular);
	ZE_DESTROY(RenderTargets.SSScatteringVelocity);

	ZE_DESTROY(Textures.Depth);
	ZE_DESTROY(Textures.NormalSpecular);
	ZE_DESTROY(Textures.SSScatteringVelocity);
}

const ZETexture2D* ZERenderStageGeometry::GetTextureDepth() const
{
	return Textures.Depth;
}

const ZETexture2D* ZERenderStageGeometry::GetTextureNormalSpecular() const
{
	return Textures.NormalSpecular;
}

const ZETexture2D* ZERenderStageGeometry::GetTextureSSScatteringVelocity() const
{
	return Textures.SSScatteringVelocity;
}

const ZERenderTarget* ZERenderStageGeometry::GetRenderTargetDepth() const
{
	return RenderTargets.Depth;
}

const ZERenderTarget* ZERenderStageGeometry::GetRenderTargetNormalSpecular() const
{
	return RenderTargets.NormalSpecular;
}

const ZERenderTarget* ZERenderStageGeometry::GetRenderTargetSSScatteringVelocity() const
{
	return RenderTargets.SSScatteringVelocity;
}

ZERenderStageType ZERenderStageGeometry::GetStageType() const
{
	return ZE_RST_GEOMETRY;
}

ZERenderStageType ZERenderStageGeometry::GetDependencies() const
{
	return ZE_RST_NONE;
}

bool ZERenderStageGeometry::ResetStates(const ZEMaterial* Material)
{
	// Reset to default states
	// ----------------------------------------------------------
	if (!ZERenderStage::ResetStates(Material))
		return false;

	// Render targets
	DefaultStates.RenderTargets[0] = RenderTargets.Depth;
	DefaultStates.RenderTargets[1] = RenderTargets.NormalSpecular;
	DefaultStates.RenderTargets[2] = RenderTargets.SSScatteringVelocity;
	DefaultStates.DepthStencilBuffer = zeGraphics->GetWindow()->GetDepthBuffer();

	ZEInt Width, Height;
	zeGraphics->GetWindow()->GetSize(Width, Height);

	// Use default viewport and scissor rectangles
	DefaultStates.ViewPorts[0].StateData.TopLeftX = 0.0f;
	DefaultStates.ViewPorts[0].StateData.TopLeftY = 0.0f;
	DefaultStates.ViewPorts[0].StateData.Width = (float)Width;
	DefaultStates.ViewPorts[0].StateData.Height = (float)Height;
	DefaultStates.ViewPorts[0].StateData.MinDepth = 0.0f;
	DefaultStates.ViewPorts[0].StateData.MaxDepth = 1.0f;

	DefaultStates.ScissorRects[0].StateData.Left = 0;
	DefaultStates.ScissorRects[0].StateData.Top = 0;
	DefaultStates.ScissorRects[0].StateData.Right = Width;
	DefaultStates.ScissorRects[0].StateData.Bottom = Height;

	// Blend state
	DefaultStates.BlendState.SetComponentWriteMask(0, ZE_CM_RED);   // Output to RT0 is float
	DefaultStates.BlendState.SetComponentWriteMask(1, ZE_CM_ALL);   // Output to RT1 is float4
	DefaultStates.BlendState.SetComponentWriteMask(2, ZE_CM_ALL);   // Output to RT2 is float4

	// Depth Stencil state
	DefaultStates.DepthStencilState.SetStencilTestEnable(false);
	DefaultStates.DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStates.DepthStencilState.SetZTestEnable(true);
	DefaultStates.DepthStencilState.SetZWriteEnable(true);

	// Rasterizer state
	DefaultStates.RasterizerState.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	DefaultStates.RasterizerState.SetFillMode(ZE_FM_SOLID);


	// Commit default states
	// ----------------------------------------------------------
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetRenderTargetArray(DefaultStates.RenderTargets);
	Device->SetDepthStencilBuffer(DefaultStates.DepthStencilBuffer);

	Device->SetBlendState(DefaultStates.BlendState);
	Device->SetDepthStencilState(DefaultStates.DepthStencilState);

	Device->SetRasterizerState(DefaultStates.RasterizerState);

	Device->SetViewport(0, DefaultStates.ViewPorts[0]);
	Device->SetScissorRectangle(0, DefaultStates.ScissorRects[0]);

	return true;
}

bool ZERenderStageGeometry::Process(const ZERenderCommand* RenderCommand)
{
	if (!ZERenderStage::Process(RenderCommand))
		return false;	

	ZEUInt PassId = 0;
	bool Continue = false;

	do
	{
		Continue = RenderCommand->Material->SetupPass(PassId++, this, RenderCommand);
	
	} while (Continue);

	PumpStreams(RenderCommand);

	return true;
}

bool ZERenderStageGeometry::Setup()
{
	if (!ZERenderStage::Setup())
		return false;

	UpdateBuffers();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	// Clear targets
	Device->ClearRenderTarget(RenderTargets.Depth, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.NormalSpecular, ZEVector4::Zero);
	Device->ClearRenderTarget(RenderTargets.SSScatteringVelocity, ZEVector4::Zero);
	Device->ClearDepthStencilBuffer(zeGraphics->GetWindow()->GetDepthBuffer(), true, true, 1.0f, 0);
	
	return true;
}

ZERenderStageGeometry::ZERenderStageGeometry()
{	
	Textures.Depth = NULL;
	Textures.NormalSpecular = NULL;
	Textures.SSScatteringVelocity = NULL;

	RenderTargets.Depth = NULL;
	RenderTargets.NormalSpecular = NULL;	
	RenderTargets.SSScatteringVelocity = NULL;
}

ZERenderStageGeometry::~ZERenderStageGeometry()
{
	DestroyBuffers();
}
