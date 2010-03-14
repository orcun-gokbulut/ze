//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXModule.cpp
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

#include "PhysXModule.h"
#include "PhysXModuleDescription.h"
#include "PhysXPhysicalWorld.h"
#include "PhysXPhysicalRigidBody.h"
#include "PhysXPhysicalStaticObject.h"
#include "PhysXPhysicalStaticMesh.h"
#include "Core/Error.h"
#include "Core/Console.h"

ZEPhysXModule::ZEPhysXModule()
{
	PhysicsSDK = NULL;
}

ZEPhysXModule::~ZEPhysXModule()
{

}

NxPhysicsSDK* ZEPhysXModule::GetPhysicsSDK()
{
	return PhysicsSDK;
}


ZEModuleDescription* ZEPhysXModule::GetModuleDescription()
{
	static ZEPhysXModuleDescription Desc;
	return &Desc;
}

bool ZEPhysXModule::IsEnabled()
{
	return true;
}

void ZEPhysXModule::SetEnabled(bool Enabled)
{

}

bool ZEPhysXModule::Initialize()
{
	zeLog("PhysX Module", "Initializing PhysX.");
	Deinitialize();

	PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &OutputStream);
	if(!PhysicsSDK)
	{
		zeError("PhysX Module", "Can not create PhysX SDK.");
		return false;
	}

	ZEPhysXComponentBase::InitializeBase(this);

	zeLog("PhysX Module", "PhysX intialized.");
	return true;
}

void ZEPhysXModule::Deinitialize()
{
	for (size_t I = 0; I < PhysicalWorlds.GetCount(); I++)
		PhysicalWorlds[I]->Destroy();

	PhysicalWorlds.Clear();

	if (PhysicsSDK != NULL)
	{
		PhysicsSDK->release();
		PhysicsSDK = NULL;
	}
}

ZEPhysicalWorld* ZEPhysXModule::CreatePhysicalWorld()
{
	return new ZEPhysXPhysicalWorld();
}

ZEPhysicalRigidBody* ZEPhysXModule::CreatePhysicalRigidBody()
{
	return new ZEPhysXPhysicalRigidBody();
}

ZEPhysicalStaticObject* ZEPhysXModule::CreatePhysicalStaticObject()
{
	return new ZEPhysXPhysicalStaticObject();
}

ZEPhysicalStaticMesh* ZEPhysXModule::CreatePhysicalStaticMesh()
{
	return new ZEPhysXPhysicalStaticMesh();
}

ZEPhysicalJoint* ZEPhysXModule::CreatePhysicalJoint()
{
	return NULL;
}

ZEPhysicalCloth* ZEPhysXModule::CreatePhysicalCloth()
{
	return NULL;
}

ZEPhysicalSoftBody* ZEPhysXModule::CreatePhysicalSoftBody()
{
	return NULL;
}

ZEPhysicalForceField* ZEPhysXModule::CreatePhysicalForceField()
{
	return NULL;
}

ZEPhysicalTrigger* ZEPhysXModule::CreatePhysicalTrigger()
{
	return NULL;
}

ZEPhysicalVehicle* ZEPhysXModule::CreatePhysicalVehicle()
{
	return NULL;
}

ZEPhysicalCharacterController* ZEPhysXModule::CreatePhysicalController()
{
	return NULL;
}
