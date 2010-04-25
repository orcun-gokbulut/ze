//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Camera.cpp
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

#include "Camera.h"
#include <math.h>
#include "GraphicsModule.h"
#include "Core/Error.h"
#include "ZEMath/Ray.h"

const ZEMatrix4x4& ZECamera::GetViewTransform()
{
	if (!UpdateViewTransform)
		return ViewTransform;
	
	ZEMatrix4x4 ViewPositionTransform, ViewRotationTransform;

	const ZEVector3& ViewPosition = GetWorldPosition();
	ZEQuaternion ViewRotation;
	ZEQuaternion::Conjugate(ViewRotation, GetWorldRotation());
	ZEMatrix4x4::CreateTranslation(ViewPositionTransform, -ViewPosition.x, -ViewPosition.y, -ViewPosition.z);
	ZEMatrix4x4::CreateRotation(ViewRotationTransform, ViewRotation);
	ZEMatrix4x4::Multiply(ViewTransform, ViewPositionTransform, ViewRotationTransform);
	
	UpdateViewTransform = false;
	return ViewTransform;
}

const ZEMatrix4x4& ZECamera::GetProjectionTransform()
{
	/*if (!UpdateProjectionTransform)
		return ProjectionTransform;*/

	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, FOV, zeGraphics->GetAspectRatio(), zeGraphics->GetNearZ(), zeGraphics->GetFarZ());
	return ProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform()
{
	if (!UpdateViewProjectionTransform)
		return ViewProjectionTransform;

	ZEMatrix4x4::Multiply(ViewProjectionTransform, GetViewTransform(), GetProjectionTransform());
	return ViewProjectionTransform;
}

void ZECamera::SetLocalPosition(const ZEVector3& NewPosition)
{
	UpdateViewFrustum = true;
	UpdateViewTransform = true;
	UpdateViewProjectionTransform = true;
	ZEComponent::SetLocalPosition(NewPosition);
}	

void ZECamera::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	UpdateViewFrustum = true;
	UpdateViewTransform = true;
	UpdateViewProjectionTransform = true;
	ZEComponent::SetLocalRotation(NewRotation);
}

void ZECamera::OwnerWorldTransformChanged()
{
	UpdateViewFrustum = true;
	UpdateViewTransform = true;
	UpdateViewProjectionTransform = true;
	ZEComponent::OwnerWorldTransformChanged();
}

void ZECamera::SetNearZ(float NearZ)
{
	this->NearZ = NearZ;

	UpdateViewFrustum = true;
	UpdateProjectionTransform = true;
	UpdateViewProjectionTransform = true;
}

float ZECamera::GetNearZ()
{
	return NearZ;
}

void ZECamera::SetFarZ(float FarZ)
{
	this->FarZ = FarZ;

	UpdateViewFrustum = true;
	UpdateProjectionTransform = true;
	UpdateViewProjectionTransform = true;
}

float ZECamera::GetFarZ()
{
	return FarZ;
}


void ZECamera::SetFOV(float FOV)
{
	this->FOV = FOV;

	UpdateViewFrustum = true;
	UpdateProjectionTransform = true;
	UpdateViewProjectionTransform = true;
}

float ZECamera::GetFOV()
{
	return FOV;
}

void ZECamera::SetAspectRatio(float AspectRatio)
{
	this->AspectRatio = AspectRatio;

	UpdateViewFrustum = true;
	UpdateProjectionTransform = true;
	UpdateViewProjectionTransform = true;
}

float ZECamera::GetAspectRatio()
{
	return AspectRatio;
}
/*
const ZEViewPoint& ZECamera::GetViewPoint()
{
	if (!UpdateViewPoint)
		return ViewPoint;
	
	ZEMatrix4x4 ViewPositionTransform, ViewRotationTransform;

	const ZEVector3& ViewPosition = GetWorldPosition();
	ZEQuaternion ViewRotation;
	ZEQuaternion::Conjugate(ViewRotation, GetWorldRotation());
	ZEMatrix4x4::CreateTranslation(ViewPositionTransform, -ViewPosition.x, -ViewPosition.y, -ViewPosition.z);
	ZEMatrix4x4::CreateRotation(ViewRotationTransform, ViewRotation);
	ZEMatrix4x4::Multiply(ViewPoint.ViewMatrix, ViewPositionTransform, ViewRotationTransform);
	
	ZEMatrix4x4::CreatePerspectiveProjection(ViewPoint.ProjMatrix, FOV, AspectRatio, zeGraphics->GetNearZ(), zeGraphics->GetFarZ());
	ZEMatrix4x4::Multiply(ViewPoint.ViewProjMatrix, ViewPoint.ViewMatrix, ViewPoint.ProjMatrix);
	ViewPoint.ViewPosition = ViewPosition;

	UpdateViewPoint = false;
	return ViewPoint;
}*/

const ZEViewVolume& ZECamera::GetViewVolume()
{
	if (!UpdateViewFrustum)
		return ViewFrustum;

	ViewFrustum.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, NearZ, FarZ);

	return ViewFrustum;
}

void ZECamera::GetScreenRay(ZERay& Ray, int ScreenX, int ScreenY)
{
	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / zeGraphics->GetScreenWidth()) - 1) / ProjMatrix.M11;
	V.y = -(((2.0f * ScreenY ) / zeGraphics->GetScreenHeight()) - 1) / ProjMatrix.M22;
	V.z =  1.0f;

	ZEMatrix4x4 InvViewMatrix;
	ZEMatrix4x4::Inverse(InvViewMatrix, GetViewTransform());

	ZEMatrix4x4::Transform3x3(Ray.v, InvViewMatrix, V);

	Ray.p.x = InvViewMatrix.M41;
	Ray.p.y = InvViewMatrix.M42;
	Ray.p.z = InvViewMatrix.M43; 
	ZEVector3::Normalize(Ray.v, Ray.v);
}

ZECamera::ZECamera()
{
	UpdateViewPoint = true;
	UpdateViewFrustum = true;
	UpdateViewTransform = true;
	UpdateViewProjectionTransform = true;
	UpdateProjectionTransform = true;
}

