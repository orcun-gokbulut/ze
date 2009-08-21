//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - AegiaPhysicsBody.h
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
#ifndef	__ZE_AEGIA_PHYSICS_BODY_H__
#define __ZE_AEGIA_PHYSICS_BODY_H__

class ZEPhysicsBody;
class ZEPhysicsBodyInfo;
class ZEVector3;
class ZEQuaternion;
class NxActor;
#include "AegiaPhysicsUtility.h"

class ZEAegiaPhysicsBody : public ZEPhysicsBody
{
	friend class ZEAegiaPhysicsModule;

private:
	ZEAegiaPhysicsBody();
	~ZEAegiaPhysicsBody();

public:
	void Initialize(ZEPhysicsBodyInfo& Info);
	void Deinitialize();

	void ApplyForce(const ZEVector3 Force) { Actor->addForce(TONX(Force),NX_FORCE); }
	void ApplyImpulse(const ZEVector3 Force) { Actor->addForce(TONX(Force),NX_IMPULSE); }
	void ApplyTorque(const ZEVector3 Force) { Actor->addTorque(TONX(Force)); }

	void ApplyLocalForce(const ZEVector3 Force) { Actor->addLocalForce(TONX(Force),NX_FORCE); }
	void ApplyLocalImpulse(const ZEVector3 Force) { Actor->addLocalForce(TONX(Force),NX_IMPULSE); }
	void ApplyLocalTorque(const ZEVector3 Force) { Actor->addLocalTorque(TONX(Force)); }

	ZEVector3 GetMassCenter() { return TOZE(Actor->getCMassLocalPosition()); }
	void SetMassCenter(const ZEVector3 Pos) { Actor->setCMassOffsetLocalPosition(TONX(Pos)); }

	ZEVector3 GetLinearVelocity() { return TOZE(Actor->getLinearVelocity()); }
	void SetLinearVelocity(const ZEVector3 Vel) { Actor->setLinearVelocity(TONX(Vel)); }

	ZEVector3 GetAngularVelocity() { return TOZE(Actor->getAngularVelocity()); }
	void SetAngularVelocity(const ZEVector3 Vel) { Actor->setAngularVelocity(TONX(Vel)); }

	ZEVector3 GetPosition() { return TOZE(Actor->getGlobalPosition()); }
	void SetPosition(const ZEVector3 Pos) { Actor->setGlobalPosition(TONX(Pos)); }

	ZEQuaternion GetOrientation() { return TOZE(Actor->getGlobalOrientationQuat()); }
	void SetOrientation(const ZEQuaternion Ori) { Actor->setGlobalOrientationQuat(TONX(Ori)); }
	
	float GetLinearDamping() { return Actor->getLinearDamping(); }
	void SetLinearDamping(float Damp) { Actor->setLinearDamping(Damp); }

	float GetAngularDamping() { return Actor->getAngularDamping(); }
	void SetAngularDamping(float Damp) { Actor->setAngularDamping(Damp); }

	ZEVector3 GetLinearMomentum() { return TOZE(Actor->getLinearMomentum()); }
	void SetLinearMomentum(ZEVector3 Moment) { Actor->setLinearMomentum(TONX(Moment)); }

	ZEVector3 GetAngularMomentum() { return TOZE(Actor->getAngularMomentum()); }
	void SetAngularMomentum(ZEVector3 Moment) { Actor->setAngularMomentum(TONX(Moment)); }

	void SetMass(float Mass) { Actor->setMass(Mass); }
	float GetMass() { return Actor->getMass(); }
	bool IsDynamic() { return Actor->isDynamic(); }

	unsigned int GetShapeCount() { return Actor->getNbShapes(); }
	ZEVector3 GetGlobalShapePosition(int Index) 
	{ 
		return TOZE(Actor->getShapes()[Index]->getGlobalPosition());
	}
	void SetGlobalShapePosition(int Index, const ZEVector3 Pos) 
	{ 
		Actor->getShapes()[Index]->setGlobalPosition(TONX(Pos));
	}
	ZEVector3 GetLocalShapePosition(int Index)
	{ 
		return TOZE(Actor->getShapes()[Index]->getLocalPosition());
	}
	void SetLocalShapePosition(int Index, const ZEVector3 Pos) 
	{ 
		Actor->getShapes()[Index]->setLocalPosition(TONX(Pos));
	}
	ZEQuaternion GetGlobalShapeOrientation(int Index)
	{ 
		NxMat33 Mat = Actor->getShapes()[Index]->getGlobalOrientation();
		NxQuat Quat;Mat.toQuat(Quat);
		return TOZE(Quat);
	}
	void SetGlobalShapeOrientation(int Index, const ZEQuaternion Ori)
	{ 
		NxQuat Quat = TONX(Ori);
		NxMat33 Mat;Mat.fromQuat(Quat);
		Actor->getShapes()[Index]->setGlobalOrientation(Mat);
		Actor->updateMassFromShapes(0,Actor->getMass());
	}
	ZEQuaternion GetLocalShapeOrientation(int Index)  
	{ 
		NxMat33 Mat = Actor->getShapes()[Index]->getLocalOrientation();
		NxQuat Quat;Mat.toQuat(Quat);
		return TOZE(Quat);
	}
	void SetLocalShapeOrientation(int Index,const ZEQuaternion Ori)
	{ 
		NxQuat Quat = TONX(Ori);
		NxMat33 Mat;Mat.fromQuat(Quat);
		Actor->getShapes()[Index]->setLocalOrientation(Mat);
		Actor->updateMassFromShapes(0,Actor->getMass());
	}

	//gets & sets
	NxActor* GetActor() { return Actor; }

private:
	NxActor* Actor;
};

#endif


//	ZEQuaternion GetGlobalOrientation() 
//	{ 
//		NxMat33 Mat = Shape->getGlobalOrientation();
//		NxQuat Quat;Mat.toQuat(Quat);
//		return TOZE(Quat);
//	}
//
//	void SetGlobalOrientation(const ZEQuaternion Ori) 
//	{
//		NxQuat Quat = TONX(Ori);
//		NxMat33 Mat;Mat.fromQuat(Quat);
//		Shape->setGlobalOrientation(Mat);
//		Shape->getActor().updateMassFromShapes(0,Shape->getActor().getMass());
//	}
//
//	ZEQuaternion GetLocalOrientation() 
//	{ 
//		NxMat33 Mat = Shape->getLocalOrientation();
//		NxQuat Quat;Mat.toQuat(Quat);
//		return TOZE(Quat);
//	}
//
//	void SetLocalOrientation(const ZEQuaternion Ori)
//	{
//		NxQuat Quat = TONX(Ori);
//		NxMat33 Mat;Mat.fromQuat(Quat);
//		Shape->setLocalOrientation(Mat);
//		Shape->getActor().updateMassFromShapes(0,Shape->getActor().getMass());
//	}
