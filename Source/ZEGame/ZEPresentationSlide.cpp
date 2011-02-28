//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPresentationSlide.cpp
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

#include "ZEPresentationSlide.h"
#include "ZECore\ZEError.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZERenderer.h"
#include "ZEGraphics\ZERenderOrder.h"
#include "ZEGraphics\ZEVertexBuffer.h"
#include "ZEGraphics\ZEFixedMaterial.h"
#include "ZEGraphics\ZETexture2DResource.h"
#include <string.h>

#include "ZEMath/ZEMathDefinitions.h"

ZEDWORD ZEPresentationSlide::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEPresentationSlide::SetPresentationSlide(const char* FileName)
{
	strcpy(PresentationSlide, FileName);
	if (Initialized)
	{
		if (Texture != NULL)
		{
			Texture->Release();
			Texture = NULL;
		}

		if (strlen(PresentationSlide) != 0)
			Texture = ZETexture2DResource::LoadSharedResource(PresentationSlide);

		if (Texture != NULL)
			Material->SetBaseMap(Texture->GetTexture());
	}
}

const char* ZEPresentationSlide::GetPresentationSlide() const
{
	return PresentationSlide;
}

void ZEPresentationSlide::Draw(ZEDrawParameters* DrawParameters)
{
	if (VertexBuffer != NULL)
	{
		RenderOrder.Lights.SetCount(DrawParameters->Lights.GetCount());
		for (size_t I = 0; I < DrawParameters->Lights.GetCount(); I++)
			RenderOrder.Lights[I] = DrawParameters->Lights[I];

		RenderOrder.VertexBuffer = VertexBuffer;
		RenderOrder.Material = Material;
		RenderOrder.WorldMatrix = GetWorldTransform();
		DrawParameters->Renderer->AddToRenderList(&RenderOrder);
	}
}

bool ZEPresentationSlide::Initialize()
{
	ZECanvas Canvas;
	Canvas.SetRotation(ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
	Canvas.AddPlane(1.0f, 1.0f);
	
	if (VertexBuffer != NULL)
		VertexBuffer->Destroy();

	VertexBuffer = Canvas.CreateStaticVertexBuffer();


	if (Material != NULL)
		Material->Destroy();

	Material = ZEFixedMaterial::CreateInstance();
	Material->SetZero();
	Material->SetTwoSided(true);
	Material->SetAmbientEnabled(true);
	Material->SetAmbientColor(ZEVector3(1.0f, 1.0f, 1.0f));
	Material->SetDiffuseEnabled(true);
	Material->SetDiffuseColor(ZEVector3(1.0f, 1.0f, 1.0f));

	if (Texture != NULL)
	{
		Texture->Release();
		Texture = NULL;
	}

	if (strlen(PresentationSlide) != 0)
		Texture = ZETexture2DResource::LoadSharedResource(PresentationSlide);

	if (Texture != NULL)
		Material->SetBaseMap(Texture->GetTexture());

	Initialized = true;

	return true;
}

void ZEPresentationSlide::Deinitialize()
{
	if (VertexBuffer != NULL)
		VertexBuffer->Destroy();

	if (Material != NULL)
		Material->Destroy();
	
	Initialized = false;
}

void ZEPresentationSlide::Tick(float ElapsedTime)
{
	if (Material != NULL)
		Material->AdvanceAnimation(ElapsedTime);
}

ZEPresentationSlide::ZEPresentationSlide()
{
	Initialized = false;
	PresentationSlide[0] = '\0';
	VertexBuffer = NULL;
	Texture = NULL;
	Material = NULL;
	
	SetBoundingVolumeMechanism(ZE_BVM_USE_LOCAL_ONLY);
	
	RenderOrder.SetZero();
	RenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.PrimitiveCount = 2;
}

ZEPresentationSlide::~ZEPresentationSlide()
{
	Deinitialize();
}

ZEPresentationSlide* ZEPresentationSlide::CreateInstance()
{
	return new ZEPresentationSlide();
}

#include "ZEPresentationSlide.h.zpp"

ZEEntityRunAt ZEPresentationSlideDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}



