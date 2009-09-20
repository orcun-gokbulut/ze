//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SkyBrush.cpp
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

#include "GameInterface/Scene.h"
#include "SkyBrush.h"
#include <string.h>

ZE_ENTITY_DESCRIPTION_START(ZESkyBrush, ZEEntity, ZE_ERA_BOTH, "", "Sky Brush")
	ZE_ENTITY_ATTRIBUTES_START()
		ZE_ENTITY_ATTRIBUTE_SEMANTIC("SkyColor", ZE_VRT_VECTOR3, ZE_EAF_NONE, ZE_EAS_COLORVALUE, ZEVector3(0.75f, 0.65f, 0.90f), "Color of the sky")
		ZE_ENTITY_ATTRIBUTE_FILENAME("SkyTexture", ZE_EAF_NONE, "Image", "", "Texture of the sky")
		ZE_ENTITY_ATTRIBUTE("SkyLightEnabled", ZE_VRT_BOOLEAN, ZE_EAF_NONE, false, "Sun light enabled")
		ZE_ENTITY_ATTRIBUTE_SEMANTIC("SkyLightColor", ZE_VRT_VECTOR3, ZE_EAF_NONE, ZE_EAS_COLORVALUE, ZEVector3(1.0f, 1.0f, 1.0f), "Color of the sun")
		ZE_ENTITY_ATTRIBUTE("SkyLightDirection", ZE_VRT_VECTOR3, ZE_EAF_NONE, ZEVector3(0.0f, -1.0f, 0.0f), "Direction of the light")
		ZE_ENTITY_ATTRIBUTE("SkyLightIntensity", ZE_VRT_FLOAT, ZE_EAF_NONE, 1.0f, "Intensity of the light")
	ZE_ENTITY_ATTRIBUTES_END()
ZE_ENTITY_DESCRIPTION_END(ZESkyBrush)

const ZEAABoundingBox& ZESkyBrush::GetWorldBoundingBox()
{
	return ZEAABoundingBox();
}

bool ZESkyBrush::IsDrawable()
{
	return true;
}

bool ZESkyBrush::IsLight()
{
	return IsEnabled() && SkyLight.IsEnabled();
}

bool ZESkyBrush::AllwaysDraw()
{
	return true;
}

void ZESkyBrush::SetSkyColor(const ZEVector3& Color)
{
	SkyMaterial->SetAmbientColor(Color);
}

const ZEVector3& ZESkyBrush::GetSkyColor()
{
	return SkyMaterial->GetAmbientColor();
}

void ZESkyBrush::SetSkyTexture(const char* FileName)
{
	if (SkyTexture != NULL)
	{
		SkyTexture->Release();
		SkyTexture = NULL;
	}

	SkyTexture = ZETextureCubeResource::LoadResource(FileName);
}

const char* ZESkyBrush::GetSkyTexture()
{
	if (SkyTexture == NULL)
		return "";
	
	return SkyTexture->GetFileName();
}

void ZESkyBrush::SetSkyLightEnabled(bool Enabled)
{
	SkyLight.SetEnabled(Enabled);
}

bool ZESkyBrush::GetSkyLightEnabled()
{
	return SkyLight.IsEnabled();
}

void ZESkyBrush::SetSkyLightColor(const ZEVector3& Color)
{
	SkyLight.SetColor(Color);
}

const ZEVector3& ZESkyBrush::GetSkyLightColor()
{
	return SkyLight.GetColor();
}

void ZESkyBrush::SetSkyLightDirection(const ZEVector3& Direction)
{
	//SkyLight.SetDirection(Direction);
}

const ZEVector3& ZESkyBrush::GetSkyLightDirection()
{
//	return SkyLight.GetDirection();
	return ZEVector3();
}

void ZESkyBrush::SetSkyLightIntensity(float Intensity)
{
	SkyLight.SetIntensity(Intensity);
}

float ZESkyBrush::GetSkyLightIntensity()
{
	return SkyLight.GetIntensity();
	
}

bool ZESkyBrush::SetAttribute(const char* AttributeName, const ZEVariant& Value)
{
	if (stricmp(AttributeName, "SkyTexture") == 0)
		SetSkyTexture(Value.GetString());
	else if (stricmp(AttributeName, "SkyColor") == 0)
		SetSkyColor(Value.GetVector3());
	else if (stricmp(AttributeName, "SkyLightEnabled") == 0)
		SetSkyLightEnabled(Value.GetBoolean());
	else if (stricmp(AttributeName, "SkyLightColor") == 0)
		SetSkyLightColor(Value.GetVector3());
	else if (stricmp(AttributeName, "SkyLightDirection") == 0)
		SetSkyLightDirection(Value.GetVector3());
	else if (stricmp(AttributeName, "SkyLightIntensity") == 0)
		SetSkyLightIntensity(Value.GetFloat());
	else
		return ZEEntity::SetAttribute(AttributeName, Value);

	return true;
}

bool ZESkyBrush::GetAttribute(const char* AttributeName, ZEVariant& Value)
{
	if (stricmp(AttributeName, "SkyTexture") == 0)
		Value =  GetSkyTexture();
	else if (stricmp(AttributeName, "SkyColor") == 0)
		Value =  GetSkyColor();
	else if (stricmp(AttributeName, "SkyLightEnabled") == 0)
		Value = GetSkyLightEnabled();
	else if (stricmp(AttributeName, "SkyLightColor") == 0)
		Value =  GetSkyLightColor();
	else if (stricmp(AttributeName, "SkyLightDirection") == 0)
		Value =  GetSkyLightDirection();
	else if (stricmp(AttributeName, "SkyLightIntensity") == 0)
		Value =  GetSkyLightIntensity();
	else
		return ZEEntity::GetAttribute(AttributeName, Value);

	return true;
}

void ZESkyBrush::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{/*
	if (SkyTexture != NULL)
	{
		SkyMaterial.DiffuseMap = (ZETexture2D*)SkyTexture->GetTexture();
		ZECamera* Camera = zeScene->GetActiveCamera();
		ZEMatrix4x4 CameraRotation, SkyRotation;
		ZEMatrix4x4::CreateRotation(CameraRotation, Camera->GetWorldRotation());
		ZEMatrix4x4::CreateRotation(SkyRotation, GetRotation());
		ZEMatrix4x4::Multiply(SkyRenderOrder.WorldMatrix, CameraRotation, SkyRotation);
		Renderer->AddToRenderOrder(&SkyRenderOrder);
	}*/
}

void ZESkyBrush::Tick(float Time)
{
}

ZESkyBrush::ZESkyBrush()
{
	SkyTexture = NULL;
	SkyBox.AddBox(2.0f, 2.0, 2.0f);
	/*
	SkyMaterial.SetZero();
	SkyMaterial.RecivesShadow = false;
	SkyMaterial.LightningEnabled = false;
	SkyMaterial.SetShaderComponents(ZESHADER_SKY);
	SkyMaterial.TwoSided = true;*/

	SkyRenderOrder.SetZero();
	SkyRenderOrder.Material = SkyMaterial;
	SkyRenderOrder.VertexBuffer = &SkyBox;
	SkyRenderOrder.PrimitiveType = ZE_RLPT_TRIANGLE;
	SkyRenderOrder.PrimitiveCount = SkyBox.Vertices.GetCount() / 3;
	SkyRenderOrder.Flags = ZE_RLF_ENABLE_WORLD_TRANSFORM | ZE_RLF_ENABLE_ZCULLING;
	SkyRenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
}

ZESkyBrush::~ZESkyBrush()
{
}
