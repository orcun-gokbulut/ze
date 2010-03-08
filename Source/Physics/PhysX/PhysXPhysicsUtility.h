//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsUtility.h
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
#ifndef	__ZE_PHYSX_PHYSICS_UTILITY_H__
#define __ZE_PHYSX_PHYSICS_UTILITY_H__

#include "NxPhysics.h"
#include "NxExtended.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"
#include "Physics/PhysicsCollisionMask.h"

#define TOZE  ZEPhysXPhysicsUtility::toZE
#define TONX  ZEPhysXPhysicsUtility::toNX
#define TONXE ZEPhysXPhysicsUtility::toNXE

class ZEPhysXPhysicsUtility
{
public:
	static ZEVector3 toZE(const NxVec3 &vec)
	{
		return ZEVector3(vec.x, vec.y, vec.z);
	}

	static ZEVector3 toZE(const NxExtendedVec3 &vec)
	{
		return ZEVector3(vec.x, vec.y, vec.z);
	}

	static NxVec3 toNX(const ZEVector3 &vec)
	{
		return NxVec3(vec.x, vec.y, vec.z);
	}

	static NxExtendedVec3 toNXE(const ZEVector3 &vec)
	{
		return NxExtendedVec3(vec.x, vec.y, vec.z);
	}

	static ZEQuaternion toZE(const NxQuat &quat)
	{
		return ZEQuaternion(quat.w, quat.x, quat.y, quat.z);
	}

	static NxQuat toNX(const ZEQuaternion &quat)//private quat. cons.
	{
		NxQuat qt;
		qt.setWXYZ(quat.w,quat.x,quat.y,quat.z);
		return qt;
	}

	static ZEPhysicsCollisionMask toZE(NxGroupsMask mask)
	{
		ZEPhysicsCollisionMask ret;
		ret.Mask1 = mask.bits0;
		ret.Mask2 = mask.bits1;
		ret.Mask3 = mask.bits2;
		ret.Mask4 = mask.bits3;
		return ret;
	}

	static NxGroupsMask toNX(ZEPhysicsCollisionMask mask)
	{
		NxGroupsMask ret;
		ret.bits0 = mask.Mask1;
		ret.bits1 = mask.Mask2;
		ret.bits2 = mask.Mask3;
		ret.bits3 = mask.Mask4;
		return ret;
	}
};

#endif
