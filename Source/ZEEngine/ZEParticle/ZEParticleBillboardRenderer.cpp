//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleBillboardRenderer.cpp
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

#include "ZEParticleBillboardRenderer.h"

#include "ZEParticle/ZEParticleSystem.h"
#include "ZERenderer/ZECamera.h"
#include "ZERenderer/ZERNView.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEGraphics/ZEGRShader.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRRenderState.h"
#include "ZEGraphics/ZEGRVertexBuffer.h"
#include "ZEGraphics/ZEGRStructuredBuffer.h"

void ZEParticleBillboardRenderer::CreateShaders()
{
	ZEGRShaderCompileOptions Options;
	Options.FileName = "#R:/ZEEngine/ZERNRenderer/Shaders/ZED11/ZERNParticleRendering.hlsl";
	Options.Model = ZEGR_SM_5_0;

	Options.Type = ZEGR_ST_VERTEX;
	Options.EntryPoint = "ZERNParticleRendering_VertexShader_Main";
	VertexShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_HULL;
	Options.EntryPoint = "ZERNParticleRendering_HullShader_Main";
	HullShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_DOMAIN;
	Options.EntryPoint = "ZERNParticleRendering_DomainShader_Main";
	DomainShader = ZEGRShader::Compile(Options);

	Options.Type = ZEGR_ST_PIXEL;
	Options.EntryPoint = "ZERNParticleRendering_PixelShader_Main";
	PixelShader = ZEGRShader::Compile(Options);

}

void ZEParticleBillboardRenderer::CreateRenderState()
{
	ZEGRRenderState RenderState;

	RenderState.SetPrimitiveType(ZEGR_PT_4_CONTROL_POINT_PATCHLIST);
	RenderState.SetRenderTargetFormat(0, ZEGR_TF_R8G8B8A8_UNORM);

	ZEGRDepthStencilState DepthStencilState;
	DepthStencilState.SetDepthTestEnable(false);
	DepthStencilState.SetDepthWriteEnable(false);
	DepthStencilState.SetStencilTestEnable(false);

	RenderState.SetDepthStencilState(DepthStencilState);

	RenderState.SetShader(ZEGR_ST_VERTEX, VertexShader);
	RenderState.SetShader(ZEGR_ST_HULL, HullShader);
	RenderState.SetShader(ZEGR_ST_DOMAIN, DomainShader);
	RenderState.SetShader(ZEGR_ST_PIXEL, PixelShader);
	RenderStateData = RenderState.Compile();
	zeCheckError(RenderStateData == NULL, , "Cannot set render state.");
}

bool ZEParticleBillboardRenderer::PreRender(const ZERNCullParameters* CullParameters)
{
	RenderCommand.Entity = (ZEEntity*)(GetOwner()->GetOwner());
	RenderCommand.Order = 0;
	RenderCommand.Priority = 0;
	RenderCommand.StageMask = Material->GetStageMask();
	RenderCommand.ExtraParameters = (void*)GetOwner();
	
	CullParameters->Renderer->AddCommand(&RenderCommand);

	return true;
}

void ZEParticleBillboardRenderer::Render(const ZERNRenderParameters* RenderParameters, const ZERNCommand* Command)
{
	ZEParticleRenderer::Render(RenderParameters, Command);

	if (GetOwner()->GetParticlePool().GetCount() == 0)
		return;

	ZEGRContext* Context = RenderParameters->Context;

	Context->SetRenderState(RenderStateData);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetStructuredBuffer(ZEGR_ST_VERTEX, 0, InstanceBuffer);
	Context->DrawInstanced(4, 0, Particles.GetCount(), 0);
}

void ZEParticleBillboardRenderer::SetAxixOfOrientation(const ZEVector3& AxisOfOrientation)
{
	this->AxisOfOrientation = AxisOfOrientation;
}

const ZEVector3& ZEParticleBillboardRenderer::GetAxisOfOrientation() const
{
	return AxisOfOrientation;
}

ZEParticleBillboardRenderer* ZEParticleBillboardRenderer::CreateInstance()
{
	return new ZEParticleBillboardRenderer();
}

bool ZEParticleBillboardRenderer::InitializeSelf()
{
	CreateShaders();
	CreateRenderState();

	InstanceBuffer = ZEGRStructuredBuffer::Create(Particles.GetCount(), sizeof(InstanceAttributes));

	return true;
}

void ZEParticleBillboardRenderer::DeinitializeSelf()
{
	InstanceBuffer.Release();
	VertexShader.Release();
	HullShader.Release();
	DomainShader.Release();
	PixelShader.Release();
	RenderStateData.Release();
}

ZEParticleBillboardRenderer::ZEParticleBillboardRenderer()
{
	AxisOfOrientation = ZEVector3::UnitZ;
}
