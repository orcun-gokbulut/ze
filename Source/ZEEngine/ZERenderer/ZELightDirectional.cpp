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

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEMath.h"
#include "ZEMath/ZEAngle.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRConstantBuffer.h"
#include "ZEGraphics/ZEGRDepthStencilBuffer.h"
#include "ZERNCuller.h"
#include "ZERNView.h"
#include "ZERNRenderParameters.h"
#include "ZERNStage.h"
#include "ZECamera.h"

#define ZE_LDF_VIEW_TRANSFORM			1
#define ZE_LDF_PROJECTION_TRANSFORM		2
#define ZE_LDF_SHADOW_MAP				4
#define ZE_LDF_VIEW_VOLUME				8
#define ZE_LDF_CONSTANT_BUFFER			16

void ZELightDirectional::UpdateCascadeTransforms(const ZERNView& View)
{
	float VerticalTopFovTangent = ZEAngle::Tan(View.VerticalFOVTop);
	float VerticalBottomFovTangent = ZEAngle::Tan(View.VerticalFOVBottom);
	float HorizontalRightTangent = ZEAngle::Tan(View.HorizontalFOVRight);
	float HorizontalLeftTangent = ZEAngle::Tan(View.HorizontalFOVLeft);

	ZEVector3 CascadeFrustumVerticesView[8];
	ZEVector3 CascadeFrustumVerticesWorld[8];
	for (ZEUInt CascadeIndex = 0; CascadeIndex < CascadeConstants.CascadeCount; CascadeIndex++)
	{
		ZECascade& Cascade = CascadeConstants.Cascades[CascadeIndex];

		float CascadeNumberDividedByCount = (float)(CascadeIndex + 1) / CascadeConstants.CascadeCount;
		Cascade.Borders.z = (CascadeIndex == 0) ? View.NearZ : CascadeConstants.Cascades[CascadeIndex - 1].Borders.w;
		Cascade.Borders.w = CascadeDistanceFactor * (View.NearZ * ZEMath::Power(View.ShadowDistance / View.NearZ, CascadeNumberDividedByCount)) + 
				  (1.0f - CascadeDistanceFactor) * (View.NearZ + (View.ShadowDistance - View.NearZ) * CascadeNumberDividedByCount);

		float NearTop = Cascade.Borders.z * VerticalTopFovTangent;
		float NearBottom = -Cascade.Borders.z * VerticalBottomFovTangent;

		float NearRight = Cascade.Borders.z * HorizontalRightTangent;
		float NearLeft = -Cascade.Borders.z * HorizontalLeftTangent;

		float FarTop = Cascade.Borders.w * VerticalTopFovTangent;
		float FarBottom = -Cascade.Borders.w * VerticalBottomFovTangent;

		float FarRight = Cascade.Borders.w * HorizontalRightTangent;
		float FarLeft = -Cascade.Borders.w * HorizontalLeftTangent;

		CascadeFrustumVerticesView[0] = ZEVector3(NearLeft, NearBottom, Cascade.Borders.z);
		CascadeFrustumVerticesView[1] = ZEVector3(NearLeft, NearTop, Cascade.Borders.z);
		CascadeFrustumVerticesView[2] = ZEVector3(NearRight, NearTop, Cascade.Borders.z);
		CascadeFrustumVerticesView[3] = ZEVector3(NearRight, NearBottom, Cascade.Borders.z);
		
		CascadeFrustumVerticesView[4] = ZEVector3(FarLeft, FarBottom, Cascade.Borders.w);
		CascadeFrustumVerticesView[5] = ZEVector3(FarLeft, FarTop, Cascade.Borders.w);
		CascadeFrustumVerticesView[6] = ZEVector3(FarRight, FarTop, Cascade.Borders.w);
		CascadeFrustumVerticesView[7] = ZEVector3(FarRight, FarBottom, Cascade.Borders.w);

		ZEAABBox CascadeFrustumAABBLight(ZEVector3(FLT_MAX), ZEVector3(-FLT_MAX));
		for (ZEUInt I = 0; I < 8; I++)
		{
			CascadeFrustumVerticesWorld[I] = View.InvViewTransform * CascadeFrustumVerticesView[I];
			ZEVector3 CascadeFrustumVertexLight = GetViewTransform() * CascadeFrustumVerticesWorld[I];

			ZEVector3::Min(CascadeFrustumAABBLight.Min, CascadeFrustumAABBLight.Min, CascadeFrustumVertexLight);
			ZEVector3::Max(CascadeFrustumAABBLight.Max, CascadeFrustumAABBLight.Max, CascadeFrustumVertexLight);
		}

		float Diameter = ZEVector3::Length(CascadeFrustumVerticesWorld[0] - CascadeFrustumVerticesWorld[6]);
		float UnitPerShadowTexel = Diameter / CascadeShadowMaps->GetWidth();

		//Offset cascade frustum aabb in light space to enclose the circle
		ZEVector3 Offset = (ZEVector3(Diameter) - (CascadeFrustumAABBLight.Max - CascadeFrustumAABBLight.Min)) * 0.5f;

		Offset.z = 0.0f;

		CascadeFrustumAABBLight.Max += Offset;
		CascadeFrustumAABBLight.Min -= Offset;

		//round cascade frustum aabb x-y coordinate in light space to corresponding shadow texel unit
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

		CascadeFrustumAABBLight.Min.z -= Range;
		CascadeFrustumAABBLight.Max.z += Range;

		float Width = CascadeFrustumAABBLight.Max.x - CascadeFrustumAABBLight.Min.x;
		float Height = CascadeFrustumAABBLight.Max.y - CascadeFrustumAABBLight.Min.y;
		float Depth = CascadeFrustumAABBLight.Min.z - CascadeFrustumAABBLight.Max.z;
		float OffsetX = (CascadeFrustumAABBLight.Max.x + CascadeFrustumAABBLight.Min.x) / -Width;
		float OffsetY = (CascadeFrustumAABBLight.Max.y + CascadeFrustumAABBLight.Min.y) / -Height;
		float OffsetZ = CascadeFrustumAABBLight.Max.z / -Depth;

		ZEMatrix4x4::Create(Cascade.ProjectionTransform,
			2.0f / Width, 0.0f, 0.0f, OffsetX,
			0.0f, 2.0f / Height, 0.0f, OffsetY,
			0.0f, 0.0f, 1.0f / Depth, OffsetZ,
			0.0f, 0.0f, 0.0f, 1.0f);

		if (CascadeIndex != (CascadeConstants.CascadeCount - 1))
		{
			float BandLength = (Cascade.Borders.w - Cascade.Borders.z) * 0.05f;
			Cascade.Band = ZEVector4(Cascade.Borders.x - BandLength, Cascade.Borders.x + BandLength, Cascade.Borders.w - BandLength, Cascade.Borders.w + BandLength);
		}
		else
		{
			Cascade.Band = ZEVector4(Cascade.Borders.x - View.ShadowFadeDistance, Cascade.Borders.x + View.ShadowFadeDistance, Cascade.Borders.w - View.ShadowFadeDistance, Cascade.Borders.w);
		}

		Cascade.ViewTransform = GetViewTransform();
		Cascade.ProjectionTransform = Cascade.ProjectionTransform;

		ZEMatrix4x4 InvLightViewTransform;
		ZEMatrix4x4::Transpose(InvLightViewTransform, GetViewTransform());

		ZEVector3 CascadePositionWorld;
		ZEMatrix4x4::Transform3x3(CascadePositionWorld, InvLightViewTransform, CascadeFrustumAABBLight.GetCenter());

		CascadeVolumes[CascadeIndex].Create(CascadePositionWorld, GetWorldRotation(), Width, Height, CascadeFrustumAABBLight.Min.z, CascadeFrustumAABBLight.Max.z);
	}
}

void ZELightDirectional::UpdateCascadeShadowMaps()
{
	if (!DirtyFlags.GetFlags(ZE_LDF_SHADOW_MAP))
		return;

	ZEUInt Size = ZELight::ConvertShadowResolution(ShadowResolution);
	CascadeShadowMaps = ZEGRTexture2D::CreateInstance(Size, Size, 1, ZEGR_TF_D32_FLOAT, ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, ZEGR_RBF_SHADER_RESOURCE | ZEGR_RBF_DEPTH_STENCIL, CascadeConstants.CascadeCount);

	DirtyFlags.UnraiseFlags(ZE_LDF_SHADOW_MAP);
}

bool ZELightDirectional::InitializeSelf()
{
	if (!ZELight::InitializeSelf())
		return false;

	CascadeConstantBuffer = ZEGRConstantBuffer::Create(sizeof(ZECascadeConstants));

	return true;
}

bool ZELightDirectional::DeinitializeSelf()
{
	CascadeConstantBuffer.Release();
	CascadeShadowMaps.Release();
	CascadeVolumes.Clear();

	return ZELight::DeinitializeSelf();
}

ZELightDirectional::ZELightDirectional()
{
	memset(&CascadeConstants, 0, sizeof(CascadeConstants));
	CascadeDistanceFactor = 0.5f;
	CascadeConstants.CascadeCount = 3;
	CascadeVolumes.Resize(3);

	Command.Entity = this;
	Command.Priority = 1;

	UseSunLight = false;
	UseMoonLight = false;
}

ZELightDirectional::~ZELightDirectional()
{

}

ZEDrawFlags ZELightDirectional::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_LIGHT_SOURCE;
}

void ZELightDirectional::SetCascadeCount(ZEUInt CascadeCount)
{
	if (CascadeConstants.CascadeCount == CascadeCount)
		return;

	CascadeConstants.CascadeCount = CascadeCount;
	CascadeVolumes.Resize(CascadeCount);

	DirtyFlags.RaiseFlags(ZE_LDF_CONSTANT_BUFFER | ZE_LDF_SHADOW_MAP);
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

void ZELightDirectional::SetCascadeDepthBias(ZEUInt CascadeIndex, float CascadeDepthBias)
{
	CascadeConstants.Cascades[CascadeIndex].DepthBias = CascadeDepthBias;
}

float ZELightDirectional::GetCascadeDepthBias(ZEUInt CascadeIndex) const
{
	return CascadeConstants.Cascades[CascadeIndex].DepthBias;
}

void ZELightDirectional::SetCascadeNormalBias(ZEUInt CascadeIndex, float CascadeNormalBias)
{
	CascadeConstants.Cascades[CascadeIndex].NormalBias = CascadeNormalBias;
}

float ZELightDirectional::GetCascadeNormalBias(ZEUInt CascadeIndex) const
{
	return CascadeConstants.Cascades[CascadeIndex].NormalBias;
}

void ZELightDirectional::SetUseSunLight(bool UseSunLight)
{
	this->UseSunLight = UseSunLight;
}

bool ZELightDirectional::GetUseSunLight() const
{
	return UseSunLight;
}

void ZELightDirectional::SetUseMoonLight(bool UseMoonLight)
{
	this->UseMoonLight = UseMoonLight;
}

bool ZELightDirectional::GetUseMoonLight() const
{
	return UseMoonLight;
}

void ZELightDirectional::SetTerrestrialColor(const ZEVector3& TerrestrialColor)
{
	this->TerrestrialColor = TerrestrialColor;
}

const ZEVector3& ZELightDirectional::GetTerrestrialColor() const
{
	return TerrestrialColor;
}

void ZELightDirectional::BindCascades(ZERNRenderer* Renderer, ZEGRContext* Context)
{
	ZEMatrix4x4 TextureTransform;
	ZEMatrix4x4::Create(TextureTransform,
		0.5f, 0.0f, 0.0f, 0.5f,
		0.0f, -0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	for (ZEUInt I = 0; I < CascadeConstants.CascadeCount; I++)
	{
		ZECascade& Cascade = CascadeConstants.Cascades[I];
		Cascade.ViewTransform = Cascade.ViewTransform * Renderer->GetView().InvViewTransform;
		Cascade.ProjectionTransform = TextureTransform * Cascade.ProjectionTransform;
	}
	
	CascadeConstantBuffer->SetData(&CascadeConstants);

	Context->SetConstantBuffers(ZEGR_ST_PIXEL, 9, 1, CascadeConstantBuffer.GetPointerToPointer());
	Context->SetConstantBuffers(ZEGR_ST_COMPUTE, 9, 1, CascadeConstantBuffer.GetPointerToPointer());
}

ZELightType ZELightDirectional::GetLightType() const
{
	return ZE_LT_DIRECTIONAL;
}

ZESize ZELightDirectional::GetViewCount() const
{
	return 1;
}

ZEGRTexture* ZELightDirectional::GetShadowMap(ZESize Index) const
{
	if (Index >= CascadeConstants.CascadeCount)
		return NULL;

	return CascadeShadowMaps;
}

const ZEViewVolume& ZELightDirectional::GetViewVolume(ZESize Index) const
{
	if (Index >= CascadeConstants.CascadeCount)
		return ZEViewCuboid();

	return CascadeVolumes[Index];
}

const ZEMatrix4x4& ZELightDirectional::GetViewTransform(ZESize Index) const
{
	if (DirtyFlags.GetFlags(ZE_LDF_VIEW_TRANSFORM))
	{
		ZEMatrix4x4::CreateViewTransform(ViewTransform, ZEVector3::Zero, GetWorldRotation());
		DirtyFlags.UnraiseFlags(ZE_LDF_VIEW_TRANSFORM);
	}

	return ViewTransform;
}

const ZEMatrix4x4& ZELightDirectional::GetProjectionTransform(ZESize Index) const
{
	if (Index >= CascadeConstants.CascadeCount)
		return ZEMatrix4x4::Identity;

	return CascadeConstants.Cascades[Index].ProjectionTransform;
}

void ZELightDirectional::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	if (Parameters->Stage->GetId() != ZERN_STAGE_SHADOWING)
		return;

	UpdateCascadeShadowMaps();
	UpdateCascadeTransforms(*Parameters->View);

	ZEGRContext* Context = Parameters->Context;

	ZERNView View = ShadowRenderer.GetView();
	View.Position = Parameters->View->Position;
	View.Rotation = GetWorldRotation();
	View.Direction = GetWorldFront();
	View.U = GetWorldRight();
	View.V = GetWorldUp();
	View.N = GetWorldFront();
	View.Viewport = NULL;

	ShadowRenderer.SetContext(Parameters->Context);
	ShadowRenderer.SetScene(Parameters->Scene);

	for (ZEUInt CascadeIndex = 0; CascadeIndex < CascadeConstants.CascadeCount; CascadeIndex++)
	{
		View.ViewVolume = &GetViewVolume(CascadeIndex);
		View.ViewProjectionTransform = GetProjectionTransform(CascadeIndex) * GetViewTransform();

		ShadowRenderer.SetView(View);

		const ZEGRDepthStencilBuffer* DepthBuffer = CascadeShadowMaps->GetDepthStencilBuffer(false, CascadeIndex);
		Context->ClearDepthStencilBuffer(DepthBuffer, true, true, 0.0f, 0x00);
		Context->SetRenderTargets(0, NULL, DepthBuffer);
		Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, DepthBuffer->GetWidth(), DepthBuffer->GetHeight()));

		ShadowRenderer.Render(0.0f);
	}
}

ZELightDirectional* ZELightDirectional::CreateInstance()
{
	return new ZELightDirectional();
}
