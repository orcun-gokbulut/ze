//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsJointInfo.h
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
#ifndef	__ZE_PHYSICS_JOINT_INFO_H__
#define __ZE_PHYSICS_JOINT_INFO_H__

class ZEVector3;
class ZEPhysicsBody;

class ZEPhysicsJointInfo
{
	public:
		enum ZEPhysicsJointType
		{
			ZE_PJT_FIXED,
			ZE_PJT_SPHERICAL,
			ZE_PJT_REVOLUTE,
			ZE_PJT_PRISMATIC,
			ZE_PJT_CYLINDRICAL,
			ZE_PJT_DISTANCE,
			ZE_PJT_POINTONLINE,
			ZE_PJT_POINTINPLANE,
			ZE_PJT_PULLEY,
			ZE_PJT_FREE
		};

										ZEPhysicsJointInfo();
		virtual							~ZEPhysicsJointInfo(){ Body1 = 0; Body2 = 0; }//NULL
		virtual ZEPhysicsJointType		GetType() = 0;

		ZEPhysicsBody*					Body1;
		ZEPhysicsBody*					Body2;
		bool							CollideBodies;

		bool							UseGlobalAnchor;
		ZEVector3						GlobalAnchor;
		bool							UseGlobalAxis;
		ZEVector3						GlobalAxis;

		ZEVector3						LocalAnchor1;
		ZEVector3						LocalAxis1;
		ZEVector3						LocalAnchor2;
		ZEVector3						LocalAxis2;

		bool							Breakable;
		float							BreakForce;
		float							BreakTorque;
};

#endif
