//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalWorld.h
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
#ifndef	__ZE_PHYSX_PHYSICAL_WORLD_H__
#define __ZE_PHYSX_PHYSICAL_WORLD_H__

#include "ZEPhysics/ZEPhysicalWorld.h"
#include "ZEPhysXComponentBase.h"
#include "ZEPhysXCollisionManager.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"

#include <NxScene.h>
#include <NxSceneDesc.h>
#include <NxMaterial.h>

class ZEPhysicalObject;
class ZEPhysXPhysicalWorld : public ZEPhysicalWorld, public ZEPhysXComponentBase
{
	friend class ZEPhysXModule;
	private:
		ZEPhysXCollisionManager						CollisionManager;

		bool										Enabled;
		bool										Visualize;

	protected:
		NxSceneDesc									SceneDesc;
		NxScene*									Scene;

		ZEArray<ZEPhysicalObject*>					PhysicalObjects;
		ZEVector3									Gravity;
		
		virtual bool								InitializeInternal();
		virtual bool								DeinitializeInternal();

													ZEPhysXPhysicalWorld();
		virtual										~ZEPhysXPhysicalWorld();
		
	public:
		NxScene*									GetScene();

		virtual const ZEArray<ZEPhysicalObject*>&	GetPhysicalObjects();
		virtual void								AddPhysicalObject(ZEPhysicalObject* Object);
		virtual void								RemovePhysicalObject(ZEPhysicalObject* Object);
	
		virtual void								SetGravity(const ZEVector3& Gravity);
		virtual ZEVector3							GetGravity();

		virtual void								SetVisualize(bool Enabled);
		virtual bool								GetVisualize();

		virtual void								SetEnabled(bool Enabled);
		virtual bool								GetEnabled();

		virtual void								Process(float ElapsedTime);
		virtual void								Update();

		virtual ZEPhysicalShape*					RayCastToClosestShape(ZERay Ray, ZEPhysicsRayCastFilterShapeType Type, ZERayCastResultDetails& ResultDetails);
};

#endif
