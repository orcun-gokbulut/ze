//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicalWorld.cpp
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

#include "PhysXPhysicalWorld.h"
#include "PhysXConversion.h"
#include "Physics/PhysicalObject.h"
#include "Core/Error.h"
#include "Graphics/Renderer.h"
#include "Graphics/SimpleMaterial.h"

#include <NxDebugRenderable.h>

static ZEVector4 NX_TO_ZE(NxU32 color)
{        
	NxF32 Blue = NxF32((color) & 0xff) / 255.0f;
	NxF32 Green = NxF32((color >> 8) & 0xff) / 255.0f;
	NxF32 Red = NxF32((color >> 16) & 0xff) / 255.0f;
	return ZEVector4(1.0f, Red, Green, Blue);
}

ZEPhysXPhysicalWorld::ZEPhysXPhysicalWorld()
{
	Scene = NULL;
	SceneDesc.userData = this;
	SceneDesc.gravity = NxVec3(0.0f, -9.8f, 0.0f);
	SceneDesc.groundPlane = true;
	SceneDesc.simType = NX_SIMULATION_SW;
	SceneDesc.upAxis = 1;
	DebugDraw.Material = NULL;
}

ZEPhysXPhysicalWorld::~ZEPhysXPhysicalWorld()
{
	if (DebugDraw.Material != NULL)
		DebugDraw.Material->Release();

	Deinitialize();
}

void ZEPhysXPhysicalWorld::InitializeDebugDraw()
{
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_BODY_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_BODY_MASS_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_BODY_LIN_VELOCITY, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_BODY_ANG_VELOCITY, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_BODY_JOINT_GROUPS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CONTACT_POINT, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CONTACT_ERROR, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CONTACT_FORCE, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_ACTOR_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_AXES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_COMPOUNDS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_VNORMALS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_EDGES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_SPHERES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_STATIC, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_DYNAMIC, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_FREE, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_CCD, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_COLLISION_SKELETONS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_EMITTERS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_POSITION, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_VELOCITY, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_KERNEL_RADIUS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_BOUNDS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_PACKETS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_MOTION_LIMIT, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_DYN_COLLISION, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_STC_COLLISION, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_MESH_PACKETS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_DRAINS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_FLUID_PACKET_DATA, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_MESH, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_COLLISIONS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_SELFCOLLISIONS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_WORKPACKETS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_SLEEP, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_SLEEP_VERTEX, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_TEARABLE_VERTICES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_TEARING, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_ATTACHMENT, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_CLOTH_VALIDBOUNDS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_MESH, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_COLLISIONS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_WORKPACKETS, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_SLEEP, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_SLEEP_VERTEX, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_TEARABLE_VERTICES, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_TEARING, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_ATTACHMENT, 1.0f);
	GetPhysicsSDK()->setParameter(NX_VISUALIZE_SOFTBODY_VALIDBOUNDS, 1.0f);
	
	const NxDebugRenderable* DebugRenderable = Scene->getDebugRenderable();

	DebugDraw.Material = ZESimpleMaterial::CreateInstance();

	DebugDraw.PointsRenderOrder.SetZero();
	DebugDraw.PointsRenderOrder.Material = DebugDraw.Material;
	DebugDraw.PointsRenderOrder.PrimitiveType = ZE_ROPT_POINT;
	DebugDraw.PointsRenderOrder.Flags = ZE_ROF_ENABLE_ZCULLING | ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM;
	DebugDraw.PointsRenderOrder.VertexBuffer = &DebugDraw.PointsVertexBuffer;
	DebugDraw.PointsRenderOrder.VertexDeclaration = ZEColoredVertex::GetVertexDeclaration();

	DebugDraw.LinesRenderOrder.SetZero();
	DebugDraw.LinesRenderOrder.Material = DebugDraw.Material;
	DebugDraw.LinesRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	DebugDraw.LinesRenderOrder.Flags = ZE_ROF_ENABLE_ZCULLING | ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM;
	DebugDraw.LinesRenderOrder.VertexBuffer = &DebugDraw.LinesVertexBuffer;
	DebugDraw.LinesRenderOrder.VertexDeclaration = ZEColoredVertex::GetVertexDeclaration();

	DebugDraw.TrianglesRenderOrder.SetZero();
	DebugDraw.TrianglesRenderOrder.Material = DebugDraw.Material;
	DebugDraw.TrianglesRenderOrder.PrimitiveType = ZE_ROPT_LINE;
	DebugDraw.TrianglesRenderOrder.Flags = ZE_ROF_ENABLE_ZCULLING | ZE_ROF_ENABLE_VIEWPROJECTION_TRANSFORM;
	DebugDraw.TrianglesRenderOrder.VertexBuffer = &DebugDraw.TrianglesVertexBuffer;
	DebugDraw.TrianglesRenderOrder.VertexDeclaration = ZEColoredVertex::GetVertexDeclaration();
}


NxScene* ZEPhysXPhysicalWorld::GetScene()
{
	return Scene;
}

const ZEArray<ZEPhysicalObject*>& ZEPhysXPhysicalWorld::GetPhysicalObjects()
{
	return PhysicalObjects;
}

void ZEPhysXPhysicalWorld::AddPhysicalObject(ZEPhysicalObject* Object)
{
	PhysicalObjects.Add(Object);
	if (Scene != NULL)
	{
		Object->SetPhysicalWorld(this);
		Object->Initialize();
	}
}

void ZEPhysXPhysicalWorld::RemovePhysicalObject(ZEPhysicalObject* Object)
{
	PhysicalObjects.DeleteValue(Object);
	Object->Deinitialize();
}

void ZEPhysXPhysicalWorld::SetGravity(const ZEVector3& Gravity)
{
	SceneDesc.gravity = ZE_TO_NX(Gravity);
	if (Scene != NULL)
		Scene->setGravity(SceneDesc.gravity);
}

ZEVector3 ZEPhysXPhysicalWorld::GetGravity()
{
	return NX_TO_ZE(SceneDesc.gravity);
}

bool ZEPhysXPhysicalWorld::Initialize()
{
	Scene = GetPhysicsSDK()->createScene(SceneDesc);
	if (Scene == NULL) 
	{
		zeError("PhysX Physical World", "Can not create scene.");
		return false;
	}

	Scene->setTiming(1.0f / 100.0f, 8, NX_TIMESTEP_FIXED);

	for (size_t I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Initialize();


	for (size_t I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Initialize();

	return true;
}

void ZEPhysXPhysicalWorld::Deinitialize()
{
	for (size_t I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Deinitialize();

	if (Scene != NULL)
	{
		GetPhysicsSDK()->releaseScene(*Scene);
		Scene = NULL;
	}
}

void ZEPhysXPhysicalWorld::Draw(ZERenderer* Renderer)
{
	if (DebugDraw.Material == NULL)
		InitializeDebugDraw();

	const NxDebugRenderable* DebugRenderable = Scene->getDebugRenderable();

	DebugDraw.PointsRenderOrder.PrimitiveCount = DebugRenderable->getNbPoints();
	DebugDraw.PointsVertexBuffer.SetCount(DebugDraw.PointsRenderOrder.PrimitiveCount);
	const NxDebugPoint* DebugPoints = DebugRenderable->getPoints();
	for (size_t I = 0; I < DebugDraw.PointsRenderOrder.PrimitiveCount; I++)
	{
		DebugDraw.PointsVertexBuffer[I].Position = NX_TO_ZE(DebugPoints[I].p);
		DebugDraw.PointsVertexBuffer[I].Color = NX_TO_ZE(DebugPoints[I].color);
	}
	if (DebugDraw.PointsRenderOrder.PrimitiveCount != 0)
		Renderer->AddToRenderList(&DebugDraw.PointsRenderOrder);


	DebugDraw.LinesRenderOrder.PrimitiveCount = DebugRenderable->getNbLines();
	DebugDraw.LinesVertexBuffer.SetCount(DebugDraw.LinesRenderOrder.PrimitiveCount * 2);
	const NxDebugLine* DebugLines = DebugRenderable->getLines();
	for (size_t I = 0; I < DebugDraw.LinesRenderOrder.PrimitiveCount * 2; I++)
	{
		DebugDraw.LinesVertexBuffer[2 * I].Position = NX_TO_ZE(DebugLines[I].p0);
		DebugDraw.LinesVertexBuffer[2 * I].Color = NX_TO_ZE(DebugLines[I].color);
		DebugDraw.LinesVertexBuffer[2 * I + 1].Position = NX_TO_ZE(DebugLines[I].p1);
		DebugDraw.LinesVertexBuffer[2 * I + 1].Color = NX_TO_ZE(DebugLines[I].color);
	}
	if (DebugDraw.LinesRenderOrder.PrimitiveCount != 0)
		Renderer->AddToRenderList(&DebugDraw.LinesRenderOrder);

	DebugDraw.TrianglesRenderOrder.PrimitiveCount = DebugRenderable->getNbTriangles();
	DebugDraw.TrianglesVertexBuffer.SetCount(DebugDraw.TrianglesRenderOrder.PrimitiveCount * 3);
	const NxDebugTriangle* DebugTriangles = DebugRenderable->getTriangles();
	for (size_t I = 0; I < DebugDraw.TrianglesRenderOrder.PrimitiveCount * 3; I++)
	{
		DebugDraw.TrianglesVertexBuffer[3 * I].Position = NX_TO_ZE(DebugTriangles[I].p0);
		DebugDraw.TrianglesVertexBuffer[3 * I].Color = NX_TO_ZE(DebugTriangles[I].color);
		DebugDraw.TrianglesVertexBuffer[3 * I + 1].Position = NX_TO_ZE(DebugTriangles[I].p1);
		DebugDraw.TrianglesVertexBuffer[3 * I + 1].Color = NX_TO_ZE(DebugTriangles[I].color);
		DebugDraw.TrianglesVertexBuffer[3 * I + 2].Position = NX_TO_ZE(DebugTriangles[I].p2);
		DebugDraw.TrianglesVertexBuffer[3 * I + 2].Color = NX_TO_ZE(DebugTriangles[I].color);
	}
	if (DebugDraw.TrianglesRenderOrder.PrimitiveCount != 0)
		Renderer->AddToRenderList(&DebugDraw.TrianglesRenderOrder);
}


void ZEPhysXPhysicalWorld::Update(float ElapsedTime)
{
    Scene->simulate(1.0f);
    Scene->flushStream();

	Scene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}
