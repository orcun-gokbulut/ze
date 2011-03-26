//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicalCallbacks.h
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
#ifndef	__ZE_PHYSICAL_CALLBACKS_H__
#define __ZE_PHYSICAL_CALLBACKS_H__

#include "ZEMath\ZEVector.h"
#include "ZEMath\ZEQuaternion.h"
#include "ZEFastDelegate.h"

#define ZE_PCCF_ON_TOUCH			1
#define ZE_PCCF_ON_START_TOUCH		2
#define ZE_PCCF_ON_END_TOUCH		4

class ZEPhysicalObject;

struct ZEPhysicalCollisionEventArgument
{
	ZEPhysicalObject*					Collider1;
	ZEPhysicalObject*					Collider2;

	ZEVector3							Position;
	ZEVector3							Normal;
	float								Power;
};

typedef fastdelegate::FastDelegate1<const ZEPhysicalCollisionEventArgument&> ZEPhysicalCollisionEvent;

struct ZEPhysicalTransformChangeEventArgument
{
	ZEPhysicalObject*					PhysicalObject;
	ZEVector3							NewPosition;
	ZEQuaternion						NewRotation;
};

typedef fastdelegate::FastDelegate1<const ZEPhysicalTransformChangeEventArgument&> ZEPhysicalTransformChangeEvent;

struct ZEPhysicalJointEventArgument
{
	ZEPhysicalObject*					PhysicalJointParent;
	ZEPhysicalObject*					PhysicalJoint;

	ZEVector3							NewPosition;
	ZEQuaternion						NewRotation;

	bool								IsJointBroken;
};

typedef fastdelegate::FastDelegate1<const ZEPhysicalJointEventArgument&> ZEPhysicalJointEvent;

struct ZEPhysicalTriggerEventArgument
{
	ZEPhysicalObject*					Trigger;
	ZEPhysicalObject*					TriggerFiredBy;

	ZEVector3							TriggerFiredAt;				

	bool								IsTriggerActive;
	bool								IsTriggerFired;
};

typedef fastdelegate::FastDelegate1<const ZEPhysicalTriggerEventArgument&> ZEPhysicalTriggerEvent;

#endif
