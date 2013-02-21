//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightPoint.cpp
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

#include "ZECanvas.h"
#include "ZERenderer.h"
#include "ZEMaterial.h"
#include "ZELightPoint.h"
#include "ZEGame/ZEScene.h"
#include "ZEMaterialLightPoint.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZEGraphicsModule.h"

const ZEViewVolume& ZELightPoint::GetViewVolume(ZESize Index)
{
	if (UpdateViewVolume)
	{
 		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
 		UpdateViewVolume = false;
	}
	return ViewVolume;
}

void ZELightPoint::Tick(float Time)
{
	// Update material and render command
	Material->LightCaster = true;
	Material->LightReciever = false;
	Material->ShadowCaster = ShadowCaster;
	Material->ShadowReciver = false;
	Material->Range = Range;
	Material->Color = Color;
	Material->Intensity = Intensity;
	Material->Attenuation = Attenuation;
	Material->WorldPosition = GetWorldPosition();
	
	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.Flags = 0;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = Material;
	RenderCommand.Pipeline = ZE_RP_3D;
	RenderCommand.PrimitiveCount = 312;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexBuffers[0] = Geometry;
}

void ZELightPoint::Draw(ZEDrawParameters* DrawParameters)
{
	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Radius = GetRange();
	LightBoundingSphere.Position = GetWorldPosition();
	
	if (!DrawParameters->ViewVolume->CullTest(LightBoundingSphere))
		DrawParameters->Renderer->AddRenderCommand(&RenderCommand);
}

bool ZELightPoint::Initialize()
{
	if (GetInitialized())
		return true;

	ZECanvas Canvas;
	Canvas.AddSphere(1.0f, 12, 12);
	Geometry = Canvas.CreateStaticVertexBuffer();

	Material = ZEMaterialLightPoint::CreateInstance();

	return ZEEntity::Initialize();
}

void ZELightPoint::Deinitialize()
{
	if (!GetInitialized())
		return;

	ZE_DESTROY(Material);
	ZE_DESTROY(Geometry);
}

ZELightPoint::ZELightPoint()
{
	Type = ZE_LT_POINT;
	Material = NULL;
	Geometry = NULL;
}

ZELightPoint::~ZELightPoint()
{
	Deinitialize();
}

ZELightPoint* ZELightPoint::CreateInstance()
{
	return new ZELightPoint();
}
