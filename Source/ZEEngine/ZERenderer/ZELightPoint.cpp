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
#include "ZERenderer/ZECamera.h"
#include "ZEMaterialLightPoint.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"

void ZELightPoint::SetColor(const ZEVector3& NewColor)
{
	Changed = true;
	Color = NewColor;
}

const ZEVector3& ZELightPoint::GetColor() const
{
	return Color;
}

void ZELightPoint::SetIntensity(float NewValue)
{
	Changed = true;
	Intensity = NewValue;
}

float ZELightPoint::GetIntensity() const
{
	return Intensity;
}

void ZELightPoint::SetRange(float NewValue)
{
	Changed = true;
	UpdateViewVolume = true;
	Range = NewValue;
}

float ZELightPoint::GetRange() const
{
	return Range;
}

void ZELightPoint::SetAttenuation(const ZEVector3& Attenuation)
{
	Changed = true;
	this->Attenuation = Attenuation;
}

void ZELightPoint::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Changed = true;
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;
}

const ZEVector3& ZELightPoint::GetAttenuation() const
{
	return Attenuation;
}

void ZELightPoint::SetPenumbraSize(float Value)
{
	Changed = true;
	PenumbraSize = Value;
}

float ZELightPoint::GetPenumbraSize() const
{
	return PenumbraSize;
}

void ZELightPoint::SetSlopeScaledBias(float Value)
{
	Changed = true;
	SlopeScaledBias = Value;
}

float ZELightPoint::GetSlopeScaledBias() const
{
	return SlopeScaledBias;
}

void ZELightPoint::SetDepthScaledBias(float Value)
{
	Changed = true;
	DepthScaledBias = Value;
}

float ZELightPoint::GetDepthScaledBias() const
{
	return DepthScaledBias;
}

const ZEViewVolume* ZELightPoint::GetLightVolume()
{
	if (UpdateViewVolume)
	{
 		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
 		UpdateViewVolume = false;
	}

	return &ViewVolume;
}

void ZELightPoint::UpdateMaterial(const ZEDrawParameters* DrawParameters)
{
	if (!Changed)
		return;

	if (DrawParameters->View->Type != ZE_VT_CAMERA)
		return;

	ZECamera* Camera = DrawParameters->View->Camera;

	ZEMaterialLightPoint::Transformations* Transforms = NULL;
	Material->LightTransformations->Lock((void**)&Transforms);

		Transforms->WorldViewMatrix = Camera->GetViewTransform() * GetWorldTransform();
		Transforms->WorldViewProjMatrix = Camera->GetViewProjectionTransform() * GetWorldTransform();

	Material->LightTransformations->Unlock();

	ZEMaterialLightPoint::Properties* Properties = NULL;
	Material->LightProperties->Lock((void**)&Properties);

		Properties->Range = Range;
		Properties->Color = Color;
		Properties->Intensity = Intensity;
		Properties->Attenuation = Attenuation;
		Properties->PenumbraSize = PenumbraSize;
		Properties->DepthScaledBias = DepthScaledBias;
		Properties->SlopeScaledBias = SlopeScaledBias;
		Properties->PixelSize = DrawParameters->RenderTarget->GetPixelSize().ToVector2();
		Properties->ViewSpacePosition = Camera->GetViewTransform() * GetWorldPosition();

	Material->LightProperties->Unlock();

	Changed = false;
}

void ZELightPoint::Tick(float Time)
{	

}

void ZELightPoint::Draw(ZEDrawParameters* DrawParameters)
{
	UpdateMaterial(DrawParameters);

	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Radius = Range;
	LightBoundingSphere.Position = GetWorldPosition();
	
	if (!DrawParameters->ViewVolume->CullTest(LightBoundingSphere))
		DrawParameters->Renderer->AddRenderCommand(&RenderCommand);

	ZELight::Draw(DrawParameters);
}

bool ZELightPoint::InitializeSelf()
{
	if (!ZELight::InitializeSelf())
		return false;

	// Vertex buffer
	ZECanvas Canvas;
	Canvas.AddSphere(1.0f, 12, 12);
	Vertices = Canvas.CreateStaticVertexBuffer();

	// Vertex layout
	static const ZEVertexElement Element[] = 
	{
		{"POSITION", 0, ZE_VET_FLOAT4, 0, 0, ZE_VU_PER_VERTEX, 0}
	};
	VertexLayout.SetLayout(Element, 1);

	// Material
	Material = ZEMaterialLightPoint::CreateInstance();

	// Render command
	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = Material;
	RenderCommand.PrimitiveCount = 312;
	RenderCommand.VertexLayout = &VertexLayout;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexBuffers[0] = Vertices;

	return true;
}

bool ZELightPoint::DeinitializeSelf()
{
	ZE_DESTROY(Material);
	ZE_DESTROY(Vertices);

	return ZELight::DeinitializeSelf();
}

ZELightPoint::ZELightPoint() : ZELight(ZE_LT_POINT)
{
	Material = NULL;
	Vertices = NULL;

	Range = 20.0f;
	Intensity = 1.0f;
	Color = ZEVector3::One;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);

	PenumbraSize = 1.0f;
	SlopeScaledBias = 0.0f;
	DepthScaledBias = 0.0f;
}

ZELightPoint::~ZELightPoint()
{
	Deinitialize();
}

ZELightPoint* ZELightPoint::CreateInstance()
{
	return new ZELightPoint();
}
