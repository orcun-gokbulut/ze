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

#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZERay.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRRenderTarget.h"

#define ZE_CDF_VIEW								0x0001
#define ZE_CDF_VIEW_FRUSTUM						0x0002
#define ZE_CDF_VIEW_TRANSFORM					0x0004
#define ZE_CDF_INV_VIEW_TRANSFORM				0x0008
#define ZE_CDF_PROJECTION_TRANSFORM				0x0010
#define ZE_CDF_INV_PROJECTION_TRANSFORM			0x0020
#define ZE_CDF_VIEW_PROJECTION_TRANSFORM		0x0040
#define ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM	0x0080

void ZECamera::ProjectionTransformChanged()
{
	CameraDirtyFlags.RaiseFlags(
		ZE_CDF_VIEW | ZE_CDF_VIEW_FRUSTUM | 
		ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_INV_PROJECTION_TRANSFORM | 
		ZE_CDF_VIEW_PROJECTION_TRANSFORM | ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM);
}

void ZECamera::LocalTransformChanged()
{
	ZEEntity::LocalTransformChanged();
	CameraDirtyFlags.RaiseFlags(
		ZE_CDF_VIEW | 
		ZE_CDF_VIEW_FRUSTUM | 
		ZE_CDF_VIEW_TRANSFORM | ZE_CDF_INV_VIEW_TRANSFORM | 
		ZE_CDF_VIEW_PROJECTION_TRANSFORM | ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM);
}

void ZECamera::ParentTransformChanged()
{
	ZEEntity::ParentTransformChanged();
	CameraDirtyFlags.RaiseFlags(
		ZE_CDF_VIEW | ZE_CDF_VIEW_TRANSFORM | 
		ZE_CDF_VIEW_FRUSTUM | ZE_CDF_INV_VIEW_TRANSFORM | 
		ZE_CDF_VIEW_PROJECTION_TRANSFORM | ZE_CDF_INV_VIEW_PROJECTION_TRANSFORM);
}


const ZEMatrix4x4& ZECamera::GetViewTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(View.ViewTransform, GetWorldPosition(), GetWorldRotation());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_TRANSFORM);
	}
	
	return View.ViewTransform;
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

const ZEMatrix4x4& ZECamera::GetProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_PROJECTION_TRANSFORM))
	{
		switch (View.ProjectionType)
		{
			case ZERN_PT_PERSPECTIVE:
				ZEMatrix4x4::CreatePerspectiveProjection(View.ProjectionTransform, GetVerticalFOV(), GetAspectRatio(), View.NearZ, View.FarZ);
				break;

			case ZERN_PT_PERSPECTIVE_OFFCENTER:
			{
				float Top = View.NearZ * ZEAngle::Tan(GetTopFOV());
				float Bottom = -View.NearZ * ZEAngle::Tan(GetBottomFOV());
				float Right = View.NearZ * ZEAngle::Tan(GetRightFOV());
				float Left = -View.NearZ * ZEAngle::Tan(GetLeftFOV());

				ZEMatrix4x4::CreatePerspectiveProjectionOffCenter(View.ProjectionTransform, Left, Right, Bottom, Top, View.NearZ, View.FarZ);
				break;
			}

			case ZERN_PT_ORTHOGONAL:
				break;

			case ZERN_PT_ORTHOGONAL_OFFCENTER:
				break;

		}

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_PROJECTION_TRANSFORM);
	}

	return View.ProjectionTransform;
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

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(View.ViewProjectionTransform, GetProjectionTransform(), GetViewTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}

	return View.ViewProjectionTransform;
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

void ZECamera::SetViewport(const ZEGRViewport& Viewport)
{
	if (this->Viewport.GetWidth() == Viewport.GetWidth() && this->Viewport.GetHeight() == Viewport.GetHeight())
		return;

	this->Viewport = Viewport;
	if (AutoAspectRatio)
		View.AspectRatio = Viewport.GetWidth() / Viewport.GetHeight();

	ProjectionTransformChanged();
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
	ProjectionTransformChanged();
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
	ProjectionTransformChanged();
}

float ZECamera::GetFarZ() const
{
	return View.FarZ;
}

void ZECamera::SetHorizontalFOV(float FOV)
{
	if (View.HorizontalFOV == FOV)
		return;

	View.HorizontalFOVRight = View.HorizontalFOVLeft = FOV * 0.5f;

	ProjectionTransformChanged();
}

float ZECamera::GetHorizontalFOV() const
{
	return (View.HorizontalFOVRight + View.HorizontalFOVLeft);
}

void ZECamera::SetVerticalFOV(float FOV)
{
	if (View.VerticalFOV == FOV)
		return;

	View.VerticalFOVTop = View.VerticalFOVBottom = FOV * 0.5f;
	View.HorizontalFOVLeft = View.HorizontalFOVRight = ZEAngle::ArcTan(ZEAngle::Tan(View.VerticalFOVTop) * View.AspectRatio);

	ProjectionTransformChanged();
}

float ZECamera::GetVerticalFOV() const
{
	return (View.VerticalFOVTop + View.VerticalFOVBottom);
}

void ZECamera::SetAutoAspectRatio(bool Enabled)
{
	if (AutoAspectRatio == Enabled)
		return;

	AutoAspectRatio = Enabled;
	ProjectionTransformChanged();
}

bool ZECamera::GetAutoAspectRatio() const
{
	return AutoAspectRatio;
}

void ZECamera::SetAspectRatio(float AspectRatio)
{
	if(View.AspectRatio == AspectRatio)
		return;

	View.AspectRatio = AspectRatio;
	ProjectionTransformChanged();
}

float ZECamera::GetAspectRatio() const
{
	if (AutoAspectRatio)
		View.AspectRatio = Viewport.GetWidth() / Viewport.GetHeight();

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

	ProjectionTransformChanged();
}

ZERNProjectionType ZECamera::GetProjectionType() const
{
	return View.ProjectionType;
}

void ZECamera::SetTopFOV(float VerticalFovTop)
{
	if(View.VerticalFOVTop == VerticalFovTop)
		return;

	View.VerticalFOVTop = VerticalFovTop;
	ProjectionTransformChanged();
}

float ZECamera::GetTopFOV() const
{
	return View.VerticalFOVTop;
}

void ZECamera::SetBottomFOV(float VerticalFovBottom)
{
	if(View.VerticalFOVBottom == VerticalFovBottom)
		return;

	View.VerticalFOVBottom = VerticalFovBottom;
	ProjectionTransformChanged();
}

float ZECamera::GetBottomFOV() const
{
	return View.VerticalFOVBottom;
}

void ZECamera::SetRightFOV(float HorizontalFovRight)
{
	if(View.HorizontalFOVRight == HorizontalFovRight)
		return;

	View.HorizontalFOVRight = HorizontalFovRight;
	ProjectionTransformChanged();
}

float ZECamera::GetRightFOV() const
{
	return View.HorizontalFOVRight;
}

void ZECamera::SetLeftFOV(float HorizontalFovLeft)
{
	if(View.HorizontalFOVLeft == HorizontalFovLeft)
		return;

	View.HorizontalFOVLeft = HorizontalFovLeft;
	ProjectionTransformChanged();
}

float ZECamera::GetLeftFOV() const
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
		if (GetProjectionType() == ZERN_PT_PERSPECTIVE)
		{
			ViewFrustum.Create(GetWorldPosition(), GetWorldRotation(), GetVerticalFOV(), GetAspectRatio(), GetNearZ(), GetFarZ());
		}
		else if (GetProjectionType() == ZERN_PT_PERSPECTIVE_OFFCENTER)
		{
			const ZEMatrix4x4& ProjectionTransform = GetProjectionTransform();
			ZEVector3 Column1 = ZEVector3(ProjectionTransform.M11, ProjectionTransform.M12, ProjectionTransform.M13);
			ZEVector3 Column2 = ZEVector3(ProjectionTransform.M21, ProjectionTransform.M22, ProjectionTransform.M23);
			ZEVector3 Column4 = ZEVector3::UnitZ;

			ZEPlane FrustumPlanes[6];
			FrustumPlanes[0] = ZEPlane(Column4 - Column1, ZEVector3::Zero);						//Right
			FrustumPlanes[1] = ZEPlane(Column4 + Column1, ZEVector3::Zero);						//Left
			FrustumPlanes[2] = ZEPlane(Column4 - Column2, ZEVector3::Zero);						//Top
			FrustumPlanes[3] = ZEPlane(Column4 + Column2, ZEVector3::Zero);						//Bottom
			FrustumPlanes[4] = ZEPlane(ZEVector3::UnitZ, ZEVector3(0.0f, 0.0f, GetNearZ()));	//Near
			FrustumPlanes[5] = ZEPlane(-ZEVector3::UnitZ, ZEVector3(0.0f, 0.0f, GetFarZ()));	//Far

			const ZEMatrix4x4 InverseViewTransform = GetInvViewTransform();
			for (ZEUInt I = 0; I < 6; I++)
			{
				ZEVector3 Normal = FrustumPlanes[I].n;
				ZEVector3 Point = FrustumPlanes[I].p;

				Normal.NormalizeSelf();
				ZEMatrix4x4::Transform3x3(FrustumPlanes[I].n, InverseViewTransform, Normal);
				FrustumPlanes[I].n.NormalizeSelf();
				ZEMatrix4x4::Transform(FrustumPlanes[I].p, InverseViewTransform, Point);
			}

			ViewFrustum.Create(FrustumPlanes[0], FrustumPlanes[3], FrustumPlanes[1], FrustumPlanes[2], FrustumPlanes[5], FrustumPlanes[4]);
		}

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
	View.NearZ = 1.0f;
	View.AspectRatio = 1.333333f;
	View.Viewport = &Viewport;
	View.ViewVolume = &ViewFrustum;
	View.ProjectionType = ZERN_PT_PERSPECTIVE;
	View.ShadowDistance = 100.0f;
	View.ShadowFadeDistance = View.ShadowDistance * 0.1f;

	AutoAspectRatio = true;

	SetVerticalFOV(ZE_PI_4);

	ProjectionTransformChanged();
}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}