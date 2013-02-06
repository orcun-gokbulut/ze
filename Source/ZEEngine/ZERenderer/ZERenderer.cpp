//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderer.cpp
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

#include "ZECamera.h"
#include "ZEMaterial.h"
#include "ZERenderer.h"
#include "ZEPointLight.h"
#include "ZERenderCommand.h"
#include "ZEProjectiveLight.h"
#include "ZEDirectionalLight.h"
#include "ZEOmniProjectiveLight.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsEventTracer.h"

#include "ZERenderStage.h"
#include "ZERenderStageForward.h"
#include "ZERenderStageLighting.h"
#include "ZERenderStageGeometry.h"
#include "ZERenderStageTransparent.h"
#include "ZERenderStagePostProcess.h"

static ZEInt RenderCommandCompare(const ZERenderCommand* A, const ZERenderCommand* B)
{
	if (A->Priority > B->Priority)
	{
		return 1;
	}
	else if (A->Priority < B->Priority)
	{
		return -1;
	}
	else
	{
		if (A->Order > B->Order)
		{
			return 1;
		}
		else if (A->Order < B->Order)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

static ZEInt LightCompare(ZELight* const* A, ZELight* const* B)
{
	if ((*A)->GetLightType() > (*B)->GetLightType())
	{
		return 1;
	}
	else if ((*A)->GetLightType() < (*B)->GetLightType())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

bool ZERenderer::Initialize() 
{
	LightList.Clear();
	CommandList.Clear();
	
	GeometryStage = new ZERenderStageGeometry();
	
	LightingStage = new ZERenderStageLighting();
	LightingStage->SetGBufferInput(GeometryStage);
	LightingStage->SetLightList(&LightList);

	ForwardStage = new ZERenderStageForward();
	ForwardStage->SetGBufferInput(GeometryStage);
	ForwardStage->SetLBufferInput(LightingStage);

	return true;
}

void ZERenderer::Deinitialize()
{
	ZE_DESTROY(ForwardStage);
	ZE_DESTROY(LightingStage);
	ZE_DESTROY(GeometryStage);
	ZE_DESTROY(TransparentStage);
	ZE_DESTROY(PostProcessStage);
}

void ZERenderer::Destroy()
{
	delete this;
}

void ZERenderer::SetCamera(ZECamera* Camera)
{
	this->Camera = Camera;
}

ZECamera* ZERenderer::GetCamera() const
{
	return Camera;
}

void ZERenderer::SetShadowMapDimension(ZEVector2 Value)
{
	ShadowMapDimesion = Value;
}

ZEVector2 ZERenderer::GetShadowMapDimension() const
{
	return ShadowMapDimesion;
}

void ZERenderer::AddToLightList(ZELight* Light)
{
	LightList.Add(Light);
}

void ZERenderer::ClearLightList()
{
	LightList.Clear();
}

void ZERenderer::AddToRenderList(ZERenderCommand* RenderCommand)
{
	CommandList.Add(*RenderCommand);
}

void ZERenderer::ClearRenderList()
{
	CommandList.Clear(true);
}

void ZERenderer::Render(float ElaspedTime)
{
	if (!zeGraphics->GetEnabled())
	{
		zeWarning("Cannot render while module is disabled.");
		return;
	}

	if (Camera == NULL)
	{
		zeWarning("Cannot render without a camera.");
		return;
	}

	// Sort commands
	LightList.Sort(LightCompare);
	CommandList.Sort(RenderCommandCompare);
	
	ZEGraphicsEventTracer* Tracer = ZEGraphicsEventTracer::GetInstance();

	// Shadow stage
	// -----------------------------------------------
	// 	Tracer->StartEvent("Shadow Stage");
	// 
	// 	ForwardStage->Setup();
	// 	for (ZESize I = 0; I < CommandList.GetCount(); ++I)
	// 		if (CommandList[I].Material->GetMaterialFlags() & ZE_MTF_FORWARD_PASS)
	// 			ForwardStage->Process(&CommandList[I]);
	// 
	// 	Tracer->EndEvent();

	// Geometry stage
	// -----------------------------------------------
	Tracer->StartEvent("Geometry Stage");
	
	GeometryStage->Setup();
	for (ZESize I = 0; I < CommandList.GetCount(); ++I)
		if (CommandList[I].Material->GetMaterialFlags() & ZE_MTF_G_BUFFER_PASS)
			GeometryStage->Process(&CommandList[I]);

	Tracer->EndEvent();

	// Lighting stage
	// -----------------------------------------------
	Tracer->StartEvent("Lighting Stage");

	LightingStage->Setup();
	LightingStage->Process(NULL);

	Tracer->EndEvent();

	// Forward stage
	// -----------------------------------------------
	Tracer->StartEvent("Forward Stage");
	
	ForwardStage->Setup();
	for (ZESize I = 0; I < CommandList.GetCount(); ++I)
		if (CommandList[I].Material->GetMaterialFlags() & ZE_MTF_FORWARD_PASS)
			ForwardStage->Process(&CommandList[I]);

	Tracer->EndEvent();

	LightList.Clear(false);
	CommandList.Clear(false);

	zeGraphics->GetDevice()->Present();
}

ZERenderer* ZERenderer::CreateInstance()
{
	return new ZERenderer();
}

ZERenderer::ZERenderer()
{
	Camera = NULL;
	
	GeometryStage = NULL;
	LightingStage = NULL;
	ForwardStage = NULL;
	TransparentStage = NULL;
	PostProcessStage = NULL;

	ShadowMapDimesion = ZEVector2(1024.0f, 1024.0f);
}

ZERenderer::~ZERenderer()
{
	Deinitialize();
}
