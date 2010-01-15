//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - LightBrush.cpp
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

#include "LightBrush.h"
#include "Core/Error.h"
#include "Graphics/Canvas.h"
#include "Graphics/RenderOrder.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/FixedMaterial.h"
#include "Graphics/Texture2DResource.h"
#include "Graphics/TextureCubeResource.h"
#include "Graphics/PointLight.h"
#include "Graphics/DirectionalLight.h"
#include "Graphics/ProjectiveLight.h"
#include "Graphics/OmniProjectiveLight.h"

ZEDWORD ZELightBrush::GetDrawFlags() const
{
	return ZE_DF_LIGHT_SOURCE | ZE_DF_DRAW | ZE_DF_CULL;
}

ZELight* ZELightBrush::GetLight()
{
	return Light;
}
		
void ZELightBrush::SetLightType(ZELightType LightType)
{
	if (LightType == this->LightType)
		return;

	if (Light != NULL)
	{
		UnregisterComponent(Light);
		delete Light;
	}

	this->LightType = LightType;

	ZECanvas Canvas;
	ZEMatrix4x4 Rotation, Offset;
	switch(LightType)
	{
		case ZE_LT_POINT:
			Light = new ZEPointLight();
			Canvas.AddSphere(2.0f, 8, 8);
			break;

		case ZE_LT_DIRECTIONAL:
			Light = new ZEDirectionalLight();
			ZEMatrix4x4::CreateOffset(Offset, ZEVector3(0.0f, 0.0f, 1.0f), ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
			Canvas.SetTransfomation(Offset);
			Canvas.AddPyramid(1.0f, 1.0f, 1.0f);

			ZEMatrix4x4::CreateRotation(Rotation, ZEQuaternion(ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
			Canvas.SetTransfomation(Rotation);
			Canvas.AddBox(0.5f, 1.0f, 0.5f);
			break;

		case ZE_LT_PROJECTIVE:
			ZEMatrix4x4::CreateRotation(Rotation, ZEQuaternion(-ZE_PI_2, ZEVector3(1.0f, 0.0f, 0.0f)));
			Canvas.SetTransfomation(Rotation);
			Canvas.AddPyramid(2.0f, 2.0f, 2.0f);
			Light = new ZEProjectiveLight();
			break;

		case ZE_LT_OMNIPROJECTIVE:
			Light = new ZEOmniProjectiveLight();
			Canvas.AddBox(2.0f, 2.0f, 2.0f);
			break;
		default:
			zeError("Light", "Light type does not supported.");
			return; 
	}

	Light->SetLocalPosition(ZEVector3(0.0f, 0.0f, 0.0f));
	Light->SetLocalRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
	Light->SetLocalScale(ZEVector3(1.0f, 1.0f, 1.0f));
	Light->SetColor(Color);
	Light->SetIntensity(Intensity);
	Light->SetRange(Range);
	Light->SetAttenuation(Attenuation.x, Attenuation.y, Attenuation.z);
	Light->SetCastsShadows(CastsShadow);
	Light->SetEnabled(GetEnabled());

	if (LightType == ZE_LT_PROJECTIVE)
	{
		if (ProjectionTexture != NULL)
			((ZEProjectiveLight*)Light)->SetProjectionTexture(ProjectionTexture->GetTexture());
		((ZEProjectiveLight*)Light)->SetFOV(ProjectionFOV);
		((ZEProjectiveLight*)Light)->SetAspectRatio(ProjectionAspectRatio);
	}

	if (LightType == ZE_LT_OMNIPROJECTIVE)
		if (CubeProjectionTexture != NULL)
			((ZEOmniProjectiveLight*)Light)->SetProjectionTexture(CubeProjectionTexture->GetTexture());

	if (RenderOrder.VertexBuffer != NULL)
		delete RenderOrder.VertexBuffer;

	ZEAABoundingBox BoundingBox;
	Canvas.CalculateBoundingBox(BoundingBox);
	SetLocalBoundingBox(BoundingBox);
	RenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();
	RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
	RegisterComponent(Light);
}

ZELightType ZELightBrush::GetLightType() const
{
	return LightType;
}

void ZELightBrush::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
	if (Light != NULL)
		Light->SetColor(NewColor);
}

const ZEVector3& ZELightBrush::GetColor() const
{
	return Color;
}

void ZELightBrush::SetIntensity(float NewValue)
{
	Intensity = NewValue;
	if (Light != NULL)
		Light->SetIntensity(NewValue);
}

float ZELightBrush::GetIntensity() const
{
	return Intensity;
}

void ZELightBrush::SetRange(float NewValue)
{
	Range = NewValue;
	if (Light != NULL)
		Light->SetRange(NewValue);

}

float ZELightBrush::GetRange() const
{
	return Range;
}

void ZELightBrush::SetProjectionFOV(float NewValue)
{
	ProjectionFOV = NewValue;
	if (Light != NULL && LightType == ZE_LT_PROJECTIVE)
		((ZEProjectiveLight*)Light)->SetFOV(NewValue);

}

float ZELightBrush::GetProjectionFOV() const
{
	return ProjectionFOV;
}

void ZELightBrush::SetProjectionAspectRatio(float NewValue)
{
	ProjectionFOV = NewValue;
	if (Light != NULL && LightType == ZE_LT_PROJECTIVE)
		((ZEProjectiveLight*)Light)->SetAspectRatio(NewValue);

}

float ZELightBrush::GetProjectionAspectRatio() const
{
	return ProjectionAspectRatio;
}

void ZELightBrush::SetEnabled(bool NewValue)
{
	ZEEntity::SetEnabled(NewValue);
	if (Light != NULL)
		Light->SetEnabled(NewValue);
}
		
void ZELightBrush::SetAttenuation(const ZEVector3& NewValue)
{
	Attenuation = NewValue;
	if (Light != NULL)
		Light->SetAttenuation(Attenuation.x, Attenuation.y, Attenuation.z);
}

const ZEVector3& ZELightBrush::GetAttenuation() const
{
	return Attenuation;
}

void ZELightBrush::SetCastsShadow(bool NewValue)
{
	CastsShadow = NewValue;
	if (Light != NULL)
	
		Light->SetCastsShadows(NewValue);
}

bool ZELightBrush::GetCastsShadow() const
{
	return CastsShadow;
}

void ZELightBrush::SetProjectionTexture(const char* TextureFile)
{
	if (ProjectionTexture != NULL)
	{
		ProjectionTexture->Release();
		ProjectionTexture = NULL;
	}

	if (CubeProjectionTexture != NULL)
	{
		CubeProjectionTexture->Release();
		ProjectionTexture = NULL;
	}

	if (LightType == ZE_LT_PROJECTIVE)
	{
		ProjectionTexture = ZETexture2DResource::LoadSharedResource(TextureFile);
		if (Light != NULL && ProjectionTexture != NULL && ProjectionTexture->GetTexture() != NULL)
			((ZEProjectiveLight*)Light)->SetProjectionTexture(ProjectionTexture->GetTexture());
	}
	else if (LightType == ZE_LT_OMNIPROJECTIVE)
	{
		CubeProjectionTexture = ZETextureCubeResource::LoadSharedResource(TextureFile);
		if (Light != NULL && CubeProjectionTexture != NULL && CubeProjectionTexture->GetTexture() != NULL)
			((ZEOmniProjectiveLight*)Light)->SetProjectionTexture(CubeProjectionTexture->GetTexture());
	
	}

}

const char* ZELightBrush::GetProjectionTexture() const
{
	if (LightType == ZE_LT_PROJECTIVE && ProjectionTexture != NULL)
		return ProjectionTexture->GetFileName();
	else if (LightType == ZE_LT_OMNIPROJECTIVE && CubeProjectionTexture != NULL)
		return CubeProjectionTexture->GetFileName();
	else
		return "";
}

void ZELightBrush::Initialize()
{
	if (Material == NULL)
		Material = ZEFixedMaterial::CreateInstance();

	Material->SetZero();
	Material->SetEmmisiveEnabled(true);
	Material->SetLightningEnabled(false);
	Material->SetTwoSided(false);
	Material->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);
	Material->UpdateMaterial();
	
	RenderOrder.Material = Material;
	RenderOrder.WorldMatrix = GetWorldTransform();
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_ROF_ENABLE_ZCULLING;
}

void ZELightBrush::Deinitialize()
{
	if (Material != NULL)
	{
		Material->Destroy();
		Material = NULL;
	}

	if (Light != NULL)
	{
		UnregisterComponent(Light);
		delete Light;
		Light = NULL;
	}

	if (ProjectionTexture != NULL)
	{
		ProjectionTexture->Release();
		ProjectionTexture = NULL;
	}

	if (CubeProjectionTexture != NULL)
	{
		CubeProjectionTexture->Release();
		CubeProjectionTexture = NULL;
	}

	if (RenderOrder.VertexBuffer != NULL)
	{
		delete RenderOrder.VertexBuffer;
		RenderOrder.VertexBuffer = NULL;
	}
}

void ZELightBrush::Reset()
{

}


void ZELightBrush::Tick(float Time)
{
}

void ZELightBrush::Update()
{

}

void ZELightBrush::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (Light != NULL)
	{
		RenderOrder.WorldMatrix = Light->GetWorldTransform();
		Material->SetEmmisiveColor(Light->GetColor());
		Renderer->AddToRenderList(&RenderOrder);
	}
}

ZELightBrush::ZELightBrush()
{
	Material = NULL;
	SetBoundingVolumeMechanism(ZE_BVM_USELOCALONLY);
	LightType = ZE_LT_NONE;
	Light = NULL;
	ProjectionTexture = NULL;
	RenderOrder.SetZero();
	Intensity = 1.0f;
	Color = ZEVector3(1.0f, 1.0f, 1.0f);
	Range = 100.0f;
	Attenuation = ZEVector3(0.0f, 0.0f, 1.0f);
	CastsShadow = false;
	ProjectionFOV = ZE_PI_2;
	ProjectionTexture = NULL;
	ProjectionAspectRatio = 1.0f;
	CubeProjectionTexture = NULL;
}

ZELightBrush::~ZELightBrush()
{
	Deinitialize();
}

#include "LightBrush.h.zpp"
