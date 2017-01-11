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
#include "ZEGraphics/ZEGRBuffer.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEGraphics/ZEGRDefinitions.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

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



void ZECamera::UpdateAutoAspectRatio()
{
	if (!AutoAspectRatio)
		return;

	View.AspectRatio = View.Viewport.GetWidth() / View.Viewport.GetHeight();

	UpdateAutoFOV();
}

void ZECamera::UpdateAutoFOV()
{
	if (!AutoFOV)
		return;

	SetFOV(GetFOV());

	ProjectionTransformChanged();
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
				float Bottom = View.NearZ * ZEAngle::Tan(GetBottomFOV());
				float Right = View.NearZ * ZEAngle::Tan(GetRightFOV());
				float Left = View.NearZ * ZEAngle::Tan(GetLeftFOV());

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
	if (View.Viewport.GetWidth() == Viewport.GetWidth() && View.Viewport.GetHeight() == Viewport.GetHeight())
		return;

	View.Viewport = Viewport;
	
	UpdateAutoAspectRatio();

	ProjectionTransformChanged();
}

const ZEGRViewport& ZECamera::GetViewport() const
{
	return View.Viewport;
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

void ZECamera::SetFOVAxis(ZECameraFOVAxis Axis)
{
	if (FOVAxis == Axis)
		return;

	FOVAxis = Axis;

	UpdateAutoFOV();
}

ZECameraFOVAxis ZECamera::GetFOVAxis() const
{
	return FOVAxis;
}

void ZECamera::SetFOV(float FOV)
{
	if (FOVAxis == ZE_CFA_VERTICAL)
	{
		View.VerticalFOV = FOV;
		View.VerticalFOVTop = View.VerticalFOV * 0.5f;
		View.VerticalFOVBottom = -View.VerticalFOVTop;

		View.HorizontalFOV = 2.0f * ZEAngle::ArcTan(ZEAngle::Tan(View.VerticalFOV / 2.0f) * GetAspectRatio());
		View.HorizontalFOVRight = View.HorizontalFOV * 0.5f;
		View.HorizontalFOVLeft = -View.HorizontalFOVRight;
	}
	else
	{
		View.HorizontalFOV = FOV;
		View.HorizontalFOVRight = View.HorizontalFOV * 0.5f;
		View.HorizontalFOVLeft = -View.HorizontalFOVRight;

		View.VerticalFOV = 2.0f * ZEAngle::ArcTan(ZEAngle::Tan(View.HorizontalFOV / 2.0f) * (1.0f / GetAspectRatio()));
		View.VerticalFOVTop = View.VerticalFOV * 0.5f;
		View.VerticalFOVBottom = -View.VerticalFOVTop;
	}
	
	ProjectionTransformChanged();
}

float ZECamera::GetFOV() const
{
	if (FOVAxis == ZE_CFA_VERTICAL)
		return View.VerticalFOV;
	else
		return View.HorizontalFOV;
}

void ZECamera::SetAutoFOV(bool Enabled)
{
	if (AutoFOV == Enabled)
		return;

	AutoFOV = Enabled;

	UpdateAutoFOV();
}

bool ZECamera::GetAutoFOV() const
{
	return AutoFOV;
}

void ZECamera::SetHorizontalFOV(float FOV)
{
	if (View.HorizontalFOV == FOV)
		return;

	View.HorizontalFOV = FOV;
	View.HorizontalFOVRight = FOV * 0.5f;
	View.HorizontalFOVLeft = -View.HorizontalFOVRight;

	ProjectionTransformChanged();
}

float ZECamera::GetHorizontalFOV() const
{
	return View.HorizontalFOVRight - View.HorizontalFOVLeft;
}

void ZECamera::SetVerticalFOV(float FOV)
{
	if (View.VerticalFOV == FOV)
		return;

	View.VerticalFOV = FOV;
	View.VerticalFOVTop = FOV * 0.5f;
	View.VerticalFOVBottom = -View.VerticalFOVTop;

	ProjectionTransformChanged();
}

float ZECamera::GetVerticalFOV() const
{
	return View.VerticalFOVTop - View.VerticalFOVBottom;
}

void ZECamera::SetAutoAspectRatio(bool Enabled)
{
	if (AutoAspectRatio == Enabled)
		return;

	AutoAspectRatio = Enabled;

	UpdateAutoAspectRatio();
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

	UpdateAutoFOV();
}

float ZECamera::GetAspectRatio() const
{
	if (AutoAspectRatio)
		View.AspectRatio = View.Viewport.GetWidth() / View.Viewport.GetHeight();

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
	View.VerticalFOV = View.VerticalFOVTop - View.VerticalFOVBottom;

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
	View.VerticalFOV = View.VerticalFOVTop - View.VerticalFOVBottom;

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
	View.HorizontalFOV = View.HorizontalFOVRight - View.HorizontalFOVLeft;

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
	View.HorizontalFOV = View.HorizontalFOVRight - View.HorizontalFOVLeft;

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
			ZEViewFrustum::Create(ViewFrustum, GetWorldPosition(), GetWorldRotation(), GetVerticalFOV(), GetAspectRatio(), GetNearZ(), GetFarZ());
		}
		else if (GetProjectionType() == ZERN_PT_PERSPECTIVE_OFFCENTER)
		{
			const ZEMatrix4x4& ProjectionTransform = GetProjectionTransform();
			ZEVector3 Column1 = ZEVector3(ProjectionTransform.M11, ProjectionTransform.M12, ProjectionTransform.M13);
			ZEVector3 Column2 = ZEVector3(ProjectionTransform.M21, ProjectionTransform.M22, ProjectionTransform.M23);
			ZEVector3 Column4 = ZEVector3::UnitZ;

			ZEPlane FrustumPlanes[6];
			FrustumPlanes[0] = ZEPlane(Column4 + Column1, ZEVector3::Zero);						// Left
			FrustumPlanes[1] = ZEPlane(Column4 - Column1, ZEVector3::Zero);						// Right
			FrustumPlanes[2] = ZEPlane(Column4 + Column2, ZEVector3::Zero);						// Down
			FrustumPlanes[3] = ZEPlane(Column4 - Column2, ZEVector3::Zero);						// Up
			FrustumPlanes[4] = ZEPlane(ZEVector3::UnitZ, ZEVector3(0.0f, 0.0f, GetNearZ()));	// Near
			FrustumPlanes[5] = ZEPlane(-ZEVector3::UnitZ, ZEVector3(0.0f, 0.0f, GetFarZ()));	// Far

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

			ZEFrustum::Create(ViewFrustum, FrustumPlanes[0], FrustumPlanes[1], FrustumPlanes[2], FrustumPlanes[3], FrustumPlanes[4], FrustumPlanes[5]);
		}

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_FRUSTUM);
	}

	return ViewFrustum;
}

ZERay ZECamera::GetScreenRay(ZEInt ScreenX, ZEInt ScreenY)
{
	ZERay Ray;

	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / (ZEInt)View.Viewport.GetWidth()) - 1) / ProjMatrix.M11;
	V.y = -(((2.0f * ScreenY ) / (ZEInt)View.Viewport.GetHeight()) - 1) / ProjMatrix.M22;
	V.z =  1.0f;

	const ZEMatrix4x4& InvViewMatrix = GetInvViewTransform();
	ZEMatrix4x4::Transform3x3(Ray.v, InvViewMatrix, V);

	Ray.p.x = InvViewMatrix.M14;
	Ray.p.y = InvViewMatrix.M24;
	Ray.p.z = InvViewMatrix.M34; 

	ZEVector3::Normalize(Ray.v, Ray.v);
	
	return Ray;
}

ZEVector2 ZECamera::GetScreenPosition(const ZEVector3& WorldPosition)
{
	ZEVector4 ClipPosition;
	ZEMatrix4x4::Transform(ClipPosition, GetViewProjectionTransform(), ZEVector4(WorldPosition, 1.0f));
	ClipPosition /= ClipPosition.w;
	
	float HalfWidth = View.Viewport.GetWidth() * 0.5f;
	float HalfHeight = View.Viewport.GetHeight() * 0.5f;

	return ZEVector2(ClipPosition.x * HalfWidth + HalfWidth, ClipPosition.y * -HalfHeight + HalfHeight);
}

ZECamera::ZECamera()
{
	CameraDirtyFlags.RaiseAll();

	View.Type = ZERN_VT_CAMERA;
	View.Entity = this;
	View.FarZ = 1000.0f;
	View.NearZ = 1.0f;
	View.AspectRatio = 1.777777f;
	View.ViewVolume = &ViewFrustum;
	View.ProjectionType = ZERN_PT_PERSPECTIVE;
	View.ShadowDistance = 100.0f;
	View.ShadowFadeDistance = View.ShadowDistance * 0.1f;
	View.Viewport = ZEGRViewport(0.0f, 0.0f, 1280.0f, 720.0f);

	FOVAxis = ZE_CFA_VERTICAL;
	AutoFOV = true;
	AutoAspectRatio = true;
	UpdateAutoAspectRatio();
}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}
