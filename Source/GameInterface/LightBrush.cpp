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

ZE_ENTITY_DESCRIPTION_START(ZELightBrush, ZEEntity, ZE_ERA_BOTH, "", "Light brush")
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_START(ZELightType)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("None", ZE_LT_NONE)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Point Light", ZE_LT_POINT)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Directional Light", ZE_LT_DIRECTIONAL)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Projective Light", ZE_LT_PROJECTIVE)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Omni Projective Light", ZE_LT_OMNIPROJECTIVE)
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_END()
	ZE_ENTITY_ATTRIBUTES_START()
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR("LightType", ZE_EAF_NONE, ZELightType, ZE_LT_NONE,"Type of the light")
		ZE_ENTITY_ATTRIBUTE("Intensity", ZEVARIANTTYPE_FLOAT, ZE_EAF_NONE, 1.0f, "Intesity of the light")
		ZE_ENTITY_ATTRIBUTE_SEMANTIC("Color", ZEVARIANTTYPE_VECTOR3, ZE_EAF_NONE, ZE_EAS_COLORVALUE, ZEVector3(1.0f, 1.0f, 1.0f), "Color of the light")
		ZE_ENTITY_ATTRIBUTE("Range", ZEVARIANTTYPE_FLOAT, ZE_EAF_NONE, 10.0f, "Range of the light")
		ZE_ENTITY_ATTRIBUTE("Attenuation", ZEVARIANTTYPE_VECTOR3, ZE_EAF_NONE, ZEVector3(0.0f, 0.0f, 1.0f), "Attenuation of the light")
		ZE_ENTITY_ATTRIBUTE("CastsShadow", ZEVARIANTTYPE_BOOLEAN, ZE_EAF_NONE, false, "Does light casts shadows ?")
		ZE_ENTITY_ATTRIBUTE_FILENAME("ProjectionTexture", ZE_EAF_NONE, "Image", "", "Projection texture")
		ZE_ENTITY_ATTRIBUTE("ProjectionFOV", ZEVARIANTTYPE_FLOAT, ZE_EAF_NONE, ZE_PI_2, "Field of view of the projection")
		ZE_ENTITY_ATTRIBUTE("ProjectionAspectRatio", ZEVARIANTTYPE_FLOAT, ZE_EAF_NONE, 1.0f, "Aspect Ratio of the projection")
	ZE_ENTITY_ATTRIBUTES_END()
ZE_ENTITY_DESCRIPTION_END(ZELightBrush)

bool ZELightBrush::IsDrawable()
{
	return true;
}

void ZELightBrush::Initialize()
{
	Material.SetZero();
	Material.SetShaderComponents(0);
	Material.LightningEnabled = false;
	Material.TwoSided = false;
	Material.Opasity = 1.0f;

	RenderList.Material = &Material;
	RenderList.WorldMatrix = GetWorldTransform();
	RenderList.PrimitiveType = ZE_RLPT_TRIANGLE;
	RenderList.VertexType = ZE_VT_SIMPLEVERTEX;
	RenderList.Flags = ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM | ZE_RLF_ENABLE_ZCULLING;
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
	Light->SetCastShadows(CastsShadow);
	Light->SetEnabled(IsEnabled());

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

	if (RenderList.VertexBuffer != NULL)
		delete RenderList.VertexBuffer;

	ZEAABoundingBox BoundingBox;
	Canvas.CalculateBoundingBox(BoundingBox);
	SetLocalBoundingBox(BoundingBox);
	RenderList.VertexBuffer = Canvas.CreateStaticVertexBuffer();
	RenderList.PrimitiveCount = Canvas.Vertices.GetCount() / 3;
	RegisterComponent(Light);
}

void ZELightBrush::SetColor(const ZEVector3& NewColor)
{
	Color = NewColor;
	if (Light != NULL)
		Light->SetColor(NewColor);
}

const ZEVector3& ZELightBrush::GetColor()
{
	return Color;
}

void ZELightBrush::SetIntensity(float NewValue)
{
	Intensity = NewValue;
	if (Light != NULL)
		Light->SetIntensity(NewValue);
}

float ZELightBrush::GetIntensity()
{
	return Intensity;
}

void ZELightBrush::SetRange(float NewValue)
{
	Range = NewValue;
	if (Light != NULL)
		Light->SetRange(NewValue);

}

float ZELightBrush::GetRange()
{
	return Range;
}

void ZELightBrush::SetProjectionFOV(float NewValue)
{
	ProjectionFOV = NewValue;
	if (Light != NULL && LightType == ZE_LT_PROJECTIVE)
		((ZEProjectiveLight*)Light)->SetFOV(NewValue);

}

float ZELightBrush::GetProjectionFOV()
{
	return ProjectionFOV;
}

void ZELightBrush::SetProjectionAspectRatio(float NewValue)
{
	ProjectionFOV = NewValue;
	if (Light != NULL && LightType == ZE_LT_PROJECTIVE)
		((ZEProjectiveLight*)Light)->SetAspectRatio(NewValue);

}

float ZELightBrush::GetProjectionAspectRatio()
{
	return ProjectionAspectRatio;
}

void ZELightBrush::SetEnabled(bool NewValue)
{
	ZEEntity::SetEnabled(NewValue);
	if (Light != NULL)
		Light->SetEnabled(NewValue);
}
		
void ZELightBrush::SetAttenuation(float DistanceSquare, float Distance, float Constant)
{
	Attenuation = ZEVector3(DistanceSquare, Distance, Constant);
	if (Light != NULL)
		Light->SetAttenuation(DistanceSquare, Distance, Constant);
}

const ZEVector3& ZELightBrush::GetAttenuation()
{
	return Attenuation;
}

bool ZELightBrush::IsCastingShadows()
{
	return CastsShadow;
}

void ZELightBrush::SetCastShadows(bool NewValue)
{
	CastsShadow = NewValue;
	if (Light != NULL)
		Light->SetCastShadows(NewValue);
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
		ProjectionTexture = ZETextureResource::LoadSharedResource(TextureFile);
		if (Light != NULL && ProjectionTexture != NULL && ProjectionTexture->GetTexture() != NULL)
			((ZEProjectiveLight*)Light)->SetProjectionTexture(ProjectionTexture->GetTexture());
	}
	else if (LightType == ZE_LT_OMNIPROJECTIVE)
	{
		CubeProjectionTexture = ZECubeTextureResource::LoadSharedResource(TextureFile);
		if (Light != NULL && CubeProjectionTexture != NULL && CubeProjectionTexture->GetTexture() != NULL)
			((ZEOmniProjectiveLight*)Light)->SetProjectionTexture(CubeProjectionTexture->GetTexture());
	
	}

}

const char* ZELightBrush::GetProjectionTexture()
{
	if (LightType == ZE_LT_PROJECTIVE && ProjectionTexture != NULL)
		return ProjectionTexture->GetFileName();
	else if (LightType == ZE_LT_OMNIPROJECTIVE && CubeProjectionTexture != NULL)
		return CubeProjectionTexture->GetFileName();
	else
		return "";
}

void ZELightBrush::Deinitialize()
{
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

	if (RenderList.VertexBuffer != NULL)
	{
		delete RenderList.VertexBuffer;
		RenderList.VertexBuffer = NULL;
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
		RenderList.WorldMatrix = Light->GetWorldTransform();
		//((ZEDefaultMaterial*)RenderList.Material)->AmbientColor = ZEVector3(0.5, 0.5, 0.5, 1.0f);
		((ZEDefaultMaterial*)RenderList.Material)->EmmisiveColor = Light->GetColor();
		Renderer->AddToRenderList(&RenderList);
	}
}

bool ZELightBrush::SetAttribute(const char* AttributeName, const ZEVariant& Value)
{
		if (strcmp(AttributeName, "LightType") == 0)
			SetLightType((ZELightType)Value.GetInteger());
		else if (strcmp(AttributeName, "Intensity") == 0)
			SetIntensity(Value.GetFloat());
		else if (strcmp(AttributeName, "Range") == 0)
			SetRange(Value.GetFloat());
		else if (strcmp(AttributeName, "Color") == 0)
			SetColor(Value.GetVector3());
		else if (strcmp(AttributeName, "Attenuation") == 0)
			SetAttenuation(Value.GetVector3().x, Value.GetVector3().y ,Value.GetVector3().z);
		else if (strcmp(AttributeName, "CastsShadow") == 0)
			SetCastShadows(Value.GetBoolean());
		else if (strcmp(AttributeName, "ProjectionTexture") == 0)
			SetProjectionTexture(Value.GetString());
		else if (strcmp(AttributeName, "ProjectionFOV") == 0)
			SetProjectionFOV(Value.GetFloat());
		else if (strcmp(AttributeName, "ProjectionAspectRatio") == 0)
			SetProjectionAspectRatio(Value.GetFloat());
		else
			return ZEEntity::SetAttribute(AttributeName, Value);

	return true;
}

bool ZELightBrush::GetAttribute(const char* AttributeName, ZEVariant& Value)
{
	if (strcmp(AttributeName, "LightType") == 0)
		Value = (int)LightType;
	else if (strcmp(AttributeName, "Intensity") == 0)
		Value = GetIntensity();
	else if (strcmp(AttributeName, "Range") == 0)
		Value = GetRange();
	else if (strcmp(AttributeName, "Attenuation") == 0)
		Value = GetAttenuation();
	else if (strcmp(AttributeName, "Color") == 0)
		Value = GetColor();
	else if (strcmp(AttributeName, "CastsShadow") == 0)
		Value = IsCastingShadows();
	else if (strcmp(AttributeName, "ProjectionTexture") == 0)
		Value = GetProjectionTexture();
	else if (strcmp(AttributeName, "ProjectionFOV") == 0)
		Value = GetProjectionFOV();
	else if (strcmp(AttributeName, "ProjectionAspectRatio") == 0)
		Value = GetProjectionAspectRatio();
	else
		return ZEEntity::GetAttribute(AttributeName, Value);

	return true;
}

ZELightBrush::ZELightBrush()
{
	SetBoundingVolumeMechanism(ZE_BVM_USELOCALONLY);
	LightType = ZE_LT_NONE;
	Light = NULL;
	ProjectionTexture = NULL;
	RenderList.SetZero();
	Material.SetZero();
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
