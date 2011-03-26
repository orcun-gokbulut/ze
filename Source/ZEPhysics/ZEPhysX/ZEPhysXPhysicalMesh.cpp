//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalMesh.cpp
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

#include "ZEPhysXPhysicalMesh.h"
#include "ZEPhysXPhysicalWorld.h"
#include "ZEPhysXStream.h"
#include "ZEPhysXConversion.h"
#include "ZECore\ZEError.h"

#include <NxShapeDesc.h>
#include <NxTriangleMeshShapeDesc.h>
#include <NxCooking.h>
#include <NxMaterial.h>
#include <NxQuat.h>
#include <NxVec3.h>

ZEPhysXPhysicalMesh::ZEPhysXPhysicalMesh()
{
	PhysicalWorld = NULL;
	Actor = NULL;
	Scale = ZEVector3::One;
	ActorDesc.body = NULL;
	ActorDesc.shapes.push_back(&TriangleMeshShapeDesc);
	ActorDesc.userData = this;
	Enabled = true;
}

ZEPhysXPhysicalMesh::~ZEPhysXPhysicalMesh()
{
	Deinitialize();
}

void ZEPhysXPhysicalMesh::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	if (PhysicalWorld == World)
		return;

	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;

	if (Actor != NULL)
	{
		Deinitialize();
		Initialize();
	}
}

ZEPhysicalWorld* ZEPhysXPhysicalMesh::GetPhysicalWorld()
{
	return PhysicalWorld;
}

void ZEPhysXPhysicalMesh::SetEnabled(bool Enabled)
{
	this->Enabled = Enabled;

	if (Enabled)
	{
		ActorDesc.flags &= ~(NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_FLUID_DISABLE_COLLISION);
		if (Actor != NULL)
		{
			Actor->clearActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->clearActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->clearActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
		}
	}
	else
	{
		ActorDesc.flags |= (NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_FLUID_DISABLE_COLLISION);
		if (Actor != NULL)
		{
			Actor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
			Actor->raiseActorFlag(NX_AF_DISABLE_RESPONSE);
			Actor->raiseActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
		}
	}
}

bool ZEPhysXPhysicalMesh::GetEnabled()
{
	return Enabled;
}

void ZEPhysXPhysicalMesh::SetPosition(const ZEVector3& NewPosition)
{
	ActorDesc.globalPose.t = ZE_TO_NX(NewPosition);
	if (Actor != NULL)
	{
		zeWarningAssert(true, "PhysX Physical Static Mesh", "Moving already initialized static object reduces performance.");
		Actor->setGlobalPosition(ActorDesc.globalPose.t);
	}
}

ZEVector3 ZEPhysXPhysicalMesh::GetPosition()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getGlobalPosition());
	else
		return NX_TO_ZE(ActorDesc.globalPose.t);
}

void ZEPhysXPhysicalMesh::SetRotation(const ZEQuaternion& NewRotation)
{
	ActorDesc.globalPose.M.fromQuat(ZE_TO_NX(NewRotation));
	if (Actor != NULL)
	{
		zeWarningAssert(true, "PhysX Physical Static Mesh", "Moving already initialized static object reduces performance.");
		Actor->setGlobalOrientationQuat(ZE_TO_NX(NewRotation));
	}
}

ZEQuaternion ZEPhysXPhysicalMesh::GetRotation()
{
	if (Actor != NULL)
		return NX_TO_ZE(Actor->getGlobalOrientationQuat());
	else
	{
		NxQuat Temp;
		ActorDesc.globalPose.M.toQuat(Temp);
		return NX_TO_ZE(Temp);
	}
}

void ZEPhysXPhysicalMesh::SetScale(const ZEVector3& NewScale)
{
	Scale = NewScale;
	if (Actor != NULL)
		ReCreate();
}

ZEVector3 ZEPhysXPhysicalMesh::GetScale()
{
	return Scale;
}

void ZEPhysXPhysicalMesh::SetSkinWidth( float Width )
{
	TriangleMeshShapeDesc.skinWidth = Width;
}

float ZEPhysXPhysicalMesh::GetSkinWidth() const
{
	return this->SkinWidth;
}

void ZEPhysXPhysicalMesh::ReCreate()
{
	ActorDesc.globalPose.t = Actor->getGlobalPosition();
	ActorDesc.globalPose.M.fromQuat(Actor->getGlobalOrientationQuat()); 
	Initialize();
}

bool ZEPhysXPhysicalMesh::SetData(const ZEVector3* Vertices, size_t VertexCount, 
										const ZEPhysicalTriangle* Triangles, size_t TriangleCount, 
										const ZEPhysicalMaterial* Materials, size_t MaterialCount)
{
	if (TriangleMeshShapeDesc.meshData != NULL)
	{
		GetPhysicsSDK()->releaseTriangleMesh(*TriangleMeshShapeDesc.meshData);
		TriangleMeshShapeDesc.meshData = NULL;
	}

	ZEArray<ZEVector3> TransformedVertices;
	TransformedVertices.SetCount(VertexCount);
	for (size_t I = 0; I < VertexCount; I++)
		ZEVector3::Multiply(TransformedVertices[I], Vertices[I], Scale);

	NxTriangleMeshDesc TriangleMeshDesc;
	TriangleMeshDesc.numVertices = VertexCount;
	TriangleMeshDesc.pointStrideBytes = sizeof(ZEVector3);
	TriangleMeshDesc.points = TransformedVertices.GetConstCArray();
	TriangleMeshDesc.numTriangles = TriangleCount;
	TriangleMeshDesc.triangles = Triangles->Indices;
	TriangleMeshDesc.triangleStrideBytes = sizeof(ZEPhysicalTriangle);
	TriangleMeshDesc.flags = NULL;

	ZEPhysXMemoryWriteStream WriteStream;
	if (!GetCookingInterface()->NxCookTriangleMesh(TriangleMeshDesc, WriteStream))
	{
		zeError("PhysX Physical Static Mesh", "Can not cook triangle mesh.");
		return false;
	}

	ZEPhysXMemoryReadStream ReadStream;
	ReadStream.SetData(WriteStream.GetData());	
	TriangleMeshShapeDesc.meshData = GetPhysicsSDK()->createTriangleMesh(ReadStream);
	if (TriangleMeshShapeDesc.meshData == NULL)
	{
		zeError("PhysX Physical Static Mesh", "Can not create triangle mesh.");
		return false;
	}
}

void ZEPhysXPhysicalMesh::SetCollisionCallbackFlags(ZEDWORD CollisionCallbackFlags)
{
	
	ActorDesc.contactReportFlags = (ActorDesc.contactReportFlags & ~(NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH)) |
		((CollisionCallbackFlags & ZE_PCCF_ON_TOUCH) ? NX_NOTIFY_ON_TOUCH : NULL) |
		((CollisionCallbackFlags & ZE_PCCF_ON_START_TOUCH) ? NX_NOTIFY_ON_START_TOUCH : NULL) |
		((CollisionCallbackFlags & ZE_PCCF_ON_END_TOUCH) ? NX_NOTIFY_ON_END_TOUCH : NULL);
	
	if (Actor != NULL)
		Actor->setContactReportFlags(ActorDesc.contactReportFlags);
	
}

ZEDWORD ZEPhysXPhysicalMesh::GetCollisionCallbackFlags()
{
	return (ActorDesc.contactReportFlags & NX_NOTIFY_ON_START_TOUCH ? ZE_PCCF_ON_START_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_END_TOUCH ? ZE_PCCF_ON_END_TOUCH : NULL) |
		(ActorDesc.contactReportFlags & NX_NOTIFY_ON_TOUCH ? ZE_PCCF_ON_TOUCH : NULL);
}

bool ZEPhysXPhysicalMesh::Initialize()
{
	Deinitialize();
	if (PhysicalWorld == NULL || PhysicalWorld->GetScene() == NULL)
		return false;
	
	NxScene* Scene = PhysicalWorld->GetScene();
	Actor = Scene->createActor(ActorDesc);
	if (Actor == NULL)
	{
		zeError("PhysX Physical Static Object", "Can not create actor.");
		return false;
	}

	return true;
}

void ZEPhysXPhysicalMesh::Deinitialize()
{
	if (Actor != NULL && PhysicalWorld != NULL && PhysicalWorld->GetScene() != NULL)
	{
		PhysicalWorld->GetScene()->releaseActor(*Actor);
		Actor = NULL;
	}
}




