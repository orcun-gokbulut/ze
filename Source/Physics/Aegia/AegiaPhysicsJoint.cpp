//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsJoint.cpp
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
#include "ZEMath/Vector.h"

#include "Physics/PhysicsJoint.h"
#include "Physics/Aegia/AegiaPhysicsJoint.h"

#include "Physics/PhysicsJointInfo.h"
#include "Physics/PhysicsFixedJointInfo.h"
#include "Physics/PhysicsSphericalJointInfo.h"
#include "Physics/PhysicsRevoluteJointInfo.h"
#include "Physics/PhysicsPrismaticJointInfo.h"
#include "Physics/PhysicsCylindricalJointInfo.h"
#include "Physics/PhysicsDistanceJointInfo.h"
#include "Physics/PhysicsPointOnLineJointInfo.h"
#include "Physics/PhysicsPointInPlaneJointInfo.h"
#include "Physics/PhysicsPulleyJointInfo.h"
#include "Physics/PhysicsFreeJointInfo.h"

#include "Physics/PhysicsBody.h"
#include "AegiaPhysicsBody.h"

#include "Physics/PhysicsWorld.h"
#include "AegiaPhysicsWorld.h"
#include "AegiaPhysicsUtility.h"

ZEAegiaPhysicsJoint::ZEAegiaPhysicsJoint() : Joint(NULL)
{
}

ZEAegiaPhysicsJoint::~ZEAegiaPhysicsJoint()
{
	Deinitialize();
}

void ZEAegiaPhysicsJoint::Initialize(ZEPhysicsJointInfo& Info)
{
	ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();

	if (Joint == NULL && World != NULL)
	{
		switch (Info.GetType())
		{
			case ZEPhysicsJointInfo::ZE_PJT_FIXED:
			{
				ZEPhysicsFixedJointInfo* FixedInfo = (ZEPhysicsFixedJointInfo*)&Info;

				//joint desc.
				NxFixedJointDesc FixedDesc;
				if (FixedInfo->Body1)
				{
					FixedDesc.actor[0] = ((ZEAegiaPhysicsBody*)FixedInfo->Body1)->GetActor();
				}
				else FixedDesc.actor[0] = NULL;
				if (FixedInfo->Body2)
				{
					FixedDesc.actor[1] = ((ZEAegiaPhysicsBody*)FixedInfo->Body2)->GetActor();
				}
				else FixedDesc.actor[1] = NULL;
				if (Info.CollideBodies)FixedDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (FixedInfo->UseGlobalAnchor)
				{
					FixedDesc.setGlobalAnchor(TONX(FixedInfo->GlobalAnchor));
				}
				else
				{
					FixedDesc.localAnchor[0] = TONX(FixedInfo->LocalAnchor1);
					FixedDesc.localAnchor[1] = TONX(FixedInfo->LocalAnchor2);
				}
				//set axis
				if (FixedInfo->UseGlobalAxis)
				{
					FixedDesc.setGlobalAxis(TONX(FixedInfo->GlobalAxis));
				}
				else
				{
					FixedDesc.localAxis[0] = TONX(FixedInfo->LocalAxis1);
					FixedDesc.localAxis[1] = TONX(FixedInfo->LocalAxis2);
				}

				if (FixedDesc.isValid())
				{
					Joint = (NxFixedJoint*)World->GetScene()->createJoint(FixedDesc);
					if (FixedInfo->Breakable)
					{
						Joint->setBreakable(FixedInfo->BreakForce,FixedInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_SPHERICAL:
			{
				ZEPhysicsSphericalJointInfo* SphericalInfo = (ZEPhysicsSphericalJointInfo*)&Info;

				//joint desc.
				NxSphericalJointDesc SphericalDesc;
				if (SphericalInfo->Body1)
				{
					SphericalDesc.actor[0] = ((ZEAegiaPhysicsBody*)SphericalInfo->Body1)->GetActor();
				}
				else SphericalDesc.actor[0] = NULL;
				if (SphericalInfo->Body2)
				{
					SphericalDesc.actor[1] = ((ZEAegiaPhysicsBody*)SphericalInfo->Body2)->GetActor();
				}
				else SphericalDesc.actor[1] = NULL;
				if (Info.CollideBodies)SphericalDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (SphericalInfo->UseGlobalAnchor)
				{
					SphericalDesc.setGlobalAnchor(TONX(SphericalInfo->GlobalAnchor));
				}
				else
				{
					SphericalDesc.localAnchor[0] = TONX(SphericalInfo->LocalAnchor1);
					SphericalDesc.localAnchor[1] = TONX(SphericalInfo->LocalAnchor2);
				}
				//set axis
				if (SphericalInfo->UseGlobalAxis)
				{
					SphericalDesc.setGlobalAxis(TONX(SphericalInfo->GlobalAxis));
				}
				else
				{
					SphericalDesc.localAxis[0] = TONX(SphericalInfo->LocalAxis1);
					SphericalDesc.localAxis[1] = TONX(SphericalInfo->LocalAxis2);
				}

				//limits
				if (SphericalInfo->SwingLimit)
				{
					SphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
					SphericalDesc.swingLimit.value = SphericalInfo->SwingLimitValue;
					SphericalDesc.swingLimit.restitution = SphericalInfo->SwingLimitRestitution;
				}

				if (SphericalInfo->TwistLimit)
				{
					SphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
					SphericalDesc.twistLimit.low.value = SphericalInfo->TwistLimitLowValue;
					SphericalDesc.twistLimit.high.value = SphericalInfo->TwistLimitHighValue;
					SphericalDesc.twistLimit.low.restitution = SphericalInfo->TwistLimitRestitution;
					SphericalDesc.twistLimit.high.restitution = SphericalInfo->TwistLimitRestitution;
				}

				if (SphericalDesc.isValid())
				{
					Joint = (NxSphericalJoint*)World->GetScene()->createJoint(SphericalDesc);
					if (SphericalInfo->Breakable)
					{
						Joint->setBreakable(SphericalInfo->BreakForce,SphericalInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_REVOLUTE:
			{
				ZEPhysicsRevoluteJointInfo* RevoluteInfo = (ZEPhysicsRevoluteJointInfo*)&Info;

				//joint desc.
				NxRevoluteJointDesc RevoluteDesc;
				if (RevoluteInfo->Body1)
				{
					RevoluteDesc.actor[0] = ((ZEAegiaPhysicsBody*)RevoluteInfo->Body1)->GetActor();
				}
				else RevoluteDesc.actor[0] = NULL;
				if (RevoluteInfo->Body2)
				{
					RevoluteDesc.actor[1] = ((ZEAegiaPhysicsBody*)RevoluteInfo->Body2)->GetActor();
				}
				else RevoluteDesc.actor[1] = NULL;
				if (Info.CollideBodies)RevoluteDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (RevoluteInfo->UseGlobalAnchor)
				{
					RevoluteDesc.setGlobalAnchor(TONX(RevoluteInfo->GlobalAnchor));
				}
				else
				{
					RevoluteDesc.localAnchor[0] = TONX(RevoluteInfo->LocalAnchor1);
					RevoluteDesc.localAnchor[1] = TONX(RevoluteInfo->LocalAnchor2);
				}
				//set axis
				if (RevoluteInfo->UseGlobalAxis)
				{
					RevoluteDesc.setGlobalAxis(TONX(RevoluteInfo->GlobalAxis));
				}
				else
				{
					RevoluteDesc.localAxis[0] = TONX(RevoluteInfo->LocalAxis1);
					RevoluteDesc.localAxis[1] = TONX(RevoluteInfo->LocalAxis2);
				}

				if (RevoluteInfo->HasLimit)
				{
					RevoluteDesc.flags |= NX_RJF_LIMIT_ENABLED;
					RevoluteDesc.limit.low.value = RevoluteInfo->LimitLowValue;
					RevoluteDesc.limit.high.value = RevoluteInfo->LimitHighValue;
					RevoluteDesc.limit.low.restitution = RevoluteInfo->LimitRestitution;
					RevoluteDesc.limit.high.restitution = RevoluteInfo->LimitRestitution;
				}

				if (RevoluteInfo->HasSpring)
				{
					RevoluteDesc.flags |= NX_RJF_SPRING_ENABLED;
					NxSpringDesc SpringDesc;
					SpringDesc.spring = RevoluteInfo->SpringValue;
					SpringDesc.damper = RevoluteInfo->SpringDamper;
					SpringDesc.targetValue = RevoluteInfo->SpringTarget;
					RevoluteDesc.spring = SpringDesc;
				}

				if (RevoluteInfo->HasMotor)
				{
					RevoluteDesc.flags |= NX_RJF_MOTOR_ENABLED;
					NxMotorDesc MotorDesc;
					MotorDesc.velTarget = RevoluteInfo->MotorVelocity;
					MotorDesc.maxForce = RevoluteInfo->MotorForce;
					MotorDesc.freeSpin = false;
					RevoluteDesc.motor = MotorDesc;
				}

				if (RevoluteDesc.isValid())
				{
					Joint = (NxRevoluteJoint*)World->GetScene()->createJoint(RevoluteDesc);
					if (RevoluteInfo->Breakable)
					{
						Joint->setBreakable(RevoluteInfo->BreakForce,RevoluteInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_PRISMATIC:
			{
				ZEPhysicsPrismaticJointInfo* PrismaticInfo = (ZEPhysicsPrismaticJointInfo*)&Info;

				//joint desc.
				NxPrismaticJointDesc PrismaticDesc;
				if (PrismaticInfo->Body1)
				{
					PrismaticDesc.actor[0] = ((ZEAegiaPhysicsBody*)PrismaticInfo->Body1)->GetActor();
				}
				else PrismaticDesc.actor[0] = NULL;
				if (PrismaticInfo->Body2)
				{
					PrismaticDesc.actor[1] = ((ZEAegiaPhysicsBody*)PrismaticInfo->Body2)->GetActor();
				}
				else PrismaticDesc.actor[1] = NULL;
				if (Info.CollideBodies)PrismaticDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (PrismaticInfo->UseGlobalAnchor)
				{
					PrismaticDesc.setGlobalAnchor(TONX(PrismaticInfo->GlobalAnchor));
				}
				else
				{
					PrismaticDesc.localAnchor[0] = TONX(PrismaticInfo->LocalAnchor1);
					PrismaticDesc.localAnchor[1] = TONX(PrismaticInfo->LocalAnchor2);
				}
				//set axis
				if (PrismaticInfo->UseGlobalAxis)
				{
					PrismaticDesc.setGlobalAxis(TONX(PrismaticInfo->GlobalAxis));
				}
				else
				{
					PrismaticDesc.localAxis[0] = TONX(PrismaticInfo->LocalAxis1);
					PrismaticDesc.localAxis[1] = TONX(PrismaticInfo->LocalAxis2);
				}

				if (PrismaticDesc.isValid())
				{
					Joint = (NxRevoluteJoint*)World->GetScene()->createJoint(PrismaticDesc);
					if (PrismaticInfo->Breakable)
					{
						Joint->setBreakable(PrismaticInfo->BreakForce,PrismaticInfo->BreakTorque);
					}
				}

				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_CYLINDRICAL:
			{
				ZEPhysicsCylindricalJointInfo* CylindricalInfo = (ZEPhysicsCylindricalJointInfo*)&Info;
				
				//joint desc.
				NxCylindricalJointDesc CylindricalDesc;
				if (CylindricalInfo->Body1)
				{
					CylindricalDesc.actor[0] = ((ZEAegiaPhysicsBody*)CylindricalInfo->Body1)->GetActor();
				}
				else CylindricalDesc.actor[0] = NULL;
				if (CylindricalInfo->Body2)
				{
					CylindricalDesc.actor[1] = ((ZEAegiaPhysicsBody*)CylindricalInfo->Body2)->GetActor();
				}
				else CylindricalDesc.actor[1] = NULL;
				if (Info.CollideBodies)CylindricalDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (CylindricalInfo->UseGlobalAnchor)
				{
					CylindricalDesc.setGlobalAnchor(TONX(CylindricalInfo->GlobalAnchor));
				}
				else
				{
					CylindricalDesc.localAnchor[0] = TONX(CylindricalInfo->LocalAnchor1);
					CylindricalDesc.localAnchor[1] = TONX(CylindricalInfo->LocalAnchor2);
				}
				//set axis
				if (CylindricalInfo->UseGlobalAxis)
				{
					CylindricalDesc.setGlobalAxis(TONX(CylindricalInfo->GlobalAxis));
				}
				else
				{
					CylindricalDesc.localAxis[0] = TONX(CylindricalInfo->LocalAxis1);
					CylindricalDesc.localAxis[1] = TONX(CylindricalInfo->LocalAxis2);
				}

				if (CylindricalDesc.isValid())
				{
					Joint = (NxRevoluteJoint*)World->GetScene()->createJoint(CylindricalDesc);
					if (CylindricalInfo->Breakable)
					{
						Joint->setBreakable(CylindricalInfo->BreakForce,CylindricalInfo->BreakTorque);
					}
				}

				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_DISTANCE:
			{
				ZEPhysicsDistanceJointInfo* DistanceInfo = (ZEPhysicsDistanceJointInfo*)&Info;
				
				//joint desc.
				NxDistanceJointDesc DistanceDesc;
				if (DistanceInfo->Body1)
				{
					DistanceDesc.actor[0] = ((ZEAegiaPhysicsBody*)DistanceInfo->Body1)->GetActor();
				}
				else DistanceDesc.actor[0] = NULL;
				if (DistanceInfo->Body2)
				{
					DistanceDesc.actor[1] = ((ZEAegiaPhysicsBody*)DistanceInfo->Body2)->GetActor();
				}
				else DistanceDesc.actor[1] = NULL;
				if (Info.CollideBodies)DistanceDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (DistanceInfo->UseGlobalAnchor)
				{
					DistanceDesc.setGlobalAnchor(TONX(DistanceInfo->GlobalAnchor));
				}
				else
				{
					DistanceDesc.localAnchor[0] = TONX(DistanceInfo->LocalAnchor1);
					DistanceDesc.localAnchor[1] = TONX(DistanceInfo->LocalAnchor2);
				}
				//set axis
				if (DistanceInfo->UseGlobalAxis)
				{
					DistanceDesc.setGlobalAxis(TONX(DistanceInfo->GlobalAxis));
				}
				else
				{
					DistanceDesc.localAxis[0] = TONX(DistanceInfo->LocalAxis1);
					DistanceDesc.localAxis[1] = TONX(DistanceInfo->LocalAxis2);
				}

				//limit
				if (DistanceInfo->HasMinLimit)
				{
					DistanceDesc.flags |= NX_DJF_MIN_DISTANCE_ENABLED;
					DistanceDesc.minDistance = DistanceInfo->MinDistance;
				}

				if (DistanceInfo->HasMaxLimit)
				{
					DistanceDesc.flags |= NX_DJF_MAX_DISTANCE_ENABLED;
					DistanceDesc.maxDistance = DistanceInfo->MaxDistance;
				}

				//spring
				if (DistanceInfo->HasSpring)
				{
					DistanceDesc.flags |= NX_DJF_SPRING_ENABLED;
					NxSpringDesc Spring;
					Spring.spring = DistanceInfo->SpringValue;
					Spring.damper = DistanceInfo->SpringDamper;
					DistanceDesc.spring = Spring;
				}

				if (DistanceDesc.isValid())
				{
					Joint = (NxDistanceJoint*)World->GetScene()->createJoint(DistanceDesc);
					if (DistanceInfo->Breakable)
					{
						Joint->setBreakable(DistanceInfo->BreakForce,DistanceInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_POINTONLINE:
			{
				ZEPhysicsPointOnLineJointInfo* PointOnLineInfo = (ZEPhysicsPointOnLineJointInfo*)&Info;
				
				//joint desc.
				NxPointOnLineJointDesc PointOnLineDesc;
				if (PointOnLineInfo->Body1)
				{
					PointOnLineDesc.actor[0] = ((ZEAegiaPhysicsBody*)PointOnLineInfo->Body1)->GetActor();
				}
				else PointOnLineDesc.actor[0] = NULL;
				if (PointOnLineInfo->Body2)
				{
					PointOnLineDesc.actor[1] = ((ZEAegiaPhysicsBody*)PointOnLineInfo->Body2)->GetActor();
				}
				else PointOnLineDesc.actor[1] = NULL;
				if (Info.CollideBodies)PointOnLineDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (PointOnLineInfo->UseGlobalAnchor)
				{
					PointOnLineDesc.setGlobalAnchor(TONX(PointOnLineInfo->GlobalAnchor));
				}
				else
				{
					PointOnLineDesc.localAnchor[0] = TONX(PointOnLineInfo->LocalAnchor1);
					PointOnLineDesc.localAnchor[1] = TONX(PointOnLineInfo->LocalAnchor2);
				}
				//set axis
				if (PointOnLineInfo->UseGlobalAxis)
				{
					PointOnLineDesc.setGlobalAxis(TONX(PointOnLineInfo->GlobalAxis));
				}
				else
				{
					PointOnLineDesc.localAxis[0] = TONX(PointOnLineInfo->LocalAxis1);
					PointOnLineDesc.localAxis[1] = TONX(PointOnLineInfo->LocalAxis2);
				}

				if (PointOnLineDesc.isValid())
				{
					Joint = (NxPointOnLineJoint*)World->GetScene()->createJoint(PointOnLineDesc);
					if (PointOnLineInfo->Breakable)
					{
						Joint->setBreakable(PointOnLineInfo->BreakForce,PointOnLineInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_POINTINPLANE:
			{
				ZEPhysicsPointInPlaneJointInfo* PointInPlaneInfo = (ZEPhysicsPointInPlaneJointInfo*)&Info;
				
				//joint desc.
				NxPointInPlaneJointDesc PointInPlaneDesc;
				if (PointInPlaneInfo->Body1)
				{
					PointInPlaneDesc.actor[0] = ((ZEAegiaPhysicsBody*)PointInPlaneInfo->Body1)->GetActor();
				}
				else PointInPlaneDesc.actor[0] = NULL;
				if (PointInPlaneInfo->Body2)
				{
					PointInPlaneDesc.actor[1] = ((ZEAegiaPhysicsBody*)PointInPlaneInfo->Body2)->GetActor();
				}
				else PointInPlaneDesc.actor[1] = NULL;
				if (Info.CollideBodies)PointInPlaneDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (PointInPlaneInfo->UseGlobalAnchor)
				{
					PointInPlaneDesc.setGlobalAnchor(TONX(PointInPlaneInfo->GlobalAnchor));
				}
				else
				{
					PointInPlaneDesc.localAnchor[0] = TONX(PointInPlaneInfo->LocalAnchor1);
					PointInPlaneDesc.localAnchor[1] = TONX(PointInPlaneInfo->LocalAnchor2);
				}
				//set axis
				if (PointInPlaneInfo->UseGlobalAxis)
				{
					PointInPlaneDesc.setGlobalAxis(TONX(PointInPlaneInfo->GlobalAxis));
				}
				else
				{
					PointInPlaneDesc.localAxis[0] = TONX(PointInPlaneInfo->LocalAxis1);
					PointInPlaneDesc.localAxis[1] = TONX(PointInPlaneInfo->LocalAxis2);
				}

				if (PointInPlaneDesc.isValid())
				{
					Joint = (NxPointInPlaneJoint*)World->GetScene()->createJoint(PointInPlaneDesc);
					if (PointInPlaneInfo->Breakable)
					{
						Joint->setBreakable(PointInPlaneInfo->BreakForce,PointInPlaneInfo->BreakTorque);
					}
				}
				break;
			}
			case ZEPhysicsJointInfo::ZE_PJT_PULLEY:
			{
				ZEPhysicsPulleyJointInfo* PulleyInfo = (ZEPhysicsPulleyJointInfo*)&Info;
				
				//joint desc.
				NxPulleyJointDesc PulleyDesc;
				if (PulleyInfo->Body1)
				{
					PulleyDesc.actor[0] = ((ZEAegiaPhysicsBody*)PulleyInfo->Body1)->GetActor();
				}
				else PulleyDesc.actor[0] = NULL;
				if (PulleyInfo->Body2)
				{
					PulleyDesc.actor[1] = ((ZEAegiaPhysicsBody*)PulleyInfo->Body2)->GetActor();
				}
				else PulleyDesc.actor[1] = NULL;
				if (Info.CollideBodies)PulleyDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (PulleyInfo->UseGlobalAnchor)
				{
					PulleyDesc.setGlobalAnchor(TONX(PulleyInfo->GlobalAnchor));
				}
				else
				{
					PulleyDesc.localAnchor[0] = TONX(PulleyInfo->LocalAnchor1);
					PulleyDesc.localAnchor[1] = TONX(PulleyInfo->LocalAnchor2);
				}
				//set axis
				if (PulleyInfo->UseGlobalAxis)
				{
					PulleyDesc.setGlobalAxis(TONX(PulleyInfo->GlobalAxis));
				}
				else
				{
					PulleyDesc.localAxis[0] = TONX(PulleyInfo->LocalAxis1);
					PulleyDesc.localAxis[1] = TONX(PulleyInfo->LocalAxis2);
				}

				//pulleys
				PulleyDesc.pulley[0] = TONX(PulleyInfo->Pulley1);
				PulleyDesc.pulley[1] = TONX(PulleyInfo->Pulley2);
				PulleyDesc.distance = PulleyInfo->Distance;
				PulleyDesc.ratio = PulleyInfo->Ratio;
				PulleyDesc.stiffness = PulleyInfo->Stiffness;

				if (PulleyInfo->IsRigid)PulleyDesc.flags |= NX_PJF_IS_RIGID;

				if (PulleyInfo->HasMotor)
				{
					PulleyDesc.flags |= NX_PJF_MOTOR_ENABLED;
					NxMotorDesc MotorDesc;
					MotorDesc.maxForce = PulleyInfo->MotorForce;		
					MotorDesc.freeSpin = false;
					MotorDesc.velTarget = PulleyInfo->MotorVelocity;
					PulleyDesc.motor = MotorDesc;
				}

				if (PulleyDesc.isValid())
				{
					Joint = (NxPulleyJoint*)World->GetScene()->createJoint(PulleyDesc);
					if (PulleyInfo->Breakable)
					{
						Joint->setBreakable(PulleyInfo->BreakForce,PulleyInfo->BreakTorque);
					}
				}
				break;
			}

			case ZEPhysicsJointInfo::ZE_PJT_FREE:
			{
				ZEPhysicsFreeJointInfo* FreeInfo = (ZEPhysicsFreeJointInfo*)&Info;
				
				//joint desc.
				NxD6JointDesc FreeDesc;
				if (FreeInfo->Body1)
				{
					FreeDesc.actor[0] = ((ZEAegiaPhysicsBody*)FreeInfo->Body1)->GetActor();
				}
				else FreeDesc.actor[0] = NULL;
				if (FreeInfo->Body2)
				{
					FreeDesc.actor[1] = ((ZEAegiaPhysicsBody*)FreeInfo->Body2)->GetActor();
				}
				else FreeDesc.actor[1] = NULL;
				if (Info.CollideBodies)FreeDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
				
				//set anchor
				if (FreeInfo->UseGlobalAnchor)
				{
					FreeDesc.setGlobalAnchor(TONX(FreeInfo->GlobalAnchor));
				}
				else
				{
					FreeDesc.localAnchor[0] = TONX(FreeInfo->LocalAnchor1);
					FreeDesc.localAnchor[1] = TONX(FreeInfo->LocalAnchor2);
				}
				//set axis
				if (FreeInfo->UseGlobalAxis)
				{
					FreeDesc.setGlobalAxis(TONX(FreeInfo->GlobalAxis));
				}
				else
				{
					FreeDesc.localAxis[0] = TONX(FreeInfo->LocalAxis1);
					FreeDesc.localAxis[1] = TONX(FreeInfo->LocalAxis2);
				}

				//set linear flags
				if (FreeInfo->XMotion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.xMotion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->XMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.xMotion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->XMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
				}

				if (FreeInfo->YMotion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.yMotion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->YMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.yMotion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->YMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
				}

				if (FreeInfo->ZMotion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.zMotion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->ZMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.zMotion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->ZMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;
				}

				//set linear limits
				FreeDesc.linearLimit.value = FreeInfo->LinearLimitValue;
				FreeDesc.linearLimit.restitution = FreeInfo->LinearLimitRestitution;
				FreeDesc.linearLimit.spring = FreeInfo->LinearLimitSpring;
				FreeDesc.linearLimit.damping = FreeInfo->LinearLimitDamper;

				//set angular flags
				if (FreeInfo->TwistMotion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.twistMotion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->TwistMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.twistMotion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->TwistMotion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
				}

				if (FreeInfo->Swing1Motion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.swing1Motion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->Swing1Motion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->Swing1Motion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
				}

				if (FreeInfo->Swing2Motion == ZEPhysicsFreeJointInfo::ZE_PJM_FREE)
				{
					FreeDesc.swing2Motion = NX_D6JOINT_MOTION_FREE;
				}
				else if (FreeInfo->Swing2Motion == ZEPhysicsFreeJointInfo::ZE_PJM_LIMITED)
				{
					FreeDesc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;
				}
				else if (FreeInfo->Swing2Motion == ZEPhysicsFreeJointInfo::ZE_PJM_LOCKED)
				{
					FreeDesc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
				}

				//set angular limits
				FreeDesc.swing1Limit.value = FreeInfo->Swing1LimitValue;
				FreeDesc.swing1Limit.restitution = FreeInfo->Swing1LimitRestitution;
				FreeDesc.swing1Limit.spring = FreeInfo->Swing1LimitSpring;
				FreeDesc.swing1Limit.damping = FreeInfo->Swing1LimitDamper;

				FreeDesc.swing2Limit.value = FreeInfo->Swing2LimitValue;
				FreeDesc.swing2Limit.restitution = FreeInfo->Swing2LimitRestitution;
				FreeDesc.swing2Limit.spring = FreeInfo->Swing2LimitSpring;
				FreeDesc.swing2Limit.damping = FreeInfo->Swing2LimitDamper;

				FreeDesc.twistLimit.low.value = FreeInfo->TwistLimitLowValue;
				FreeDesc.twistLimit.low.restitution = FreeInfo->TwistLimitRestitution;
				FreeDesc.twistLimit.low.spring = FreeInfo->TwistLimitSpring;
				FreeDesc.twistLimit.low.damping = FreeInfo->TwistLimitDamper;
				
				FreeDesc.twistLimit.high.value = FreeInfo->TwistLimitHighValue;
				FreeDesc.twistLimit.high.restitution = FreeInfo->TwistLimitRestitution;
				FreeDesc.twistLimit.high.spring = FreeInfo->TwistLimitSpring;
				FreeDesc.twistLimit.high.damping = FreeInfo->TwistLimitDamper;

				//set linear motors
				FreeDesc.drivePosition = TONX(FreeInfo->LinearMotorPosition);
				FreeDesc.driveLinearVelocity = TONX(FreeInfo->LinearMotorVelocity);

				//x motor
				if (FreeInfo->LinearXMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_POSITION)
				{
					FreeDesc.xDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
				}
				else if (FreeInfo->LinearXMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_VELOCITY)
				{
					FreeDesc.xDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
				}
				FreeDesc.xDrive.forceLimit = FreeInfo->LinearXMotorForce;
				FreeDesc.xDrive.spring = FreeInfo->LinearXMotorSpring;
				FreeDesc.xDrive.damping = FreeInfo->LinearXMotorDamper;

				//y motor
				if (FreeInfo->LinearYMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_POSITION)
				{
					FreeDesc.yDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
				}
				else if (FreeInfo->LinearYMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_VELOCITY)
				{
					FreeDesc.yDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
				}
				FreeDesc.yDrive.forceLimit = FreeInfo->LinearYMotorForce;
				FreeDesc.yDrive.spring = FreeInfo->LinearYMotorSpring;
				FreeDesc.yDrive.damping = FreeInfo->LinearYMotorDamper;

				//z motor
				if (FreeInfo->LinearZMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_POSITION)
				{
					FreeDesc.zDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
				}
				else if (FreeInfo->LinearZMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_VELOCITY)
				{
					FreeDesc.zDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
				}
				FreeDesc.zDrive.forceLimit = FreeInfo->LinearZMotorForce;
				FreeDesc.zDrive.spring = FreeInfo->LinearZMotorSpring;
				FreeDesc.zDrive.damping = FreeInfo->LinearZMotorDamper;

				//set angular motos
				FreeDesc.driveOrientation = TONX(FreeInfo->AngularMotorOrientation);
				FreeDesc.driveAngularVelocity = TONX(FreeInfo->AngularMotorVelocity);

				FreeDesc.flags = NX_D6JOINT_SLERP_DRIVE;
				if (FreeInfo->AngularMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_POSITION)
				{
					FreeDesc.slerpDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
				}
				else if (FreeInfo->AngularMotor == ZEPhysicsFreeJointInfo::ZE_PJMT_VELOCITY)
				{
					FreeDesc.slerpDrive.driveType = NX_D6JOINT_DRIVE_VELOCITY;
				}
				FreeDesc.slerpDrive.forceLimit = FreeInfo->AngularMotorForce;
				FreeDesc.slerpDrive.spring = FreeInfo->AngularMotorSpring;
				FreeDesc.slerpDrive.damping = FreeInfo->AngularMotorDamper;

				if (FreeDesc.isValid())
				{
					Joint = (NxD6Joint*)World->GetScene()->createJoint(FreeDesc);
					if (FreeInfo->Breakable)
					{
						Joint->setBreakable(FreeInfo->BreakForce,FreeInfo->BreakTorque);
					}
				}

				break;
			}
		}
	}
}

void ZEAegiaPhysicsJoint::Deinitialize()
{
	if (Joint != NULL && ZEAegiaPhysicsWorld::getSingletonPtr() != NULL)
	{
		ZEAegiaPhysicsWorld* World = ZEAegiaPhysicsWorld::getSingletonPtr();
		World->GetScene()->releaseJoint(*Joint);
		Joint = NULL;
	}
}

void ZEAegiaPhysicsJoint::SetJointMotor(float MotorForce, float MotorVelocity)
{
	if (Joint->isRevoluteJoint())
	{
		NxRevoluteJoint* rj = (NxRevoluteJoint*)Joint;
		NxRevoluteJointDesc revDesc;
	    rj->saveToDesc(revDesc);

	    revDesc.motor.velTarget = MotorVelocity;
	    revDesc.motor.maxForce = MotorForce;

		rj->loadFromDesc(revDesc);
	}
}
