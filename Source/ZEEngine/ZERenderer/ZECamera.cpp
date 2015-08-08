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
#include "ZEGraphics/ZEGRScreen.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZERay.h"

#define ZE_CDF_NONE								0
#define ZE_CDF_VIEW								1
#define ZE_CDF_VIEW_FRUSTUM						2
#define ZE_CDF_VIEW_TRANSFORM					4
#define ZE_CDF_PROJECTION_TRANSFORM				8
#define ZE_CDF_VIEW_PROJECTION_TRANSFORM		16
#define ZE_CDF_CONSTANT_BUFFER					32
#define ZE_CDF_ALL								0xFFFFFFFF

bool ZECamera::InitializeSelf()
{
	ConstantBuffer = ZEGRConstantBuffer::Create(sizeof(Constants));
	if (ConstantBuffer == NULL)
	{
		zeError("Cannot create camera constant buffer.");
		return false;
	}

	return true;
}

bool ZECamera::DeinitializeSelf()
{
	ZEGR_RELEASE(ConstantBuffer);
	return true;
}

const ZEMatrix4x4& ZECamera::GetViewTransform()
{
	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(Constants.ViewTransform, GetWorldPosition(), GetWorldRotation());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_TRANSFORM);
	}
	
	return Constants.ViewTransform;
}

const ZEMatrix4x4& ZECamera::GetProjectionTransform()
{
	UpdateAutoParameters();
	
	if (CameraDirtyFlags.GetFlags(ZE_CDF_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::CreatePerspectiveProjection(Constants.ProjectionTransform, Constants.VerticalFOV, Constants.AspectRatio, Constants.NearZ, Constants.FarZ);
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_PROJECTION_TRANSFORM);
	}

	return Constants.ProjectionTransform;
}

const ZEMatrix4x4& ZECamera::GetViewProjectionTransform()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM))
	{
		ZEMatrix4x4::Multiply(Constants.ViewProjectionTransform, GetProjectionTransform(), GetViewTransform());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	}

	return Constants.ViewProjectionTransform;
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
}

ZEGRConstantBuffer* ZECamera::GetConstantBuffer()
{
	if (ConstantBuffer == NULL || Screen == NULL)
		return NULL;

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_FRUSTUM))
	{
		Constants.Width = Screen->GetWidth();
		Constants.Height = Screen->GetHeight();
		GetViewTransform();
		GetProjectionTransform();
		GetViewProjectionTransform();

		void* Buffer;
		ConstantBuffer->Lock(&Buffer);
		memcpy(Buffer, &Constants, sizeof(Constants));
		ConstantBuffer->Unlock();

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_CONSTANT_BUFFER);
	}

	return ConstantBuffer;
}

void ZECamera::SetScreen(ZEGRScreen* Screen)
{
	this->Screen = Screen;
}

ZEGRScreen* ZECamera::GetScreen()
{
	return Screen;
}

void ZECamera::SetNearZ(float NearZ)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	Constants.NearZ = NearZ;
}

float ZECamera::GetNearZ() const
{
	return Constants.NearZ;
}

void ZECamera::SetFarZ(float FarZ)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	Constants.FarZ = FarZ;
}

float ZECamera::GetFarZ() const
{
	return Constants.FarZ;
}

void ZECamera::SetHorizontalFOV(float FOV)
{
	UpdateAutoParameters();
	Constants.HorizontalFOV = FOV;
	SetVerticalFOV(2.0f * ZEAngle::ArcTan(ZEAngle::Tan(FOV * 0.5f) / Constants.AspectRatio));
}

float ZECamera::GetHorizontalFOV() const
{
	return 2.0f * ZEAngle::ArcTan(ZEAngle::Tan(Constants.VerticalFOV * 0.5f) * Constants.AspectRatio);
}

void ZECamera::SetVerticalFOV(float FOV)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	Constants.VerticalFOV = FOV;
}

float ZECamera::GetVerticalFOV() const
{
	return Constants.VerticalFOV;
}

void ZECamera::SetAutoAspectRatio(bool Enabled)
{
	AutoAspectRatio = Enabled;
}

bool ZECamera::GetAutoAspectRatio()
{
	return AutoAspectRatio;
}

void ZECamera::SetAspectRatio(float AspectRatio)
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_ALL & ~ZE_CDF_VIEW_TRANSFORM);
	Constants.AspectRatio = AspectRatio;
}

float ZECamera::GetAspectRatio() const
{
	if (AutoAspectRatio)
	{
		if (Screen == NULL)
			return 1.0f;

		return (float)Screen->GetWidth() / (float)Screen->GetHeight();
	}

	return Constants.AspectRatio;
}

void ZECamera::SetShadowDistance(float Value)
{
	Constants.ShadowDistance = Value;
}

float ZECamera::GetShadowDistance() const
{
	return Constants.ShadowDistance;
}

void ZECamera::SetShadowFadeDistance(float Value)
{
	Constants.ShadowFadeDistance = Value;
}

float ZECamera::GetShadowFadeDistance() const
{
	return Constants.ShadowFadeDistance;
}

const ZERNView& ZECamera::GetView()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW))
	{


		View.Type = ZERN_VT_CAMERA;
		View.Entity = this;
		View.Position = GetWorldPosition();
		View.Rotation = GetWorldRotation();
		View.Direction = GetWorldFront();

		View.ProjectionType = ZERN_PT_PERSPECTIVE;
		View.Width = 2.0f;
		View.Height = 2.0f;
		View.VerticalFOV = GetVerticalFOV();
		View.HorizontalFOV = GetHorizontalFOV();
		View.AspectRatio = GetAspectRatio();

		View.ViewTransform = GetViewTransform();
		View.ViewProjectionTransform = GetProjectionTransform();
		View.ViewProjectionTransform = GetViewProjectionTransform();

		View.Screen = Screen;
		View.ViewVolume = &GetViewVolume();

		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW);
	}

	return View;
}

const ZEViewVolume& ZECamera::GetViewVolume()
{
	UpdateAutoParameters();

	if (CameraDirtyFlags.GetFlags(ZE_CDF_VIEW_FRUSTUM))
	{
		ViewFrustum.Create(GetWorldPosition(), GetWorldRotation(), GetVerticalFOV(), GetAspectRatio(), GetNearZ(), GetFarZ());
		CameraDirtyFlags.UnraiseFlags(ZE_CDF_VIEW_FRUSTUM);
	}

	return ViewFrustum;
}

void ZECamera::GetScreenRay(ZERay& Ray, ZEInt ScreenX, ZEInt ScreenY)
{
	if (Screen == NULL)
		return;

	ZEVector3 V;
	const ZEMatrix4x4& ProjMatrix = GetProjectionTransform();
	V.x =  (((2.0f * ScreenX ) / Screen->GetWidth()) - 1) / ProjMatrix.M11;
	V.y = -(((2.0f * ScreenY ) / Screen->GetHeight()) - 1) / ProjMatrix.M22;
	V.z =  1.0f;

	ZEMatrix4x4 InvViewMatrix;
	ZEMatrix4x4::Inverse(InvViewMatrix, GetViewTransform());

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
	
	return ZEVector2((ClipPosition.x +  0.5f) * (float)Screen->GetWidth(), (-ClipPosition.y +  0.5f) * (float)Screen->GetWidth());
}

ZECamera::ZECamera()
{
	CameraDirtyFlags.RaiseFlags(ZE_CDF_VIEW | ZE_CDF_VIEW_FRUSTUM | ZE_CDF_VIEW_TRANSFORM | ZE_CDF_PROJECTION_TRANSFORM | ZE_CDF_VIEW_PROJECTION_TRANSFORM);
	
	Screen = NULL;

	Constants.FarZ = 10000.0f;
	Constants.NearZ = 1.0f;
	Constants.AspectRatio = 1.0f;
	SetHorizontalFOV(ZE_PI_2);
	AutoAspectRatio = true;
	
	Constants.ShadowDistance = 1000.0f;
	Constants.ShadowFadeDistance = Constants.ShadowDistance * 0.1f;
}

ZECamera* ZECamera::CreateInstance()
{
	return new ZECamera();
}
