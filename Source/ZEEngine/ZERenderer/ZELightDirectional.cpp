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

#include "ZELightDirectional.h"
#include "ZERNCuller.h"
#include "ZERNView.h"
#include "ZEDS/ZEArray.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEMath.h"
#include "ZERNRenderParameters.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERNStageShadowmapGeneration.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRContext.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8
#define ZE_LDF_CONSTANT_BUFFER			16

void ZELightDirectional::UpdateCascadeTransforms(ZEScene* Scene, const ZERNView& View)
{

	ZEAABBox SceneAABBLight;
	CalculateSceneBoundingBoxLight(&SceneAABBLight, Scene);

	ZEViewFrustum* Frustum = (ZEViewFrustum*)View.ViewVolume;
	float ViewFrustumFovTangent = ZEAngle::Tan(Frustum->GetFOV() * 0.5f);
	float ViewFrustumAspectRatio = Frustum->GetAspectRatio();

	ZEVector3 CascadeFrustumVerticesView[8];
	ZEVector3 CascadeFrustumVerticesLight[8];

	for(ZEUInt CascadeIndex = 0; CascadeIndex < CascadeConstants.CascadeCount; CascadeIndex++)
	{
		ZECascade& Cascade = CascadeConstants.Cascades[CascadeIndex];

		float CascadeNumberDividedByCount = (float)(CascadeIndex + 1) / CascadeConstants.CascadeCount;
		Cascade.NearZView = (CascadeIndex == 0) ? View.NearZ : CascadeConstants.Cascades[CascadeIndex - 1].FarZView;
		Cascade.FarZView = CascadeDistanceFactor * (View.NearZ * ZEMath::Power(View.ShadowDistance / View.NearZ, CascadeNumberDividedByCount)) + 
				  (1.0f - CascadeDistanceFactor) * (View.NearZ + (View.ShadowDistance - View.NearZ) * CascadeNumberDividedByCount);

		float FarY = Cascade.FarZView * ViewFrustumFovTangent;
		float FarX = FarY * ViewFrustumAspectRatio;

		float NearZDividedByFarZ = Cascade.NearZView / Cascade.FarZView;
		float NearX = FarX * NearZDividedByFarZ;
		float NearY = FarY * NearZDividedByFarZ;

		CascadeFrustumVerticesView[0] = ZEVector3(-NearX, -NearY, Cascade.NearZView);
		CascadeFrustumVerticesView[1] = ZEVector3(-NearX, NearY, Cascade.NearZView);
		CascadeFrustumVerticesView[2] = ZEVector3(NearX, NearY, Cascade.NearZView);
		CascadeFrustumVerticesView[3] = ZEVector3(NearX, -NearY, Cascade.NearZView);

		CascadeFrustumVerticesView[4] = ZEVector3(-FarX, -FarY, Cascade.FarZView);
		CascadeFrustumVerticesView[5] = ZEVector3(-FarX, FarY, Cascade.FarZView);
		CascadeFrustumVerticesView[6] = ZEVector3(FarX, FarY, Cascade.FarZView);
		CascadeFrustumVerticesView[7] = ZEVector3(FarX, -FarY, Cascade.FarZView);

		ZEAABBox CascadeFrustumAABBLight(ZEVector3(FLT_MAX), ZEVector3(FLT_MIN));
		for(ZEUInt I = 0; I < 8; I++)
		{
			CascadeFrustumVerticesLight[I] = GetViewTransform() * View.InvViewTransform * CascadeFrustumVerticesView[I];

			ZEVector3::Min(CascadeFrustumAABBLight.Min, CascadeFrustumAABBLight.Min, CascadeFrustumVerticesLight[I]);
			ZEVector3::Max(CascadeFrustumAABBLight.Max, CascadeFrustumAABBLight.Max, CascadeFrustumVerticesLight[I]);
		}

		//Find the radius of minimal enclosing circle of Cascade frustum x-y coordinates in light space
		//Offset cascade frustum aabb in light space to enclose the circle
		float Radius = ZEVector3::Length(CascadeFrustumVerticesLight[0] - CascadeFrustumVerticesLight[6]);
		ZEVector3 Offset = (ZEVector3(Radius) - (CascadeFrustumAABBLight.Max - CascadeFrustumAABBLight.Min)) * 0.5f;

		Offset.z = 0.0f;

		CascadeFrustumAABBLight.Max += Offset;
		CascadeFrustumAABBLight.Min -= Offset;

		//round cascade frustum aabb x-y coordinate in light space to corresponding shadow texel unit
		float UnitPerShadowTexel = Radius / CascadeShadowMaps->GetWidth();

		CascadeFrustumAABBLight.Min.x /= UnitPerShadowTexel;
		CascadeFrustumAABBLight.Min.y /= UnitPerShadowTexel;

		CascadeFrustumAABBLight.Min.x = ZEMath::Floor(CascadeFrustumAABBLight.Min.x);
		CascadeFrustumAABBLight.Min.y = ZEMath::Floor(CascadeFrustumAABBLight.Min.y);

		CascadeFrustumAABBLight.Min.x *= UnitPerShadowTexel;
		CascadeFrustumAABBLight.Min.y *= UnitPerShadowTexel;

		CascadeFrustumAABBLight.Max.x /= UnitPerShadowTexel;
		CascadeFrustumAABBLight.Max.y /= UnitPerShadowTexel;

		CascadeFrustumAABBLight.Max.x = ZEMath::Floor(CascadeFrustumAABBLight.Max.x);
		CascadeFrustumAABBLight.Max.y = ZEMath::Floor(CascadeFrustumAABBLight.Max.y);

		CascadeFrustumAABBLight.Max.x *= UnitPerShadowTexel;
		CascadeFrustumAABBLight.Max.y *= UnitPerShadowTexel;

		//Extend cascade frustum aabb minz and maxz to scene aabb minz and maxz in light space, if required
		CascadeFrustumAABBLight.Min.z = ZEMath::Min(CascadeFrustumAABBLight.Min.z, SceneAABBLight.Min.z);
		CascadeFrustumAABBLight.Max.z = ZEMath::Max(CascadeFrustumAABBLight.Max.z, SceneAABBLight.Max.z);

		float Width = CascadeFrustumAABBLight.Max.x - CascadeFrustumAABBLight.Min.x;
		float Height = CascadeFrustumAABBLight.Max.y - CascadeFrustumAABBLight.Min.y;
		float Depth = CascadeFrustumAABBLight.Max.z - CascadeFrustumAABBLight.Min.z;
		float OffsetX = (CascadeFrustumAABBLight.Max.x + CascadeFrustumAABBLight.Min.x) / -Width;
		float OffsetY = (CascadeFrustumAABBLight.Max.y + CascadeFrustumAABBLight.Min.y) / -Height;
		float OffsetZ = CascadeFrustumAABBLight.Min.z / -Depth;

		ZEMatrix4x4::Create(Cascade.ProjectionTransform,
			2.0f / Width, 0.0f, 0.0f, OffsetX,
			0.0f, 2.0f / Height, 0.0f, OffsetY,
			0.0f, 0.0f, 1.0f / Depth, OffsetZ,
			0.0f, 0.0f, 0.0f, 1.0f);

		if(CascadeIndex != (CascadeConstants.CascadeCount - 1))
		{
			float BandLength = (Cascade.FarZView - Cascade.NearZView) * 0.05f;
			Cascade.Band = ZEVector2(Cascade.FarZView - BandLength, Cascade.FarZView + BandLength);
		}
		else
		{
			Cascade.Band = ZEVector2(Cascade.FarZView - View.ShadowFadeDistance, Cascade.FarZView);
		}

		CascadeVolumes[CascadeIndex].Create(ZEVector3::Zero, GetWorldRotation(), Width, Height, CascadeFrustumAABBLight.Min.z, CascadeFrustumAABBLight.Max.z);

	}
}

void ZELightDirectional::UpdateCascadeShadowMaps()
{
	if(!DirtyFlags.GetFlags(ZE_LDF_SHADOW_MAP))
		return;

	ZEUInt Size = ZELight::ConvertShadowResolution(ShadowResolution);

	CascadeShadowMaps.Release();
	CascadeShadowMaps = ZEGRTexture2D::CreateInstance(Size, Size, CascadeConstants.CascadeCount, 1, ZEGR_TF_D32_FLOAT, false, true);

	DirtyFlags.UnraiseFlags(ZE_LDF_SHADOW_MAP);
}

void ZELightDirectional::CalculateSceneBoundingBoxLight(ZEAABBox* Out, ZEScene* Scene)
{
	ZEAABBox SceneAABBWorld(ZEVector3(FLT_MAX), ZEVector3(FLT_MIN));

	const ZESmartArray<ZEEntity*>& Entities = Scene->GetEntities();
	ZEUInt EntityCount = Entities.GetCount();
	for(ZEUInt I = 0; I < EntityCount; I++)
	{
		ZEEntity* Entity = Entities[I];
		if(Entity->GetDrawFlags().GetFlags(ZE_DF_DRAW | ZE_DF_LIGHT_RECEIVER))
			ZEAABBox::Combine(SceneAABBWorld, Entity->GetWorldBoundingBox(), SceneAABBWorld);
	}

	ZEAABBox SceneAABBLight(ZEVector3(FLT_MAX), ZEVector3(FLT_MIN));
	for(ZEUInt I = 0; I < 8; I++)
	{
		ZEVector3 SceneAABBVertexLight = GetViewTransform() * SceneAABBWorld.GetVertex(I);

		ZEVector3::Min(SceneAABBLight.Min, SceneAABBLight.Min, GetViewTransform() * SceneAABBVertexLight);
		ZEVector3::Max(SceneAABBLight.Max, SceneAABBLight.Max, GetViewTransform() * SceneAABBVertexLight);
	}

	*Out = SceneAABBLight;
}


ZELightDirectional::ZELightDirectional()
{
	CascadeDistanceFactor = 0.5f;

	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_SHADOWING;
}

ZELightDirectional::~ZELightDirectional()
{
	DeinitializeSelf();
}

bool ZELightDirectional::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	CascadeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZECascadeConstants));

	ShadowRenderer.AddStage(new ZERNStageShadowmapGeneration());
	ShadowRenderer.Initialize();

	return true;
}

bool ZELightDirectional::DeinitializeSelf()
{
	CascadeConstantBuffer.Release();
	CascadeShadowMaps.Release();
	CascadeVolumes.Clear();

	ShadowRenderer.Deinitialize();

	return ZEEntity::DeinitializeSelf();
}

void ZELightDirectional::SetCascadeCount(ZEUInt CascadeCount)
{
	if(CascadeConstants.CascadeCount == CascadeCount)
		return;

	CascadeConstants.CascadeCount = CascadeCount;
	CascadeVolumes.Resize(CascadeCount);

	DirtyFlags.RaiseFlags(ZE_LDF_CONSTANT_BUFFER);
}

ZEUInt ZELightDirectional::GetCascadeCount() const
{
	return CascadeConstants.CascadeCount;
}

void ZELightDirectional::SetCascadeDistanceFactor(float CascadeDistanceFactor)
{
	this->CascadeDistanceFactor = CascadeDistanceFactor;
}

float ZELightDirectional::GetCascadeDistanceFactor() const
{
	return CascadeDistanceFactor;
}

ZELightType ZELightDirectional::GetLightType() const
{
	return ZE_LT_DIRECTIONAL;
}

ZELightDirectional* ZELightDirectional::CreateInstance()
{
	return new ZELightDirectional();
}

ZESize ZELightDirectional::GetViewCount()
{
	return 1;
}

ZEGRTexture* ZELightDirectional::GetShadowMap(ZESize Index) const
{
	if(Index >= CascadeConstants.CascadeCount)
		return NULL;

	return CascadeShadowMaps;
}

const ZEViewVolume& ZELightDirectional::GetViewVolume(ZESize Index)
{
	if(Index >= CascadeConstants.CascadeCount)
		return ZEViewCuboid();

	return CascadeVolumes[Index];
}

const ZEMatrix4x4& ZELightDirectional::GetViewTransform(ZESize Index)
{
	if(DirtyFlags.GetFlags(ZE_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, ZEVector3::Zero, GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightDirectional::GetProjectionTransform(ZESize Index)
{
	if(Index >= CascadeConstants.CascadeCount)
		return ProjectionTransform;

	return CascadeConstants.Cascades[Index].ProjectionTransform;
}

bool ZELightDirectional::PreRender(const ZERNCullParameters* CullParameters)
{
	return ZELight::PreRender(CullParameters);
}

void ZELightDirectional::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	UpdateCascadeShadowMaps();
	UpdateCascadeTransforms(Parameters->Scene, *Parameters->View);

	ZEUInt Count = (ZEUInt)CascadeConstants.CascadeCount;
	for(ZEUInt CascadeIndex = 0; CascadeIndex < Count; CascadeIndex++)
	{
		ZERNView View = ShadowRenderer.GetView();
		View.Position = ZEVector3::Zero;
		View.Rotation = GetWorldRotation();
		View.Direction = GetWorldFront();
		View.U = GetWorldRight();
		View.V = GetWorldUp();
		View.N = GetWorldFront();

		View.ViewVolume = &GetViewVolume(CascadeIndex);
		View.ViewProjectionTransform = GetProjectionTransform(CascadeIndex) * GetViewTransform();

		ShadowRenderer.SetView(View);

		ZEGRContext* Context = Parameters->Context;

		ZEGRDepthStencilBuffer* DepthBuffer = CascadeShadowMaps->GetDepthStencilBuffer(CascadeIndex);
		Context->ClearDepthStencilBuffer(DepthBuffer, true, false, 1.0f, 0x00);
		Context->SetRenderTargets(0, NULL, DepthBuffer);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

		ZELight::Render(Parameters, Command);
	}
}
