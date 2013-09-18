//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightProjective.cpp
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

#include "ZEError.h"
#include "ZECanvas.h"
#include "ZERenderer.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGame/ZEScene.h"
#include "ZELightProjective.h"
#include "ZERenderer/ZECamera.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEMaterialLightProjective.h"
#include "ZEGraphics/ZEConstantBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZETexture/ZETexture2DResource.h"

void ZELightProjective::SetFOV(float Value)
{
	FOV = Value;
}

float ZELightProjective::GetFOV() const
{
	return FOV;
}

void ZELightProjective::SetAspectRatio(float Value)
{
	AspectRatio = Value;
}

float ZELightProjective::GetAspectRatio() const
{
	return AspectRatio;
}

void ZELightProjective::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
	PropertyChanged = true;
}

const ZEVector3& ZELightProjective::GetColor() const
{
	return Color;
}

void ZELightProjective::SetIntensity(float NewValue)
{
	Intensity = NewValue;
	PropertyChanged = true;
}

float ZELightProjective::GetIntensity() const
{
	return Intensity;
}

void ZELightProjective::SetRange(float NewValue)
{
	Range = NewValue;
	PropertyChanged = true;
}

float ZELightProjective::GetRange() const
{
	return Range;
}

void ZELightProjective::SetAttenuation(const ZEVector3& Attenuation)
{
	this->Attenuation = Attenuation;
	PropertyChanged = true;
}

void ZELightProjective::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation.x = Constant;
	Attenuation.y = Distance;
	Attenuation.z = DistanceSquare;
	PropertyChanged = true;
}

const ZEVector3& ZELightProjective::GetAttenuation() const
{
	return Attenuation;
}

void ZELightProjective::SetPenumbraSize(float Value)
{
	PenumbraSize = Value;
	PropertyChanged = true;
}

float ZELightProjective::GetPenumbraSize() const
{
	return PenumbraSize;
}

void ZELightProjective::SetSlopeScaledBias(float Value)
{
	SlopeScaledBias = Value;
	PropertyChanged = true;
}

float ZELightProjective::GetSlopeScaledBias() const
{
	return SlopeScaledBias;
}

void ZELightProjective::SetDepthScaledBias(float Value)
{
	DepthScaledBias = Value;
	PropertyChanged = true;
}

float ZELightProjective::GetDepthScaledBias() const
{
	return DepthScaledBias;
}

void ZELightProjective::SetTextureSampler(const ZESamplerState& Sampler)
{
	zeDebugCheck(Material == NULL, "Null material");
	Material->SamplerState = Sampler;
}

const ZESamplerState& ZELightProjective::GetTextureSampler() const
{
	zeDebugCheck(Material == NULL, "Null material");
	return Material->SamplerState;
}

void ZELightProjective::SetProjectionTexture(const ZETexture2D* Texture)
{
	zeDebugCheck(Texture == NULL, "Null Pointer");
	ProjectionTexture = Texture;
}

const ZETexture2D* ZELightProjective::GetProjectionTexture() const
{
	zeDebugCheck(Material == NULL, "Null material");
	return Material->ProjectionTexture;
}

void ZELightProjective::UpdateMaterial(const ZEDrawParameters* DrawParameters)
{
	Material->ProjectionTexture = ProjectionTexture;

	const ZEView* View = DrawParameters->View;

	if (TransformChanged)
	{
		ZEMaterialLightProjective::VSProperties0* Transformations = NULL;
		Material->LightVSProperties0->Lock((void**)&Transformations);
	
			ZEMatrix4x4 WorldTransform;
			float TanFovRange = ZEAngle::Tan(FOV * 0.5f) * Range;
			ZEVector3 Scale(TanFovRange * AspectRatio * 2.0f, TanFovRange * 2.0f, Range);
			ZEMatrix4x4::CreateOrientation(WorldTransform, GetWorldPosition(), GetWorldRotation(), Scale);

			Transformations->WorldMatrix = WorldTransform;

		Material->LightVSProperties0->Unlock();
		TransformChanged = false;
	}

	if (PropertyChanged)
	{
		ZEMaterialLightProjective::PSProperties0* Properties = NULL;
		Material->LightPSProperties0->Lock((void**)&Properties);
	
			Properties->Color = Color;
			Properties->Attenuation = Attenuation;	
			Properties->Intensity = Intensity;
	
		Material->LightPSProperties0->Unlock();
		PropertyChanged = false;
	}

	// Use a dirty flag here to check if view transform is changed ?
	if (true)
	{
		// Projection Transformation
		ZEMatrix4x4 ViewMatrix;
		ZEMatrix4x4 ProjectionMatrix;
		ZEMatrix4x4 TextureMatrix;
		ZEMatrix4x4 InvCameraViewMatrix;
		ZEMatrix4x4::Inverse(InvCameraViewMatrix, View->GetViewTransform());

		ZEMatrix4x4::CreateViewTransform(ViewMatrix, GetWorldPosition(), GetWorldRotation());
		ZEMatrix4x4::CreatePerspectiveProjection(ProjectionMatrix, FOV, AspectRatio, 0.01f, Range);
		ZEMatrix4x4::Create(TextureMatrix,	0.5f,  0.0f, 0.0f, 0.5f,
											0.0f, -0.5f, 0.0f, 0.5f,
											0.0f,  0.0f, 1.0f, 0.0f,
											0.0f,  0.0f, 0.0f, 1.0f);

		ZEMaterialLightProjective::PSProperties1* Properties = NULL;
		Material->LightPSProperties1->Lock((void**)&Properties);

			Properties->ViewSpaceCenter = View->GetViewTransform() * GetWorldPosition();
			Properties->ProjectionMatrix = TextureMatrix * ProjectionMatrix * ViewMatrix * InvCameraViewMatrix;

		Material->LightPSProperties1->Unlock();		
	}
}

void ZELightProjective::OnTransformChanged()
{
	TransformChanged = true;

	ZELight::OnTransformChanged();	
}

bool ZELightProjective::InitializeSelf()
{
	if (!ZELight::InitializeSelf())
		return false;

	// Vertex buffer
	ZECanvas Canvas;
	Canvas.SetRotation(ZEQuaternion(-ZE_PI_2, ZEVector3::UnitX));
	Canvas.SetTranslation(ZEVector3::UnitZ);
	Canvas.AddPyramid(1.0f, 1.0f, 1.0f);
	VertexBuffer = Canvas.CreateStaticVertexBuffer();
	
	// Material
	Material = ZEMaterialLightProjective::CreateInstance();

	// Vertex layout
	static const ZEVertexElement Element[] = 
	{
		{"POSITION", 0, ZE_VET_FLOAT3, 0, 0, ZE_VU_PER_VERTEX, 0}
	};
	VertexLayout.SetLayout(Element, 1);

	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = Material;
	RenderCommand.PrimitiveCount = 6;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexLayout = &VertexLayout;
	RenderCommand.VertexBuffers[0] = VertexBuffer;

	return true;
}

bool ZELightProjective::DeinitializeSelf()
{
	ZE_DESTROY(Material);
	ZE_DESTROY(VertexBuffer);

	return ZELight::DeinitializeSelf();
}

ZELightType ZELightProjective::GetLightType() const
{
	return ZE_LT_PROJECTIVE;
}

void ZELightProjective::Tick(float Time)
{

}

void ZELightProjective::Draw(ZEDrawParameters* DrawParameters)
{
	// Draw if only viewed by a camera
	if (DrawParameters->View->GetViewType() != ZE_VT_CAMERA)
		return;

	if (ProjectionTexture == NULL)
		return;

	ZEBSphere LightBoundingSphere;
	LightBoundingSphere.Radius = GetRange();
	LightBoundingSphere.Position = GetWorldPosition();
	
	if (DrawParameters->View->GetViewVolume()->CullTest(LightBoundingSphere))
		return;

	UpdateMaterial(DrawParameters);
	DrawParameters->Bucket->AddRenderCommand(&RenderCommand);

	ZELight::Draw(DrawParameters);
}

ZELightProjective::ZELightProjective()
{
	PropertyChanged = true;
	TransformChanged = true;

	FOV = ZE_PI_4;
	AspectRatio = 1.0f;
	
	Range = 20.0f;
	Intensity = 1.0f;
	Color = ZEVector3::One;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);

	PenumbraSize = 1.0f;
	SlopeScaledBias = 0.0f;
	DepthScaledBias = 0.0f;

	ProjectionTexture = NULL;

	VertexBuffer = NULL;
	Material = NULL;
}

ZELightProjective::~ZELightProjective()
{
	Deinitialize();
}

ZELightProjective* ZELightProjective::CreateInstance()
{
	return new ZELightProjective();
}
