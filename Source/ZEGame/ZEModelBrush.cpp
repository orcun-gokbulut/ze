//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelBrush.cpp
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

#include "ZEModelBrush.h"
#include "ZECore\ZEError.h"
#include "ZEGraphics\ZECanvas.h"
#include "ZEModel\ZEModel.h"
#include "ZEModel\ZEModelResource.h"

#include <string.h>

ZEDWORD ZEModelBrush::GetDrawFlags() const
{
	return ZE_DF_DRAW | ZE_DF_CULL | ZE_DF_LIGHT_RECIVER;
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

void ZEModelBrush::SetModelFile(const char* ModelFile)
{
	if (ModelResource != NULL)
		ModelResource->Release();

	if (strcmp(ModelFile, "") != 0)
		ModelResource = ZEModelResource::LoadSharedResource(ModelFile);
	else
		ModelResource = NULL;

	if (Model != NULL)
		Model->SetModelResource(ModelResource);
}	

const char* ZEModelBrush::GetModelFile() const
{
	if (Model->GetModelResource() != NULL)
		return Model->GetModelResource()->GetFileName();
	else
		return "";
}

void ZEModelBrush::SetEnablePhysics(bool Enable)
{
	Model->SetPhysicsEnabled(Enable);
}

bool ZEModelBrush::GetEnablePhysics() const
{
	return Model->GetPhysicsEnabled();
}

void ZEModelBrush::SetAnimationState(ZEModelAnimationState AnimationState)
{
//	Model->SetAnimationState(AnimationState);
}

ZEModelAnimationState ZEModelBrush::GetAnimationState() const
{
//	return Model->GetAnimationState();
	return ZE_MAS_STOPPED;
}

void ZEModelBrush::SetAnimationType(ZEModelAnimationType AnimationType)
{
	Model->SetAnimationType(AnimationType);
}

ZEModelAnimationType ZEModelBrush::GetAnimationType() const
{
	return Model->GetAnimationType();
}

void ZEModelBrush::SetAnimationName(const char* AnimationName)
{
//	Model->SetAnimationByName(strcmp(AnimationName, "") == 0 ? NULL : AnimationName);
}

const char* ZEModelBrush::GetAnimationName() const
{
/*	if (Model->GetAnimation() != NULL)
		return Model->GetAnimation()->Name;
	else*/
		return "";
}

void ZEModelBrush::SetAnimationLooping(bool AnimationLooping)
{
//	Model->SetAnimationLooping(AnimationLooping);
}

bool ZEModelBrush::GetAnimationLooping() const
{
//	return Model->GetAnimationLooping();
	return false;
}

void ZEModelBrush::SetAnimationFrame(int AnimationFrame)
{
//	Model->SetAnimationFrame(AnimationFrame);
}

int ZEModelBrush::GetAnimationFrame() const
{
//	return Model->GetAnimationFrame();
	return 0;
}

void ZEModelBrush::SetAnimationSpeed(float AnimationSpeed)
{
//	Model->SetAnimationSpeed(AnimationSpeed);
}

float ZEModelBrush::GetAnimationSpeed() const
{
//	return Model->GetAnimationSpeed();
	return 0.0f;
}

bool ZEModelBrush::Initialize()
{
	if (Model == NULL)
	{
		Model = new ZEModel();
		RegisterComponent(Model);
		if (ModelResource != NULL)
			Model->SetModelResource(ModelResource);
		Model->Initialize();
	}

	return true;
}

void ZEModelBrush::Deinitialize()
{
	if (Model != NULL)
	{
		UnregisterComponent(Model);
		Model->Destroy();
		Model = NULL;
	}

	if (ModelResource != NULL)
	{
		ModelResource->Release();
		ModelResource = NULL;
	}
}

void ZEModelBrush::Reset()
{

}

void ZEModelBrush::Update()
{

}

void ZEModelBrush::Draw(ZEDrawParameters* DrawParameters)
{
	if (Model != NULL)
		Model->Draw(DrawParameters);
}

ZEModelBrush::ZEModelBrush()
{
	Model = NULL;
	ModelResource = NULL;
	SetBoundingVolumeMechanism(ZE_BVM_USE_COMPONENTS);
}

ZEModelBrush::~ZEModelBrush()
{
	Deinitialize();
}

#include "ZEModelBrush.h.zpp"

ZEEntityRunAt ZEModelBrushDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}





