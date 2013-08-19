//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECamera.cpp
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
#include "ZEGraphicsModule.h"
#include "ZEError.h"
#include "ZEMath/ZERay.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"

#define ZE_CDF_NONE								0
#define ZE_CDF_VIEW								1
#define ZE_CDF_VIEW_FRUSTUM						2
#define ZE_CDF_VIEW_TRANSFORM					4
#define ZE_CDF_PROJECTION_TRANSFORM				8
#define ZE_CDF_VIEW_PROJECTION_TRANSFORM		16
#define ZE_CDF_ALL								0xFFFFFFFF

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZECamera)

const ZEMatrix4x4& ZECamera::GetViewTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_TRANSFORM);
	}
	
	return ViewTransform;
}

const ZEMatrix4x4& ZECamera::GetProjectionTransform()
{
	UpdateAutoParameters();
	
	if (CameraDirtyFlags.GetFlags(ZE_CDF_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, VerticalFOV, EffectiveAspectRatio, EffectiveNearZ, EffectiveFarZ);
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_PROJECTION_TRANSFORM);
	}

	return ProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(ViewProjectionTransform, GetProjectionTransform(), GetViewTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}

	return ViewProjectionTransform;
}

void ZECamera::SetPosition(const ZEVector3& NewPosition)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_PROJECTION_TRANSFORM);
	ZEEntity::SetPosition(NewPosition);
}	

void ZECamera::SetRotation(const ZEQuaternion& NewRotation)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_PROJECTION_TRANSFORM);
	ZEEntity::SetRotation(NewRotation);
}

void ZECamera::OnTransformChanged()
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_PROJECTION_TRANSFORM);
	ZEEntity::OnTransformChanged();
}

void ZECamera::UpdateAutoParameters()
{
	if (AutoAspectRatio && EffectiveAspectRatio != zeGraphics->GetAspectRatio())
	{
		EffectiveAspectRatio = zeGraphics->GetAspectRatio();
		CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	}
			
	if (AutoZ && (EffectiveNearZ != zeGraphics->GetNearZ() || EffectiveFarZ != zeGraphics->GetFarZ()))
	{
		EffectiveNearZ = zeGraphics->GetNearZ();
		EffectiveFarZ = zeGraphics->GetFarZ();
		CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	}
}

void ZECamera::SetAutoZ(bool Enabled)
{
	AutoZ = Enabled;

	if (!Enabled)
	{
		EffectiveNearZ = NearZ;
		EffectiveFarZ = FarZ;
	}
}

bool ZECamera::GetAutoZ()
{
	return AutoZ;
}

void ZECamera::SetNearZ(float NearZ)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	this->NearZ = NearZ;

	if (!AutoZ)
		this->EffectiveNearZ = NearZ;
}

float ZECamera::GetNearZ() const
{
	return NearZ;
}

void ZECamera::SetFarZ(float FarZ)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	this->FarZ = FarZ;

	if (!AutoZ)
		this->EffectiveFarZ = FarZ;
}

float ZECamera::GetFarZ() const
{
	return FarZ;
}

void ZECamera::SetHorizontalFOV(float FOV)
{
	UpdateAutoParameters();

	SetVerticalFOV(2.0f * ZEAngle::ArcTan(ZEAngle::Tan(FOV * 0.5f) / EffectiveAspectRatio));
}

float ZECamera::GetHorizontalFOV() const
{
	return 2.0f * ZEAngle::ArcTan(ZEAngle::Tan(VerticalFOV * 0.5f) * EffectiveAspectRatio);
}

void ZECamera::SetVerticalFOV(float FOV)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);

	this->VerticalFOV = FOV;
}

float ZECamera::GetVerticalFOV() const
{
	return VerticalFOV;
}

void ZECamera::SetAutoAspectRatio(bool Enabled)
{
	AutoAspectRatio = Enabled;

	if (!Enabled)
		EffectiveAspectRatio = AspectRatio;
}

bool ZECamera::GetAutoAspectRatio()
{
	return AutoAspectRatio;
}

void ZECamera::SetAspectRatio(float AspectRatio)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);

	this->AspectRatio = AspectRatio;

	if (!AutoAspectRatio)
		this->EffectiveAspectRatio = AspectRatio;
}

float ZECamera::GetAspectRatio() const
{
	return AspectRatio;
}

void ZECamera::SetShadowDistance(float Value)
{
	ShadowDistance = Value;
}

float ZECamera::GetShadowDistance() const
{
	return ShadowDistance;
}

void ZECamera::SetShadowFadeDistance(float Value)
{
	ShadowFadeDistance = Value;
}

float ZECamera::GetShadowFadeDistance() const
{
	return ShadowFadeDistance;
}

const ZEView& ZECamera::GetView()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW))
	{
		View.Type = ZE_VPT_CAMERA;
		View.Camera = this;
		View.FOV = GetHorizontalFOV();
		View.ProjectionTransform = GetProjectionTransform();
		View.ViewTransform = GetViewTransform();
		View.ViewProjectionTransform = GetViewProjectionTransform();
		View.Rotation = GetWorldRotation();
		View.Direction = GetWorldFront();

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW);
	}

	return View;
}

const ZEViewVolume& ZECamera::GetViewVolume()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_FRUSTUM))
	{
		ViewFrustum.Create(GetWorldPosition(), GetWorldRotation(), VerticalFOV, EffectiveAspectRatio, EffectiveNearZ, EffectiveFarZ);
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_FRUSTUM);
	}

	return ViewFrustum;
}

void ZECamera::GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY)
{
	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / zeGraphics->GetScreenWidth()) - 1) / ProjMatrix.M11; //ScreenWidthden Camera ViewPort Size a alınmalı
	V.y = -(((2.0f * ScreenY ) / zeGraphics->GetScreenHeight()) - 1) / ProjMatrix.M22;
	V.z =  1.0f;

	ZEMatrix4x4 InvViewMatrix;
	ZEMatrix4x4::Inverse(InvViewMatrix, GetViewTransform());

	ZEMatrix4x4::Transform3x3(Ray.v, InvViewMatrix, V);

	Ray.p.x = InvViewMatrix.M14;
	Ray.p.y = InvViewMatrix.M24;
	Ray.p.z = InvViewMatrix.M34; 
	ZEVector3::Normalize(Ray.v, Ray.v);
}

ZECamera::ZECamera()
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_VIEW | ZE_CDF_VIEW_FRUSTUM | ZE_CDF_VIEW_TRANSFORM | ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	
	AutoZ = true;
	AutoAspectRatio = true;

	FarZ = zeGraphics->GetFarZ();
	EffectiveFarZ = FarZ;
	NearZ = zeGraphics->GetNearZ();
	EffectiveNearZ = NearZ;
	AspectRatio = zeGraphics->GetAspectRatio();
	EffectiveAspectRatio = AspectRatio;
	SetHorizontalFOV(ZE_PI_2);
	
	ShadowDistance = 1000.0f;
	ShadowFadeDistance = ShadowDistance * 0.1f;
}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}
