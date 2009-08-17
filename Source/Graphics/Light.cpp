//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Light.cpp
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

#include "Light.h"
#include "Core/Core.h"
#include "GameInterface/Scene.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  CLight  //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ZELight::IsLight()
{
	return true;
}

void ZELight::SetLocalPosition(const ZEVector3& NewPosition)
{
	UpdateViewVolume = true;
	ZEComponent::SetLocalPosition(NewPosition);
}

void ZELight::SetLocalRotation(const ZEQuaternion& NewRotation)
{
	UpdateViewVolume = true;
	ZEComponent::SetLocalRotation(NewRotation);
}


void ZELight::OwnerWorldTransformChanged()
{
	UpdateViewVolume = true;
	ZEComponent::OwnerWorldTransformChanged();
}


void ZELight::SetColor(const ZEVector3& NewColor)
{
	RenderOrderLight.Color = NewColor;
}

const ZEVector3& ZELight::GetColor()
{
	return RenderOrderLight.Color;
}

void ZELight::SetIntensity(float NewValue)
{
	RenderOrderLight.Intensity = NewValue;
}

float ZELight::GetIntensity()
{
	return RenderOrderLight.Intensity;
}

void ZELight::SetRange(float NewValue)
{
	RenderOrderLight.Range = NewValue;
}

float ZELight::GetRange()
{
	return RenderOrderLight.Range;
}

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	RenderOrderLight.Attenuation.x = Constant;
	RenderOrderLight.Attenuation.y = Distance;
	RenderOrderLight.Attenuation.z = DistanceSquare;
}

const ZEVector3& ZELight::GetAttenuation()
{
	return RenderOrderLight.Attenuation;
}

bool ZELight::IsEnabled()
{
	return Enabled;
}

void ZELight::SetEnabled(bool NewValue)
{
	Enabled = NewValue;
}

bool ZELight::IsCastingShadows()
{
	return CastsShadows;
}

void ZELight::SetCastShadows(bool NewValue)
{
	CastsShadows = NewValue;
}

const ZERLLight* ZELight::GetRenderOrderLight()
{
	RenderOrderLight.Position = ZEVector3(GetWorldPosition());
	ZEQuaternion::VectorProduct(RenderOrderLight.Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion InvRotation;
	ZEQuaternion::Conjugate(InvRotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RenderOrderLight.LightRotationMatrix, InvRotation);
	return &RenderOrderLight;
}

ZELight::ZELight()
{
	RenderOrderLight.SetZero();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPointLight  CPointLight  CPointLight  CPointLight  CPointLight  CPointLight  CPointLight  CPointLight          //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEPointLight::GetLightType()
{
	return ZE_LT_POINT;
}

void ZEPointLight::SetCastShadows(bool NewValue)
{
	RenderOrderLight.CubeShadowMap = CastsShadows ? ShadowMap : NULL;
	CastsShadows = NewValue;
}

const ZETextureCube* ZEPointLight::GetShadowMap()
{
	return ShadowMap;
}

void ZEPointLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
}

const ZEViewVolume& ZEPointLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		UpdateViewVolume = false;
	}
	
	return ViewVolume;
}

ZEPointLight::ZEPointLight()
{
	RenderOrderLight.Type = ZE_RLLT_POINT;
	ShadowMap = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEDirectionalLight::GetLightType()
{
	return ZE_LT_DIRECTIONAL;
}

const ZETexture2D* ZEDirectionalLight::GetShadowMap()
{
	return ShadowMap;
}

void ZEDirectionalLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
}

ZEDirectionalLight::ZEDirectionalLight()
{
	RenderOrderLight.Type = ZE_RLLT_DIRECTIONAL;
	ShadowMap = NULL;
}

const ZEViewVolume& ZEDirectionalLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ZEVector3 Direction;
		ZEQuaternion::VectorProduct(Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
		ViewVolume.Create(GetWorldPosition(), Direction, GetRange());
		UpdateViewVolume = false;
	}

	return ViewVolume;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEProjectiveLight  ZEProjectiveLight  ZEProjectiveLight  ZEProjectiveLight  ZEProjectiveLight  ZEProjectiveLight  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEProjectiveLight::GetLightType()
{
	return ZE_LT_PROJECTIVE;
}

void ZEProjectiveLight::SetFOV(float FOV)
{
	this->FOV = FOV;
}

float ZEProjectiveLight::GetFOV()
{
	return FOV;
}

void ZEProjectiveLight::SetAspectRatio(float AspectRatio)
{
	this->AspectRatio = AspectRatio;
}

float ZEProjectiveLight::GetAspectRatio()
{
	return AspectRatio;
}

void ZEProjectiveLight::SetProjectionTexture(const ZETexture2D* Texture)
{
	RenderOrderLight.ProjectionMap = Texture;
}

const ZETexture2D* ZEProjectiveLight::GetProjectionTexture()
{
	return RenderOrderLight.ProjectionMap;
}

const ZETexture2D* ZEProjectiveLight::GetShadowMap()
{
	return RenderOrderLight.ShadowMap;
}

void ZEProjectiveLight::SetShadowMap(int Width, int Height)
{
	if (RenderOrderLight.ShadowMap != NULL)
		((ZETexture2D*)RenderOrderLight.ShadowMap)->Release();
	else
		RenderOrderLight.ShadowMap = zeGraphics->CreateTexture();

	if (!((ZETexture2D*)RenderOrderLight.ShadowMap)->Create(Width, Height, ZE_TPF_SHADOWMAP, true))
	{
		zeError("Projective Light", "Can not create shadow map texture.");
		return;
	}
}

void ZEProjectiveLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
	if (!IsCastingShadows() || RenderOrderLight.ShadowMap == NULL || RenderOrderLight.ShadowMap->IsEmpty())
		return;

	ZEViewPoint ViewPoint;
	ViewPoint.ViewPosition = GetWorldPosition();
	
	ZEMatrix4x4 PerspectiveMatrix, ViewMatrix, ViewProjMatrix, TranslationMatrix, RotationMatrix;
	ZEMatrix4x4::CreateTranslation(TranslationMatrix, -ViewPoint.ViewPosition.x, -ViewPoint.ViewPosition.y, -ViewPoint.ViewPosition.z);
	
	ZEQuaternion Rotation;
	ZEQuaternion::Conjugate(Rotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RotationMatrix, Rotation);

	ZEMatrix4x4::Multiply(ViewMatrix, TranslationMatrix, RotationMatrix);

	ZEMatrix4x4::CreatePerspectiveProjection(PerspectiveMatrix, FOV, AspectRatio, zeGraphics->GetNearZ(), GetRange());
	ZEMatrix4x4::Multiply(ViewPoint.ViewProjMatrix, ViewMatrix, PerspectiveMatrix);

	ShadowRenderer->SetOutput((ZETexture2D*)RenderOrderLight.ShadowMap);
	ShadowRenderer->ClearList();
//	ShadowRenderer->SetCamera(ViewPoint);
	Scene->CullScene(ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();
}

const ZERLLight* ZEProjectiveLight::GetRenderOrderLight()
{

	ZEVector3 Position = GetWorldPosition();
	ZEQuaternion Rotation;
	
	ZEMatrix4x4 PerspectiveMatrix, ViewMatrix, ViewProjMatrix, TranslationMatrix, RotationMatrix;
	ZEMatrix4x4::CreateTranslation(TranslationMatrix, -Position.x, -Position.y, -Position.z);
	
	ZEQuaternion::Conjugate(Rotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RotationMatrix, Rotation);

	ZEMatrix4x4::Multiply(ViewMatrix, TranslationMatrix, RotationMatrix);

	if (RenderOrderLight.ProjectionMap != NULL)
	{
		float OffsetW = 0.5f + (0.5f / (float)RenderOrderLight.ProjectionMap->GetWidth());
		float OffsetH = 0.5f + (0.5f / (float)RenderOrderLight.ProjectionMap->GetHeight());
		ZEMatrix4x4 ScaleBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
									0.0f, -0.5f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									OffsetW, OffsetH, 0.0f, 1.0f);

		ZEMatrix4x4::CreatePerspectiveProjection(PerspectiveMatrix, FOV, AspectRatio, zeGraphics->GetNearZ(), GetRange());
		ZEMatrix4x4::Multiply(ViewProjMatrix, ViewMatrix, PerspectiveMatrix);
		ZEMatrix4x4::Multiply(RenderOrderLight.LightViewProjMatrix, ViewProjMatrix, ScaleBiasMatrix);
	}

	ZEQuaternion::VectorProduct(RenderOrderLight.Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));

	if (CastsShadows)
	{
	}
	else
		RenderOrderLight.ShadowMap = NULL;


	RenderOrderLight.Position = ZEVector3(GetWorldPosition());
	return &RenderOrderLight;
}

const ZEViewVolume& ZEProjectiveLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetWorldRotation(), FOV, AspectRatio, 0.0f, GetRange());
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

ZEProjectiveLight::ZEProjectiveLight()
{
	RenderOrderLight.Type = ZE_RLLT_PROJECTIVE;
	RenderOrderLight.ShadowMap = NULL;
}

ZEProjectiveLight::~ZEProjectiveLight()
{
	if (RenderOrderLight.ShadowMap != NULL)
		((ZETexture2D*)RenderOrderLight.ShadowMap)->Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEOmniProjectiveLight::GetLightType()
{
	return ZE_LT_OMNIPROJECTIVE;
}

const ZETextureCube* ZEOmniProjectiveLight::GetShadowMap()
{
	return ShadowMap;
}

const ZETextureCube* ZEOmniProjectiveLight::GetProjectionTexture()
{
	return RenderOrderLight.CubeProjectionMap;
}

void ZEOmniProjectiveLight::SetProjectionTexture(const ZETextureCube* Texture)
{
	RenderOrderLight.CubeProjectionMap = Texture;
}

void ZEOmniProjectiveLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
}

const ZEViewVolume& ZEOmniProjectiveLight::GetViewVolume()
{
	if (UpdateViewVolume)
	{
		ViewVolume.Create(GetWorldPosition(), GetRange(), 0.0f);
		UpdateViewVolume = false;
	}

	return ViewVolume;
}

ZEOmniProjectiveLight::ZEOmniProjectiveLight()
{
	RenderOrderLight.Type = ZE_RLLT_OMNIPROJECTIVE;
	ProjectionTexture = NULL;
	ShadowMap = NULL;
}
