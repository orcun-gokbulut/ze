//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsDebugComponent.cpp
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

#include "PhysicsDebugComponent.h"

#include "Graphics/GraphicsModule.h"
#include "GameInterface/Game.h"
#include "GameInterface/Player.h"
#include "GameInterface/Scene.h"
#include "Physics/PhysicsModule.h"
#include "Physics/PhysicalWorld.h"
#include "Physics/PhysicalRigidBody.h"
#include "Physics/PhysicalShapes.h"
#include "Physics/PhysX/PhysXPhysicalWorld.h"

#include <NxScene.h>
#include <NxActor.h>
#include <NxPlaneShapeDesc.h>

bool ZEPhysicsDebugComponent::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player != NULL)
	{
		Player = (ZEPlayer*)zeGame->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Player->SetRotation(ZEQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	if (PhysicalRigidBody != NULL)
	{
		ZEPhysXPhysicalWorld* World = (ZEPhysXPhysicalWorld*)zeScene->GetPhysicalWorld();
		NxScene* PhysicalScene = World->GetScene();

		NxActorDesc ActorDesc;
		ActorDesc.body = NULL;

		NxPlaneShapeDesc PlaneShapeDesc;
		PlaneShapeDesc.normal = NxVec3(0.0f, 1.0f, 0.0f);
		PlaneShapeDesc.d = 0.0f;
		PlaneShapeDesc.localPose.t = NxVec3(0.0f, 0.0f, 0.0f);
		ActorDesc.shapes.push_back(&PlaneShapeDesc);

		NxActor* Actor = PhysicalScene->createActor(ActorDesc);

		ZEPhysicalRigidBody* PhysicalBody = ZEPhysicalRigidBody::CreateInstance();
		PhysicalBody->SetPhysicalWorld(World);
		PhysicalBody->AddPhysicalShape(new ZEPhysicalSphereShape());
	}

	return true;
}

void ZEPhysicsDebugComponent::Deinitialize()
{
	if (Player != NULL)
	{
		Player->Destroy();
		Player = NULL;
	}

	if (PhysicalRigidBody != NULL)
	{
		PhysicalRigidBody->Destroy();
		PhysicalRigidBody = NULL;
	}
}

ZEPhysicsDebugComponent::ZEPhysicsDebugComponent()
{
	PhysicalRigidBody = NULL;
	Player = NULL;
}

ZEPhysicsDebugComponent::~ZEPhysicsDebugComponent()
{
	Deinitialize();
}
