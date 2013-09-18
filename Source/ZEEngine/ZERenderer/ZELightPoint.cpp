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
	Color = NewColor;
	PropertyChanged = true;
}

const ZEVector3& ZELightPoint::GetColor() const
{
	return Color;
}

void ZELightPoint::SetIntensity(float NewValue)
{
	Intensity = NewValue;
	PropertyChanged = true;
}

float ZELightPoint::GetIntensity() const
{
	return Intensity;
}

void ZELightPoint::SetRange(float NewValue)
{
	Range = NewValue;
	PropertyChanged = true;
}

float ZELightPoint::GetRange() const
{
	return Range;
}

void ZELightPoint::SetAttenuation(const ZEVector3& Attenuation)
{
	this->Attenuation = Attenuation;
	PropertyChanged = true;
}

void ZELightPoint::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;
	PropertyChanged = true;
}

const ZEVector3& ZELightPoint::GetAttenuation() const
{
	return Attenuation;
}

void ZELightPoint::SetPenumbraSize(float Value)
{
	PenumbraSize = Value;
	PropertyChanged = true;
}

float ZELightPoint::GetPenumbraSize() const
{
	return PenumbraSize;
}

void ZELightPoint::SetSlopeScaledBias(float Value)
{
	SlopeScaledBias = Value;
	PropertyChanged = true;
}

float ZELightPoint::GetSlopeScaledBias() const
{
	return SlopeScaledBias;
}

void ZELightPoint::SetDepthScaledBias(float Value)
{
	DepthScaledBias = Value;
	PropertyChanged = true;
}

float ZELightPoint::GetDepthScaledBias() const
{
	return DepthScaledBias;
}

void ZELightPoint::UpdateMaterial(const ZEDrawParameters* DrawParameters)
{
	const ZEView* View = DrawParameters->View;

	if (TransformChanged)
	{
		ZEMaterialLightPoint::VSProperties0* Transformations = NULL;
		Material->LightVSProperties0->Lock((void**)&Transformations);

			ZEMatrix4x4::CreateOrientation(Transformations->WorldTransform, GetWorldPosition(), ZEQuaternion::Identity, ZEVector3(Range, Range, Range));
			
		Material->LightVSProperties0->Unlock();

		TransformChanged = false;
	}

	if (PropertyChanged)
	{
		ZEMaterialLightPoint::PSProperties0* Properties0 = NULL;
		Material->LightPSProperties0->Lock((void**)&Properties0);

			Properties0->Color = Color;
			Properties0->Intensity = Intensity;
			Properties0->Attenuation = Attenuation;

		Material->LightPSProperties0->Unlock();
	
		PropertyChanged = false;
	}

	// Use a dirty flag here to check if view transform is changed ?
	if (true)
	{
		ZEMaterialLightPoint::PSProperties1* Properties1 = NULL;
		Material->LightPSProperties1->Lock((void**)&Properties1);

			Properties1->ViewSpaceCenter = View->GetViewTransform() * GetWorldPosition();

		Material->LightPSProperties1->Unlock();
	}
}

void ZELightPoint::OnTransformChanged()
{
	TransformChanged = true;

	ZELight::OnTransformChanged();	
}

ZELightType ZELightPoint::GetLightType() const
{
	return ZE_LT_POINT;
}

void ZELightPoint::Tick(float Time)
{	

}

void ZELightPoint::Draw(ZEDrawParameters* DrawParameters)
{
	// Draw if only viewed by a camera
	if (DrawParameters->View->GetViewType() != ZE_VT_CAMERA)
		return;

	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Radius = Range;
	LightBoundingSphere.Position = GetWorldPosition();
	
	if (DrawParameters->View->GetViewVolume()->CullTest(LightBoundingSphere))
		return;

	UpdateMaterial(DrawParameters);
	DrawParameters->Bucket->AddRenderCommand(&RenderCommand);

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
		{"POSITION", 0, ZE_VET_FLOAT3, 0, 0, ZE_VU_PER_VERTEX, 0}
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

ZELightPoint::ZELightPoint()
{
	PropertyChanged = true;
	TransformChanged = true;

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
