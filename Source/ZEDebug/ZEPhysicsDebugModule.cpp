//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicsDebugModule.cpp
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

#include "ZEPhysicsDebugModule.h"

#include "ZECore\ZEConsole.h"
#include "ZEGraphics\ZEGraphicsModule.h"
#include "ZEGraphics\ZEFixedMaterial.h"
#include "ZEGraphics\ZETexture2DResource.h"
#include "ZEGame\ZEGame.h"
#include "ZEGame\ZEPlayer.h"
#include "ZEGame\ZECanvasBrush.h"
#include "ZEGame\ZEScene.h"
#include "ZEPhysics\ZEPhysicsModule.h"
#include "ZEPhysics\ZEPhysicalWorld.h"
#include "ZEPhysics\ZEPhysicalRigidBody.h"
#include "ZEPhysics\ZEPhysicalShapes.h"
#include "ZEPhysics\ZEPhysicalMesh.h"
#include "ZEModel\ZEModel.h"
#include "ZEModel\ZEModelMesh.h"
#include "ZEModel\ZEModelBone.h"
#include "ZEGame\ZEEntityProvider.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEGraphics\ZEPointLight.h"
#include "ZEMap\ZEPortalMap\ZEPortalMap.h"
#include "ZEMap\ZEMapResource.h"
#include "ZEMap\ZEPortalMap\ZEPortalMapPortal.h"
#include "ZEMath\ZEMathDefinitions.h"

#include "ZEMap\ZEPortalMap\ZEPortalMapResource.h"

#include "../ZEGraphics/ZEDirectionalLight.h"

void ZEPhysicsDebugModule::TransformChanged(const ZEPhysicalTransformChangeEventArgument& TransformChange)
{
	float Pitch, Yaw, Roll;
	ZEQuaternion::ConvertToEulerAngles(Pitch, Yaw, Roll, TransformChange.NewRotation);

	/*zeLog("Physical Object", "Transform Changed. Object: %x, New Position: [%f, %f, %f], New Orientation: [%f, %f, %f]", 
	TransformChange.PhysicalObject,
	TransformChange.NewPosition.x, TransformChange.NewPosition.y, TransformChange.NewPosition.z,
	Pitch, Yaw, Roll);*/
}

void ZEPhysicsDebugModule::ColisionDetected(const ZEPhysicalCollisionEventArgument& Collision)
{
	zeLog("Physical Object", "Collision Occured: Object1 : %x, Object2 : %x", Collision.Collider1, Collision.Collider2);
}


bool ZEPhysicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = ZEPlayer::CreateInstance();
		Player->SetPosition(ZEVector3(-7.0f, 44.0f, 7.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	ZEPhysicalWorld* World = (ZEPhysicalWorld*)zeScene->GetPhysicalWorld();

	World->SetVisualize(true);
	//Scene->SetVisualDebugElements(ZE_VDE_ALL);
	Scene->SetVisualDebugElements(ZE_VDE_NONE);
	World->SetEnabled(true);

	ZEDirectionalLight* Light = ZEDirectionalLight::CreateInstance();
	ZEQuaternion TempQuat;
	ZEQuaternion::CreateFromEuler(TempQuat, -ZE_PI_4, -ZE_PI_4, 0);
	Light->SetRotation(TempQuat);
	Light->SetIntensity(0.6f);
	Scene->AddEntity(Light);

	/*
	TestBody = ZEModel::CreateInstance();
	TestBody->SetModelFile("Orkun.zemodel");
	TestBody->SetPosition(ZEVector3(0,50,0));
	Scene->AddEntity(TestBody);
	*/

	TestBody = ZEModel::CreateInstance();
	TestBody->SetModelFile("demlik.zemodel");
	TestBody->SetPosition(ZEVector3(0,60,0));
	Scene->AddEntity(TestBody);

	/*int StartPos = -40.0f;

	for (int I = 0; I < 10; I++)
	{
		TestBody = ZEModel::CreateInstance();
		TestBody->SetModelFile("Orkun.zemodel");
		TestBody->SetPosition(ZEVector3(StartPos, 50, 0));
		StartPos = StartPos + 3;
		Scene->AddEntity(TestBody);
	}*/
	
	//TestBody = ZEModel::CreateInstance();
	//TestBody->SetModelFile("Orkun.zemodel");
	//TestBody->SetPosition(ZEVector3(0,60,0));
	//Scene->AddEntity(TestBody);

	ZEPointLight* PointLight = ZEPointLight::CreateInstance();
	PointLight->SetIntensity(2);
	PointLight->SetAttenuation(ZEVector3(0.01f, 0.0f, 0.01f));
	PointLight->SetPosition(ZEVector3(0,70,0));
	Scene->AddEntity(PointLight);


//	NxPhysicsSDK* gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
//	gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);



	return true;
}

void ZEPhysicsDebugModule::Deinitialize()
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

void ZEPhysicsDebugModule::Process(float ElapsedTime)
{
	static float TotalTime = 0.0f;

	TotalTime += ElapsedTime;
	if (TotalTime > 5.0f)
	{
		const ZECullStatistics& Stats = zeScene->GetCullStatistics();

		/*zeLog("Scene", 
		"TotalEntityCount: %d, \r\n"
		"TotalComponentCount: %d, \r\n"
		"TotalLightCount: %d, \r\n"
		"DrawableEntityCount: %d, \r\n"
		"DrawableComponentCount: %d, \r\n"
		"VisibleEntityCount: %d, \r\n"
		"VisibleComponentCount: %d, \r\n"
		"VisibleLightCount: %d, \r\n"
		"CulledEntityCount: %d, \r\n"
		"CulledComponentCount: %d, \r\n"
		"CulledLightCount: %d, \r\n"
		"MaxLightPerEntity: %d, \r\n"
		"MaxLightPerComponent: %d",
		Stats.TotalEntityCount,
		Stats.TotalComponentCount,
		Stats.TotalLightCount,
		Stats.DrawableEntityCount,
		Stats.DrawableComponentCount,
		Stats.VisibleEntityCount,
		Stats.VisibleComponentCount,
		Stats.VisibleLightCount,
		Stats.CulledEntityCount,
		Stats.CulledComponentCount,
		Stats.CulledLightCount,
		Stats.MaxLightPerEntity,
		Stats.MaxLightPerComponent);*/
		TotalTime = 0.0f;
	}
}


ZEPhysicsDebugModule::ZEPhysicsDebugModule()
{
	PhysicalRigidBody = NULL;
	Player = NULL;
}

ZEPhysicsDebugModule::~ZEPhysicsDebugModule()
{
	Deinitialize();
}
