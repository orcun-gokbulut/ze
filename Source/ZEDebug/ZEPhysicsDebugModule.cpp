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
#include "ZEPhysics\ZEPhysicalStaticMesh.h"
#include "ZEPhysics\ZEPhysX\ZEPhysXPhysicalWorld.h"
#include "ZEPhysics\ZEPhysX\ZEPhysXPhysicalJoint.h"
#include "ZEModel\ZEModel.h"
#include "ZEModel\ZEModelMesh.h"
#include "ZEModel\ZEModelBone.h"
#include "ZEMap\ZEPortalMap\ZEPortalMap.h"
#include "ZEMap\ZEMapResource.h"
#include "ZEMap\ZEPortalMap\ZEPortalMapPortal.h"

#include "ZEMap\ZEPortalMap\ZEPortalMapResource.h"

#include <NxScene.h>
#include <NxActor.h>
#include <NxPlaneShapeDesc.h>

void ZEPhysicsDebugModule::TransformChanged(const ZEPhysicalTransformChange& TransformChange)
{
	float Pitch, Yaw, Roll;
	ZEQuaternion::ConvertToEulerAngles(Pitch, Yaw, Roll, TransformChange.NewRotation);

	/*zeLog("Physical Object", "Transform Changed. Object: %x, New Position: [%f, %f, %f], New Orientation: [%f, %f, %f]", 
		TransformChange.PhysicalObject,
		TransformChange.NewPosition.x, TransformChange.NewPosition.y, TransformChange.NewPosition.z,
		Pitch, Yaw, Roll);*/
}

void ZEPhysicsDebugModule::ColisionDetected(const ZEPhysicalCollision& Collision)
{
	zeLog("Physical Object", "Collision Occured: Object1 : %x, Object2 : %x", Collision.Collider1, Collision.Collider2);
}

#include "ZEGame\ZELightBrush.h"

bool ZEPhysicsDebugModule::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = (ZEPlayer*)zeGame->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 10.0f, -1.0f));
		Player->SetRotation(ZEQuaternion::Identity);
		Player->GetCamera()->SetNearZ(zeGraphics->GetNearZ());
		Player->GetCamera()->SetFarZ(zeGraphics->GetFarZ());
		Scene->SetActiveCamera(Player->GetCamera());
		Scene->AddEntity(Player);
	}

	if (PhysicalRigidBody == NULL)
	{
		ZEPhysXPhysicalWorld* World = (ZEPhysXPhysicalWorld*)zeScene->GetPhysicalWorld();

		PhysicalRigidBody = ZEPhysicalRigidBody::CreateInstance();
		PhysicalRigidBody->SetPosition(ZEVector3(0.0f, 100.0f, 1.0f));
		Shape.SetRadius(10.0f);
		PhysicalRigidBody->AddPhysicalShape(&Shape);
		Shape2.SetPosition(ZEVector3(10.0f, 0.0f, 0.0f));
		Shape2.SetRadius(4.0f);
		PhysicalRigidBody->AddPhysicalShape(&Shape2);
		PhysicalRigidBody->SetMass(10.0f);
		PhysicalRigidBody->SetKinematic(false);
		PhysicalRigidBody->SetGravityEnabled(true);
		PhysicalRigidBody->SetCollisionCallback(ZEPhysicalCollisionCallback(this, &ZEPhysicsDebugModule::ColisionDetected));
		PhysicalRigidBody->SetTransformChangeCallback(ZEPhysicalTransformChangeCallback(this, &ZEPhysicsDebugModule::TransformChanged));

		PhysicalRigidBody->SetLinearDamping(0.01f);
		//World->AddPhysicalObject(PhysicalRigidBody);
		PhysicalRigidBody->ApplyForce(ZEVector3(1000.0f, 0.0f, 0.0f));

		ZECanvasBrush* CanvasBrush = new ZECanvasBrush();
		//CanvasBrush->SetRotation(ZEQuaternion(ZE_PI_8, ZEVector3(0.0f, 1.0f, 0.0f)));
		//CanvasBrush->SetScale(ZEVector3(0.5f, 0.5f, 0.5f));
		CanvasBrush->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
		CanvasBrush->Canvas.LoadFromFile("TestCanvasMap.zeCanvas");
		CanvasBrush->UpdateCanvas();
		ZEFixedMaterial* CanvasMaterial = ZEFixedMaterial::CreateInstance();
		
		CanvasBrush->Material = CanvasMaterial;
		CanvasMaterial->SetZero();
		CanvasMaterial->SetLightningEnabled(true);
		CanvasMaterial->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);
		CanvasMaterial->SetTwoSided(false);
		CanvasMaterial->SetRecivesShadow(false);
		CanvasMaterial->SetAmbientEnabled(true);
		CanvasMaterial->SetAmbientColor(ZEVector3(0.4f, 0.4f, 0.4f));
		CanvasMaterial->SetDiffuseEnabled(true);
		CanvasMaterial->SetDiffuseColor(ZEVector3::One);
		//CanvasMaterial->SetDiffuseMap(ZETexture2DResource::LoadResource("Test\\Diffuse.tga")->GetTexture());
		CanvasMaterial->SetSpecularEnabled(true);
		CanvasMaterial->SetSpecularColor(ZEVector3::One);
		CanvasMaterial->SetSpecularShininess(64.0f);
		CanvasMaterial->UpdateMaterial();
		zeScene->AddEntity(CanvasBrush);

		ZEArray<ZEVector3> PhysicalVertices;
		ZEArray<ZEPhysicalTriangle> PhysicalTriangles;

		PhysicalVertices.SetCount(CanvasBrush->Canvas.Vertices.GetCount());
		PhysicalTriangles.SetCount(PhysicalVertices.GetCount() / 3);

		for (size_t I = 0; I < PhysicalVertices.GetCount(); I++)
			PhysicalVertices[I] = CanvasBrush->Canvas.Vertices[I].Position;

		for (size_t I = 0; I < PhysicalTriangles.GetCount(); I++)
		{
			PhysicalTriangles[I].MaterialIndex = 0;
			PhysicalTriangles[I].Indices[0] =  3 * I;
			PhysicalTriangles[I].Indices[1] =  3 * I + 1;
			PhysicalTriangles[I].Indices[2] =  3 * I + 2;
		}

		ZEPhysicalStaticMesh* PhysicalMesh = ZEPhysicalStaticMesh::CreateInstance();
		PhysicalMesh->SetPosition(CanvasBrush->GetPosition());
		PhysicalMesh->SetRotation(CanvasBrush->GetRotation());
		PhysicalMesh->SetScale(CanvasBrush->GetScale());

		PhysicalMesh->SetData(PhysicalVertices.GetConstCArray(), PhysicalVertices.GetCount(),
			PhysicalTriangles.GetConstCArray(), PhysicalTriangles.GetCount(),
			NULL, 0);

		World->AddPhysicalObject(PhysicalMesh);

		/*
		ZEPhysicalBoxShape BoxShape;
		ZEPhysicalRigidBody* TestBody = ZEPhysicalRigidBody::CreateInstance();
		TestBody->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
		TestBody->SetRotation(ZEQuaternion(ZE_PI_4, ZEVector3::UnitY));
		BoxShape.SetHeight(1);
		BoxShape.SetWidth(1);
		BoxShape.SetLength(1);
		TestBody->AddPhysicalShape(&BoxShape);
		TestBody->SetKinematic(true);
		World->AddPhysicalObject(TestBody);
		*/

		/*Model = new ZEModel();
		Model->SetModelResource(ZEModelResource::LoadResource("test.zeModel"));
		Model->SetScale(ZEVector3(1.0f, 1.0f, 1.0f));
		Scene->AddEntity(Model);
		//Model->GetModel()->GetBones()[5].SetRelativeRotation(ZEQuaternion(ZE_PI_4, ZEVector3(0.0f, 1.0f, 0.0f)));
		//Model->GetModel()->GetMeshes()[0].SetLocalScale(ZEVector3(0.1, 0.1, 0.1));
		Model->SetAnimationByName("Test");
		//Model->SetAnimationState(ZE_MAS_PLAYING);
		Model->SetAnimationSpeed(66.0f);
		Model->SetAnimationLooping(true);*/

		//zeGame->GetScene()->LoadEnvironment("catacombs.zeMap");

		World->SetVisualize(true);
		Scene->SetVisualDebugElements(ZE_VDE_ALL);
		World->SetEnabled(true);
		

		ZELightBrush* Light = new ZELightBrush();
		Light->SetLightType(ZE_LT_POINT);
		Light->SetPosition(ZEVector3(0.0f, 15.0f, -15.0f));
		Light->SetScale(ZEVector3::One);
		Light->GetLight()->SetRange(15000);
		Light->SetRotation(ZEQuaternion::Identity);
		Light->GetLight()->SetColor(ZEVector3::One);
		Light->GetLight()->SetAttenuation(0.001f, 0.0f, 1.0f);
		Light->GetLight()->SetIntensity(2.0f);
		Light->GetLight()->SetCastsShadows(false);
		/*Light->SetProjectionFOV(ZE_PI_2);
		Light->SetProjectionAspectRatio(1.0f);
		Light->SetProjectionTexture("test/pavyon.bmp");*/
		//Scene->AddEntity(Light);

		/*Model = (ZEModelBrush*)zeGame->CreateEntityInstance("ZEModelBrush");
		Model->SetModelFile("test2.zeModel");
		Scene->AddEntity(Model);*/

		NxPhysicsSDK* gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);

		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);

		/*
		ZEPhysicalCapsuleShape Object1Shape;
		ZEPhysicalRigidBody* Object1 = ZEPhysicalRigidBody::CreateInstance();
		Object1 = ZEPhysicalRigidBody::CreateInstance();
		Object1->SetPosition(ZEVector3(0.0f, 10.0f, 0.0f));
		Object1Shape.SetHeight(2.0f);
		Object1Shape.SetRadius(0.5f);
		Object1->AddPhysicalShape(&Object1Shape);
		Object1->SetKinematic(true);

		World->AddPhysicalObject(Object1);


		ZEPhysicalCapsuleShape Object2Shape;
		ZEPhysicalRigidBody* Object2 = ZEPhysicalRigidBody::CreateInstance();
		Object2 = ZEPhysicalRigidBody::CreateInstance();
		Object2->SetPosition(ZEVector3(0.0f, 7.0f, 0.0f));
		Object2Shape.SetHeight(2.0f);
		Object2Shape.SetRadius(0.5f);
		Object2->AddPhysicalShape(&Object2Shape);
		Object2->SetKinematic(false);
		Object2->SetLinearDamping(0.5f);

		World->AddPhysicalObject(Object2);


		ZEPhysicalCapsuleShape Object3Shape;
		ZEPhysicalRigidBody* Object3 = ZEPhysicalRigidBody::CreateInstance();
		Object3->SetPosition(ZEVector3(0.0f, 4.0f, 0.0f));
		Object3Shape.SetHeight(2.0f);
		Object3Shape.SetRadius(0.5f);
		Object3->AddPhysicalShape(&Object3Shape);
		Object3->SetKinematic(false);
		Object3->SetLinearDamping(0.5f);

		World->AddPhysicalObject(Object3);


		ZEPhysicalJoint* Joint = ZEPhysicalJoint::CreateInstance();
		Joint->SetBodyA(Object1);
		Joint->SetBodyB(Object2);

		ZEQuaternion TempRotation;
		ZEQuaternion::Create(TempRotation, ZE_PI / 2, ZEVector3(0.0f, 0.0f, -1.0f));

		Joint->SetPosition(ZEVector3(0.0f, 8.0f, 0.0f));
		Joint->SetRotation(TempRotation);

		Joint->SetTwistMotion(ZE_PJMOTION_FREE);
		Joint->SetSwing1Motion(ZE_PJMOTION_FREE);
		Joint->SetSwing2Motion(ZE_PJMOTION_FREE);
		Joint->SetXMotion(ZE_PJMOTION_LOCKED);
		Joint->SetYMotion(ZE_PJMOTION_LOCKED);
		Joint->SetZMotion(ZE_PJMOTION_LOCKED);

		World->AddPhysicalObject(Joint);


		ZEPhysicalJoint* Joint2 = ZEPhysicalJoint::CreateInstance();
		Joint2->SetBodyA(Object2);
		Joint2->SetBodyB(Object3);

		Joint2->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
		Joint2->SetRotation(TempRotation);

		Joint2->SetTwistMotion(ZE_PJMOTION_FREE);
		Joint2->SetSwing1Motion(ZE_PJMOTION_FREE);
		Joint2->SetSwing2Motion(ZE_PJMOTION_FREE);
		Joint2->SetXMotion(ZE_PJMOTION_LOCKED);
		Joint2->SetYMotion(ZE_PJMOTION_LOCKED);
		Joint2->SetZMotion(ZE_PJMOTION_LOCKED);

		World->AddPhysicalObject(Joint2);*/

	}

	ZEModelBrush* Skeleton = new ZEModelBrush();
	Skeleton->SetModelFile("3Bones.zemodel");
	Scene->AddEntity(Skeleton);
	Skeleton->SetPosition(ZEVector3(0,0,2));

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
	if (TotalTime > 1.0f)
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





