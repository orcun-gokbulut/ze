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

#include "Core/Console.h"
#include "Graphics/GraphicsModule.h"
#include "Graphics/FixedMaterial.h"
#include "Graphics/Texture2DResource.h"
#include "GameInterface/Game.h"
#include "GameInterface/Player.h"
#include "GameInterface/CanvasBrush.h"
#include "GameInterface/Scene.h"
#include "Physics/PhysicsModule.h"
#include "Physics/PhysicalWorld.h"
#include "Physics/PhysicalRigidBody.h"
#include "Physics/PhysicalShapes.h"
#include "Physics/PhysicalStaticMesh.h"
#include "Physics/PhysX/PhysXPhysicalWorld.h"


#include <NxScene.h>
#include <NxActor.h>
#include <NxPlaneShapeDesc.h>

void ZEPhysicsDebugComponent::TransformChanged(const ZEPhysicalTransformChange& TransformChange)
{
	float Pitch, Yaw, Roll;
	ZEQuaternion::ConvertToEulerAngles(Pitch, Yaw, Roll, TransformChange.NewRotation);

	/*zeLog("Physical Object", "Transform Changed. Object: %x, New Position: [%f, %f, %f], New Orientation: [%f, %f, %f]", 
		TransformChange.PhysicalObject,
		TransformChange.NewPosition.x, TransformChange.NewPosition.y, TransformChange.NewPosition.z,
		Pitch, Yaw, Roll);*/
}

void ZEPhysicsDebugComponent::ColisionDetected(const ZEPhysicalCollision& Collision)
{
	zeLog("Physical Object", "Collision Occured: Object1 : %x, Object2 : %x", Collision.Collider1, Collision.Collider2);
}

bool ZEPhysicsDebugComponent::Initialize()
{
	ZEScene* Scene = zeGame->GetScene();

	// Create the player
	if (Player == NULL)
	{
		Player = (ZEPlayer*)zeGame->CreateEntityInstance("ZEPlayer");
		Player->SetPosition(ZEVector3(0.0f, 5.0f, 0.0f));
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
		PhysicalRigidBody->SetCollisionCallback(ZEPhysicalCollisionCallback(this, &ZEPhysicsDebugComponent::ColisionDetected));
		PhysicalRigidBody->SetTransformChangeCallback(ZEPhysicalTransformChangeCallback(this, &ZEPhysicsDebugComponent::TransformChanged));

		//PhysicalRigidBody->SetLinearDamping(0.01f);
		World->AddPhysicalObject(PhysicalRigidBody);
		PhysicalRigidBody->ApplyForce(ZEVector3(1000.0f, 0.0f, 0.0f));

		ZECanvasBrush* CanvasBrush = new ZECanvasBrush();
		CanvasBrush->SetRotation(ZEQuaternion(ZE_PI_8, ZEVector3(0.0f, 1.0f, 0.0f)));
		CanvasBrush->SetScale(ZEVector3(0.5f, 0.5f, 0.5f));
		CanvasBrush->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));
		CanvasBrush->Canvas.LoadCanvasFile("Test\\test.zeCanvas");
		CanvasBrush->UpdateCanvas();
		ZEFixedMaterial* CanvasMaterial = ZEFixedMaterial::CreateInstance();
		
		CanvasBrush->Material = CanvasMaterial;
		CanvasMaterial->SetZero();
		CanvasMaterial->SetLightningEnabled(true);
		CanvasMaterial->SetTransparancyMode(ZE_MTM_NOTRANSPARACY);
		CanvasMaterial->SetTwoSided(false);
		CanvasMaterial->SetRecivesShadow(false);
		CanvasMaterial->SetAmbientEnabled(true);
		CanvasMaterial->SetAmbientColor(ZEVector3(0.1f, 0.1f, 0.1f));
		CanvasMaterial->SetDiffuseEnabled(true);
		CanvasMaterial->SetDiffuseColor(ZEVector3::One);
		CanvasMaterial->SetDiffuseMap(ZETexture2DResource::LoadResource("Test\\Diffuse.tga")->GetTexture());
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

		Model = (ZEModelBrush*)zeGame->CreateEntityInstance("ZEModelBrush");
		Model->SetModelFile("Warren\\Warren.zeModel");
		Scene->AddEntity(Model);
		//zeGame->GetScene()->LoadEnvironment("catacombs.zeMap");
		World->SetVisualize(true);
		Scene->SetVisualDebugElements(ZE_VDE_ALL);
		World->SetEnabled(true);
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

void ZEPhysicsDebugComponent::Process(float ElapsedTime)
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


ZEPhysicsDebugComponent::ZEPhysicsDebugComponent()
{
	PhysicalRigidBody = NULL;
	Player = NULL;
}

ZEPhysicsDebugComponent::~ZEPhysicsDebugComponent()
{
	Deinitialize();
}
