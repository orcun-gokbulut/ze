//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageShadowing.cpp
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

#include "ZERNStageShadowing.h"

#include "ZERNStageID.h"
#include "ZEDS/ZEString.h"
#include "ZEGraphics/ZEGRTexture.h"

#define ZERN_SSDF_OUTPUT0		1
#define ZERN_SSDF_OUTPUT1		2
#define ZERN_SSDF_OUTPUT_ALL	(ZERN_SSDF_OUTPUT0 | ZERN_SSDF_OUTPUT1)

bool ZERNStageShadowing::InitializeInternal()
{
	if (!ZERNStage::InitializeInternal())
		return false;

	return true;
}

bool ZERNStageShadowing::DeinitializeInternal()
{
	DirtyFlags.RaiseAll();

	DirectionalShadowMaps.Release();
	ProjectiveShadowMaps.Release();

	CascadeCount = 4;
	DirectionalShadowMapCount = 1;
	ProjectiveShadowMapCount = 4;

	DirectionalShadowMapIndex = 0;
	ProjectiveShadowMapIndex = 0;

	DirectionalShadowMapsResolution = ZE_LSR_HIGH;
	ProjectiveShadowMapsResolution = ZE_LSR_MEDIUM;

	return ZERNStage::DeinitializeInternal();
}

void ZERNStageShadowing::CreateOutput(const ZEString& Name)
{
	if (Name == "DirectionalShadowMaps")
	{
		if (DirtyFlags.GetFlags(ZERN_SSDF_OUTPUT0))
		{
			ZEUInt ShadowMapRes = ZELight::ConvertShadowResolution(DirectionalShadowMapsResolution);
			ZEUInt ArrayCount = DirectionalShadowMapCount * CascadeCount;
			DirectionalShadowMaps = ZEGRTexture::CreateResource(ZEGR_TT_2D, ShadowMapRes, ShadowMapRes, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, ArrayCount);
			DirtyFlags.UnraiseFlags(ZERN_SSDF_OUTPUT0);
		}
	}
	else if (Name == "ProjectiveShadowMaps")
	{
		if (DirtyFlags.GetFlags(ZERN_SSDF_OUTPUT1))
		{
			ZEUInt ShadowMapRes = ZELight::ConvertShadowResolution(ProjectiveShadowMapsResolution);
			ZEUInt ArrayCount = ProjectiveShadowMapCount;
			ProjectiveShadowMaps = ZEGRTexture::CreateResource(ZEGR_TT_2D, ShadowMapRes, ShadowMapRes, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_STATIC, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, ArrayCount);
			DirtyFlags.UnraiseFlags(ZERN_SSDF_OUTPUT1);
		}
	}
}

ZEInt ZERNStageShadowing::GetId() const
{
	return ZERN_STAGE_SHADOWING;
}

const ZEString& ZERNStageShadowing::GetName() const
{
	static const ZEString Name = "Stage Shadowing";
	return Name;
}

void ZERNStageShadowing::SetCascadeShadowMapsResolution(ZELightShadowResolution ShadowResolution)
{
	if (DirectionalShadowMapsResolution == ShadowResolution)
		return;

	DirectionalShadowMapsResolution = ShadowResolution;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZELightShadowResolution ZERNStageShadowing::GetCascadeShadowMapsResolution() const
{
	return DirectionalShadowMapsResolution;
}

void ZERNStageShadowing::SetProjectiveShadowMapsResolution(ZELightShadowResolution ShadowResolution)
{
	if (ProjectiveShadowMapsResolution == ShadowResolution)
		return;

	ProjectiveShadowMapsResolution = ShadowResolution;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT1);
}

ZELightShadowResolution ZERNStageShadowing::GetProjectiveShadowMapsResolution() const
{
	return ProjectiveShadowMapsResolution;
}

void ZERNStageShadowing::SetCascadeCount(ZEUInt CascadeCount)
{
	if (this->CascadeCount == CascadeCount)
		return;

	this->CascadeCount = CascadeCount;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZEUInt ZERNStageShadowing::GetCascadeCount() const
{
	return CascadeCount;
}

void ZERNStageShadowing::SetDirectionalShadowMapCount(ZEUInt DirectionalShadowMapCount)
{
	if (this->DirectionalShadowMapCount == DirectionalShadowMapCount)
		return;

	this->DirectionalShadowMapCount = DirectionalShadowMapCount;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT0);
}

ZEUInt ZERNStageShadowing::GetDirectionalShadowMapCount() const
{
	return DirectionalShadowMapCount;
}

void ZERNStageShadowing::SetProjectiveShadowMapCount(ZEUInt ProjectiveShadowMapCount)
{
	if (this->ProjectiveShadowMapCount == ProjectiveShadowMapCount)
		return;

	this->ProjectiveShadowMapCount = ProjectiveShadowMapCount;

	DirtyFlags.RaiseFlags(ZERN_SSDF_OUTPUT1);
}

ZEUInt ZERNStageShadowing::GetProjectiveShadowMapCount() const
{
	return ProjectiveShadowMapCount;
}

bool ZERNStageShadowing::Setup(ZEGRContext* Context)
{
	if (!ZERNStage::Setup(Context))
		return false;

	if (GetCommands().GetCount() == 0)
		return false;

	DirectionalShadowMapIndex = 0;
	ProjectiveShadowMapIndex = 0;

	//ShadowRenderer.SetContext(Context);
	//
	//ze_for_each(Command, GetCommands())
	//{
	//	if (Command->Entity == NULL)
	//		continue;
	//
	//	if (ZEClass::IsDerivedFrom(ZELight::Class(), Command->Entity->GetClass()))
	//	{
	//		zeWarning("Non-light entity had been added to stage shadowing");
	//		continue;
	//	}
	//
	//	ZELight* Light = static_cast<ZELight*>(Command->Entity);
	//	ZERNView View = ShadowRenderer.GetView();
	//	View.Position = RenderParameters->View->Position;
	//	View.Rotation = Light->GetWorldRotation();
	//	View.Direction = Light->GetWorldFront();
	//	View.U = Light->GetWorldRight();
	//	View.V = Light->GetWorldUp();
	//	View.N = Light->GetWorldFront();
	//
	//	switch (Light->GetLightType())
	//	{
	//		case ZE_LT_DIRECTIONAL:
	//			{
	//				for (ZEUInt I = 0; I < CascadeCount; I++)
	//				{
	//
	//				}
	//			}
	//			break;
	//		case ZE_LT_PROJECTIVE:
	//			{
	//
	//			}
	//			break;
	//		default:
	//			break;
	//	}
	//
	//	ZERNPreRenderParameters PreRenderParameters;
	//	PreRenderParameters.Renderer = &ShadowRenderer;
	//	PreRenderParameters.View = &View;
	//	PreRenderParameters.Type = ZERN_PRT_SHADOW;
	//
	//	ShadowRenderer.StartScene(RenderParameters->Scene->GetConstantBuffer());
	//	RenderParameters->Scene->PreRender(&Parameters);
	//	ShadowRenderer.EndScene();
	//
	//	const ZEGRDepthStencilBuffer* DepthBuffer = DirectionalShadowMaps->GetDepthStencilBuffer(false, CascadeIndex);
	//	Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
	//	Context->SetRenderTargets(0, NULL, DepthBuffer);
	//	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));
	//
	//	ShadowRenderer.Render();
	//}

	return true;
}

ZERNStageShadowing::ZERNStageShadowing()
{
	DirtyFlags.RaiseAll();

	CascadeCount = 4;
	DirectionalShadowMapCount = 1;
	ProjectiveShadowMapCount = 4;
	
	DirectionalShadowMapIndex = 0;
	ProjectiveShadowMapIndex = 0;

	DirectionalShadowMapsResolution = ZE_LSR_HIGH;
	ProjectiveShadowMapsResolution = ZE_LSR_MEDIUM;

	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&DirectionalShadowMaps), "DirectionalShadowMaps", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
	AddOutputResource(reinterpret_cast<ZEHolder<const ZEGRResource>*>(&ProjectiveShadowMaps), "ProjectiveShadowMaps", ZERN_SRUT_WRITE, ZERN_SRCF_CREATE_OWN);
}
