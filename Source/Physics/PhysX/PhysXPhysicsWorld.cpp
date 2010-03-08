//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsWorld.cpp
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
#include "NxCooking.h"
#include "NxControllerManager.h"
#include "ZEMath/Vector.h"
#include "GameInterface/CanvasBrush.h"
#include "Physics/PhysicsCollision.h"
#include "Physics/PhysicsTrigger.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsWorldInfo.h"
#include "PhysXPhysicsWorld.h"
#include "PhysXPhysicsUtility.h"
#include "Core/Core.h"
#include "Core/Error.h"
#include "Core/Console.h"
#include "GameInterface/Scene.h"

ZEPhysXPhysicsWorld* ZEPhysXPhysicsWorld::ms_Singleton = 0;

ZEPhysXPhysicsWorld* ZEPhysXPhysicsWorld::getSingletonPtr(void)
{
	return ms_Singleton;
}

ZEPhysXPhysicsWorld::ZEPhysXPhysicsWorld() : PhysicsSdk(NULL), PhysicsScene(NULL), Cooker(NULL), ControllerManager(0), ControllerAlloc(0), Report(), DelegateC(0), DelegateT(0), Debugger(NULL), DebugView(false)
{
	ms_Singleton = this;
}

ZEPhysXPhysicsWorld::~ZEPhysXPhysicsWorld()
{
	ms_Singleton = 0;
	Deinitialize();
}

void ZEPhysXPhysicsWorld::Initialize(ZEPhysicsWorldInfo& Info)
{
	if (PhysicsSdk == NULL)
	{
		PhysicsSdk = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
		if(!PhysicsSdk)
		{
			zeLog("PhysX Physics World", "Physx Sdk Creation Error, Wrong Sdk Dll Version?");
			return;
		}
		else
		{
			float debug_scale = 1.0f;
			DebugView = Info.DebugView;
			/*if (DebugView)debug_scale = 1.0f;*/
			PhysicsSdk->setParameter(NX_VISUALIZATION_SCALE, debug_scale);
			PhysicsSdk->setParameter(NX_SKIN_WIDTH, 0.01f);
			PhysicsSdk->setParameter(NX_VISUALIZE_WORLD_AXES, 1);
			PhysicsSdk->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
			PhysicsSdk->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
			PhysicsSdk->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 1);
			PhysicsSdk->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);
			//PhysicsSdk->setParameter(NX_VISUALIZE_JOINT_LIMITS, 2);
			PhysicsSdk->setParameter(NX_TRIGGER_TRIGGER_CALLBACK, 1);
			if (Info.ContiniousDetection)PhysicsSdk->setParameter(NX_CONTINUOUS_CD, 1);
			//PhysicsSdk->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
		}
	}

	//create scene
	if (PhysicsScene == NULL)
	{
		NxSceneDesc sceneDesc;
		sceneDesc.gravity = TONX(Info.Gravity);
		sceneDesc.simType = NX_SIMULATION_SW;
		PhysicsScene = PhysicsSdk->createScene(sceneDesc);

		if(!PhysicsScene)
		{
			zeLog("PhysX Physics World", "Physx Scene Creation Error.");
		}

		float timestep = 1.0f/100.0f;
		PhysicsScene->setTiming(timestep,8,NX_TIMESTEP_FIXED);

		//collision filtering
		PhysicsScene->setFilterOps(NX_FILTEROP_OR, NX_FILTEROP_OR, NX_FILTEROP_AND);
		PhysicsScene->setFilterBool(true);
		NxGroupsMask zeroMask;
		zeroMask.bits0=zeroMask.bits1=zeroMask.bits2=zeroMask.bits3=0;
		PhysicsScene->setFilterConstant0(zeroMask);
		PhysicsScene->setFilterConstant1(zeroMask);

		//collison callback
		PhysicsScene->setUserContactReport(&Report);
		PhysicsScene->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_START_TOUCH);

		//trigger callback
		PhysicsScene->setUserTriggerReport(&Report);

		//set default mat
		NxMaterial* defaultMaterial = PhysicsScene->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0.25);
		defaultMaterial->setStaticFriction(0.5);
		defaultMaterial->setDynamicFriction(0.5);
	}

	//set cooker
	if (Cooker == NULL)
	{
		Cooker = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	}

	//set controller manager
	if (ControllerManager == NULL)
	{
		ControllerAlloc = new ControllerAllocator();
		ControllerManager = NxCreateControllerManager(ControllerAlloc);
	}

	//set debug renderer
	if (Debugger == NULL)
	{
		Debugger = new ZECanvasBrush();
		Debugger->PrimitiveType = ZE_ROPT_LINE;
		Debugger->Canvas.Clean();
		Debugger->UpdateCanvas();
		Debugger->Material = ZEFixedMaterial::CreateInstance();
		Debugger->Material->SetZero();
		((ZEFixedMaterial*)Debugger->Material)->SetLightningEnabled(false);
		((ZEFixedMaterial*)Debugger->Material)->SetAmbientColor(ZEVector3(0.75f, 0.0f, 0.0f));
		Debugger->Material->UpdateMaterial();
		Debugger->SetVisible(DebugView);
		zeScene->AddEntity(Debugger);
	}
}

void ZEPhysXPhysicsWorld::Deinitialize()
{
	//cooker
	if (Cooker)
	{
		Cooker = NULL;
	}

	//controller manager
	if (ControllerManager)
	{
		delete ControllerAlloc;
		ControllerAlloc = NULL;
		NxReleaseControllerManager(ControllerManager);
		ControllerManager = NULL;
	}

	//scene
	if (PhysicsScene)
	{
		PhysicsScene->fetchResults(NX_RIGID_BODY_FINISHED,true);
		PhysicsSdk->releaseScene(*PhysicsScene);
		PhysicsScene = NULL;
	}

	//sdk
	if (PhysicsSdk)
	{
		PhysicsSdk->release();
		PhysicsSdk = NULL;
	}

	//debugger
	if (Debugger)
	{
		delete Debugger;
		Debugger = NULL;
	}
}

void ZEPhysXPhysicsWorld::ShowDebugView(bool Show) 
{ 
	DebugView = Show;
	if (Debugger)
	{
		Debugger->SetVisible(Show);

		if (DebugView)
		{
			Debugger->Canvas.Clean();

			const NxDebugRenderable* data = PhysicsScene->getDebugRenderable();
			NxU32 NbLines = NULL;
			if (data)
			{
				NbLines = data->getNbLines();
			}
			const NxDebugLine* Lines;
			if (data)
			{
				Lines = data->getLines();
			}
			
			while(NbLines--)
			{
				Debugger->Canvas.AddLine(ZEVector3(Lines->p0.x,Lines->p0.y,Lines->p0.z),ZEVector3(Lines->p1.x,Lines->p1.y,Lines->p1.z));
				Lines++;
			}

			Debugger->UpdateCanvas();
		}
	}
}

void ZEPhysXPhysicsWorld::Update(const float ElapsedTime)
{
	//simulate
	PhysicsScene->simulate(ElapsedTime);
	PhysicsScene->flushStream();

	//debug render
	/*if (DebugView)
	{
		Debugger->Canvas.Clean();

		const NxDebugRenderable* data = PhysicsScene->getDebugRenderable();
		NxU32 NbLines = NULL;
		if (data)
		{
			NbLines = data->getNbLines();
		}
		const NxDebugLine* Lines;
		if (data)
		{
			Lines = data->getLines();
		}
		
		while(NbLines--)
		{
			Debugger->Canvas.AddLine(ZEVector3(Lines->p0.x,Lines->p0.y,Lines->p0.z),ZEVector3(Lines->p1.x,Lines->p1.y,Lines->p1.z));
			Lines++;
		}

		Debugger->UpdateCanvas();
	}*/

	while(!PhysicsScene->fetchResults(NX_RIGID_BODY_FINISHED, false)){}
}
