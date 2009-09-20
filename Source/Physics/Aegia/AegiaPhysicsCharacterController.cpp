//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsCharacterController.cpp
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

#include "NxPhysics.h"
#include "NxControllerManager.h"
#include "NxBoxController.h"
#include "NxCapsuleController.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"

#include "Physics/PhysicsCharacterController.h"
#include "Physics/PhysicsCharacterControllerInfo.h"
#include "Physics/Aegia/AegiaPhysicsCharacterController.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Aegia/AegiaPhysicsWorld.h"
#include "Physics/PhysicsCollisionMask.h"


class ZEControllerHitReport : public NxUserControllerHitReport
{
public:
	virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit)
	{
		if(1 && hit.shape)
		{
			NxActor& actor = hit.shape->getActor();

			if (actor.isDynamic())
			{
				if(hit.dir.y == 0.0f)
				{
					NxF32 coeff = hit.length * 64.0f;
					actor.addForceAtLocalPos(hit.dir*coeff, NxVec3(0,0,0), NX_IMPULSE);
				}
			}
		}
		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

}ControllerHitReport;

ZEAegiaPhysicsCharacterController::ZEAegiaPhysicsCharacterController() : Controller(NULL), Velocity(0,0,0), CollisionFlag(0)
{
}

ZEAegiaPhysicsCharacterController::~ZEAegiaPhysicsCharacterController()
{
	Deinitialize();
}

void ZEAegiaPhysicsCharacterController::Initialize(ZEPhysicsCharacterControllerInfo& Info)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();

	if (Controller == NULL && World != NULL)
	{
		NxCapsuleControllerDesc CapsuleDesc;
		CapsuleDesc.height = Info.Height;
		CapsuleDesc.radius = Info.Radius;
		CapsuleDesc.stepOffset = Info.StepLimit;
		CapsuleDesc.slopeLimit = NxMath::cos(NxMath::degToRad(Info.SlopeLimit));
		CapsuleDesc.skinWidth = 0.1;
		CapsuleDesc.position.x = Info.Position.x;
		CapsuleDesc.position.y = Info.Position.y;
		CapsuleDesc.position.z = Info.Position.z;
		CapsuleDesc.upDirection = NX_Y;
		CapsuleDesc.callback = &ControllerHitReport;

		if (CapsuleDesc.isValid())
		{
			//Controller = (NxCapsuleController*)World->GetControllerManager()->createController(World->GetScene(), CapsuleDesc);
		}
	}
}

void ZEAegiaPhysicsCharacterController::Deinitialize()
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();

	if (Controller != NULL && World != NULL)
	{
		World->GetControllerManager()->releaseController(*Controller);
	}
}

void ZEAegiaPhysicsCharacterController::Update(float ElapsedTime)
{
	ZEVector3 Displacement;
	ZEVector3::Scale(Displacement, Velocity, ElapsedTime);
	ZEPhysicsCollisionMask Mask;Mask.Full();
	Controller->move(TONX(Displacement), Mask.Mask1, 0.0, CollisionFlag);
}
