//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsWorld.h
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

#pragma once
#ifndef	__ZE_AEGIA_PHYSICS_WORLD_H__
#define __ZE_AEGIA_PHYSICS_WORLD_H__

class ZEPhysicsWorld;
class ZEPhysicsWorldInfo;
class ZEVector3;
class ZEPhysicsCollision;
class ZEPhysicsTrigger;
class NxPhysicsSDK;
class NxScene;
class NxCookingInterface;
class NxControllerManager;
class ZECanvasBrush;
#include "Singleton.h"

class NxContactPair;
#include "NxUserContactReport.h"
#include "Physics/AegiaPhysicsReport.h"
#include "Physics/AegiaPhysicsUtility.h"

class ControllerAllocator : public NxUserAllocator 
{
public:
	virtual void*   mallocDEBUG(size_t size, const char* fileName, int line)    { return ::malloc(size); }
	virtual void*   malloc(size_t size)                                         { return ::malloc(size); }
	virtual void*   realloc(void* memory, size_t size)                          { return ::realloc(memory, size); }
	virtual void    free(void* memory)                                          { ::free(memory); }
};

class ZEAegiaPhysicsWorld : public ZEPhysicsWorld, public ZESingleton<ZEAegiaPhysicsWorld>
{
	friend class ZEAegiaPhysicsReport;
	friend class ZEAegiaPhysicsModule;

private:
	ZEAegiaPhysicsWorld();
	~ZEAegiaPhysicsWorld();
public:
	static ZEAegiaPhysicsWorld& getSingleton();
	static ZEAegiaPhysicsWorld* getSingletonPtr();

	void Initialize(ZEPhysicsWorldInfo& Info);
	void Deinitialize();
	void Update(const float ElapsedTime);
	void ShowDebugView(bool Show);


	//gets & sets
	NxPhysicsSDK* GetSdk() { return PhysicsSdk; }
	NxScene* GetScene() { return PhysicsScene; }
	NxCookingInterface* GetCooker() { return Cooker; }
	NxControllerManager* GetControllerManager() { return ControllerManager; }
	void SetCollisionCallback(CollisionDelegate Function) { DelegateC = Function; }
	void SetTriggerCallback(TriggerDelegate Function) { DelegateT = Function; }
	
	ZEVector3 GetGravity() { NxVec3 v;PhysicsScene->getGravity(v);return TOZE(v); }
	void SetGravity(ZEVector3 Vector) { PhysicsScene->setGravity(TONX(Vector)); }

private:
	NxPhysicsSDK* PhysicsSdk;
	NxScene*      PhysicsScene;
	NxCookingInterface* Cooker;
	NxControllerManager* ControllerManager;
	ControllerAllocator* ControllerAlloc;
	ZEAegiaPhysicsReport Report;
	CollisionDelegate DelegateC;
	TriggerDelegate DelegateT;
	ZECanvasBrush* Debugger;
	bool DebugView;
};

#endif
