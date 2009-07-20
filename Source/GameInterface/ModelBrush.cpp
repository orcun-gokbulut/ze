//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ModelBrush.cpp
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

#include "ModelBrush.h"
#include "Core/Error.h"
#include "Graphics/Canvas.h"

ZE_ENTITY_DESCRIPTION_START(ZEModelBrush, ZEEntity, ZE_ERA_BOTH, "", "This entity used for displaying animated visual models in the game environments like statues, mechineries, furnitures and etc.")
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_START(ZEModelAnimationState)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Static Frame", ZE_MAS_STATICFRAME)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Playing", ZE_MAS_PLAYING)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Paused", ZE_MAS_PAUSED)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Stopped", ZE_MAS_STOPPED)
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_END()
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_START(ZEModelAnimationType)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("No Animation", ZE_MAT_NOANIMATION)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Predifined", ZE_MAT_PREDEFINED)
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR_ELEMENT("Physical", ZE_MAT_PHYSICAL)
	ZE_ENTITY_ATTRIBUTE_ENUMURATOR_END()
	ZE_ENTITY_ATTRIBUTES_START()
		ZE_ENTITY_ATTRIBUTE_FILENAME("ModelFile", ZE_EAF_NONE, "zeModel", "", "Filename of the model that will be used")
		ZE_ENTITY_ATTRIBUTE("EnablePhysics", ZEVARIANTTYPE_BOOLEAN, ZE_EAF_NONE, true, "Will model brush will have physical interaction")
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR("AnimationState", ZE_EAF_NONE, ZEModelAnimationState, ZE_MAS_STOPPED, "Animation state of the model brush.")
		ZE_ENTITY_ATTRIBUTE_ENUMURATOR("AnimationType", ZE_EAF_NONE, ZEModelAnimationType, ZE_MAT_NOANIMATION, "Animation type of the model brush.")
		ZE_ENTITY_ATTRIBUTE("AnimationName", ZEVARIANTTYPE_STRING, ZE_EAF_NONE, "", "Animation name of the model brush")
		ZE_ENTITY_ATTRIBUTE("AnimationLooping", ZEVARIANTTYPE_BOOLEAN, ZE_EAF_NONE, true, "Animation loooping")
		ZE_ENTITY_ATTRIBUTE("AnimationFrame", ZEVARIANTTYPE_INTEGER, ZE_EAF_NONE, 0, "Animation Frame")
		ZE_ENTITY_ATTRIBUTE("AnimationSpeed", ZEVARIANTTYPE_FLOAT, ZE_EAF_NONE, 0, "Speed of the animation being played.")
	ZE_ENTITY_ATTRIBUTES_END()
ZE_ENTITY_DESCRIPTION_END(ZEModelBrush)

bool ZEModelBrush::IsDrawable()
{
	return true;
}

		
void ZEModelBrush::SetModelFile(const char* ModelFile)
{
	Model->SetModelResource(ZEModelResource::LoadSharedResource(ModelFile));
}

const char* ZEModelBrush::GetModelFileName()
{
	if (Model != NULL)
		return Model->GetModelResource()->GetFileName();
}
	
ZEModel* ZEModelBrush::GetModel()
{
	return Model;
}	

const ZEAABoundingBox& ZEModelBrush::GetLocalBoundingBox()
{
	return Model->GetLocalBoundingBox();
}

const ZEAABoundingBox& ZEModelBrush::GetWorldBoundingBox()
{
	return Model->GetWorldBoundingBox();
}

 void ZEModelBrush::Initialize()
{
	if (Model == NULL)
	{
		Model = new ZEModel();
		RegisterComponent(Model);
	}
}

void ZEModelBrush::Deinitialize()
{
	if (Model != NULL)
	{
		UnregisterComponent(Model);
		delete Model;
	}
}

void ZEModelBrush::Reset()
{

}

void ZEModelBrush::Update()
{

}

void ZEModelBrush::Draw(ZERenderer* Renderer, const ZESmartArray<const ZERLLight*>& Lights)
{
	if (Model != NULL)
		Model->Draw(Renderer, Lights);
}

bool ZEModelBrush::SetAttribute(const char* AttributeName, const ZEVariant& Value)
{
	if (strcmp(AttributeName, "ModelFile") == 0)
		if (strcmp(Value.GetString(), "") != 0)
			Model->SetModelResource(ZEModelResource::LoadSharedResource(Value.GetString()));
		else
			Model->SetModelResource(NULL);
	else if (strcmp(AttributeName, "EnablePhysics") == 0)
		Model->SetPhysicsEnabled(Value.GetBoolean());
	else if (strcmp(AttributeName, "AnimationState") == 0)
		Model->SetAnimationState((ZEModelAnimationState)Value.GetInteger());
	else if (strcmp(AttributeName, "AnimationType") == 0)
		Model->SetAnimationType((ZEModelAnimationType)Value.GetInteger());
	else if (strcmp(AttributeName, "AnimationName") == 0)
		Model->SetAnimationByName(strcmp(Value.GetString(), "") == 0 ? NULL : Value.GetString());
	else if (strcmp(AttributeName, "AnimationLooping") == 0)
		Model->SetAnimationLooping(Value.GetBoolean());
	else if (strcmp(AttributeName, "AnimationFrame") == 0)
		Model->SetAnimationFrame(Value.GetInteger());
	else if (strcmp(AttributeName, "AnimationSpeed") == 0)
		Model->SetAnimationSpeed(Value.GetFloat());
	else
		return ZEEntity::SetAttribute(AttributeName, Value);

	return true;
}

bool ZEModelBrush::GetAttribute(const char* AttributeName, ZEVariant& Value)
{
	if (strcmp(AttributeName, "ModelFile") == 0)
		if (Model == NULL || Model->GetModelResource() == NULL)
			Value = "";
		else
			Value =  Model->GetModelResource()->GetFileName();
	else if (strcmp(AttributeName, "EnablePhysics") == 0)
		Value = Model->GetPhysicsEnabled();
	else if (strcmp(AttributeName, "AnimationState") == 0)
		Value = Model->GetAnimationState();
	else if (strcmp(AttributeName, "AnimationType") == 0)
		Value = Model->GetAnimationType();
	else if (strcmp(AttributeName, "AnimationName") == 0)
		Value = (Model->GetAnimation() != NULL ? Model->GetAnimation()->Name : "");
	else if (strcmp(AttributeName, "AnimationLooping") == 0)
		Value = Model->GetAnimationLooping();
	else if (strcmp(AttributeName, "AnimationFrame") == 0)
		Value = (int)Model->GetAnimationFrame();
	else if (strcmp(AttributeName, "AnimationSpeed") == 0)
		Value = (float)Model->GetAnimationSpeed();
	else
		return ZEEntity::GetAttribute(AttributeName, Value);

	return true;
}
ZEModelBrush::ZEModelBrush()
{
	Model = NULL;
	SetBoundingVolumeMechanism(ZE_BVM_USECOMPONENTS);
}

ZEModelBrush::~ZEModelBrush()
{
	Deinitialize();
}
