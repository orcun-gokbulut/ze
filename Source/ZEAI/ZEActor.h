//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEActor.h
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
#ifndef __ZE_ACTOR_H__
#define __ZE_ACTOR_H__

#include "ZEDS\ZEArray.h"
#include "ZEDS\ZEFlags.h"
#include "ZEMath\ZEVector.h"
#include "ZEDS\ZEString.h"

class ZESteering;

class ZEActor
{
	private:
		ZEString				Name;

		ZEVector3				Position;
		float					Rotation;

		ZEVector3				LinearVelocity;
		float					AngularVelocity;

		ZEVector3				LinearAcceleration;
		float					AngularAcceleration;

		float					MaxAngularAcceleration;
		float					MaxLinearAcceleration;

		float					MaxLinearVelocity;
		float					MaxAngularVelocity;

		float					Radius;

		ZEArray<ZESteering*>	Steerings;

	public:

		void					SetName(const ZEString& Name);
		const ZEString&			GetName();

		void					SetPosition(const ZEVector3& Position);
		const ZEVector3&		GetPosition();

		void					SetRotation(float Rotation);
		float					GetRotation();

		void					SetFaceRotation(float Rotation);
		float					GetFaceRotation();

		void					SetLinearVelocity(const ZEVector3& Velocity);
		const ZEVector3&		GetLinearVelocity();

		void					SetAngularVelocity(float Velocity);
		float					GetAngularVelocity();

		const ZEVector3&		GetLinearAcceleration();
		float					GetAngularAcceleration();

		void					SetMaxAngularVelocity(float Velocity);
		float					GetMaxAngularVelocity();

		void					SetMaxLinearVelocity(float Velocity);
		float					GetMaxLinearSpeed();

		void					SetMaxLinearAcceleration(float Acceleration);
		float					GetMaxLinearAcceleration();

		void					SetMaxAngularAngular(float Acceleration);
		float					SetMaxAngularAngular();

		void					SetMaxAngularAcceleration(float Acceleration);
		float					GetMaxAngularAcceleration();
	
		void					SetRadius(float Radius);
		float					GetRadius();

		const ZEArray<ZESteering*>&	GetSteerings();
		void					AddSteering(ZESteering* Steering);
		void					RemoveSteering(ZESteering* Steering);

		virtual void			Tick(float ElapsedTime);

								ZEActor();
								~ZEActor();
};

#endif
