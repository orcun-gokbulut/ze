//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightOmniProjective.cpp
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
#include "ZEGame/ZEScene.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGraphics/ZETexture.h"
#include "ZELightOmniProjective.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEMaterialLightOmniProjective.h"

void ZELightOmniProjective::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
	PropertyChanged = true;
}

const ZEVector3& ZELightOmniProjective::GetColor() const
{
	return Color;
}

void ZELightOmniProjective::SetIntensity(float NewValue)
{
	Intensity = NewValue;
	PropertyChanged = true;
}

float ZELightOmniProjective::GetIntensity() const
{
	return Intensity;
}

void ZELightOmniProjective::SetRange(float NewValue)
{
	Range = NewValue;
	PropertyChanged = true;
}

float ZELightOmniProjective::GetRange() const
{
	return Range;
}

void ZELightOmniProjective::SetAttenuation(const ZEVector3& Attenuation)
{
	this->Attenuation = Attenuation;
	PropertyChanged = true;
}

void ZELightOmniProjective::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;
	PropertyChanged = true;
}

const ZEVector3& ZELightOmniProjective::GetAttenuation() const
{
	return Attenuation;
}

void ZELightOmniProjective::SetPenumbraSize(float Value)
{
	PenumbraSize = Value;
	PropertyChanged = true;
}

float ZELightOmniProjective::GetPenumbraSize() const
{
	return PenumbraSize;
}

void ZELightOmniProjective::SetSlopeScaledBias(float Value)
{
	SlopeScaledBias = Value;
	PropertyChanged = true;
}

float ZELightOmniProjective::GetSlopeScaledBias() const
{
	return SlopeScaledBias;
}

void ZELightOmniProjective::SetDepthScaledBias(float Value)
{
	DepthScaledBias = Value;
	PropertyChanged = true;
}

float ZELightOmniProjective::GetDepthScaledBias() const
{
	return DepthScaledBias;
}

void ZELightOmniProjective::SetTextureSampler(const ZESamplerState& Sampler)
{
	zeDebugCheck(Material == NULL, "Null material");
	Material->SamplerState = Sampler;
}

const ZESamplerState& ZELightOmniProjective::GetTextureSampler() const
{
	zeDebugCheck(Material == NULL, "Null material");
	return Material->SamplerState;
}

void ZELightOmniProjective::SetProjectionTexture(const ZETextureCube* Texture)
{
	zeDebugCheck(Texture == NULL, "Null Pointer");
	ProjectionTexture = Texture;
}

const ZETextureCube* ZELightOmniProjective::GetProjectionTexture() const
{
	zeDebugCheck(Material == NULL, "Null material");
	return Material->ProjectionTexture;
}

void ZELightOmniProjective::UpdateMaterial(const ZEDrawParameters* DrawParameters)
{
	Material->ProjectionTexture = ProjectionTexture;

	const ZEView* View = DrawParameters->View;

	if (TransformChanged)
	{
		ZEMatrix4x4 WorldTransform;
		ZEMatrix4x4::CreateOrientation(WorldTransform, GetWorldPosition(), GetWorldRotation(), ZEVector3(Range, Range, Range));

		ZEMaterialLightOmniProjective::VSProperties0* Properties = NULL;
		Material->LightVSProperties0->Lock((void**)&Properties);

			Properties->WorldMatrix = WorldTransform;

		Material->LightVSProperties0->Unlock();
		TransformChanged = false;
	}

	if (PropertyChanged)
	{
		ZEMaterialLightOmniProjective::PSProperties0* Properties = NULL;
		Material->LightPSProperties0->Lock((void**)&Properties);

			Properties->Color = Color;
			Properties->Intensity = Intensity;
			Properties->Attenuation = Attenuation;
			
		Material->LightPSProperties0->Unlock();
		PropertyChanged = false;
	}

	// Use a dirty flag here to check if view transform is changed ?
	if (true)
	{
		// Projection Transform
		ZEMatrix4x4 ProjectionMatrix;
		ZEQuaternion ProjectionRotation;
		
		ZEQuaternion::Product(ProjectionRotation, GetWorldRotation().Conjugate(), View->GetWorldRotation());
		ZEQuaternion::Normalize(ProjectionRotation, ProjectionRotation);
		ZEMatrix4x4::CreateRotation(ProjectionMatrix, ProjectionRotation);

		ZEMaterialLightOmniProjective::PSProperties1* Properties = NULL;
		Material->LightPSProperties1->Lock((void**)&Properties);

			Properties->ProjectionMatrix = ProjectionMatrix;
			Properties->ViewSpaceCenter = View->GetViewTransform() * GetWorldPosition();

		Material->LightPSProperties1->Unlock();
	}
}

void ZELightOmniProjective::OnTransformChanged()
{
	TransformChanged = true;

	ZELight::OnTransformChanged();	
}

bool ZELightOmniProjective::InitializeSelf()
{
	if (!ZELight::InitializeSelf())
		return false;

	// Vertex buffer
	ZECanvas Canvas;
	Canvas.AddSphere(1.0f, 24, 24);
	VertexBuffer = Canvas.CreateStaticVertexBuffer();

	// Material
	Material = ZEMaterialLightOmniProjective::CreateInstance();

	// Vertex layout
	static const ZEVertexElement Element[] = 
	{
		{"POSITION", 0, ZE_VET_FLOAT3, 0, 0, ZE_VU_PER_VERTEX, 0}
	};
	VertexLayout.SetLayout(Element, 1);

	// Render command
	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = Material;
	RenderCommand.PrimitiveCount = 1200;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexLayout = &VertexLayout;
	RenderCommand.VertexBuffers[0] = VertexBuffer;

	return true;
}

bool ZELightOmniProjective::DeinitializeSelf()
{
	ZE_DESTROY(Material);
	ZE_DESTROY(VertexBuffer);

	return ZELight::DeinitializeSelf();
}

ZELightType ZELightOmniProjective::GetLightType() const
{
	return ZE_LT_OMNIPROJECTIVE;
}

void ZELightOmniProjective::Tick(float Time)
{

}

void ZELightOmniProjective::Draw(ZEDrawParameters* DrawParameters)
{
	// Draw if only viewed by a camera
	if (DrawParameters->View->GetViewType() != ZE_VT_CAMERA)
		return;

	if (ProjectionTexture == NULL)
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

ZELightOmniProjective::ZELightOmniProjective()
{
	PropertyChanged = true;
	TransformChanged = true;

	Material = NULL;
	VertexBuffer = NULL;

	Range = 20.0f;
	Intensity = 1.0f;
	Color = ZEVector3::One;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);

	ProjectionTexture = NULL;

	PenumbraSize = 1.0f;
	SlopeScaledBias = 0.0f;
	DepthScaledBias = 0.0f;
}

ZELightOmniProjective::~ZELightOmniProjective()
{
	Deinitialize();
}

ZELightOmniProjective* ZELightOmniProjective::CreateInstance()
{
	return new ZELightOmniProjective();
}
