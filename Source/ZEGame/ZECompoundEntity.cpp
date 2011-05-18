//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompoundEntity.cpp
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

#include "ZECompoundEntity.h"
#include "ZEMath\ZEMatrix.h"
#include "ZEMath\ZERay.h"
#include "ZECore\ZEError.h"
#include "ZEComponent.h"
#include <string.h>

void ZECompoundEntity::UpdateComponentTransforms()
{
	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->OnTransformChanged();
}

void ZECompoundEntity::RegisterComponent(ZEComponent* Component)
{
	zeAssert(Component->Owner != NULL, "Component already has a owner. Can not register component.");

	Component->Owner = this;
	
	if (GetInitialized())
		Component->Initialize();

	Components.Add(Component);
}

void ZECompoundEntity::UnregisterComponent(ZEComponent* Component)
{
	if (GetInitialized())
		Component->Deinitialize();

	Components.DeleteValue(Component);
	Component->Owner = NULL;

	Component->Destroy();
} 
		
ZEDWORD ZECompoundEntity::GetRayCastFlags() const
{
	return 0;
}

ZEEntityType ZECompoundEntity::GetEntityType()
{
	return ZE_ET_COMPOUND;
}

const ZEArray<ZEComponent*>& ZECompoundEntity::GetComponents() const
{
	return Components; 
}

void ZECompoundEntity::SetPosition(const ZEVector3& NewPosition) 
{
	ZEEntity::SetPosition(NewPosition);
	UpdateComponentTransforms();
}

void ZECompoundEntity::SetRotation(const ZEQuaternion& NewRotation) 
{
	ZEEntity::SetRotation(NewRotation);
	UpdateComponentTransforms();
}

void ZECompoundEntity::SetScale(const ZEVector3& NewScale)
{
	ZEEntity::SetScale(NewScale);
	UpdateComponentTransforms();
}

void ZECompoundEntity::SetVelocity(const ZEVector3& NewVelocity)
{
	ZEEntity::SetVelocity(NewVelocity);
	//Velocity = NewVelocity;
}

bool ZECompoundEntity::Initialize()
{
	if (GetInitialized())
		return false;

	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Initialize();

	return ZEEntity::Initialize();
}

void ZECompoundEntity::Deinitialize()
{
	if (GetInitialized())
		return;

	for (size_t I = 0; I < Components.GetCount(); I++)
		Components[I]->Deinitialize();

	ZEEntity::Deinitialize();
}

void ZECompoundEntity::Tick(float Time)
{
	ZEEntity::Tick(Time);
	for (size_t I = 0; I < Components.GetCount(); I++)
		if (Components[I]->GetEnabled())
			Components[I]->Tick(Time);
}

ZECompoundEntity::ZECompoundEntity()
{

}

ZECompoundEntity::~ZECompoundEntity()
{
	Deinitialize();
	zeWarningAssert(Components.GetCount() != 0, "Entity : %s. There are uninitialized component available.", GetClassDescription()->GetName());
}

ZEEntityRunAt ZECompoundEntityDescription::GetRunAt() const
{
	return ZE_ERA_BOTH;
}

#include "ZECompoundEntity.h.zpp"





