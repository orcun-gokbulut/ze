//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsReport.cpp
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
#include "PhysXPhysicsReport.h"

#include "ZEMath/Vector.h"
#include "Physics/PhysicsCollision.h"
#include "PhysXPhysicsCollision.h"
#include "Physics/PhysicsTrigger.h"
#include "PhysXPhysicsTrigger.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsWorldInfo.h"
#include "PhysXPhysicsWorld.h"
#include "PhysXPhysicsUtility.h"

void ZEPhysXPhysicsReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		if (!World->DelegateC.empty())
		{
			NxContactStreamIterator j(pair.stream);
			
			if (j.goNextPair() && j.goNextPatch() && j.goNextPoint())
			{
				ZEPhysXPhysicsCollision Coll((ZEPhysicsBody*)pair.actors[0]->userData,
											 (ZEPhysicsBody*)pair.actors[1]->userData,
													               TOZE(j.getPoint()),
													         TOZE(j.getPatchNormal()),
													         j.getPointNormalForce());
				World->DelegateC(&Coll);
			}
		}
	}
}

void ZEPhysXPhysicsReport::onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
{
	ZEPhysXPhysicsWorld* World = ZEPhysXPhysicsWorld::getSingletonPtr();
	if (World != NULL)
	{
		if (!World->DelegateT.empty())
		{
				ZEPhysXPhysicsTrigger Trig((ZEPhysicsBody*)triggerShape.userData,
										   (ZEPhysicsBody*)  otherShape.userData,
													                     status);
				World->DelegateT(&Trig);
		}
	}
}
