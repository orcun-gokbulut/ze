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
	RenderListLight.Color = NewColor;
}

const ZEVector3& ZELight::GetColor()
{
	return RenderListLight.Color;
}

void ZELight::SetIntensity(float NewValue)
{
	RenderListLight.Intensity = NewValue;
}

float ZELight::GetIntensity()
{
	return RenderListLight.Intensity;
}

void ZELight::SetRange(float NewValue)
{
	RenderListLight.Range = NewValue;
}

float ZELight::GetRange()
{
	return RenderListLight.Range;
}

void ZELight::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	RenderListLight.Attenuation.x = Constant;
	RenderListLight.Attenuation.y = Distance;
	RenderListLight.Attenuation.z = DistanceSquare;
}

const ZEVector3& ZELight::GetAttenuation()
{
	return RenderListLight.Attenuation;
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

const ZERLLight* ZELight::GetRenderListLight()
{
	RenderListLight.Position = ZEVector3(GetWorldPosition());
	ZEQuaternion::VectorProduct(RenderListLight.Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));
	ZEQuaternion InvRotation;
	ZEQuaternion::Conjugate(InvRotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RenderListLight.LightRotationMatrix, InvRotation);
	return &RenderListLight;
}

ZELight::ZELight()
{
	RenderListLight.SetZero();
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
	RenderListLight.CubeShadowMap = CastsShadows ? ShadowMap : NULL;
	CastsShadows = NewValue;
}

const ZECubeTexture* ZEPointLight::GetShadowMap()
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
	RenderListLight.Type = ZE_RLLT_POINT;
	ShadowMap = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  CDirectionalLight  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEDirectionalLight::GetLightType()
{
	return ZE_LT_DIRECTIONAL;
}

const ZETexture* ZEDirectionalLight::GetShadowMap()
{
	return ShadowMap;
}

void ZEDirectionalLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
}

ZEDirectionalLight::ZEDirectionalLight()
{
	RenderListLight.Type = ZE_RLLT_DIRECTIONAL;
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

void ZEProjectiveLight::SetProjectionTexture(const ZETexture* Texture)
{
	RenderListLight.ProjectionMap = Texture;
}

const ZETexture* ZEProjectiveLight::GetProjectionTexture()
{
	return RenderListLight.ProjectionMap;
}

const ZETexture* ZEProjectiveLight::GetShadowMap()
{
	return RenderListLight.ShadowMap;
}

void ZEProjectiveLight::SetShadowMap(int Width, int Height)
{
	if (RenderListLight.ShadowMap != NULL)
		((ZETexture*)RenderListLight.ShadowMap)->Release();
	else
		RenderListLight.ShadowMap = zeGraphics->CreateTexture();

	if (!((ZETexture*)RenderListLight.ShadowMap)->Create(Width, Height, ZE_TPF_SHADOWMAP, true))
	{
		zeError("Projective Light", "Can not create shadow map texture.");
		return;
	}
}

void ZEProjectiveLight::RenderShadowMap(ZEScene* Scene, ZERenderer* ShadowRenderer)
{
	if (!IsCastingShadows() || RenderListLight.ShadowMap == NULL || RenderListLight.ShadowMap->IsEmpty())
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

	ShadowRenderer->SetOutput((ZETexture*)RenderListLight.ShadowMap);
	ShadowRenderer->ClearList();
	ShadowRenderer->SetViewPoint(ViewPoint);
	Scene->CullScene(ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();
}

const ZERLLight* ZEProjectiveLight::GetRenderListLight()
{

	ZEVector3 Position = GetWorldPosition();
	ZEQuaternion Rotation;
	
	ZEMatrix4x4 PerspectiveMatrix, ViewMatrix, ViewProjMatrix, TranslationMatrix, RotationMatrix;
	ZEMatrix4x4::CreateTranslation(TranslationMatrix, -Position.x, -Position.y, -Position.z);
	
	ZEQuaternion::Conjugate(Rotation, GetWorldRotation());
	ZEMatrix4x4::CreateRotation(RotationMatrix, Rotation);

	ZEMatrix4x4::Multiply(ViewMatrix, TranslationMatrix, RotationMatrix);

	if (RenderListLight.ProjectionMap != NULL)
	{
		float OffsetW = 0.5f + (0.5f / (float)RenderListLight.ProjectionMap->GetWidth());
		float OffsetH = 0.5f + (0.5f / (float)RenderListLight.ProjectionMap->GetHeight());
		ZEMatrix4x4 ScaleBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
									0.0f, -0.5f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									OffsetW, OffsetH, 0.0f, 1.0f);

		ZEMatrix4x4::CreatePerspectiveProjection(PerspectiveMatrix, FOV, AspectRatio, zeGraphics->GetNearZ(), GetRange());
		ZEMatrix4x4::Multiply(ViewProjMatrix, ViewMatrix, PerspectiveMatrix);
		ZEMatrix4x4::Multiply(RenderListLight.LightViewProjMatrix, ViewProjMatrix, ScaleBiasMatrix);
	}

	ZEQuaternion::VectorProduct(RenderListLight.Direction, GetWorldRotation(), ZEVector3(0.0f, 0.0f, 1.0f));

	if (CastsShadows)
	{
	}
	else
		RenderListLight.ShadowMap = NULL;


	RenderListLight.Position = ZEVector3(GetWorldPosition());
	return &RenderListLight;
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
	RenderListLight.Type = ZE_RLLT_PROJECTIVE;
	RenderListLight.ShadowMap = NULL;
}

ZEProjectiveLight::~ZEProjectiveLight()
{
	if (RenderListLight.ShadowMap != NULL)
	{
		((ZETexture*)RenderListLight.ShadowMap)->Release();
		delete RenderListLight.ShadowMap;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight  ZEOmniProjectiveLight //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ZELightType ZEOmniProjectiveLight::GetLightType()
{
	return ZE_LT_OMNIPROJECTIVE;
}

const ZECubeTexture* ZEOmniProjectiveLight::GetShadowMap()
{
	return ShadowMap;
}

const ZECubeTexture* ZEOmniProjectiveLight::GetProjectionTexture()
{
	return RenderListLight.CubeProjectionMap;
}

void ZEOmniProjectiveLight::SetProjectionTexture(const ZECubeTexture* Texture)
{
	RenderListLight.CubeProjectionMap = Texture;
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
	RenderListLight.Type = ZE_RLLT_OMNIPROJECTIVE;
	ProjectionTexture = NULL;
	ShadowMap = NULL;
}
