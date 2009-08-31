//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - box.cpp
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

#include "Core/Core.h"
#include "Core/Module.h"
#include "GameInterface/Scene.h"
#include "Physics/PhysicsModule.h"
#include "GameInterface/CanvasBrush.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Quaternion.h"

#include "NxPhysics.h"
#include "NxCooking.h"
#include "Physics/Aegia/AegiaPhysicsModule.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsWorldInfo.h"
#include "Physics/Aegia/AegiaPhysicsWorld.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsBodyInfo.h"
#include "Physics/Aegia/AegiaPhysicsBody.h"
#include "Physics/PhysicsShapeInfo.h"
#include "Physics/PhysicsBoxShapeInfo.h"

#include "box.h"

box::box(ZEScene* scene, const char* tex, float mass, ZEVector3 poss, ZEQuaternion ori, ZEVector3 scl, bool kinematic, bool trigger, ZEPhysicsCollisionMask mask, float ldamp, float adamp)
{
	model = new ZECanvasBrush();
	model->Canvas.AddBox(1,1,1);
	model->SetPosition(poss);
	model->SetScale(scl);
	model->UpdateCanvas();
	model->Material.SetZero();
	model->Material.LightningEnabled = true;
	model->Material.SetShaderComponents(ZESHADER_DIFFUSEMAP);
	model->Material.AmbientColor = ZEVector3(0.0f, 0.0f, 0.0f);
	model->Material.DiffuseColor = ZEVector3(1.0f, 1.0f, 1.0f);
	model->Material.SpecularColor = ZEVector3(1.0f, 1.0f, 1.0f);
	model->Material.SpecularFactor = 64.0f;
	model->Material.DiffuseMap = ZETextureResource::LoadResource(tex)->GetTexture();
	scene->AddEntity(model);

	ZEPhysicsBoxShapeInfo BoxShapeInfo;
	BoxShapeInfo.Trigger = trigger;
	BoxShapeInfo.CollisionMask = mask;
	BoxShapeInfo.Dimensions = scl * 0.5;
	ZEPhysicsBodyInfo BodyInfo;
	BodyInfo.ShapeInfos.Add(&BoxShapeInfo);
	BodyInfo.Mass = mass;
	BodyInfo.Position = poss;
	BodyInfo.Orientation = ori;
	BodyInfo.Kinematic = kinematic;
	BodyInfo.LinearDamp = ldamp;
	BodyInfo.AngularDamp = adamp;
	actor = zePhysics->CreateBody();
	actor->Initialize(BodyInfo);
}

box::~box()
{
}
