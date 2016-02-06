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
#include "ZEError.h"

#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZERay.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

#define ZE_CDF_NONE								0
#define ZE_CDF_VIEW								1
#define ZE_CDF_VIEW_FRUSTUM						2
#define ZE_CDF_VIEW_TRANSFORM					4
#define ZE_CDF_PROJECTION_TRANSFORM				8
#define ZE_CDF_VIEW_PROJECTION_TRANSFORM		16
#define ZE_CDF_INV_VIEW_TRANSFORM				32
#define ZE_CDF_INV_PROJECTION_TRANSFORM			64
#define ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM	128
#define ZE_CDF_ALL								0xFFFFFFFF

const ZEMatrix4x4& ZECamera::GetViewTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(View.ViewTransform, GetWorldPosition(), GetWorldRotation());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_TRANSFORM);
		CameraDirtyFlags.RaiseFlags(ZE_CDF_INV_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM | ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}
	
	return View.ViewTransform;
}

const ZEMatrix4x4& ZECamera::GetProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_PROJECTION_TRANSFORM))
	{
		switch (View.ProjectionType)
		{
			case ZERN_PT_PERSPECTIVE:
				ZEMatrix4x4::CreatePerspectiveProjection(View.ProjectionTransform, View.VerticalFOV, View.AspectRatio, View.NearZ, View.FarZ);
				break;

			case ZERN_PT_PERSPECTIVE_OFFCENTER:
			{
				float Top = View.NearZ * ZEAngle::Tan(GetVerticalFovTop());
				float Bottom = -View.NearZ * ZEAngle::Tan(GetVerticalFovBottom());
				float Right = View.NearZ * ZEAngle::Tan(GetHorizontalFovRight());
				float Left = -View.NearZ * ZEAngle::Tan(GetHorizontalFovLeft());

				ZEMatrix4x4::CreatePerspectiveProjectionOffCenter(View.ProjectionTransform, Left, Right, Bottom, Top, View.NearZ, View.FarZ);
				break;
			}


			case ZERN_PT_ORTHOGONAL:
				break;

			case ZERN_PT_ORTHOGONAL_OFFCENTER:
				break;

		}

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_PROJECTION_TRANSFORM);
		CameraDirtyFlags.RaiseFlags(ZE_CDF_INV_PROJECTION_TRANSFORM | ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM | ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}

	return View.ProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(View.ViewProjectionTransform, GetProjectionTransform(), GetViewTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}

	return View.ViewProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetInvViewTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_INV_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(View.InvViewTransform, GetViewTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_INV_VIEW_TRANSFORM);
	}

	return View.InvViewTransform;
}

const ZEMatrix4x4& ZECamera::GetInvProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_INV_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Inverse(View.InvProjectionTransform, GetProjectionTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_INV_PROJECTION_TRANSFORM);
	}

	return View.InvProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetInvViewProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(View.InvViewProjectionTransform, GetInvViewTransform(), GetInvProjectionTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM);
	}

	return View.InvViewProjectionTransform;
}

void ZECamera::SetPosition(const ZEVector3& NewPosition)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_INV_PROJECTION_TRANSFORM));
	ZEEntity::SetPosition(NewPosition);
}	

void ZECamera::SetRotation(const ZEQuaternion& NewRotation)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_INV_PROJECTION_TRANSFORM));
	ZEEntity::SetRotation(NewRotation);
}

void ZECamera::OnTransformChanged()
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_INV_PROJECTION_TRANSFORM));
	ZEEntity::OnTransformChanged();
}

void ZECamera::SetViewport(const ZEGRViewport& Viewport)
{
	if (this->Viewport.GetWidth() != Viewport.GetWidth() ||	this->Viewport.GetHeight() != Viewport.GetHeight())
	{
		this->Viewport = Viewport;
		CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
		if (AutoAspectRatio)
			View.AspectRatio = Viewport.GetWidth() / Viewport.GetHeight();
	}
}

const ZEGRViewport& ZECamera::GetViewport() const
{
	return Viewport;
}

void ZECamera::SetNearZ(float NearZ)
{
	if (View.NearZ == NearZ)
		return;

	View.NearZ = NearZ;
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
}

float ZECamera::GetNearZ() const
{
	return View.NearZ;
}

void ZECamera::SetFarZ(float FarZ)
{
	if (View.FarZ == FarZ)
		return;

	View.FarZ = FarZ;
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
}

float ZECamera::GetFarZ() const
{
	return View.FarZ;
}

void ZECamera::SetHorizontalFOV(float FOV)
{
	if (View.HorizontalFOV == FOV)
		return;

	View.HorizontalFOV = FOV;
	SetVerticalFOV(2.0f * ZEAngle::ArcTan(ZEAngle::Tan(FOV * 0.5f) / View.AspectRatio));
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
}

float ZECamera::GetHorizontalFOV() const
{
	return 2.0f * ZEAngle::ArcTan(ZEAngle::Tan(View.VerticalFOV * 0.5f) * View.AspectRatio);
}

void ZECamera::SetVerticalFOV(float FOV)
{
	if (View.VerticalFOV == FOV)
		return;

	View.VerticalFOV = FOV;
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
}

float ZECamera::GetVerticalFOV() const
{
	return View.VerticalFOV;
}

void ZECamera::SetAutoAspectRatio(bool Enabled)
{
	if (AutoAspectRatio == Enabled)
		return;

	AutoAspectRatio = Enabled;
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
}

bool ZECamera::GetAutoAspectRatio() const
{
	return AutoAspectRatio;
}

void ZECamera::SetAspectRatio(float AspectRatio)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~(ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM));
	View.AspectRatio = AspectRatio;
}

float ZECamera::GetAspectRatio() const
{
	if (AutoAspectRatio)
		View.AspectRatio = (float)Viewport.GetWidth() / (float)Viewport.GetHeight();

	return View.AspectRatio;
}

void ZECamera::SetShadowDistance(float Value)
{
	if(View.ShadowDistance == Value)
		return;

	View.ShadowDistance = Value;

	CameraDirtyFlags.RaiseFlags(ZE_CDF_VIEW);
}

float ZECamera::GetShadowDistance() const
{
	return View.ShadowDistance;
}

void ZECamera::SetShadowFadeDistance(float Value)
{
	if(View.ShadowFadeDistance == Value)
		return;

	View.ShadowFadeDistance = Value;

	CameraDirtyFlags.RaiseFlags(ZE_CDF_VIEW);
}

float ZECamera::GetShadowFadeDistance() const
{
	return View.ShadowFadeDistance;
}

void ZECamera::SetProjectionType(ZERNProjectionType ProjectionType)
{
	if(View.ProjectionType == ProjectionType)
		return;

	View.ProjectionType = ProjectionType;

	CameraDirtyFlags.RaiseFlags(ZE_CDF_PROJECTION_TRANSFORM);
}

ZERNProjectionType ZECamera::GetProjectionType() const
{
	return View.ProjectionType;
}

void ZECamera::SetVerticalFovTop(float VerticalFovTop)
{
	View.VerticalFOVTop = VerticalFovTop;
}

float ZECamera::GetVerticalFovTop() const
{
	return View.VerticalFOVTop;
}

void ZECamera::SetVerticalFovBottom(float VerticalFovBottom)
{
	View.VerticalFOVBottom = VerticalFovBottom;
}

float ZECamera::GetVerticalFovBottom() const
{
	return View.VerticalFOVBottom;
}

void ZECamera::SetHorizontalFovRight(float HorizontalFovRight)
{
	View.HorizontalFOVRight = HorizontalFovRight;
}

float ZECamera::GetHorizontalFovRight() const
{
	return View.HorizontalFOVRight;
}

void ZECamera::SetHorizontalFovLeft(float HorizontalFovLeft)
{
	View.HorizontalFOVLeft = HorizontalFovLeft;
}

float ZECamera::GetHorizontalFovLeft() const
{
	return View.HorizontalFOVLeft;
}

const ZERNView& ZECamera::GetView()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW))
	{
		View.Position = GetWorldPosition();
		View.Rotation = GetWorldRotation();
		View.Direction = GetWorldFront();
		View.U = GetWorldRight();
		View.V = GetWorldUp();
		View.N = GetWorldFront();

		View.VerticalFOV = GetVerticalFOV();
		View.HorizontalFOV = GetHorizontalFOV();
		View.AspectRatio = GetAspectRatio();

		GetViewTransform();
		GetProjectionTransform();
		GetViewProjectionTransform();
		GetInvViewTransform();
		GetInvProjectionTransform();
		GetInvViewProjectionTransform();

		View.NearZ = GetNearZ();
		View.FarZ = GetFarZ();

		View.ProjectionType = GetProjectionType();
		View.Type = ZERN_VT_CAMERA;
		View.Viewport = &GetViewport();
		View.ViewVolume = &GetViewVolume();

		View.ShadowDistance = GetShadowDistance();
		View.ShadowFadeDistance = GetShadowFadeDistance();

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW);
	}

	return View;
}

const ZEViewVolume& ZECamera::GetViewVolume()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_FRUSTUM))
	{
		ViewFrustum.Create(GetWorldPosition(), GetWorldRotation(), GetVerticalFOV(), GetAspectRatio(), GetNearZ(), GetFarZ());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_FRUSTUM);
	}

	return ViewFrustum;
}

void ZECamera::GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY)
{
	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / (ZEInt)View.Viewport->GetWidth()) - 1) / ProjMatrix.M11;
	V.y = -(((2.0f * ScreenY ) / (ZEInt)View.Viewport->GetHeight()) - 1) / ProjMatrix.M22;
	V.z =  1.0f;

	const ZEMatrix4x4& InvViewMatrix = GetInvViewTransform();
	ZEMatrix4x4::Transform3x3(Ray.v, InvViewMatrix, V);

	Ray.p.x = InvViewMatrix.M14;
	Ray.p.y = InvViewMatrix.M24;
	Ray.p.z = InvViewMatrix.M34; 

	ZEVector3::Normalize(Ray.v, Ray.v);
}

ZEVector2 ZECamera::GetScreenPosition(const ZEVector3& WorldPosition)
{
	ZEVector4 ClipPosition;
	ZEMatrix4x4::Transform(ClipPosition, GetViewProjectionTransform(), ZEVector4(WorldPosition, 1.0f));
	ClipPosition /= ClipPosition.w;
	
	float HalfWidth = View.Viewport->GetWidth() * 0.5f;
	float HalfHeight = View.Viewport->GetHeight() * 0.5f;

	return ZEVector2(ClipPosition.x * HalfWidth + HalfWidth, ClipPosition.y * -HalfHeight + HalfHeight);
}

ZECamera::ZECamera()
{
	CameraDirtyFlags.RaiseAll();

	View.Type = ZERN_VT_CAMERA;
	View.Entity = this;
	View.FarZ = 1000.0f;
	View.NearZ = 0.1f;
	View.AspectRatio = 1.0f;
	View.Viewport = &Viewport;
	View.ViewVolume = &ViewFrustum;
	View.ProjectionType = ZERN_PT_PERSPECTIVE;
	SetVerticalFOV(ZE_PI_4);
	AutoAspectRatio = true;

	View.ShadowDistance = 100.0f;
	View.ShadowFadeDistance = View.ShadowDistance * 0.1f;
}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}
