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

#include "ZERenderer.h"
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

void ZEShadowCascaded::UpdateBuffers()
{
	ZEUInt Width = ShadowMapDimension.x + 0.5f;
	ZEUInt Height = ShadowMapDimension.y + 0.5f;

	// Deallocate unused or changed render targets
	for (ZESize Index = 0; Index < ZE_SHADOW_MAX_CASCADE_COUNT; ++Index)
	{
		if (Cascades[Index].ShadowMap != NULL && ((Index > CascadeCount) || (Cascades[Index].ShadowMap->GetWidth() != Width) || (Cascades[Index].ShadowMap->GetHeight() != Height)))
		{
			ZE_DESTROY(Cascades[Index].RenderTarget);
			ZE_DESTROY(Cascades[Index].ShadowMap);
		}
	}

	for (ZESize Index = 0; Index < CascadeCount; ++Index)
	{
		if (Cascades[Index].ShadowMap == NULL)
		{
			Cascades[Index].ShadowMap = ZETexture2D::CreateInstance();
			Cascades[Index].ShadowMap->CreateStatic(Width, Height, 1, ZE_TPF_F32, true, NULL);
			Cascades[Index].RenderTarget = Cascades[Index].ShadowMap->CreateRenderTarget(0);
		}
	}
}

void ZEShadowCascaded::DestroyBuffers()
{
	for (ZESize Index = 0; Index < ZE_SHADOW_MAX_CASCADE_COUNT; ++Index)
	{
		if (Cascades[Index].ShadowMap != NULL)
		{
			ZE_DESTROY(Cascades[Index].RenderTarget);
			ZE_DESTROY(Cascades[Index].ShadowMap);
		}
	}
}

void ZEShadowCascaded::Update(ZECamera* Camera)
{
	// Fetch camera constants
	float CamareFOV = Camera->GetFOV();
	float TangentHalfFov = ZEAngle::Tan(CamareFOV * 0.5f);
	float CameraAspectRatio = Camera->GetAspectRatio();
	ZEVector3 CameraUp = Camera->GetWorldUp();
	ZEVector3 CameraFront = Camera->GetWorldFront();
	ZEVector3 CameraRight = Camera->GetWorldRight();
	ZEVector3 CameraPosition = Camera->GetWorldPosition();
	ZEQuaternion CameraRotation = Camera->GetWorldRotation();

	// Fetch light constants
	ZEVector3 LightUp = OwnerLight->GetWorldUp();
	ZEVector3 LightFront = OwnerLight->GetWorldFront();
	ZEVector3 LightRight = OwnerLight->GetWorldRight();
	float ShadowNearZ = Camera->GetNearZ();
	float ShadowFarZ =  ZEMath::Min(Camera->GetFarZ(), Camera->GetVisibleShadowDistance());

	// Per cascade calculations and rendering
	for (ZESize CascadeN = 0; CascadeN < CascadeCount; ++CascadeN)
	{
		// Set Split Distance
		float CascadeRatioNear = (float)CascadeN / (float)CascadeCount;
		float LinearTermNear = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioNear;
		float LogarithmicTermNear = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioNear);
		
		float CascadeRatioFar = ((float)CascadeN + 1.0f) / (float)CascadeCount;
		float LinearTermFar = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioFar;
		float LogarithmicTermFar = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioFar);

		Cascades[CascadeN].End = SplitBias * LogarithmicTermFar + (1.0f - SplitBias) * LinearTermFar;
		Cascades[CascadeN].Start = SplitBias * LogarithmicTermNear + (1.0f - SplitBias) * LinearTermNear;
		Cascades[CascadeN].Depth = Cascades[CascadeN].End - Cascades[CascadeN].Start;

		// near/far plane centers
		ZEVector3 FarPlaneCenter = CameraPosition + CameraFront * Cascades[CascadeN].End;
		ZEVector3 NearPlaneCenter = CameraPosition + CameraFront * Cascades[CascadeN].Start;

		// near/far plane half with/height
		float FarPlaneHalfHeight = TangentHalfFov * Cascades[CascadeN].End;
		float FarPlaneHalfWidth = FarPlaneHalfHeight * CameraAspectRatio;
		float NearPlaneHalfHeight = TangentHalfFov * Cascades[CascadeN].Start;
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
		ZEMatrix4x4::CreateLookAtTransform(TempLightViewMatrix, LookAt - LightFront * ShadowViewDepth, LookAt, LightUp);

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
//  	ScaleX = 1.0f / ZEMath::Floor(1.0f / ScaleX * ScaleQuantizer) * ScaleQuantizer;
//  	ScaleY = 1.0f / ZEMath::Floor(1.0f / ScaleY * ScaleQuantizer) * ScaleQuantizer;
//  
//  	float HalfShadowResolution = ShadowRenderer->GetShadowResolution() * 0.5f;
//  	OffsetX = ZEMath::Floor(OffsetX * HalfShadowResolution) / HalfShadowResolution;
//  	OffsetY = ZEMath::Floor(OffsetY * HalfShadowResolution) / HalfShadowResolution;

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
		
		Cascades[CascadeN].ShadowView.Create(ZEOBBox(ViewCenter, LightRight, LightUp, LightFront, ViewHalfSize));

		// Create view transform again with final and corrected world space coordinates
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateLookAtTransform(LightViewMatrix, ViewCenter + (-LightFront * ViewHalfDepth), ViewCenter + (LightFront * ViewHalfDepth), LightUp);

		// Create projection transform again with final and corrected world space coordinates
		ZEMatrix4x4 LightProjMatrix;
		ZEMatrix4x4::CreateOrthographicProjection(LightProjMatrix, ViewHalfWidth * 2.0f, ViewHalfHeight * 2.0f, 0.0f, ViewHalfDepth * 2.0f);

		Cascades[CascadeN].ShadowTransform = LightProjMatrix * LightViewMatrix;
	}
}

void ZEShadowCascaded::Draw(ZEDrawParameters* LightDrawParameters)
{
	zeCriticalError("Not Implemented yet.");

	ZESize VolumeCount = GetShadowVolumeCount();
	for (ZESize I = 0; I < VolumeCount; ++I)
	{
		ZEView View;
		memset(&View, 0, sizeof(ZEView));
		View.Light = OwnerLight;
		View.Type = ZE_VT_LIGHT;
		View.Camera = NULL;

		ZEDrawParameters ShadowParameters;
		ShadowParameters.View = &View;
		ShadowParameters.Stages = ZE_RST_SHADOW;
		ShadowParameters.Renderer = LightDrawParameters->Renderer;
		ShadowParameters.Time = LightDrawParameters->Time;
		ShadowParameters.ElapsedTime = LightDrawParameters->ElapsedTime;
		ShadowParameters.FrameId = LightDrawParameters->FrameId;
		ShadowParameters.ViewVolume = &Cascades[I].ShadowView;

		LightDrawParameters->Renderer->CreateCommandBucket(LightDrawParameters);
	}
}
		
bool ZEShadowCascaded::Initialize(ZELight* Light)
{
	zeDebugCheck(Light == NULL, "Null pointer.");

	if (!ZEShadow::Initialize(Light))
		return false;

	UpdateBuffers();
}

void ZEShadowCascaded::Deinitialize()
{
	ZEShadow::Deinitialize();

	DestroyBuffers();
}

ZEShadowCascaded::ZEShadowCascaded()
{
	SplitBias = 0.5f;
	CascadeCount = 3;
	ShadowViewDepth = 500.0f;

	memset(Cascades, 0, sizeof(ZECascade) * ZE_SHADOW_MAX_CASCADE_COUNT);
}

ZEShadowCascaded::~ZEShadowCascaded()
{
	Deinitialize();
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

void ZEShadowCascaded::SetShadowViewDepth(float Value)
{
	ShadowViewDepth = Value;
}

float ZEShadowCascaded::GetShadowViewDepth() const
{
	return ShadowViewDepth;
}

const ZECascade* ZEShadowCascaded::GetCascade(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_SHADOW_MAX_CASCADE_COUNT, "Index out of range");
	return &Cascades[Index];
}

ZEUInt ZEShadowCascaded::GetShadowVolumeCount() const
{
	return CascadeCount;
}

const ZEViewVolume*	ZEShadowCascaded::GetShadowVolume(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_SHADOW_MAX_CASCADE_COUNT, "Index out of range");
	
	return &Cascades[Index].ShadowView;
}
