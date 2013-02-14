//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalWorld.cpp
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

#include "ZEPhysXPhysicalWorld.h"
#include "ZEPhysXConversion.h"
#include "ZEPhysics/ZEPhysicalObject.h"
#include "ZEPhysics/ZEPhysicalRigidBody.h"
#include "ZEError.h"
#include "ZERenderer/ZERenderer.h"
#include <NxDebugRenderable.h>
#include <NxRay.h>

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
	SceneDesc.groundPlane = false;
	SceneDesc.simType = NX_SIMULATION_SW;
	SceneDesc.upAxis = 1;
	Enabled = true;
}

ZEPhysXPhysicalWorld::~ZEPhysXPhysicalWorld()
{

	Deinitialize();
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

void ZEPhysXPhysicalWorld::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;
}

bool ZEPhysXPhysicalWorld::GetEnabled()
{
	return Enabled;
}

bool ZEPhysXPhysicalWorld::Initialize()
{	
	SceneDesc.gravity = ZE_TO_NX(ZEVector3(0.0f, -9.8f, 0.0f));
	SceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;

	Scene = GetPhysicsSDK()->createScene(SceneDesc);
	if (Scene == NULL) 
	{
		zeError("Can not create scene.");
		return false;
	}

	NxMaterial* DefaultMaterial = Scene->getMaterialFromIndex(0);
	DefaultMaterial->setRestitution(0.5f);
	DefaultMaterial->setDynamicFriction(0.5f);
	DefaultMaterial->setStaticFriction(0.5f);

	Scene->setTiming(1.0f / 60.0f, 4, NX_TIMESTEP_FIXED);

	for (ZESize I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Initialize();

	for (ZESize I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Initialize();

	Scene->setUserContactReport(&CollisionManager);

	return true;
}

void ZEPhysXPhysicalWorld::Deinitialize()
{
	for (ZESize I = 0; I < PhysicalObjects.GetCount(); I++)
		PhysicalObjects[I]->Deinitialize();

	if (Scene != NULL)
	{
		GetPhysicsSDK()->releaseScene(*Scene);
		Scene = NULL;
	}
}

void ZEPhysXPhysicalWorld::Draw(ZERenderer* Renderer)
{

}

void ZEPhysXPhysicalWorld::Process(float ElapsedTime)
{
	Scene->simulate(ElapsedTime);

	Scene->flushStream();
}

void ZEPhysXPhysicalWorld::Destroy()
{
	GetModule()->PhysicalWorlds.DeleteValue(this);
	delete this; 
}

void ZEPhysXPhysicalWorld::Update()
{
	Scene->fetchResults(NX_ALL_FINISHED, true);

	NxU32 TransformCount = 0;
	NxActiveTransform *ActiveTransforms = Scene->getActiveTransforms(TransformCount);

	for (ZESize I = 0; I < (ZESize)TransformCount; I++)
	{
		if (ActiveTransforms[I].userData != NULL)
		{
			const ZEPhysicalTransformChangeEvent& Callback = ((ZEPhysicalObject*)ActiveTransforms[I].userData)->GetTransformChangeEvent();
			if (Callback != NULL)
			{
				ZEVector3 NewPosition = NX_TO_ZE(ActiveTransforms[I].actor2World.t);

				NxQuat Quat;
				ActiveTransforms[I].actor2World.M.toQuat(Quat);
				ZEQuaternion NewRotation = NX_TO_ZE(Quat);

				ZEPhysicalObject* PhysicalObject = ((ZEPhysicalObject*)ActiveTransforms[I].userData);

				Callback(PhysicalObject, NewPosition, NewRotation);
			}
		}
	}
}

ZEPhysicalShape* ZEPhysXPhysicalWorld::RayCastToClosestShape(ZERay Ray, ZEPhysicsRayCastFilterShapeType Type, ZERayCastResultDetails& Result)
{
	if (!Ray.v.IsNormalized())
		Ray.v.NormalizeSelf();

	NxRay TempRay(ZE_TO_NX(Ray.p), ZE_TO_NX(Ray.v));
	NxRaycastHit TempResult;
	NxShape* TempShape = this->GetScene()->raycastClosestShape(TempRay, (NxShapesType)Type, TempResult);

	if (TempShape != NULL)
	{
		Result.ImpactWorldPosition = NX_TO_ZE(TempResult.worldImpact);
		Result.ImpactWorldNormal = NX_TO_ZE(TempResult.worldNormal);
		Result.ImpactDistance = TempResult.distance;

		return (ZEPhysicalShape*)TempShape->userData;
	}
	else
		return NULL;
}
