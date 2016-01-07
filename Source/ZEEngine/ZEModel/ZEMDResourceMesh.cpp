//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceMesh.cpp
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

#include "ZEMDResourceMesh.h"

#include "ZEML\ZEMLReader.h"
#include "ZEGraphics\ZEVertexBuffer.h"
#include "ZEGraphics\ZEIndexBuffer.h"

// ZEMDResourceMeshLOD
/////////////////////////////////////////////////////////////////////////////

void ZEModelResourceMeshLOD::SetLevel(ZEInt32 LODLevel)
{
	this->Level = LODLevel;
}

ZEInt32 ZEModelResourceMeshLOD::GetLevel() const 
{
	return Level;
}

void ZEModelResourceMeshLOD::SetStartDistance(ZEInt32 LODStartDistance)
{
	this->StartDistance = LODStartDistance;
}

ZEInt32 ZEModelResourceMeshLOD::GetStartDistance() const 
{
	return StartDistance;
}

void ZEModelResourceMeshLOD::SetEndDistance(ZEInt32 LODEndDistance)
{
	this->EndDistance = LODEndDistance;
}

ZEInt32 ZEModelResourceMeshLOD::GetEndDistance() const 
{
	return EndDistance;
}

void ZEModelResourceMeshLOD::SetVertices(const ZEArray<ZEModelVertex>& Vertices)
{
	this->Vertices = Vertices;
}

const ZEArray<ZEModelVertex>& ZEModelResourceMeshLOD::GetVertices() const
{
	return Vertices;
}

void ZEModelResourceMeshLOD::SetVerticesSkin(const ZEArray<ZESkinnedModelVertex>& VerticesSkin)
{
	this->VerticesSkin = VerticesSkin;
}

const ZEArray<ZESkinnedModelVertex>& ZEModelResourceMeshLOD::GetVerticesSkin() const
{
	return VerticesSkin;
}

void ZEModelResourceMeshLOD::SetIndices(const ZEArray<ZEUInt16>& Indices)
{
	this->Indices = Indices;
}

const ZEArray<ZEUInt16>& ZEModelResourceMeshLOD::GetIndices() const
{
	return Indices;
}

void ZEModelResourceMeshLOD::SetAffectingBoneIds(const ZEArray<ZEUInt16>& BoneIds)
{
	AffectingBoneIds = BoneIds;
}

const ZEArray<ZEUInt16>& ZEModelResourceMeshLOD::GetAffectingBoneIds() const
{
	return AffectingBoneIds;
}

void ZEModelResourceMeshLOD::SetVertexBuffer(ZEStaticVertexBuffer* VertexBuffer)
{
	Vertices.Clear();
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::GetVertexBuffer() const
{
	UpdateVertexBuffer();
	return VertexBuffer;
}

void ZEModelResourceMeshLOD::SetVertexBufferSkin(ZEStaticVertexBuffer* VertexBufferSkin)
{
	this->VertexBufferSkin = VertexBufferSkin;
}

ZEStaticVertexBuffer* ZEModelResourceMeshLOD::GetVertexBufferSkin() const
{
	UpdateVertexBufferSkin();
	return VertexBufferSkin;
}

void ZEModelResourceMeshLOD::SetIndexBuffer(ZEStaticIndexBuffer* IndexBuffer)
{
	this->IndexBuffer = IndexBuffer;
}

ZEStaticIndexBuffer* ZEModelResourceMeshLOD::GetIndexBuffer() const
{
	UpdateIndexBuffer();
	return IndexBuffer;
}

void ZEModelResourceMeshLOD::SetMaterialId(ZEInt32 MaterialId)
{
	this->MaterialId = MaterialId;
}

ZEInt32 ZEModelResourceMeshLOD::GetMaterialId() const 
{
	return MaterialId;
}

void ZEModelResourceMeshLOD::SetMaterialFilePath(const ZEString& MaterialFilePath)
{
	this->MaterialFilePath = MaterialFilePath;
}

const ZEString& ZEModelResourceMeshLOD::GetMaterialFilePath()
{
	return MaterialFilePath;
}

void ZEModelResourceMeshLOD::SetMaterial(ZEMaterial* Material)
{
	this->Material = Material;
}

ZEMaterial* ZEModelResourceMeshLOD::GetMaterial()
{
	return Material;
}

bool ZEModelResourceMeshLOD::Load(const ZEMLReaderNode& LODNode)
{
	zeCheckError(!LODNode.IsValid(), false, "Invalid LOD node.");
	zeCheckError(LODNode.GetName() != "LOD", false, "Invalid LOD node name.");

	SetLevel(LODNode.ReadInt32("Level", LODNode.ReadInt32("LODLevel", 0)));
	SetStartDistance(LODNode.ReadInt32("StartDistance", LODNode.ReadInt32("LODStartDistance", GetLevel() * 30)));
	SetEndDistance(LODNode.ReadInt32("EndDistance", LODNode.ReadInt32("LODEndDistance", 100000)));
	SetMaterialId(LODNode.ReadInt32("MaterialId"));

	Vertices.SetCount(LODNode.ReadDataSize("Vertices") / sizeof(ZESkinnedModelVertex));
	if (!LODNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZESkinnedModelVertex), Vertices.GetCount()))
		return false;

	VerticesSkin.SetCount(LODNode.ReadDataSize("VerticesSkin") / sizeof(ZESkinnedModelVertex));
	if (VerticesSkin.GetCount() != 0)
	{
		if (!LODNode.ReadDataItems("VerticesSkin", VerticesSkin.GetCArray(), sizeof(ZESkinnedModelVertex), VerticesSkin.GetCount()))
			return false;
	}

	AffectingBoneIds.SetCount(LODNode.ReadDataSize("AffectingBoneIds") / sizeof(ZEUInt32));
	if (AffectingBoneIds.GetCount() != 0)
	{
		if (!LODNode.ReadDataItems("AffectingBoneIds", AffectingBoneIds.GetCArray(), sizeof(ZEUInt32), AffectingBoneIds.GetCount()))
			return false;
	}
	
	return true;
}

bool ZEModelResourceMeshLOD::Save(ZEMLWriterNode& LODNode) const
{
	return false;
}

ZEModelResourceMeshLOD::ZEModelResourceMeshLOD() : Link(this)
{
	Level = 0;
	StartDistance = 0;
	EndDistance = 1000000;
	VertexBuffer = NULL;
	VertexBufferSkin = NULL;
	IndexBuffer = NULL;
	MaterialId = -1;
	Material = NULL;
}

ZEModelResourceMeshLOD::~ZEModelResourceMeshLOD()
{
	if (VertexBuffer != NULL)
	{
		VertexBuffer->Release();
		VertexBuffer = NULL;
	}

	if (VertexBufferSkin != NULL)
	{
		VertexBufferSkin->Release();
		VertexBuffer = NULL;
	}

	if (IndexBuffer = NULL)
	{
		IndexBuffer->Release();
		IndexBuffer = NULL;
	}
}


// ZEMDResourceMesh
/////////////////////////////////////////////////////////////////////////////

void ZEModelResourceMesh::SetParentMesh(ZEInt32 ParentMesh)
{
	this->ParentMesh = ParentMesh;
}

ZEInt32 ZEModelResourceMesh::GetParentMesh() const 
{
	return ParentMesh;
}

void ZEModelResourceMesh::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEModelResourceMesh::GetName() const 
{
	return Name;
}

void ZEModelResourceMesh::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
}

const ZEAABBox& ZEModelResourceMesh::GetBoundingBox() const 
{
	return BoundingBox;
}

void ZEModelResourceMesh::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEModelResourceMesh::GetPosition() const 
{
	return Position;
}

void ZEModelResourceMesh::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEModelResourceMesh::GetRotation() const 
{
	return Rotation;
}

void ZEModelResourceMesh::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEModelResourceMesh::GetScale() const 
{
	return Scale;
}

void ZEModelResourceMesh::SetVisible(bool IsVisible)
{
	this->Visible = IsVisible;
}

bool ZEModelResourceMesh::GetVisible() const 
{
	return Visible;
}

void ZEModelResourceMesh::SetSkinned(bool IsSkinned)
{
	this->Skinned = IsSkinned;
}

bool ZEModelResourceMesh::GetSkinned() const 
{
	return Skinned;
}
void ZEModelResourceMesh::SetUserDefinedProperties(const ZEString& UserDefinedProperties)
{
	this->UserDefinedProperties = UserDefinedProperties;
}

const ZEString& ZEModelResourceMesh::GetUserDefinedProperties() const 
{
	return UserDefinedProperties;
}

const ZEList2<ZEModelResourceMeshLOD>& ZEModelResourceMesh::GetLODs()
{
	return LODs;
}

const ZEList2<const ZEModelResourceMeshLOD>& ZEModelResourceMesh::GetLODs() const
{
	return *(const ZEList2<const ZEModelResourceMeshLOD>*)&LODs;
}

void ZEModelResourceMesh::AddLOD(ZEModelResourceMeshLOD* LOD)
{
	LODs.AddEnd(&LOD->Link);
}

void ZEModelResourceMesh::RemoveLOD(ZEModelResourceMeshLOD* LOD)
{
	LODs.Remove(&LOD->Link);
}

bool ZEModelResourceMesh::Load(const ZEMLReaderNode& MeshNode)
{
	zeCheckError(!MeshNode.IsValid(), false, "Invalid Mesh node.");
	zeCheckError(MeshNode.GetName() != "Mesh", false, "Invalid Mesh node name.");

	SetParentMesh(MeshNode.ReadInt32("ParentMesh", -1));
	SetName(MeshNode.ReadString("Name"));
	SetPosition(MeshNode.ReadVector3("Position", ZEVector3::Zero));
	SetRotation(MeshNode.ReadQuaternion("Rotation", ZEQuaternion::Identity));
	SetScale(MeshNode.ReadVector3("Scale", ZEVector3::One));
	SetVisible(MeshNode.ReadBoolean("Visible", MeshNode.ReadBoolean("IsVisible", true)));
	SetSkinned(MeshNode.ReadBoolean("Skinned", MeshNode.ReadBoolean("IsSkinned", false)));
	SetUserDefinedProperties(MeshNode.ReadString("UserDefinedProperties"));
	ZEMLReaderNode BoundingBoxNode = MeshNode.GetNode("BoundingBox");
	ZEAABBox BoundingBox;
	BoundingBox.Max = BoundingBoxNode.ReadVector3("Max");
	BoundingBox.Min = BoundingBoxNode.ReadVector3("Min");
	SetBoundingBox(BoundingBox);

	ZEMLReaderNode LODsNode = MeshNode.GetNode("LODs");
	ZESize LODCount = LODsNode.GetNodeCount("LOD");
	for (ZESize I = 0; I < LODCount; I++)
	{
		ZEMLReaderNode LODNode = LODsNode.GetNode("LOD", I);
		if (!LODNode.IsValid())
			return false;
		
		ZEPointer<ZEModelResourceMeshLOD> LOD = new ZEModelResourceMeshLOD();
		if (!LOD->Load(LODNode))
			return false;

		AddLOD(LOD.Transfer());
	}

	return true;
}

bool ZEModelResourceMesh::Save(ZEMLWriterNode& MeshNode) const
{
	return false;
}

ZEModelResourceMesh::ZEModelResourceMesh() : Link(this)
{
	ParentMesh = -1;
	BoundingBox.Min = ZEVector3::Zero;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	Visible = true;
	Skinned = false;
}

ZEModelResourceMesh::~ZEModelResourceMesh()
{
	ze_for_each(LOD, LODs)
		delete &LOD.GetItem();
}
