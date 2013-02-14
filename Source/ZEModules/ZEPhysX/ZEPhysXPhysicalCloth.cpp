//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXPhysicalCloth.cpp
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

#include "ZEPhysXPhysicalCloth.h"
#include "ZEModules\ZEPhysX\ZEPhysXConversion.h"
#include "ZEMath\ZEAngle.h"
#include "NxBoxShapeDesc.h"

enum ZEClothBehaviourFlags
{
	ZE_CBF_PRESSURE					= 1,
	ZE_CBF_STATIC					= 2,
	ZE_CBF_GRAVITY					= 32,
	ZE_CBF_BENDING					= 64,
	ZE_CBF_BENDING_ORTHO			= 128,
	ZE_CBF_DAMPING					= 256,
	ZE_CBF_COLLISION_TWOWAY			= 512,
	ZE_CBF_TEARABLE					= 4096
};

void ZEPhysXPhysicalCloth::ReCreate()
{
	Deinitialize();
	Initialize();
}

ZEArray<ZEVector3>& ZEPhysXPhysicalCloth::GetVertices()
{
	return ClothVertices;
}

ZEArray<ZEUInt32>& ZEPhysXPhysicalCloth::GetIndices()
{
	return ClothIndices;
}

void ZEPhysXPhysicalCloth::SetEnabled(bool IsEnabled /*= true*/)
{
	if(IsEnabled)
	{
		if((ClothDesc.flags & ZE_CBF_STATIC) == ZE_CBF_STATIC)
			ClothDesc.flags &= ~ZE_CBF_STATIC;
	}
	else
		ClothDesc.flags |= ZE_CBF_STATIC;
}

bool ZEPhysXPhysicalCloth::GetEnabled() const
{
	return (ClothDesc.flags & ZE_CBF_STATIC) == ZE_CBF_STATIC;
}

void ZEPhysXPhysicalCloth::SetPhysicalWorld(ZEPhysicalWorld* World)
{
	PhysicalWorld = (ZEPhysXPhysicalWorld*)World;
	if (Cloth != NULL)
		ReCreate();
}

ZEPhysicalWorld* ZEPhysXPhysicalCloth::GetPhysicalWorld()
{
	return PhysicalWorld;
}

void ZEPhysXPhysicalCloth::SetPosition(const ZEVector3& Position)
{
	ClothDesc.globalPose.t = ZE_TO_NX(Position);
}

ZEVector3 ZEPhysXPhysicalCloth::GetPosition() const
{
	return NX_TO_ZE(ClothDesc.globalPose.t);
}

void ZEPhysXPhysicalCloth::SetRotation(const ZEQuaternion& Rotation)
{
	ClothDesc.globalPose.M.fromQuat(ZE_TO_NX(Rotation));
}

ZEQuaternion ZEPhysXPhysicalCloth::GetRotation() const
{
	NxQuat Temp;
	ClothDesc.globalPose.M.toQuat(Temp);
	return NX_TO_ZE(Temp);
}

void ZEPhysXPhysicalCloth::SetBendingMode(bool IsEnabled)
{
	if(IsEnabled)
		ClothDesc.flags |= ZE_CBF_BENDING;
	else
		if((ClothDesc.flags & ZE_CBF_BENDING) == ZE_CBF_BENDING)
			ClothDesc.flags &= ~ZE_CBF_BENDING;
}

bool ZEPhysXPhysicalCloth::GetBendingMode() const
{
	return (ClothDesc.flags & ZE_CBF_BENDING) == ZE_CBF_BENDING;
}

void ZEPhysXPhysicalCloth::SetBendingStiffness(float BendingStiffness)
{
	ClothDesc.bendingStiffness = BendingStiffness;
}

float ZEPhysXPhysicalCloth::GetBendingStiffness() const
{
	return ClothDesc.bendingStiffness;
}

void ZEPhysXPhysicalCloth::SetStretchingStiffness(float StretchingStiffness)
{
	ClothDesc.stretchingStiffness = StretchingStiffness;
}

float ZEPhysXPhysicalCloth::GetStretchingStiffness() const
{
	return ClothDesc.stretchingStiffness;
}

void ZEPhysXPhysicalCloth::SetDampingMode(bool IsEnabled)
{
	if(IsEnabled)
		ClothDesc.flags |= ZE_CBF_DAMPING;
	else
		if((ClothDesc.flags & ZE_CBF_DAMPING) == ZE_CBF_DAMPING)
			ClothDesc.flags &= ~ZE_CBF_DAMPING;
}

bool ZEPhysXPhysicalCloth::GetDampingMode() const
{
	return (ClothDesc.flags & ZE_CBF_DAMPING) == ZE_CBF_DAMPING;
}

void ZEPhysXPhysicalCloth::SetDampingCoefficient(float DampingCoefficient)
{
	ClothDesc.dampingCoefficient = DampingCoefficient;
}

float ZEPhysXPhysicalCloth::GetDampingCoefficient() const
{
	return ClothDesc.dampingCoefficient;
}

void ZEPhysXPhysicalCloth::SetFriction(float Friction)
{
	ClothDesc.friction = Friction;
}

float ZEPhysXPhysicalCloth::GetFriction() const
{
	return ClothDesc.friction;
}

void ZEPhysXPhysicalCloth::SetPressureMode(bool IsEnabled)
{
	if(IsEnabled)
		ClothDesc.flags |= ZE_CBF_PRESSURE;
	else
		if((ClothDesc.flags & ZE_CBF_PRESSURE) == ZE_CBF_PRESSURE)
			ClothDesc.flags &= ~ZE_CBF_PRESSURE;
}

bool ZEPhysXPhysicalCloth::GetPressureMode() const
{
	return (ClothDesc.flags & ZE_CBF_PRESSURE) == ZE_CBF_PRESSURE;
}

void ZEPhysXPhysicalCloth::SetPressure(float Pressure)
{
	ClothDesc.pressure = Pressure;
}

float ZEPhysXPhysicalCloth::GetPressure() const
{
	return ClothDesc.pressure;
}

void ZEPhysXPhysicalCloth::SetTearableMode(bool IsEnabled)
{
	if(IsEnabled)
	{
		ClothDesc.flags |= ZE_CBF_TEARABLE;
		ClothMeshDesc.flags |= NX_CLOTH_MESH_TEARABLE;
	}
	else
		if((ClothDesc.flags & ZE_CBF_TEARABLE) == ZE_CBF_TEARABLE)
			ClothDesc.flags &= ~ZE_CBF_TEARABLE;
}

bool ZEPhysXPhysicalCloth::GetTearableMode() const
{
	return (ClothDesc.flags & ZE_CBF_TEARABLE) == ZE_CBF_TEARABLE;
}

void ZEPhysXPhysicalCloth::SetTearFactor(float TearFactor)
{
	ClothDesc.tearFactor = TearFactor;
}

float ZEPhysXPhysicalCloth::GetTearFactor() const
{
	return ClothDesc.tearFactor;
}

void ZEPhysXPhysicalCloth::SetAttachmentTearFactor(float AttachmentTearFactor)
{
	ClothDesc.attachmentTearFactor = AttachmentTearFactor;
}

float ZEPhysXPhysicalCloth::GetAttachmentTearFactor() const
{
	return ClothDesc.attachmentTearFactor;
}

void ZEPhysXPhysicalCloth::SetThickness(float Thickness)
{
	ClothDesc.thickness = Thickness;
}

float ZEPhysXPhysicalCloth::GetThickness() const
{
	return ClothDesc.thickness;
}

void ZEPhysXPhysicalCloth::SetSolverIterations(ZEUInt SolverIterationCount)
{
	ClothDesc.solverIterations = SolverIterationCount;
}

ZEUInt ZEPhysXPhysicalCloth::GetSolverIterations() const
{
	return ClothDesc.solverIterations;
}

void ZEPhysXPhysicalCloth::AttachVertex(const ZEUInt VertexId, const ZEVector3& WorldPosition)
{
	Attachments.Add();
	Attachments.GetLastItem().VertexId = VertexId;
	Attachments.GetLastItem().AttachmentShape = NULL;
	Attachments.GetLastItem().AttachmentPosition = WorldPosition;
	Attachments.GetLastItem().AttachmentStatus = ZE_PCVA_GLOBAL;

	Cloth->attachVertexToGlobalPosition(VertexId, ZE_TO_NX(WorldPosition));
}

void ZEPhysXPhysicalCloth::AttachVertex(ZEUInt VertexId, const ZEPhysicalShape* Shape, const ZEVector3& LocalPosition)
{
	
}

void ZEPhysXPhysicalCloth::DetachVertex(const ZEUInt VertexId)
{
	Cloth->freeVertex(VertexId);

	for (ZESize I = 0; I < Attachments.GetCount(); I++)
		if (Attachments[I].VertexId == VertexId)
			Attachments.DeleteAt(I);
}

ZEClothVertexAttachment ZEPhysXPhysicalCloth::GetVertexAttachment(ZEUInt VertexId) const
{
	for(ZESize I = 0; I < Attachments.GetCount(); I++)
		if(Attachments[I].VertexId == VertexId)
			return Attachments[I];
}

bool ZEPhysXPhysicalCloth::TearVertex(const ZEUInt VertexId, const ZEVector3& Normal)
{
	return Cloth->tearVertex(VertexId, ZE_TO_NX(Normal));
}

void WeldVertices(ZEArray<ZEVector3>& InputVertices, ZEArray<ZEVector3>& OutputVertices, ZEArray<ZEUInt32>& OutputIndices)
{
	ZESize ParticleCount = InputVertices.GetCount();

	OutputVertices.Clear();

	OutputIndices.Clear();
	OutputIndices.SetCount(ParticleCount);

	for (ZESize I = 0; I < ParticleCount; I++)
	{
		OutputIndices[I] = (ZEUInt32)I;
	}

	ZEArray<ZEVector3> TempVerts = InputVertices;
	for(ZESize I = 0; I < ParticleCount; I++)
	{
		for(ZESize J = I + 1; J < ParticleCount; J++)
		{
			if(InputVertices[I] == TempVerts[J])
			{
				TempVerts[J] = ZEVector3(ZE_FLOAT_MAX, ZE_FLOAT_MAX, ZE_FLOAT_MAX);
			}
		}
	}

	for(ZESize I = 0; I < ParticleCount; I++)
	{
		if(TempVerts[I] != ZEVector3(ZE_FLOAT_MAX, ZE_FLOAT_MAX, ZE_FLOAT_MAX))
		{
			OutputVertices.Add(TempVerts[I]);
		}
	}

	for(ZESize I = 0; I < OutputVertices.GetCount(); I++)
	{
		for(ZESize J = 0; J < ParticleCount; J++)
		{
			if(OutputVertices[I] == InputVertices[J])
			{
				OutputIndices[J] = (ZEUInt32)I;
			}
		}
	}
}

bool ZEPhysXPhysicalCloth::Initialize()
{
	if (Cloth != NULL || PhysicalWorld == NULL || PhysicalWorld->GetScene() == NULL)
		return false;

	ClothMeshDesc.weldingDistance = 0;
	ClothMeshDesc.flags |= NX_CLOTH_MESH_WELD_VERTICES;

	ClothDesc.flags |= ZE_CBF_GRAVITY;
	ClothDesc.flags |= ZE_CBF_COLLISION_TWOWAY;

	ParticleCount = (ZEUInt32)ClothVertices.GetCount();

	ZEArray<ZEVector3> WeldedVertices;
	WeldVertices(ClothVertices, WeldedVertices, ClothIndices);
	
	ClothMeshDesc.numVertices				= (NxU32)WeldedVertices.GetCount();
	ClothMeshDesc.numTriangles				= (NxU32)ClothIndices.GetCount() / 3;
	ClothMeshDesc.pointStrideBytes			= (NxU32)sizeof(ZEVector3);
	ClothMeshDesc.triangleStrideBytes		= (NxU32)sizeof(ZEUInt32) * 3;
	ClothMeshDesc.points					= WeldedVertices.GetCArray();
	ClothMeshDesc.triangles					= ClothIndices.GetCArray();

	NxInitCooking();
	ZEPhysXMemoryWriteStream MemoryWriteStream;
	NxCookClothMesh(ClothMeshDesc, MemoryWriteStream);

	ZEPhysXMemoryReadStream MemoryReadStream;
	MemoryReadStream.SetData(MemoryWriteStream.GetData());
	ClothMesh = PhysicalWorld->GetPhysicsSDK()->createClothMesh(MemoryReadStream);
	NxCloseCooking();

	MeshData.verticesPosBegin = ClothVertices.GetCArray();
	MeshData.verticesPosByteStride = sizeof(ZEVector3);
	MeshData.maxVertices = ParticleCount;
	MeshData.numVerticesPtr = &ParticleCount;
	
	MeshData.indicesBegin = ClothIndices.GetCArray();
	MeshData.indicesByteStride = sizeof(ZEUInt32);
	MeshData.maxIndices = ParticleCount;
	MeshData.numIndicesPtr = &ParticleCount;

	*MeshData.numVerticesPtr = 0;
	*MeshData.numIndicesPtr = 0;

	ClothDesc.clothMesh = ClothMesh;
	ClothDesc.meshData	= MeshData;

	NxScene* Scene = PhysicalWorld->GetScene();
	Cloth = (NxCloth*)Scene->createCloth(ClothDesc);
	if(Cloth == NULL)
	{
		zeError("Can not create cloth.");
		return false;
	}

	WeldedVertices.Clear();

	return true;
}

void ZEPhysXPhysicalCloth::Deinitialize()
{
	if (Cloth != NULL)
	{
		PhysicalWorld->GetScene()->releaseCloth(*Cloth);	
		Cloth = NULL;
	}
}

ZEPhysXPhysicalCloth::ZEPhysXPhysicalCloth()
{
	Cloth = NULL;
	PhysicalWorld = NULL;
	ClothDesc.userData = this;
}

ZEPhysXPhysicalCloth::~ZEPhysXPhysicalCloth()
{
	Deinitialize();
}


