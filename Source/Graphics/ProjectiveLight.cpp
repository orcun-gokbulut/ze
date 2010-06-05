//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ProjectiveLight.cpp
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

#include "ProjectiveLight.h"
#include "Texture2D.h"
#include "TextureCube.h"
#include "Core/Error.h"
#include "Game/Scene.h"
#include "ShadowRenderer.h"
#include "GraphicsModule.h"

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
	ProjectionMap = Texture;
}

const ZETexture2D* ZEProjectiveLight::GetProjectionTexture()
{
	return ProjectionMap;
}

const ZEMatrix4x4& ZEProjectiveLight::GetProjectionMatrix()
{
	return ZEMatrix4x4::Identity;
}

const ZETexture2D* ZEProjectiveLight::GetShadowMap()
{
	return ShadowMap;
}

void ZEProjectiveLight::SetShadowMap(int Width, int Height)
{
	if (ShadowMap != NULL)
		ShadowMap->Release();
	else
		ShadowMap = ZETexture2D::CreateInstance();

	if (!ShadowMap->Create(Width, Height, ZE_TPF_DEPTH, true))
	{
		zeError("Projective Light", "Can not create shadow map texture.");
		return;
	}
}

void ZEProjectiveLight::RenderShadowMap(ZEScene* Scene, ZEShadowRenderer* ShadowRenderer)
{
	if (!GetCastsShadows() || ShadowMap == NULL || ShadowMap->IsEmpty())
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

	//ShadowRenderer->SetOutput((ZETexture2D*)ShadowMap);
	ShadowRenderer->ClearList();
//	ShadowRenderer->SetCamera(ViewPoint);
	Scene->CullScene((ZERenderer*)ShadowRenderer, GetViewVolume(), false);
	ShadowRenderer->Render();
}
/*
const ZELight* ZEProjectiveLight::GetRenderOrderLight()
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
}*/

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
	ShadowMap = NULL;
}

ZEProjectiveLight::~ZEProjectiveLight()
{
	if (ShadowMap != NULL)
		ShadowMap->Destroy();
}
