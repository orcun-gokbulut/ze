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

// Returns world space view volume(for culling purposes) of directional light which contains the cascade represented by Index
const ZEViewVolume& ZEDirectionalLight::GetViewVolume(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT, "Index out of range");
	return ViewVolumes[Index];
}

// Returns the transformation matrix(crop * proj * view) of light's cascade represented by Index
const ZEMatrix4x4& ZEDirectionalLight::GetViewTransform(ZESize Index)
{
	zeDebugCheck(Index >= MAX_CASCADE_COUNT, "Index out of range");
	return ShadowTransforms[Index];
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

ZEDirectionalLight::ZEDirectionalLight()
{
	SplitBias = 0.95f;
	CascadeCount = 4;

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
	if (DrawParameters->Pass != ZE_RP_COLOR)
		return;

	ZELight::Draw(DrawParameters);

	if (!CastsShadows)
		return;

	CreateRenderTargets();

	// Update cascade transformations
	ZECamera* Camera = DrawParameters->View->Camera;

	// Fetch constants
	float NearZ = Camera->GetNearZ();
	float FarZ = Camera->GetShadowDistance();
	
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
	SplitDistances[0] = NearZ;
	SplitDistances[CascadeCount] = FarZ;

	// Fetch constants
	float FOV = Camera->GetFOV();
	float AspectRatio = Camera->GetAspectRatio();
	ZEVector3 Position = Camera->GetWorldPosition();
	ZEQuaternion Rotation = Camera->GetWorldRotation();

	ZEVector3 CameraUp = Camera->GetWorldUp();
	ZEVector3 CameraRight = Camera->GetWorldRight();
	ZEVector3 CameraFront = Camera->GetWorldFront();

	// For every cascade level
	ZEVector4 FrustumCorners[8];
	ZEVector4 TransformedFrustumCorners[8];

	// Calculate near and far plane corners of frustum
	for (ZESize CascadeN = 0; CascadeN < CascadeCount; ++CascadeN)
	{
		ZEVector3 NearPlaneCenter = Position + CameraFront * SplitDistances[CascadeN];
		ZEVector3 FarPlaneCenter = Position + CameraFront * SplitDistances[CascadeN + 1];

		float NearPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[CascadeN];
		float NearPlaneHalfWidth = NearPlaneHalfHeight * AspectRatio;

		float FarPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[CascadeN + 1];
		float FarPlaneHalfWidth = FarPlaneHalfHeight * AspectRatio;

		ZEVector3 FarHalfUpVector = CameraUp * FarPlaneHalfHeight;
		ZEVector3 NearHalfUpVector = CameraUp * NearPlaneHalfHeight;
		ZEVector3 FarHalfRightVector = CameraRight * FarPlaneHalfWidth;
		ZEVector3 NearHalfRightVector = CameraRight * NearPlaneHalfWidth;

		FrustumCorners[0] = ZEVector4(NearPlaneCenter - NearHalfRightVector - NearHalfUpVector, 1.0f);	// Near Left Down
		FrustumCorners[1] = ZEVector4(NearPlaneCenter - NearHalfRightVector + NearHalfUpVector, 1.0f);	// Near Left Up
		FrustumCorners[2] = ZEVector4(NearPlaneCenter + NearHalfRightVector + NearHalfUpVector, 1.0f);	// Near Right Up
		FrustumCorners[3] = ZEVector4(NearPlaneCenter + NearHalfRightVector - NearHalfUpVector, 1.0f);	// Near Right Down

		FrustumCorners[4] = ZEVector4(FarPlaneCenter - FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  Left Down
		FrustumCorners[5] = ZEVector4(FarPlaneCenter - FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far  Left CameraUp
		FrustumCorners[6] = ZEVector4(FarPlaneCenter + FarHalfRightVector + FarHalfUpVector, 1.0f);		// Far Right CameraUp
		FrustumCorners[7] = ZEVector4(FarPlaneCenter + FarHalfRightVector - FarHalfUpVector, 1.0f);		// Far  CameraRight Down

		// Calculate light view proj transform
		// Look at transform should contain light's frustum depth info
		ZEMatrix4x4 LightViewMatrix;
		ZEMatrix4x4::CreateLookAtTransform(LightViewMatrix, -GetWorldFront() * Range, ZEVector3::Zero, GetWorldUp());

		// No need to use accurate near and far z values, crop matrix will make those values meaningless
		ZEMatrix4x4 LightProjMatrix;
		float FarPlaneHeight = 2.0f * ZEAngle::Tan(FOV * 0.5f) * FarZ;
		ZEMatrix4x4::CreateOrthographicProjection(LightProjMatrix, FarPlaneHeight * AspectRatio, FarPlaneHeight, 1.0f, 10.0f);

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

		// Pull z back, to avoid casters between light and light's near z
		Min.z = 0.0f;

		// Create crop matrix
		float ScaleX = 2.0f / (Max.x - Min.x);
		float ScaleY = 2.0f / (Max.y - Min.y);
		float ScaleZ = 1.0f / (Max.z - Min.z);
		float OffsetX = -0.5f * (Max.x + Min.x) * ScaleX;
		float OffsetY = -0.5f * (Max.y + Min.y) * ScaleY;
		float OffsetZ = -Min.z * ScaleZ;

		ZEMatrix4x4 CropMatrix(	ScaleX, 0.0f,	0.0f,	OffsetX,
								0.0f,	ScaleY,	0.0f,	OffsetY,
								0.0f,	0.0f,	ScaleZ, OffsetZ,
								0.0f,	0.0f,	0.0f,	1.0f);

		// Final transformation matrix which is crop * proj * view
		ShadowTransforms[CascadeN] = CropMatrix * LightViewProjMatrix;

		// Clip Space Corners
		static const ZEVector4 ClipCorners[8] = {
			ZEVector4(-1.0f, -1.0f, 0.0f, 1.0f),	// Near Left Down
			ZEVector4(-1.0f, +1.0f, 0.0f, 1.0f),	// Near Left Up
			ZEVector4(+1.0f, +1.0f, 0.0f, 1.0f),	// Near Right Up
			ZEVector4(+1.0f, -1.0f, 0.0f, 1.0f),	// Near Right Down
			ZEVector4(-1.0f, -1.0f, 1.0f, 1.0f),	// Far  Left Down
			ZEVector4(-1.0f, +1.0f, 1.0f, 1.0f),	// Far  Left Up
			ZEVector4(+1.0f, +1.0f, 1.0f, 1.0f),	// Far  Right Up
			ZEVector4(+1.0f, -1.0f, 1.0f, 1.0f)		// Far  Right Down
		};

		ZEMatrix4x4 LightInvViewProjCrop;
		ZEMatrix4x4::Inverse(LightInvViewProjCrop, ShadowTransforms[CascadeN]);

		// World Space Corners
		ZEVector4 WorldCorners[8];
		for (ZESize I = 0; I < 8; ++I)
		{
			ZEMatrix4x4::Transform(WorldCorners[I], LightInvViewProjCrop, ClipCorners[I]);
			WorldCorners[I] /= WorldCorners[I].w;
		}

		ZEVector3 ViewUp		= ((WorldCorners[1] - WorldCorners[0]).ToVector3()).Normalize();
		ZEVector3 ViewFront		= ((WorldCorners[4] - WorldCorners[0]).ToVector3()).Normalize();
		ZEVector3 ViewRight		= ((WorldCorners[3] - WorldCorners[0]).ToVector3()).Normalize();
		ZEVector3 ViewCenter	= (WorldCorners[6] + WorldCorners[0]).ToVector3() * 0.5f;

		float ViewHalfDepth		= ((WorldCorners[4] - WorldCorners[0]) * 0.5f).ToVector3().Length();
		float ViewHalfWidth		= ((WorldCorners[3] - WorldCorners[0]) * 0.5f).ToVector3().Length();
		float ViewHalfHeight	= ((WorldCorners[1] - WorldCorners[0]) * 0.5f).ToVector3().Length();
		ZEVector3 ViewHalfSize	= ZEVector3(ViewHalfWidth, ViewHalfHeight, ViewHalfDepth);

		ZEOBBox OBBox(ViewCenter, ViewRight, ViewUp, ViewFront, ViewHalfSize);
		ViewVolumes[CascadeN].Create(OBBox);
	}

	ZEShadowRenderer* ShadowRenderer = (ZEShadowRenderer*)zeScene->GetShadowRenderer();

	for (ZESize CascadeN = 0; CascadeN < CascadeCount; ++CascadeN)
	{
		ShadowRenderer->ClearLists();
	
		ZEDrawParameters ShadowDrawParameters;
		ShadowDrawParameters.ElapsedTime = DrawParameters->ElapsedTime;
		ShadowDrawParameters.FrameId = DrawParameters->FrameId;
		ShadowDrawParameters.Pass = ZE_RP_SHADOW_MAP;
		ShadowDrawParameters.Renderer = ShadowRenderer;
		ShadowDrawParameters.ViewVolume = &ViewVolumes[CascadeN];
		ShadowDrawParameters.View = DrawParameters->View;
		ShadowDrawParameters.CustomData = &ShadowTransforms[CascadeN];
		ShadowDrawParameters.ViewPort = ShadowMaps[CascadeN]->GetViewPort();

		zeScene->GetSceneCuller().CullScene(zeScene, &ShadowDrawParameters);

		
		ShadowRenderer->SetLight(this);
		ShadowRenderer->SetDrawParameters(&ShadowDrawParameters);

		ShadowRenderer->Render();
	}
}
