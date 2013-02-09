//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysicalWorld.h
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
#ifndef	__ZE_PHYSICAL_WORLD_H__
#define __ZE_PHYSICAL_WORLD_H__

#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZERay.h"

class ZEPhysicsWorldInfo;
class ZEPhysicsCollision;
class ZEPhysicsTrigger;
class ZEPhysicalObject;
class ZEPhysicalShape;
class ZERenderer;

struct ZERayCastResultDetails
{
	ZEVector3 ImpactWorldPosition;
	ZEVector3 ImpactWorldNormal;
	float	  ImpactDistance;
};

enum ZEPhysicsRayCastFilterShapeType
{
	ZE_PRCFST_STATIC_SHAPES		= 1,
	ZE_PRCFST_DYNAMIC_SHAPES	= 2,
	ZE_PRCFST_ALL_SHAPES		= 3,
};

class ZEPhysicalWorld
{
	protected:
										ZEPhysicalWorld(){}
		virtual							~ZEPhysicalWorld(){}

	public:
		virtual void					SetGravity(const ZEVector3& Gravity) = 0;
		virtual ZEVector3				GetGravity() = 0;

		virtual const 
		ZEArray<ZEPhysicalObject*>&		GetPhysicalObjects() = 0;
		virtual void					AddPhysicalObject(ZEPhysicalObject* Object) = 0;
		virtual void					RemovePhysicalObject(ZEPhysicalObject* Object) = 0;

		virtual void					SetEnabled(bool Enabled) = 0;
		virtual bool					GetEnabled() = 0;

		virtual bool					Initialize() = 0;
		virtual void					Deinitialize() = 0;

		virtual void					Process(float ElapsedTime) = 0;
		virtual void					Update() = 0;
		
		virtual void					Destroy();

		virtual void					Draw(ZERenderer* Renderer) = 0;

		virtual ZEPhysicalShape*		RayCastToClosestShape(ZERay Ray, ZEPhysicsRayCastFilterShapeType Type, ZERayCastResultDetails& ResultDetails) = 0;

		static ZEPhysicalWorld*			CreateInstance();
};

#endif
