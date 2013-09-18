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
#include "ZELight.h"
#include "ZEMath/ZERay.h"
#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGraphicsModule.h"

const ZEMatrix4x4& ZECamera::GetViewTransform() const
{
	if (!DirtyViewTransform)
		return ViewTransform;
	
	ZEMatrix4x4::CreateViewTransform(ViewTransform, GetWorldPosition(), GetWorldRotation())	;

	DirtyViewTransform = false;
	
	return ViewTransform;
}

const ZEMatrix4x4& ZECamera::GetProjectionTransform() const
{
	if (!DirtyProjectionTransform)
		return ProjectionTransform;
	
	float AspectRatio = ViewDimension.x / ViewDimension.y;
	ZEMatrix4x4::CreatePerspectiveProjection(ProjectionTransform, Fov, AspectRatio, NearZ, FarZ);
	
	DirtyProjectionTransform = false;

	return ProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform() const
{	
	if (!DirtyViewProjectionTransform)
		return ViewProjectionTransform;

	ViewProjectionTransform = GetProjectionTransform() * GetViewTransform();

	return ViewProjectionTransform;
}

void ZECamera::SetShadowFadeDistance(float Value)
{
	ShadowFadeDistance = Value;
}

float ZECamera::GetShadowFadeDistance() const
{
	return ShadowFadeDistance;
}
		
void ZECamera::SetVisibleShadowDistance(float Value)
{
	VisibleShadowDistance = Value;
}

float ZECamera::GetVisibleShadowDistance() const
{
	return VisibleShadowDistance;
}

ZEViewType ZECamera::GetViewType() const
{
	return ZE_VT_CAMERA;
}

const ZEViewVolume* ZECamera::GetViewVolume() const
{
	if (!DirtyViewVolume)
		return &ViewVolume;

	float AspectRatio = ViewDimension.x / ViewDimension.y;
	ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), Fov, AspectRatio, NearZ, FarZ);

	DirtyViewVolume = false;

	return &ViewVolume;
}

void ZECamera::GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY)
{
	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / (ZEUInt)ViewDimension.x) - 1) / ProjMatrix.M11;
	V.y = -(((2.0f * ScreenY ) / (ZEUInt)ViewDimension.y) - 1) / ProjMatrix.M22;
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
	VisibleShadowDistance = 200.0f;
	ShadowFadeDistance = VisibleShadowDistance * 0.1f;

	ViewTransform = ZEMatrix4x4::Identity;
	ProjectionTransform = ZEMatrix4x4::Identity;
	ViewProjectionTransform = ZEMatrix4x4::Identity;
}

ZECamera::~ZECamera()
{

}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}
