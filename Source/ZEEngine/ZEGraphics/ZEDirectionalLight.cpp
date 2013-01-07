//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectionalLight.cpp
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

#include "ZETexture2D.h"
#include "ZECore/ZECore.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEShadowRenderer.h"
#include "ZEGraphicsModule.h"
#include "ZEDirectionalLight.h"
#include "ZEMath/ZEViewCuboid.h"
#include "ZEGame/ZEEntityProvider.h"
#include "ZECamera.h"
#include "ZEMath/ZEViewFrustum.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEMath.h"

ZE_META_REGISTER_CLASS(ZEEntityProvider, ZEDirectionalLight);


void ZEDirectionalLight::CreateRenderTargets()
{
	ZEUInt Dimension = ((ZEShadowRenderer*)zeScene->GetShadowRenderer())->GetShadowResolution();
	
	ZESize I = 0;
	for ( ; I < CascadeCount; ++I)
	{
		if (ShadowMaps[I] == NULL)
		{
			// Create if not allocated
			ShadowMaps[I] = ZETexture2D::CreateInstance();
			ShadowMaps[I]->Create(Dimension, Dimension, 1, ZE_TPF_F32, true);
		}
		else
		{
			// Recreate if changed
			if (ShadowMaps[I]->GetWidth() != Dimension || ShadowMaps[I]->GetHeight() != Dimension)
			{
				ShadowMaps[I]->Destroy();
				ShadowMaps[I] = ZETexture2D::CreateInstance();
				ShadowMaps[I]->Create(Dimension, Dimension, 1, ZE_TPF_F32, true);
			}
		}
	}
	// Destroy unused shadow maps
	for ( ; I < MAX_CASCADE_COUNT; ++I)
	{
		if (ShadowMaps[I] != NULL)
		{
			ShadowMaps[I]->Destroy();
			ShadowMaps[I] = NULL;
		}
	}
}

void ZEDirectionalLight::DestroyRenderTargets()
{
	for (ZESize I = 0; I < MAX_CASCADE_COUNT; ++I)
	{
		if (ShadowMaps[I] != NULL)
		{
			ShadowMaps[I]->Destroy();
			ShadowMaps[I] = NULL;
		}
	}
}

ZELightType ZEDirectionalLight::GetLightType()
{
	return ZE_LT_DIRECTIONAL;
}

void ZEDirectionalLight::SetMaxShadowDepth(float Value)
{
	MaxShadowDepth = Value;
}

float ZEDirectionalLight::GetMaxShadowDepth() const
{
	return MaxShadowDepth;
}

void ZEDirectionalLight::SetSplitBias(float Bias)
{
	zeDebugCheck(Bias > 1.0f || Bias < 0.0f, "Bias out of 0 - 1 range.");
	SplitBias = Bias;
}

float ZEDirectionalLight::GetSplitBias() const
{
	return SplitBias;
}

void ZEDirectionalLight::SetCascadeCount(ZESize Count)
{
	zeDebugCheck(Count >= MAX_CASCADE_COUNT, "Count should be smaller MAX_CASCADE_COUNT");
	CascadeCount = Count;
}

ZESize ZEDirectionalLight::GetCascadeCount() const
{
	return CascadeCount;
}

float ZEDirectionalLight::GetSplitDistance(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT + 1, "Index out of range");
	return SplitDistances[Index];
}

ZETexture2D* ZEDirectionalLight::GetShadowMap(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT, "Index out of range");
	return ShadowMaps[Index];
}

ZESize ZEDirectionalLight::GetViewCount()
{
	return CascadeCount;
}

// Creates a world space view volume that represents a single cascade of camera frustum
const ZEViewVolume& ZEDirectionalLight::GetViewVolume(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT, "Index out of range");
	
	return ViewVolumes[Index];

// 	//For every cascade level
// 	ZEVector3 FrustumCorners[8];
// 	ZEVector4 TransformedFrustumCorners[8];
// 	
// 	// Calculate near and far plane corners of frustum
// 	float NearPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[Index];
// 	float FarPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[Index + 1];
// 	
// 	float FarPlaneHalfWidth = FarPlaneHalfHeight * AspectRatio;
// 	float NearPlaneHalfWidth = NearPlaneHalfHeight * AspectRatio;
// 	
// 	ZEVector3 NearPlaneCenter = Position + Front * SplitDistances[Index];
// 	ZEVector3 FarPlaneCenter = Position + Front * SplitDistances[Index + 1];
// 	
// 	ZEVector3 FarHalfUpVector = Up * FarPlaneHalfHeight;
// 	ZEVector3 NearHalfUpVector = Up * NearPlaneHalfHeight;
// 	ZEVector3 FarHalfRightVector = Right * FarPlaneHalfWidth;
// 	ZEVector3 NearHalfRightVector = Right * NearPlaneHalfWidth;
// 	
// 	// Create world space view volume
// 	zeCriticalError("This part is missing. Create final view volume here.");
// 	
// 	ZEVector3 FrustumCenter = (NearPlaneCenter + FarPlaneCenter) * 0.5f;
// 	float HalfDepth = ZEVector3::Length(NearPlaneCenter - FrustumCenter);
// 	float MaxDimension = ZEMath::Max(FarPlaneHalfWidth, ZEMath::Max(FarPlaneHalfHeight, HalfDepth));
// 	
// 	ZEOBBox OBBox(FrustumCenter, Right, Up, Front, MaxDimension);
}

const ZEMatrix4x4& ZEDirectionalLight::GetViewTransform(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT, "Index out of range");

	return LightTransformations[Index];
}

bool ZEDirectionalLight::Initialize()
{
	if (GetInitialized())
		return false;
	
	ZEEntity::Initialize();

	return true;
}

void ZEDirectionalLight::Deinitialize()
{
	if (!GetInitialized())
		return;

	DestroyRenderTargets();
}

void ZEDirectionalLight::RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer)
{
	if (!GetCastsShadow())
	{
		DestroyRenderTargets();
		return;
	}

	CreateRenderTargets();
	
	ShadowRenderer->SetFace(true);
	ShadowRenderer->SetLight(this);
	ShadowRenderer->SetViewPort(NULL); // Not used, shadow maps are queried by shadow renderer automatically

	ShadowRenderer->Render();
}

ZEDirectionalLight::ZEDirectionalLight()
{
	SplitBias = 0.7f;
	CascadeCount = 4;
	MaxShadowDepth = 400.0f;

	memset(ShadowMaps, NULL, sizeof(ZETexture2D*) * MAX_CASCADE_COUNT);
}

ZEDirectionalLight::~ZEDirectionalLight()
{
	Deinitialize();
}

ZEDirectionalLight* ZEDirectionalLight::CreateInstance()
{
	return new ZEDirectionalLight();
}

void ZEDirectionalLight::Draw(ZEDrawParameters* DrawParameters)
{
	// Update cascade transformations
	ZECamera* Camera = zeScene->GetActiveCamera();

	// Fetch constants
	float FarZ = MaxShadowDepth;
	float NearZ = Camera->GetNearZ();

	// Calculate split distances
	for (ZESize SplitN = 0; SplitN <= CascadeCount; ++SplitN)
	{
		float CascadeRatio = (float)SplitN / (float)CascadeCount;
		float LinearTerm = NearZ + (FarZ - NearZ) * CascadeRatio;
		float LogarithmicTerm = NearZ * ZEMath::Power(FarZ / NearZ, CascadeRatio);

		float SplitDistance = SplitBias * LogarithmicTerm + (1.0f - SplitBias) * LinearTerm;

		// Keep distances
		SplitDistances[SplitN] = SplitDistance;
	}

	// Fetch constants
	float FOV = Camera->GetFOV();
	float AspectRatio = Camera->GetAspectRatio();
	ZEVector3 Position = Camera->GetWorldPosition();
	ZEQuaternion Rotation = Camera->GetWorldRotation();
		
	ZEVector3 Up = Camera->GetWorldUp();
	ZEVector3 Right = Camera->GetWorldRight();
	ZEVector3 Front = Camera->GetWorldFront();

	// For every cascade level
	ZEVector4 FrustumCorners[8];
	ZEVector4 TransformedFrustumCorners[8];

	// Calculate near and far plane corners of frustum
	for (ZESize CascadeN = 0; CascadeN < CascadeCount; ++CascadeN)
	{
		ZEVector3 NearPlaneCenter = Position + Front * SplitDistances[CascadeN];
		ZEVector3 FarPlaneCenter = Position + Front * SplitDistances[CascadeN + 1];

		float NearPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[CascadeN];
		float NearPlaneHalfWidth = NearPlaneHalfHeight * AspectRatio;

		float FarPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[CascadeN + 1];
		float FarPlaneHalfWidth = FarPlaneHalfHeight * AspectRatio;

		ZEVector3 FarHalfUpVector = Up * FarPlaneHalfHeight;
		ZEVector3 NearHalfUpVector = Up * NearPlaneHalfHeight;
		ZEVector3 FarHalfRightVector = Right * FarPlaneHalfWidth;
		ZEVector3 NearHalfRightVector = Right * NearPlaneHalfWidth;

		FrustumCorners[0] = ZEVector4(NearPlaneCenter - NearHalfRightVector - NearHalfUpVector, 1.0f);	// Near Left Down
		FrustumCorners[1] = ZEVector4(NearPlaneCenter - NearHalfRightVector + NearHalfUpVector, 1.0f);	// Near Left Up
		FrustumCorners[2] = ZEVector4(NearPlaneCenter + NearHalfRightVector + NearHalfUpVector, 1.0f);	// Near Right Up
		FrustumCorners[3] = ZEVector4(NearPlaneCenter + NearHalfRightVector - NearHalfUpVector, 1.0f);	// Near Right Down

		FrustumCorners[4] = ZEVector4(FarPlaneCenter - FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  Left Down
		FrustumCorners[5] = ZEVector4(FarPlaneCenter - FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far  Left Up
		FrustumCorners[6] = ZEVector4(FarPlaneCenter + FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far  Right Up
		FrustumCorners[7] = ZEVector4(FarPlaneCenter + FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  Right Down

		// Calculate light view proj transform
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateLookAtTransform(LightViewMatrix, -GetWorldFront(), ZEVector3::Zero, GetWorldUp());
	
		// No need to use accurate near and far z values since we will use a crop matrix
		ZEMatrix4x4 LightProjMatrix;
		float FarPlaneHeight = 2.0f * ZEAngle::Tan(FOV * 0.5f) * MaxShadowDepth;
		ZEMatrix4x4::CreateOrthographicProjection(LightProjMatrix, FarPlaneHeight * AspectRatio, FarPlaneHeight, 0.01f, 50.0f);

		ZEMatrix4x4 LightViewProjMatrix = LightProjMatrix * LightViewMatrix;
	
		// Transform world positions to light clip space
		for (ZESize I = 0; I < 8; ++I)
		{
			ZEMatrix4x4::Transform(TransformedFrustumCorners[I], LightViewProjMatrix, FrustumCorners[I]);
			TransformedFrustumCorners[I] /= TransformedFrustumCorners[I].w;
		}

		// Find min max of transformed vertices
		ZEVector3 Min(FLT_MAX, FLT_MAX, FLT_MAX);
		ZEVector3 Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (ZESize I = 0; I < 8; ++I)
		{
			if (TransformedFrustumCorners[I].x < Min.x)	Min.x = TransformedFrustumCorners[I].x;
			if (TransformedFrustumCorners[I].y < Min.y) Min.y = TransformedFrustumCorners[I].y;
			if (TransformedFrustumCorners[I].z < Min.z) Min.z = TransformedFrustumCorners[I].z;
			if (TransformedFrustumCorners[I].x > Max.x) Max.x = TransformedFrustumCorners[I].x;
			if (TransformedFrustumCorners[I].y > Max.y) Max.y = TransformedFrustumCorners[I].y;
			if (TransformedFrustumCorners[I].z > Max.z) Max.z = TransformedFrustumCorners[I].z;
		}

		// Create crop matrix
		float ScaleX = 2.0f / (Max.x - Min.x);
		float ScaleY = 2.0f / (Max.y - Min.y);
		float ScaleZ = 1.0f / (Max.z - Min.z);
		float OffsetX = -0.5f * (Max.x + Min.x) * ScaleX;
		float OffsetY = -0.5f * (Max.y + Min.y) * ScaleY;
		float OffsetZ = -1.0f * Min.z * ScaleZ;

		ZEMatrix4x4 CropMatrix(	ScaleX, 0.0f,	0.0f,	OffsetX,
								0.0f,	ScaleY,	0.0f,	OffsetY,
								0.0f,	0.0f,	ScaleZ, OffsetZ,
								0.0f,	0.0f,	0.0f,	1.0f);

		// Final transformation matrix which is crop * proj * view
		LightTransformations[CascadeN] = CropMatrix * LightViewProjMatrix;

		// Create final clip space corners
		for (ZESize I = 0; I < 8; ++I)
		{
			ZEMatrix4x4::Transform(TransformedFrustumCorners[I], LightTransformations[CascadeN], FrustumCorners[I]);
			TransformedFrustumCorners[I] /= TransformedFrustumCorners[I].w;
		}

		// Create world space View Volumes

		// Do not include Crop matrix
		ZEMatrix4x4 LightInvViewProjMatrix;
		ZEMatrix4x4::Inverse(LightInvViewProjMatrix, LightViewProjMatrix);

		// Create world space corners
		for (ZESize I = 0; I < 8; ++I)
		{
			ZEMatrix4x4::Transform(FrustumCorners[I], LightInvViewProjMatrix, TransformedFrustumCorners[I]);
			FrustumCorners[I] /= FrustumCorners[I].w;
		}

		ZEVector4 Temp0 = (FrustumCorners[0] + FrustumCorners[6]) * 0.5f;	// (near-left-bottom + far-right-top) / 2 = Diagonal / 2
		ZEVector3 Center = ZEVector3(Temp0.x, Temp0.y, Temp0.z);
		ZEVector4 Temp1 = (FrustumCorners[0] - FrustumCorners[3]) * 0.5f;	// (near-left-bottom - near-right-bottom) / 2 = width / 2
		ZEVector3 Half = ZEVector3(Temp1.x, Temp1.y, Temp1.z);

		ZEOBBox OBBox(Center, Right, Up, Front, Half);
		ViewVolumes[CascadeN].Create(OBBox);
	}

	ZELight::Draw(DrawParameters);
}
