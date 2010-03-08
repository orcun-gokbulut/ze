//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsModule.cpp
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
#include "PhysXPhysicsModule.h"
#include "PhysXPhysicsModuleDescription.h"

#include "Physics/PhysicsWorld.h"
#include "PhysXPhysicsWorld.h"

#include "Physics/PhysicsBody.h"
#include "PhysXPhysicsBody.h"

#include "Physics/PhysicsJoint.h"
#include "PhysXPhysicsJoint.h"

#include "Physics/PhysicsVehicle.h"
#include "PhysXPhysicsVehicle.h"

#include "Physics/PhysicsMaterial.h"
#include "PhysXPhysicsMaterial.h"

#include "Physics/PhysicsCharacterController.h"
#include "PhysXPhysicsCharacterController.h"

ZEModuleDescription* ZEPhysXPhysicsModule::GetModuleDescription()
{
	static ZEPhysXPhysicsModuleDescription Desc;
	return &Desc;
}

bool ZEPhysXPhysicsModule::Initialize()
{
	zeLog("PhysX Module", "Initializing PhysX Physics module.");
	Enabled = true;
	return true;
}

void ZEPhysXPhysicsModule::Deinitialize()
{
	Enabled = false;
	zeLog("PhysX Module", "Destroying PhysX Physics module.");
}

ZEPhysicsWorld* ZEPhysXPhysicsModule::CreateWorld()
{
	if (ZEPhysXPhysicsWorld::getSingletonPtr() == NULL)
	{
		new ZEPhysXPhysicsWorld();
	}
	return ZEPhysXPhysicsWorld::getSingletonPtr();
}

ZEPhysicsBody* ZEPhysXPhysicsModule::CreateBody()
{
	return new ZEPhysXPhysicsBody();
}

ZEPhysicsJoint* ZEPhysXPhysicsModule::CreateJoint()
{
	return new ZEPhysXPhysicsJoint();
}

ZEPhysicsVehicle* ZEPhysXPhysicsModule::CreateVehicle()
{
	return new ZEPhysXPhysicsVehicle();
}

ZEPhysicsMaterial* ZEPhysXPhysicsModule::CreateMaterial()
{
	return new ZEPhysXPhysicsMaterial();
}

ZEPhysicsCharacterController* ZEPhysXPhysicsModule::CreateController()
{
	return new ZEPhysXPhysicsCharacterController();
}

bool ZEPhysXPhysicsModule::DestroyWorld(ZEPhysicsWorld* World)
{
	delete (ZEPhysXPhysicsWorld*)World;
	World = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::DestroyBody(ZEPhysicsBody* Body)
{
	delete (ZEPhysXPhysicsBody*)Body;
	Body = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::DestroyJoint(ZEPhysicsJoint* Joint)
{
	delete (ZEPhysXPhysicsJoint*)Joint;
	Joint = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::DestroyVehicle(ZEPhysicsVehicle* Joint)
{
	delete (ZEPhysXPhysicsVehicle*)Joint;
	Joint = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::DestroyMaterial(ZEPhysicsMaterial* Material)
{
	delete (ZEPhysXPhysicsMaterial*)Material;
	Material = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::DestroyController(ZEPhysicsCharacterController* Controller)
{
	delete (ZEPhysXPhysicsCharacterController*)Controller;
	Controller = NULL;
	return true;
}

bool ZEPhysXPhysicsModule::CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEVector3& Point, ZEPhysicsCollisionMask Mask)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxRay Ray(TONX(Origin), TONX(Direction));
		NxRaycastHit Hit;

		//Get the closest shape
		NxShape* ClosestShape = World->GetScene()->raycastClosestShape(Ray, NX_ALL_SHAPES, Hit, -1, NX_MAX_F32, -1, &ZEPhysXPhysicsUtility::toNX(Mask));
		if (ClosestShape)
		{
			Point = TOZE(Hit.worldImpact);
			return true;
		}
	}
	return false;
}

bool ZEPhysXPhysicsModule::CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEPhysicsBody** Contact, ZEPhysicsCollisionMask Mask)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxRay Ray(TONX(Origin), TONX(Direction));
		NxRaycastHit Hit;

		//Get the closest shape
		NxShape* ClosestShape = World->GetScene()->raycastClosestShape(Ray, NX_ALL_SHAPES, Hit, -1, NX_MAX_F32, -1, &ZEPhysXPhysicsUtility::toNX(Mask));
		if (ClosestShape)
		{
			*Contact = (ZEPhysicsBody*)ClosestShape->getActor().userData;
			return true;
		}
	}
	*Contact = NULL;
	return false;
}

bool ZEPhysXPhysicsModule::BoxSweep(ZEVector3 Center, ZEVector3 Dimensions, ZEVector3 Motion, ZEVector3& Point, ZEPhysicsCollisionMask Mask)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxBox Box;
		Box.center	= TONX(Center);
		Box.extents	= TONX(Dimensions);;

		NxSweepQueryHit Result;
		if (World->GetScene()->linearOBBSweep(Box, TONX(Motion), NX_SF_STATICS|NX_SF_DYNAMICS, NULL, 1, &Result, NULL, -1, &ZEPhysXPhysicsUtility::toNX(Mask)) > 0)
		{
			Point = TOZE(Result.point);
			return true;
		}
	}
	return false;
}

bool ZEPhysXPhysicsModule::CapsuleSweep(ZEVector3 Center, float Radius, ZEVector3 Motion, ZEVector3& Point, ZEPhysicsCollisionMask Mask)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		NxCapsule Capsule;
		Capsule.radius	= Radius;
		Capsule.p0 = TONX(Center);
		Capsule.p1 = TONX(Center);
	    
		NxSweepQueryHit Result;
		if (World->GetScene()->linearCapsuleSweep(Capsule, TONX(Motion), NX_SF_STATICS|NX_SF_DYNAMICS, NULL, 1, &Result, NULL, -1, &ZEPhysXPhysicsUtility::toNX(Mask)) > 0)
		{
			Point = TOZE(Result.point);
			return true;
		}
	}
	return false;
}
