//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysicsBody.h
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
#ifndef	__ZE_PHYSICS_BODY_H__
#define __ZE_PHYSICS_BODY_H__

class ZEPhysicsBodyInfo;
class ZEVector3;
class ZEQuaternion;
class ZEPhysicsShape;

class ZEPhysicsBody
{
	protected:
								ZEPhysicsBody(){}
		virtual					~ZEPhysicsBody(){}

	public:
		virtual void			Initialize(ZEPhysicsBodyInfo& Info) = 0;
		virtual void			Deinitialize() = 0;

		virtual void			ApplyForce(const ZEVector3) = 0;
		virtual void			ApplyImpulse(const ZEVector3) = 0;
		virtual void			ApplyTorque(const ZEVector3) = 0;

		virtual void			ApplyLocalForce(const ZEVector3) = 0;
		virtual void			ApplyLocalImpulse(const ZEVector3) = 0;
		virtual void			ApplyLocalTorque(const ZEVector3) = 0;

		virtual ZEVector3		GetMassCenter() = 0;
		virtual void			SetMassCenter(const ZEVector3) = 0;

		virtual ZEVector3		GetLinearVelocity() = 0;
		virtual void			SetLinearVelocity(const ZEVector3)  = 0;

		virtual ZEVector3		GetAngularVelocity() = 0;
		virtual void			SetAngularVelocity(const ZEVector3) = 0;

		virtual ZEVector3		GetPosition() = 0;
		virtual void			SetPosition(const ZEVector3) = 0;
		
		virtual ZEQuaternion	GetOrientation() = 0;
		virtual void			SetOrientation(const ZEQuaternion) = 0;

		virtual float			GetLinearDamping() = 0;
		virtual void			SetLinearDamping(float) = 0;

		virtual float			GetAngularDamping() = 0;
		virtual void			SetAngularDamping(float) = 0;

		virtual ZEVector3		GetLinearMomentum() = 0;
		virtual void			SetLinearMomentum(ZEVector3) = 0;

		virtual ZEVector3		GetAngularMomentum() = 0;
		virtual void			SetAngularMomentum(ZEVector3) = 0;

		virtual void			SetMass(float) = 0;
		virtual float			GetMass() = 0;
		virtual bool			IsDynamic() = 0;

		virtual unsigned int	GetShapeCount() = 0;
		virtual ZEVector3		GetGlobalShapePosition(int Index) = 0;
		virtual void			SetGlobalShapePosition(int Index, const ZEVector3) = 0;
		virtual ZEVector3		GetLocalShapePosition(int Index) = 0;
		virtual void			SetLocalShapePosition(int Index, const ZEVector3) = 0;
		virtual ZEQuaternion	GetGlobalShapeOrientation(int Index) = 0;
		virtual void			SetGlobalShapeOrientation(int Index, const ZEQuaternion) = 0;
		virtual ZEQuaternion	GetLocalShapeOrientation(int Index) = 0;
		virtual void			SetLocalShapeOrientation(int Index,const ZEQuaternion) = 0;
};

#endif
