//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsModule.cpp
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

#include "Core/Error.h"
#include "Core/Console.h"
#include "Core/Module.h"
#include "ZEMath/Vector.h"

#include "Physics/PhysicsModule.h"
#include "Physics/AegiaPhysicsModule.h"
#include "Physics/AegiaPhysicsModuleDescription.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/AegiaPhysicsWorld.h"

#include "Physics/PhysicsBody.h"
#include "Physics/AegiaPhysicsBody.h"

#include "Physics/PhysicsJoint.h"
#include "Physics/AegiaPhysicsJoint.h"

#include "Physics/PhysicsVehicle.h"
#include "Physics/AegiaPhysicsVehicle.h"

#include "Physics/PhysicsMaterial.h"
#include "Physics/AegiaPhysicsMaterial.h"

#include "Physics/PhysicsCharacterController.h"
#include "Physics/AegiaPhysicsCharacterController.h"

ZEModuleDescription* ZEAegiaPhysicsModule::GetModuleDescription()
{
	static ZEAegiaPhysicsModuleDescription Desc;
	return &Desc;
}

bool ZEAegiaPhysicsModule::Initialize()
{
	zeLog("Initializing Aegia Physics module.\n");
	Enabled = true;
	return true;
}

void ZEAegiaPhysicsModule::Deinitialize()
{
	Enabled = false;
	zeLog("Destroying Aegia Physics module.\n");
}

ZEPhysicsWorld* ZEAegiaPhysicsModule::CreateWorld()
{
	if (ZEAegiaPhysicsWorld::getSingletonPtr() == NULL)
	{
		new ZEAegiaPhysicsWorld();
	}
	return ZEAegiaPhysicsWorld::getSingletonPtr();
}

ZEPhysicsBody* ZEAegiaPhysicsModule::CreateBody()
{
	return new ZEAegiaPhysicsBody();
}

ZEPhysicsJoint* ZEAegiaPhysicsModule::CreateJoint()
{
	return new ZEAegiaPhysicsJoint();
}

ZEPhysicsVehicle* ZEAegiaPhysicsModule::CreateVehicle()
{
	return new ZEAegiaPhysicsVehicle();
}

ZEPhysicsMaterial* ZEAegiaPhysicsModule::CreateMaterial()
{
	return new ZEAegiaPhysicsMaterial();
}

ZEPhysicsCharacterController* ZEAegiaPhysicsModule::CreateController()
{
	return new ZEAegiaPhysicsCharacterController();
}

bool ZEAegiaPhysicsModule::DestroyWorld(ZEPhysicsWorld* World)
{
	delete (ZEAegiaPhysicsWorld*)World;
	World = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::DestroyBody(ZEPhysicsBody* Body)
{
	delete (ZEAegiaPhysicsBody*)Body;
	Body = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::DestroyJoint(ZEPhysicsJoint* Joint)
{
	delete (ZEAegiaPhysicsJoint*)Joint;
	Joint = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::DestroyVehicle(ZEPhysicsVehicle* Joint)
{
	delete (ZEAegiaPhysicsVehicle*)Joint;
	Joint = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::DestroyMaterial(ZEPhysicsMaterial* Material)
{
	delete (ZEAegiaPhysicsMaterial*)Material;
	Material = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::DestroyController(ZEPhysicsCharacterController* Controller)
{
	delete (ZEAegiaPhysicsCharacterController*)Controller;
	Controller = NULL;
	return true;
}

bool ZEAegiaPhysicsModule::CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEVector3& Point, ZEPhysicsCollisionMask Mask)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxRay Ray(TONX(Origin), TONX(Direction));
		NxRaycastHit Hit;

		//Get the closest shape
		NxShape* ClosestShape = World->GetScene()->raycastClosestShape(Ray, NX_ALL_SHAPES, Hit, -1, NX_MAX_F32, -1, &ZEAegiaPhysicsUtility::toNX(Mask));
		if (ClosestShape)
		{
			Point = TOZE(Hit.worldImpact);
			return true;
		}
		return false;
	}
	return false;
}

bool ZEAegiaPhysicsModule::CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEPhysicsBody** Contact, ZEPhysicsCollisionMask Mask)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxRay Ray(TONX(Origin), TONX(Direction));
		NxRaycastHit Hit;

		//Get the closest shape
		NxShape* ClosestShape = World->GetScene()->raycastClosestShape(Ray, NX_ALL_SHAPES, Hit, -1, NX_MAX_F32, -1, &ZEAegiaPhysicsUtility::toNX(Mask));
		if (ClosestShape)
		{
			*Contact = (ZEPhysicsBody*)ClosestShape->getActor().userData;
			return true;
		}
		return false;
	}
	*Contact = NULL;
	return false;
}

