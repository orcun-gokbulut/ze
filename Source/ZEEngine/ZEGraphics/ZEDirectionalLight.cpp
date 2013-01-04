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

ZETexture2D* ZEDirectionalLight::GetShadowMap(ZESize CascadeIndex)
{
	zeDebugCheck(CascadeIndex >= MAX_CASCADE_COUNT, "Index out of range");

	return ShadowMaps[CascadeIndex];
}

const ZEViewVolume& ZEDirectionalLight::GetViewVolume()
{
	ZEViewCuboid Volume;
	if (UpdateViewVolume)
	{
		UpdateViewVolume = false;
	}

	return Volume;
}

const ZEMatrix4x4& ZEDirectionalLight::GetLightTransformation(ZESize CascadeIndex)
{
	zeDebugCheck(CascadeIndex >= MAX_CASCADE_COUNT, "Index out of range");

	static const ZEUInt CascadeUpdateIntervals[4] = {2, 4, 6, 8};

	//if (UpdateShadowTransform)
	{
		ZECamera* Camera = zeScene->GetActiveCamera();

		// Fetch constants
		float FarZ = 500.0f; // far z for shadows are smaller than camera far z
		float NearZ = Camera->GetNearZ();

		if (!SplitsInitialized)
		{
			// Calculate split depths
			for (ZESize SplitN = 0; SplitN <= CascadeCount; ++SplitN)
			{
			
					float CascadeRatio = (float)SplitN / (float)CascadeCount;
					float LinearTerm = NearZ + (FarZ - NearZ) * CascadeRatio;
					float LogarithmicTerm = NearZ * ZEMath::Power(FarZ / NearZ, CascadeRatio);

					float SplitDistance = SplitBias * LogarithmicTerm + (1.0f - SplitBias) * LinearTerm;

					// Keep distances
					SplitDistances[SplitN] = SplitDistance;
			}
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
		ZEVector3 FrustumCorners[8];
		ZEVector4 TransformedFrustumCorners[8];

		for (ZESize FrustumN = 0; FrustumN < CascadeCount; ++FrustumN)
		{
			if (zeCore->GetFrameId() % CascadeUpdateIntervals[FrustumN] == 0)
			{
				// Calculate near and far plane corners of frustum
				float NearPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[FrustumN];
				float NearPlaneHalfWidth = NearPlaneHalfHeight * AspectRatio;

				float FarPlaneHalfHeight = ZEAngle::Tan(FOV * 0.5f) * SplitDistances[FrustumN+1];
				float FarPlaneHalfWidth = FarPlaneHalfHeight * AspectRatio;

				ZEVector3 NearPlaneCenter = Position + Front * SplitDistances[FrustumN];
				ZEVector3 FarPlaneCenter = Position + Front * SplitDistances[FrustumN+1];

				FrustumCorners[0] = ZEVector3(NearPlaneCenter - Right * NearPlaneHalfWidth - Up * NearPlaneHalfHeight); // Near Plane | Left Down
				FrustumCorners[1] = ZEVector3(NearPlaneCenter - Right * NearPlaneHalfWidth + Up * NearPlaneHalfHeight);	// Near Plane | Left Up
				FrustumCorners[2] = ZEVector3(NearPlaneCenter + Right * NearPlaneHalfWidth + Up * NearPlaneHalfHeight);	// Near Plane | Right Up
				FrustumCorners[3] = ZEVector3(NearPlaneCenter + Right * NearPlaneHalfWidth - Up * NearPlaneHalfHeight);	// Near Plane | Right Down

				FrustumCorners[4] = ZEVector3(FarPlaneCenter - Right * FarPlaneHalfWidth - Up * FarPlaneHalfHeight);	// Far Plane | Left Down
				FrustumCorners[5] = ZEVector3(FarPlaneCenter - Right * FarPlaneHalfWidth + Up * FarPlaneHalfHeight);	// Far Plane | Left Up
				FrustumCorners[6] = ZEVector3(FarPlaneCenter + Right * FarPlaneHalfWidth + Up * FarPlaneHalfHeight);	// Far Plane | Right Up
				FrustumCorners[7] = ZEVector3(FarPlaneCenter + Right * FarPlaneHalfWidth - Up * FarPlaneHalfHeight);	// Far Plane | Right Down

				// Calculate light view proj transform
				ZEMatrix4x4 LightViewMatrix;
				ZEMatrix4x4::CreateLookAtTransform(LightViewMatrix, -GetWorldFront(), ZEVector3::Zero, GetWorldUp());
			
				// NOTE: near z and far z for light projection matrix are hard coded, later it can be added to interface
				ZEMatrix4x4 LightProjMatrix;
				float FarPlaneHeight = 2.0f * ZEAngle::Tan(FOV * 0.5f) * FarZ;
				ZEMatrix4x4::CreateOrthographicProjection(LightProjMatrix, FarPlaneHeight * AspectRatio, FarPlaneHeight, 0.01f, 500.0f);

				ZEMatrix4x4 LightViewProjMatrix = LightProjMatrix * LightViewMatrix;
			
				// Transform world positions to light clip space
				for (ZESize I = 0; I < 8; ++I)
				{
					ZEMatrix4x4::Transform(TransformedFrustumCorners[I], LightViewProjMatrix, ZEVector4(FrustumCorners[I], 1.0f));
					TransformedFrustumCorners[I] /= TransformedFrustumCorners[I].w;
				}

				// Find min max of transformed vertices
				ZEVector3 Min(FLT_MAX, FLT_MAX, FLT_MAX);
				ZEVector3 Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
				for (ZESize I = 0; I < 8; ++I)
				{
					if (TransformedFrustumCorners[I].x < Min.x)
						Min.x = TransformedFrustumCorners[I].x;
					if (TransformedFrustumCorners[I].y < Min.y)
						Min.y = TransformedFrustumCorners[I].y;
					if (TransformedFrustumCorners[I].z < Min.z)
						Min.z = TransformedFrustumCorners[I].z;
					if (TransformedFrustumCorners[I].x > Max.x)
						Max.x = TransformedFrustumCorners[I].x;
					if (TransformedFrustumCorners[I].y > Max.y)
						Max.y = TransformedFrustumCorners[I].y;
					if (TransformedFrustumCorners[I].z > Max.z)
						Max.z = TransformedFrustumCorners[I].z;
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

				// Final transformation matrix
				LightTransformations[FrustumN] = CropMatrix * LightViewProjMatrix;
			}
		}

		UpdateShadowTransform = false;
	}

	return LightTransformations[CascadeIndex];
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
	ShadowRenderer->SetViewPort(NULL); // Shadow renderer will get each render target

	ShadowRenderer->Render();
}

ZEDirectionalLight::ZEDirectionalLight()
{
	CascadeCount = 4;
	SplitBias = 0.8f;
	SplitsInitialized = false;

	for (ZESize I = 0; I < MAX_CASCADE_COUNT; ++I)
	{
		ShadowMaps[I] = NULL;
		SplitDistances[I] = 0.0f;
		LightTransformations[I] = ZEMatrix4x4::Identity;
	}

	SplitDistances[MAX_CASCADE_COUNT] = 0.0f;
}

ZEDirectionalLight::~ZEDirectionalLight()
{
	Deinitialize();
}

ZEDirectionalLight* ZEDirectionalLight::CreateInstance()
{
	return new ZEDirectionalLight();
}
