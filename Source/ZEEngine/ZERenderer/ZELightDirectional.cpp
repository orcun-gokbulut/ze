//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightDirectional.cpp
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
#include "ZERenderer.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEMath.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEVector.h"
#include "ZELightDirectional.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEGraphics/ZEGraphicsModule.h"


void ZELightDirectional::UpdateCascades()
{
	ZECamera* Camera = zeScene->GetActiveCamera();

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
	ZEVector3 LightUp = GetWorldUp();
	ZEVector3 LightFront = GetWorldFront();
	ZEVector3 LightRight = GetWorldRight();
	float ShadowNearZ = Camera->GetNearZ();
	float ShadowFarZ =  ZEMath::Min(Camera->GetFarZ(), Camera->GetVisibleShadowDistance());

	// Per cascade calculations and rendering
	for (ZESize CascadeN = 0; CascadeN < CascadeCount; ++CascadeN)
	{
		if (zeCore->GetFrameId() % Cascades[CascadeN].UpdateInterval != 0)
			continue;

		// Set Split Distance
		float CascadeRatioNear = (float)CascadeN / (float)CascadeCount;
		float LinearTermNear = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioNear;
		float LogarithmicTermNear = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioNear);
		
		float CascadeRatioFar = ((float)CascadeN + 1.0f) / (float)CascadeCount;
		float LinearTermFar = ShadowNearZ + (ShadowFarZ - ShadowNearZ) * CascadeRatioFar;
		float LogarithmicTermFar = ShadowNearZ * ZEMath::Power(ShadowFarZ / ShadowNearZ, CascadeRatioFar);

		Cascades[CascadeN].FarZ = CascadeSplitBias * LogarithmicTermFar + (1.0f - CascadeSplitBias) * LinearTermFar;
		Cascades[CascadeN].NearZ = CascadeSplitBias * LogarithmicTermNear + (1.0f - CascadeSplitBias) * LinearTermNear;
		Cascades[CascadeN].Depth = Cascades[CascadeN].FarZ - Cascades[CascadeN].NearZ;

		// near/far plane centers
		ZEVector3 FarPlaneCenter = CameraPosition + CameraFront * Cascades[CascadeN].FarZ;
		ZEVector3 NearPlaneCenter = CameraPosition + CameraFront * Cascades[CascadeN].NearZ;

		// near/far plane half with/height
		float FarPlaneHalfHeight = TangentHalfFov * Cascades[CascadeN].FarZ;
		float FarPlaneHalfWidth = FarPlaneHalfHeight * CameraAspectRatio;
		float NearPlaneHalfHeight = TangentHalfFov * Cascades[CascadeN].NearZ;
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
		ZEMatrix4x4::CreateLookAtTransform(TempLightViewMatrix, LookAt - LightFront * Range, LookAt, LightUp);

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
		
		Cascades[CascadeN].ViewVolume.Create(ZEOBBox(ViewCenter, LightRight, LightUp, LightFront, ViewHalfSize));

		// Create view transform again with final and corrected world space coordinates
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateLookAtTransform(LightViewMatrix, ViewCenter + (-LightFront * ViewHalfDepth), ViewCenter + (LightFront * ViewHalfDepth), LightUp);

		// Create projection transform again with final and corrected world space coordinates
		ZEMatrix4x4 LightProjMatrix;
		ZEMatrix4x4::CreateOrthographicProjection(LightProjMatrix, ViewHalfWidth * 2.0f, ViewHalfHeight * 2.0f, 0.0f, ViewHalfDepth * 2.0f);

		Cascades[CascadeN].ShadowTransform = LightProjMatrix * LightViewMatrix;
	}
}

void ZELightDirectional::UpdateRenderTargets()
{
	ZEVector2 Dimension = zeScene->GetRenderer()->GetShadowMapDimension();
	ZEUInt Width = (ZEUInt)(Dimension.x + 0.5f);
	ZEUInt Height = (ZEUInt)(Dimension.y + 0.5f);

	// Deallocate unused or changed render targets
	for (ZESize Index = 0; Index < ZE_DL_MAX_CASCADE_COUNT; ++Index)
	{
		if (Cascades[Index].ShadowMap != NULL && ((Index > CascadeCount) || (Cascades[Index].ShadowMap->GetWidth() != Width) || (Cascades[Index].ShadowMap->GetHeight() != Height)))
		{
			ZE_DESTROY(Cascades[Index].ShadowMapRenderTarget);
			ZE_DESTROY(Cascades[Index].ShadowMap);
		}
	}

	for (ZESize Index = 0; Index < CascadeCount; ++Index)
	{
		if (Cascades[Index].ShadowMap == NULL)
		{
			Cascades[Index].ShadowMap = ZETexture2D::CreateInstance();
			Cascades[Index].ShadowMap->CreateStatic(Width, Height, 1, ZE_TPF_F32, true, NULL);
			Cascades[Index].ShadowMapRenderTarget = Cascades[Index].ShadowMap->CreateRenderTarget(0);
		}
	}
}

void ZELightDirectional::DestroyRenderTargets()
{
	for (ZESize Index = 0; Index < ZE_DL_MAX_CASCADE_COUNT; ++Index)
	{
		if (Cascades[Index].ShadowMap != NULL)
		{
			ZE_DESTROY(Cascades[Index].ShadowMapRenderTarget);
			ZE_DESTROY(Cascades[Index].ShadowMap);
		}
	}
}

void ZELightDirectional::SetCascadeCount(ZESize Value)
{
	zeDebugCheck(Value > ZE_DL_MAX_CASCADE_COUNT, "Count should be smaller MAX_CASCADE_COUNT");
	CascadeCount = Value;
}

ZESize ZELightDirectional::GetCascadeCount() const
{
	return CascadeCount;
}
		
void ZELightDirectional::SetSplitBias(float Value)
{
	CascadeSplitBias = Value;
}

float ZELightDirectional::GetSplitBias() const
{
	return CascadeSplitBias;
}

float ZELightDirectional::GetFarZ(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].FarZ;
}

float ZELightDirectional::GetNearZ(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].NearZ;
}

float ZELightDirectional::GetDepth(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].Depth;
}

void ZELightDirectional::SetUpdateInterval(ZESize Index, ZEUInt Value)
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	Cascades[Index].UpdateInterval = Value;
}

ZEUInt ZELightDirectional::GetUpdateInterval(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].UpdateInterval;
}

const ZETexture2D* ZELightDirectional::GetShadowMap(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].ShadowMap;
}

const ZEMatrix4x4& ZELightDirectional::GetShadowTransform(ZESize Index) const
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].ShadowTransform;
}

const ZEViewVolume& ZELightDirectional::GetViewVolume(ZESize Index)
{
	zeDebugCheck(Index >= ZE_DL_MAX_CASCADE_COUNT, "Index out of range");
	return Cascades[Index].ViewVolume;
}

void ZELightDirectional::Draw(ZEDrawParameters* DrawParameters)
{
	if (DrawParameters->Pass == ZE_RP_SHADOW_MAP)
		return;

	ZELight::Draw(DrawParameters);

	if (!ShadowCaster)
		return;

	UpdateRenderTargets();
	UpdateCascades();
}

bool ZELightDirectional::Initialize()
{
	if (GetInitialized())
		return false;
	
	ZEEntity::Initialize();

	return true;
}

void ZELightDirectional::Deinitialize()
{
	if (!GetInitialized())
		return;

	DestroyRenderTargets();
}

ZELightDirectional::ZELightDirectional()
{
	Type = ZE_LT_DIRECTIONAL;

	CascadeCount = 1;
	CascadeSplitBias = 0.8f;

	ShadowSlopeScaledBias = 0.4f;
	ShadowDepthScaledBias = 0.6f;
	
	for (ZESize I = 0; I < ZE_DL_MAX_CASCADE_COUNT; ++I)
	{
		Cascades[I].Index = (ZEUInt)I;
		Cascades[I].ShadowMap = NULL;
		Cascades[I].ShadowMapRenderTarget = NULL;
		Cascades[I].UpdateInterval = (ZEUInt)I + 1;
		Cascades[I].ShadowTransform = ZEMatrix4x4::Identity;
		Cascades[I].NearZ = Cascades[I].FarZ = Cascades[I].Depth = 0.0f;
	}
}

ZELightDirectional::~ZELightDirectional()
{
	Deinitialize();
}

ZELightDirectional* ZELightDirectional::CreateInstance()
{
	return new ZELightDirectional();
}
