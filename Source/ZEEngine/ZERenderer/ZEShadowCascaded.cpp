//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEShadowCascaded.cpp
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

#include "ZEView.h"
#include "ZERenderer.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEShadowCascaded.h"
#include "ZERenderer/ZELight.h"
#include "ZERenderer/ZECamera.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEMath/ZEViewCuboid.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZERenderer/ZERenderStageShadow.h"

/************************************************************************/
/*                        ZEViewCascaded                                */
/************************************************************************/
void ZEViewCascaded::SetCascadeIndex(ZEUInt Index)
{
	CascadeIndex = Index;
}

ZEUInt ZEViewCascaded::GetCascadeIndex() const
{
	return CascadeIndex;
}

void ZEViewCascaded::SetOwnerShadow(const ZEShadowCascaded* Owner)
{
	zeDebugCheck(Owner == NULL, "Null Pointer");
	OwnerShadow = Owner;
}

const ZEShadowCascaded* ZEViewCascaded::GetOwnerShadow() const
{
	return OwnerShadow;
}

void ZEViewCascaded::Update(ZECamera* Camera, ZEUInt CascadeCount, float SplitBias, float ViewDepth)
{
	zeDebugCheck(Camera == NULL, "Null pointer.");
	zeDebugCheck(OwnerShadow == NULL, "View not initialized yet.");

	const ZELight* Light = OwnerShadow->GetOwnerLight();

	// Fetch camera and light properties
	float CameraFarZ = Camera->GetFarZ();
	float CameraNearZ = Camera->GetNearZ();
	float CamareFOV = Camera->GetFov();
	float TangentHalfFov = ZEAngle::Tan(CamareFOV * 0.5f);
	float CameraAspectRatio = Camera->GetAspectRatio();
	float CameraShadowRange = Camera->GetVisibleShadowDistance();
	ZEVector3 CameraPosition = Camera->GetWorldPosition();

	ZEVector3 LightUp = Light->GetWorldUp();
	ZEVector3 LightFront = Light->GetWorldFront();
	ZEVector3 LightRight = Light->GetWorldRight();
	ZEVector3 CameraUp = Camera->GetWorldUp();
	ZEVector3 CameraFront = Camera->GetWorldFront();
	ZEVector3 CameraRight = Camera->GetWorldRight();

	float ShadowNearZ = CameraNearZ;
	float ShadowFarZ =  ZEMath::Min(CameraFarZ, CameraShadowRange);

	// Set Split Distance
	float CascadeRatioNear = (float)CascadeIndex / (float)CascadeCount;
	float LinearTermNear = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioNear;
	float LogarithmicTermNear = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioNear);
		
	float CascadeRatioFar = ((float)CascadeIndex + 1.0f) / (float)CascadeCount;
	float LinearTermFar = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioFar;
	float LogarithmicTermFar = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioFar);

	float CascadeEnd = SplitBias * LogarithmicTermFar + (1.0f - SplitBias) * LinearTermFar;
	float CascadeStart = SplitBias * LogarithmicTermNear + (1.0f - SplitBias) * LinearTermNear;

	// near/far plane centers
	ZEVector3 FarPlaneCenter = CameraPosition + CameraFront * CascadeEnd;
	ZEVector3 NearPlaneCenter = CameraPosition + CameraFront * CascadeStart;

	// near/far plane half with/height
	float FarPlaneHalfHeight = TangentHalfFov * CascadeEnd;
	float FarPlaneHalfWidth = FarPlaneHalfHeight * CameraAspectRatio;
	float NearPlaneHalfHeight = TangentHalfFov * CascadeStart;
	float NearPlaneHalfWidth = NearPlaneHalfHeight * CameraAspectRatio;

	// near/far plane half right/up vectors
	ZEVector3 FarHalfUpVector = CameraUp * FarPlaneHalfHeight;
	ZEVector3 NearHalfUpVector = CameraUp * NearPlaneHalfHeight;
	ZEVector3 FarHalfRightVector = CameraRight * FarPlaneHalfWidth;
	ZEVector3 NearHalfRightVector = CameraRight * NearPlaneHalfWidth;

	// World space frustum corners
	static ZEVector4 WorldFrustumCorners[8];
	WorldFrustumCorners[0] = ZEVector4(NearPlaneCenter - NearHalfRightVector - NearHalfUpVector, 1.0f);		// Near Left Down
	WorldFrustumCorners[1] = ZEVector4(NearPlaneCenter - NearHalfRightVector + NearHalfUpVector, 1.0f);		// Near Left Up
	WorldFrustumCorners[2] = ZEVector4(NearPlaneCenter + NearHalfRightVector + NearHalfUpVector, 1.0f);		// Near Right Up
	WorldFrustumCorners[3] = ZEVector4(NearPlaneCenter + NearHalfRightVector - NearHalfUpVector, 1.0f);		// Near Right Down
	WorldFrustumCorners[4] = ZEVector4(FarPlaneCenter - FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  Left Down
	WorldFrustumCorners[5] = ZEVector4(FarPlaneCenter - FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far  Left Up
	WorldFrustumCorners[6] = ZEVector4(FarPlaneCenter + FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far Right Up
	WorldFrustumCorners[7] = ZEVector4(FarPlaneCenter + FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  Right Down

	// Look at same position for all cascades other vise frustum wont be covered completely in light clip space
	ZEVector3 LookAt = CameraPosition;

	// Look at transform
	// position and look at coordinates should be accurate
	ZEMatrix4x4 TempLightViewMatrix;
	ZEMatrix4x4::CreateLookAtTransform(TempLightViewMatrix, LookAt - LightFront * ViewDepth, LookAt, LightUp);

	// Symbolic orthographic projection
	// width, height, near, far does not matter at all
	ZEMatrix4x4 TempLightProjMatrix;
	float FarPlaneHeight = 2.0f * TangentHalfFov * ShadowFarZ;
	ZEMatrix4x4::CreateOrthographicProjection(TempLightProjMatrix, FarPlaneHeight * CameraAspectRatio, FarPlaneHeight, 1.0f, 10.0f);

	// Lights view projection transform
	ZEMatrix4x4 LightViewProjMatrix = TempLightProjMatrix * TempLightViewMatrix;

	// Transform world positions to light clip space
	// Transformation is linear so no need to divide result to its w
	static ZEVector4 ClipFrustumCorners[8];
	for (ZESize I = 0; I < 8; ++I)
		ZEMatrix4x4::Transform(ClipFrustumCorners[I], LightViewProjMatrix, WorldFrustumCorners[I]);

	// Find min/max of vertices in clip space
	ZEVector3 Min(FLT_MAX, FLT_MAX, FLT_MAX);
	ZEVector3 Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (ZESize I = 0; I < 8; ++I)
	{
		if (ClipFrustumCorners[I].x < Min.x) Min.x = ClipFrustumCorners[I].x;
		if (ClipFrustumCorners[I].y < Min.y) Min.y = ClipFrustumCorners[I].y;
		if (ClipFrustumCorners[I].z < Min.z) Min.z = ClipFrustumCorners[I].z;
			
		if (ClipFrustumCorners[I].x > Max.x) Max.x = ClipFrustumCorners[I].x;
		if (ClipFrustumCorners[I].y > Max.y) Max.y = ClipFrustumCorners[I].y;
		if (ClipFrustumCorners[I].z > Max.z) Max.z = ClipFrustumCorners[I].z;
	}
								   
	// Pull z back, to avoid casters between light and light's near z
	Min.z = 0.0f;

	// Create crop matrix that will maximize the bounding box in clip space
	float ScaleQuantizer = 64.0f;
		
	float ScaleX = 2.0f / (Max.x - Min.x);
	float ScaleY = 2.0f / (Max.y - Min.y);
	float ScaleZ = 1.0f / (Max.z - Min.z);
	float OffsetX = -0.5f * (Max.x + Min.x) * ScaleX;
	float OffsetY = -0.5f * (Max.y + Min.y) * ScaleY;
	float OffsetZ = -Min.z * ScaleZ;

	// Correct Shimmering in scale and offset
	// ScaleX = 1.0f / ZEMath::Floor(1.0f / ScaleX * ScaleQuantizer) * ScaleQuantizer;
	// ScaleY = 1.0f / ZEMath::Floor(1.0f / ScaleY * ScaleQuantizer) * ScaleQuantizer;
	// 
	// float HalfShadowResolution = ShadowRenderer->GetShadowResolution() * 0.5f;
	// OffsetX = ZEMath::Floor(OffsetX * HalfShadowResolution) / HalfShadowResolution;
	// OffsetY = ZEMath::Floor(OffsetY * HalfShadowResolution) / HalfShadowResolution;

	// Create crop matrix
	ZEMatrix4x4 CropMatrix(	ScaleX, 0.0f,	0.0f,	OffsetX,
							0.0f,	ScaleY,	0.0f,	OffsetY,
							0.0f,	0.0f,	ScaleZ, OffsetZ,
							0.0f,	0.0f,	0.0f,	1.0f);

	// Final shadow transformation matrix
	ZEMatrix4x4 LightViewProjCropMatrix = CropMatrix * LightViewProjMatrix;

	// Calculate cascades view volume
	// Clip space to world space transformation
	ZEMatrix4x4 LightInvViewProjCrop;
	ZEMatrix4x4::Inverse(LightInvViewProjCrop, LightViewProjCropMatrix);

	// Clip space min max
	static const ZEVector4 ClipMin(-1.0f, -1.0f, 0.0f, 1.0f);
	static const ZEVector4 ClipMax(+1.0f, +1.0f, +1.0f, 1.0f);

	// Only transform 2 vertices of clip space to world
	// Transformation is linear(orthographic) so no need to divide result to its w
	ZEVector4 WorldMin;
	ZEVector4 WorldMax;
	ZEMatrix4x4::Transform(WorldMin, LightInvViewProjCrop, ClipMin);
	ZEMatrix4x4::Transform(WorldMax, LightInvViewProjCrop, ClipMax);

	// Find half diagonal vector and view center
	ZEVector3 HalfDiagonalVector= (WorldMax - WorldMin).ToVector3() * 0.5f;
	ZEVector3 ViewCenter = HalfDiagonalVector + WorldMin.ToVector3();
		
	// Project half diagonal to light axises and find width, height, depth
	float ViewHalfHeight = ZEVector3::DotProduct(LightUp, HalfDiagonalVector);
	float ViewHalfWidth	= ZEVector3::DotProduct(LightRight, HalfDiagonalVector);
	float ViewHalfDepth	= ZEVector3::DotProduct(LightFront, HalfDiagonalVector);
	ZEVector3 ViewHalfSize(ViewHalfWidth, ViewHalfHeight, ViewHalfDepth);
		
	ShadowVolume.Create(ZEOBBox(ViewCenter, LightRight, LightUp, LightFront, ViewHalfSize));

	// Create view transform again with final and corrected world space coordinates
	ZEMatrix4x4::CreateLookAtTransform(ViewTransform, ViewCenter + (-LightFront * ViewHalfDepth), ViewCenter + (LightFront * ViewHalfDepth), LightUp);

	// Create projection transform again with final and corrected world space coordinates
	ZEMatrix4x4::CreateOrthographicProjection(ProjectionTransform, ViewHalfWidth * 2.0f, ViewHalfHeight * 2.0f, 0.0f, ViewHalfDepth * 2.0f);
}

ZEViewType ZEViewCascaded::GetViewType() const
{
	return ZE_VT_LIGHT;
}

const ZEViewVolume* ZEViewCascaded::GetViewVolume() const
{
	return &ShadowVolume;
}

const ZEMatrix4x4& ZEViewCascaded::GetViewTransform() const
{	
	return ViewTransform;
}

const ZEMatrix4x4& ZEViewCascaded::GetProjectionTransform() const
{
	return ProjectionTransform;
}

const ZEMatrix4x4& ZEViewCascaded::GetViewProjectionTransform() const
{
	return ViewProjectionTransform;
}

ZEViewCascaded::ZEViewCascaded()
{

}

ZEViewCascaded::~ZEViewCascaded()
{
	CascadeIndex = 0;
	OwnerShadow = NULL;
}

/************************************************************************/
/*							ZEShadowCascaded                            */
/************************************************************************/
#define FLOAT_EQUAL_CHECK(A, B)		(ZEMath::Abs((A) - (B)) > ZE_ZERO_THRESHOLD)

inline static bool CheckCameraChanged(float FovA, float AspectRatioA, float NearZA, float FarZA, float ShadowRangeA, const ZEVector3& PositionA, const ZEQuaternion& RotationA,
									  float FovB, float AspectRatioB, float NearZB, float FarZB, float ShadowRangeB, const ZEVector3& PositionB, const ZEQuaternion& RotationB)
{
	if (PositionA != PositionB)
		return true;

	if (RotationA != RotationB)
		return true;

	if (FLOAT_EQUAL_CHECK(FovA, FovB))
		return true;

	if (FLOAT_EQUAL_CHECK(FarZA, FarZB))
		return true;

	if (FLOAT_EQUAL_CHECK(NearZA, NearZB))
		return true;

	if (FLOAT_EQUAL_CHECK(ShadowRangeA, ShadowRangeB))
		return true;

	if (FLOAT_EQUAL_CHECK(AspectRatioA, AspectRatioB))
		return true;

	return false;
}

inline static bool CheckLightChanged(const ZEQuaternion& RotationA, const ZEQuaternion& RotationB)
{
	if (RotationA != RotationB)
		return true;

	return false;
}

void ZEShadowCascaded::UpdateBuffers()
{
	ZEUInt NewWidth = (ZEUInt)(MapResolution.x + 0.5f);
	ZEUInt NewHeight = (ZEUInt)(MapResolution.y + 0.5f);

	// Deallocate unused or changed render targets
	for (ZESize I = 0; I < ZE_SHADOW_MAX_CASCADE_COUNT; ++I)
	{
		if (ShadowMaps[I] == NULL)
			continue;
	
		ZEUInt OldWidth = ShadowMaps[I]->GetWidth();
		ZEUInt OldHeight = ShadowMaps[I]->GetHeight();
			
		if (NewWidth != OldWidth || NewHeight != OldHeight)
		{
			ZE_DESTROY(RenderTargets[I]);
			ZE_DESTROY(ShadowMaps[I]);
		}
	}

	for (ZESize I = 0; I < CascadeCount; ++I)
	{
		if (ShadowMaps[I] != NULL)
			continue;

		ShadowMaps[I] = ZETexture2D::CreateInstance();
		ShadowMaps[I]->CreateStatic(NewWidth, NewHeight, 1, ZE_TPF_F32, true, NULL);
		RenderTargets[I] = ShadowMaps[I]->CreateRenderTarget(0);
	}
}

void ZEShadowCascaded::DestroyBuffers()
{
	for (ZESize I = 0; I < ZE_SHADOW_MAX_CASCADE_COUNT; ++I)
	{
		ZE_DESTROY(RenderTargets[I]);
		ZE_DESTROY(ShadowMaps[I]);
	}
}

bool ZEShadowCascaded::Initialize(ZELight* Light)
{
	zeDebugCheck(Light == NULL, "Null pointer.");

	if (!ZEShadow::Initialize(Light))
		return false;

	for (ZESize I = 0; I < ZE_SHADOW_MAX_CASCADE_COUNT; ++I)
	{
		CascadeViews[I].SetOwnerShadow(this);
		CascadeViews[I].SetCascadeIndex((ZEUInt)I);
		OwnerLight->AddChildEntity(&CascadeViews[I]);
	}

	UpdateBuffers();

	return true;
}

void ZEShadowCascaded::Deinitialize()
{
	ZEShadow::Deinitialize();

	for (ZESize I = 0; I < ZE_SHADOW_MAX_CASCADE_COUNT; ++I)
	{
		OwnerLight->RemoveChildEntity(&CascadeViews[I]);
	}
	
	DestroyBuffers();
}

void ZEShadowCascaded::SetCascadeCount(ZEUInt Value)
{
	CascadeCount = Value;
}

ZEUInt ZEShadowCascaded::GetCascadeCount() const
{
	return CascadeCount;
}

void ZEShadowCascaded::SetSplitBias(float Value)
{
	SplitBias = Value;
}

float ZEShadowCascaded::GetSplitBias() const
{
	return SplitBias;
}

void ZEShadowCascaded::SetViewDepth(float Value)
{
	ViewDepth = Value;
}

float ZEShadowCascaded::GetViewDepth() const
{
	return ViewDepth;
}


ZEUInt ZEShadowCascaded::GetViewCount() const
{
	return CascadeCount;
}

const ZEView* ZEShadowCascaded::GetView(ZESize Index) const
{
	zeDebugCheck(Index >= CascadeCount, "Index out of range");

	return &CascadeViews[Index];
}

void ZEShadowCascaded::Update(ZECamera* Camera)
{
	zeDebugCheck(Camera == NULL, "Null Pointer");
	zeDebugCheck(OwnerLight == NULL, "Shdow not initialized yet.");
	
	if (!GetInitialized())
		return;

	// Fetch camera and light properties
	float CameraFarZ = Camera->GetFarZ();
	float CameraNearZ = Camera->GetNearZ();
	float CamareFOV = Camera->GetFov();
	float CameraAspectRatio = Camera->GetAspectRatio();
	float CameraShadowRange = Camera->GetVisibleShadowDistance();
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	ZEQuaternion CameraRotation = Camera->GetWorldRotation();
	ZEQuaternion LightRotation = OwnerLight->GetWorldRotation();

	// Check if view needs update
	bool LightChanged = CheckLightChanged(LightOldRotation, LightRotation);

	bool CameraChanged = CheckCameraChanged(CamareFOV, CameraAspectRatio, CameraNearZ, CameraFarZ, CameraShadowRange, CameraPosition, CameraRotation, 
							CameraOldFov, CameraOldAspectRatio, CameraOldNearZ, CameraOldFarZ, CameraOldShadowRange, CameraOldPosition, CameraOldRotation);
	
	if (!(CameraChanged || LightChanged))
		return;

	CameraOldFov = CamareFOV;
	CameraOldFarZ = CameraFarZ;
	CameraOldNearZ = CameraNearZ;
	CameraOldAspectRatio = CameraAspectRatio;
	CameraOldShadowRange = CameraShadowRange;
	CameraOldPosition = CameraPosition;
	CameraOldRotation = CameraRotation;
	LightOldRotation = LightRotation;

	for (ZEUInt I = 0; I < CascadeCount; ++I)
	{
		CascadeViews[(ZESize)I].Update(Camera, CascadeCount, SplitBias, ViewDepth);
	}
}

void ZEShadowCascaded::Draw(ZEDrawParameters* LightDrawParameters)
{
	zeDebugCheck(OwnerLight == NULL, "Shdow not initialized yet.");

	if (!GetInitialized())
		return;

	for (ZESize I = 0; I < (ZESize)CascadeCount; ++I)
	{
		ZERenderStageShadowData* Data = new ZERenderStageShadowData();
		Data->Create(&CascadeViews[I]);

		// Create a command bucket for this draw parameters
		ZECommandBucket* Bucket = NULL;
		Bucket = LightDrawParameters->Renderer->CreateCommandBucket(ZE_RST_SHADOW, Data);

		// Create drawp parameters
		ZEDrawParameters ShadowDrawParameters;
		ShadowDrawParameters.Bucket = Bucket;
		ShadowDrawParameters.View = &CascadeViews[I];
		ShadowDrawParameters.Stages = ZE_RST_SHADOW;
		ShadowDrawParameters.Renderer = LightDrawParameters->Renderer;
		ShadowDrawParameters.Time = LightDrawParameters->Time;
		ShadowDrawParameters.ElapsedTime = LightDrawParameters->ElapsedTime;
		ShadowDrawParameters.FrameId = LightDrawParameters->FrameId;

		// Cull the scene based on draw parameters
		zeScene->GetSceneCuller().CullScene(zeScene, &ShadowDrawParameters);
	}
}

ZEShadowCascaded::ZEShadowCascaded()
{
	SplitBias = 0.5f;
	CascadeCount = 3;
	ViewDepth = 500.0f;
	
	for (ZESize I = 0; I < ZE_SHADOW_MAX_CASCADE_COUNT; ++I)
	{
		ShadowMaps[I] = NULL;
		RenderTargets[I] = NULL;
	}
}

ZEShadowCascaded::~ZEShadowCascaded()
{
	Deinitialize();
}
