//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicsModule.h
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

#include "ZECore\ZEModule.h"

class ZEPhysicalWorld;
class ZEPhysicalRigidBody;
class ZEPhysicalStaticRigidBody;
class ZEPhysicalStaticMesh;
class ZEPhysicalJoint;
class ZEPhysicalCloth;
class ZEPhysicalSoftBody;
class ZEPhysicalForceField;
class ZEPhysicalStaticHeightField;
class ZEPhysicalTrigger;
class ZEPhysicalVehicle;
class ZEPhysicalCharacterController;

#define zePhysics ZEPhysicsModule::GetInstance()

class ZEPhysicsModule : public ZEModule
{
	public:
		virtual ZEPhysicalWorld*				CreatePhysicalWorld() = 0;

		virtual ZEPhysicalRigidBody*			CreatePhysicalRigidBody() = 0;
		virtual ZEPhysicalStaticRigidBody*		CreatePhysicalStaticRigidBody() = 0;
		virtual ZEPhysicalSoftBody*				CreatePhysicalSoftBody() = 0;
		virtual ZEPhysicalCloth*				CreatePhysicalCloth() = 0;
		virtual ZEPhysicalStaticMesh*			CreatePhysicalStaticMesh() = 0;
		virtual ZEPhysicalStaticHeightField*	CreatePhysicalStaticHeightField() = 0;

		virtual ZEPhysicalForceField*			CreatePhysicalForceField() = 0;

		virtual ZEPhysicalJoint*				CreatePhysicalJoint() = 0;

		virtual ZEPhysicalTrigger*				CreatePhysicalTrigger() = 0;

		virtual ZEPhysicalVehicle*				CreatePhysicalVehicle() = 0;
		virtual ZEPhysicalCharacterController*	CreatePhysicalController() = 0;

		virtual	void							Process(float ElapsedTime) = 0;
		virtual void							UpdateWorlds() = 0;

		static ZEPhysicsModule*					GetInstance();
};

#endif



