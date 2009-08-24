//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsModule.h
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
#ifndef	__ZE_PHYSICS_MODULE_H__
#define __ZE_PHYSICS_MODULE_H__

class ZEModule;
class ZEPhysicsWorld;
class ZEPhysicsBody;
class ZEPhysicsJoint;
class ZEPhysicsVehicle;
class ZEPhysicsMaterial;
class ZEPhysicsCharacterController;
class ZEVector3;
class ZEPhysicsCollisionMask;

#define zePhysics zeCore->GetPhysicsModule()

class ZEPhysicsModule : public ZEModule
{
public:
	virtual ~ZEPhysicsModule(){}
	static ZEPhysicsModule*	GetInstance() { return 0; }
	
	virtual ZEPhysicsWorld* CreateWorld()					 = 0;
	virtual ZEPhysicsBody*  CreateBody()					 = 0;
	virtual ZEPhysicsJoint* CreateJoint()					 = 0;
	virtual ZEPhysicsVehicle* CreateVehicle()		         = 0;
	virtual ZEPhysicsMaterial* CreateMaterial()				 = 0;
	virtual ZEPhysicsCharacterController* CreateController() = 0;

	//                        ZEPhysicsWorld** to set a null to parameter ?
	virtual bool DestroyWorld(ZEPhysicsWorld*)					   = 0;
	virtual bool DestroyBody(ZEPhysicsBody*)					   = 0;
	virtual bool DestroyJoint(ZEPhysicsJoint*)					   = 0;
	virtual bool DestroyVehicle(ZEPhysicsVehicle*)	               = 0;
	virtual bool DestroyMaterial(ZEPhysicsMaterial*)			   = 0;
	virtual bool DestroyController(ZEPhysicsCharacterController*)  = 0;

	virtual bool CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEVector3& Point, ZEPhysicsCollisionMask Mask) = 0;
	virtual bool CastRay(ZEVector3 Origin, ZEVector3 Direction, ZEPhysicsBody** Contact, ZEPhysicsCollisionMask Mask) = 0;
};

#endif
