//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXConversion.h
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
#ifndef	__ZE_PHYSX_CONVERSION_H__
#define __ZE_PHYSX_CONVERSION_H__

#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEQuaternion.h"
#include <NxVec3.h>
#include <NxQuat.h>

__forceinline const NxVec3 ZE_TO_NX(const ZEVector3& Vector)
{
	return NxVec3(Vector.x, Vector.y, Vector.z);
}

__forceinline const NxQuat ZE_TO_NX(const ZEQuaternion& Quaternion)
{
	NxQuat Temp;
	Temp.x = Quaternion.x;
	Temp.y = Quaternion.y;
	Temp.z = Quaternion.z;
	Temp.w = Quaternion.w;
	return Temp;
}

__forceinline const ZEVector3 NX_TO_ZE(const NxVec3& Vector)
{
	return ZEVector3(Vector.x, Vector.y, Vector.z);
}

__forceinline const ZEQuaternion NX_TO_ZE(const NxQuat& Quaternion)
{
	return ZEQuaternion(Quaternion.w, Quaternion.x, Quaternion.y, Quaternion.z);
}

#endif




