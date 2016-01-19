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
#include "ZEGraphics\ZEGRVertexBuffer.h"
#include "ZEGraphics\ZEGRIndexBuffer.h"


// ZEMDResourceDraw
/////////////////////////////////////////////////////////////////////////////

void ZEMDResourceDraw::SetOffset(ZESize PoligonOffset)
{
	this->Offset = PoligonOffset;
}

ZESize ZEMDResourceDraw::GetOffset() const 
{
	return Offset;
}

void ZEMDResourceDraw::SetCount(ZESize PoligonCount)
{
	this->Count = PoligonCount;
}

ZESize ZEMDResourceDraw::GetCount() const 
{
	return Count;
}

void ZEMDResourceDraw::SetMaterial(ZEHolder<ZERNMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<ZERNMaterial> ZEMDResourceDraw::GetMaterial() const 
{
	return Material;
}

bool ZEMDResourceDraw::Load(ZEMLReaderNode& DrawNode)
{
	zeCheckError(!DrawNode.IsValid(), false, "Invalid Draw node.");
	zeCheckError(DrawNode.GetName() != "Draw", false, "Invalid Draw node name.");

	SetOffset(DrawNode.ReadUInt32("Offset", 0));
	SetCount(DrawNode.ReadUInt32("Count", 0));

	return true;
}

bool ZEMDResourceDraw::Save(ZEMLWriterNode& DrawNode) const
{
	return false;
}

ZEMDResourceDraw::ZEMDResourceDraw()
{
	Offset = 0;
	Count = 0;
}


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

void ZEModelResourceMeshLOD::SetStartDistance(float StartDistance)
{
	this->StartDistance = StartDistance;
}

float ZEModelResourceMeshLOD::GetStartDistance() const 
{
	return StartDistance;
}

void ZEModelResourceMeshLOD::SetEndDistance(float EndDistance)
{
	this->EndDistance = EndDistance;
}

float ZEModelResourceMeshLOD::GetEndDistance() const 
{
	return EndDistance;
}

void ZEModelResourceMeshLOD::SetVertexType(ZEMDVertexType VertexType)
{
	this->VertexType = VertexType;
}

ZEMDVertexType ZEModelResourceMeshLOD::GetVertexType() const
{
	return VertexType;
}

void ZEModelResourceMeshLOD::SetIndexType(ZEMDVertexIndexType IndexType)
{
	this->IndexType = IndexType;
}

ZEMDVertexIndexType ZEModelResourceMeshLOD::GetIndexType() const
{
	return IndexType;
}

void ZEModelResourceMeshLOD::SetVertices(const ZEArray<ZEMDVertex>& Vertices)
{
	this->Vertices = Vertices;
	this->VerticesSkin.Clear();
}

const ZEArray<ZEMDVertex>& ZEModelResourceMeshLOD::GetVertices() const
{
	return Vertices;
}

void ZEModelResourceMeshLOD::SetVerticesSkin(const ZEArray<ZEMDVertexSkin>& Vertices)
{
	this->VerticesSkin = VerticesSkin;
	this->Vertices.Clear();
}

const ZEArray<ZEMDVertexSkin>& ZEModelResourceMeshLOD::GetVerticesSkin() const
{
	return VerticesSkin;
}

void ZEModelResourceMeshLOD::SetIndices(const ZEArray<ZEUInt16>& Indices)
{
	this->Indices = Indices;
	this->Indices32.Clear();
}

const ZEArray<ZEUInt16>& ZEModelResourceMeshLOD::GetIndices() const
{
	return Indices;
}

void ZEModelResourceMeshLOD::SetIndices32(const ZEArray<ZEUInt32>& Indices)
{
	this->Indices32 = Indices;
	this->Indices.Clear();
}

const ZEArray<ZEUInt32>& ZEModelResourceMeshLOD::GetIndices32() const
{
	return Indices32;
}

void ZEModelResourceMeshLOD::SetAffectingBoneIds(const ZEArray<ZEUInt16>& BoneIds)
{
	AffectingBoneIds = BoneIds;
}

const ZEArray<ZEUInt16>& ZEModelResourceMeshLOD::GetAffectingBoneIds() const
{
	return AffectingBoneIds;
}

void ZEModelResourceMeshLOD::SetVertexBufferBase(ZEHolder<ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferBase = VertexBuffer;
}

ZEHolder<ZEGRVertexBuffer> ZEModelResourceMeshLOD::GetVertexBufferBase() const
{
	return VertexBufferBase;
}

void ZEModelResourceMeshLOD::SetVertexBufferNormals(ZEHolder<ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferNormals = VertexBuffer;
}

ZEHolder<ZEGRVertexBuffer> ZEModelResourceMeshLOD::GetVertexBufferNormals() const
{
	return VertexBufferNormals;
}

void ZEModelResourceMeshLOD::SetVertexBufferSkin(ZEHolder<ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferSkin = VertexBuffer;
}

ZEHolder<ZEGRVertexBuffer> ZEModelResourceMeshLOD::GetVertexBufferSkin() const
{
	return VertexBufferSkin;
}

void ZEModelResourceMeshLOD::SetVertexBufferExtra(ZEHolder<ZEGRVertexBuffer> VertexBuffer)
{
	this->VertexBufferExtra = VertexBuffer;
}

ZEHolder<ZEGRVertexBuffer> ZEModelResourceMeshLOD::GetVertexBufferExtra() const
{
	return VertexBufferExtra;
}

void ZEModelResourceMeshLOD::SetIndexBuffer(ZEHolder<ZEGRIndexBuffer> IndexBuffer)
{
	this->IndexBuffer = IndexBuffer;
}

ZEHolder<ZEGRIndexBuffer> ZEModelResourceMeshLOD::GetIndexBuffer() const
{
	return IndexBuffer;
}

void ZEModelResourceMeshLOD::SetMaterial(ZEHolder<ZERNMaterial> Material)
{
	this->Material = Material;
}

ZEHolder<const ZERNMaterial> ZEModelResourceMeshLOD::GetMaterial() const
{
	return Material;
}

const ZEArray<ZEMDResourceDraw>& ZEModelResourceMeshLOD::GetDraws() const
{
	return Draws;
}

void ZEModelResourceMeshLOD::AddDraw(const ZEMDResourceDraw& Draw)
{
	Draws.Add(Draw);
}

void ZEModelResourceMeshLOD::RemoveDraw(ZESize Index)
{
	Draws.Remove(Index);
}

bool ZEModelResourceMeshLOD::Load(const ZEMLReaderNode& LODNode)
{
	zeCheckError(!LODNode.IsValid(), false, "Invalid LOD node.");
	zeCheckError(LODNode.GetName() != "LOD", false, "Invalid LOD node name.");

	Vertices.Clear();
	VerticesSkin.Clear();
	Indices.Clear();
	AffectingBoneIds.Clear();

	IndexBuffer = NULL;
	VertexBufferBase = NULL;
	VertexBufferNormals = NULL;
	VertexBufferSkin = NULL;
	VertexBufferExtra = NULL;
	Material = NULL;

	SetLevel(LODNode.ReadInt32("Level", LODNode.ReadInt32("LODLevel", 0)));
	SetStartDistance(LODNode.ReadFloat("StartDistance", (float)LODNode.ReadInt32("LODStartDistance", GetLevel() * 30)));
	SetEndDistance(LODNode.ReadFloat("EndDistance", (float)LODNode.ReadInt32("LODEndDistance", 100000)));
	SetVertexType((ZEMDVertexType)LODNode.ReadInt32("VertexType", ZEMD_VT_NORMAL));
	SetIndexType((ZEMDVertexIndexType)LODNode.ReadInt32("IndexType", ZEMD_VIT_NONE));

	ZEString MaterialFilePath = LODNode.ReadString("MaterialFilePath");
	if (!MaterialFilePath.IsEmpty())
		SetMaterialFilePath(MaterialFilePath);

	if (GetVertexType() == ZEMD_VT_NORMAL)
	{
		Vertices.SetCount(LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertex));
		if (!LODNode.ReadDataItems("Vertices", Vertices.GetCArray(), sizeof(ZEMDVertex), Vertices.GetCount()))
			return false;
	}
	else if (GetVertexType() == ZEMD_VT_SKINNED)
	{
		VerticesSkin.SetCount(LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexSkin));
		if (!LODNode.ReadDataItems("Vertices", VerticesSkin.GetCArray(), sizeof(ZEMDVertexSkin), VerticesSkin.GetCount()))
			return false;

		AffectingBoneIds.SetCount(LODNode.ReadDataSize("AffectingBoneIds") / sizeof(ZEUInt32));
		if (!LODNode.ReadDataItems("AffectingBoneIds", AffectingBoneIds.GetCArray(), sizeof(ZEUInt32), AffectingBoneIds.GetCount()))
			return false;

	}

	if (GetIndexType() == ZEMD_VIT_16BIT)
	{
		Indices.SetCount(LODNode.ReadDataSize("Indices") / sizeof(ZEUInt16));
		if (!LODNode.ReadDataItems("Indices", Indices.GetCArray(), sizeof(ZEUInt16), Indices.GetCount()))
			return false;
	}
	else if (GetIndexType() == ZEMD_VIT_32BIT)
	{
		Indices32.SetCount(LODNode.ReadDataSize("Indices32") / sizeof(ZEUInt32));
		if (!LODNode.ReadDataItems("Indices32", Indices32.GetCArray(), sizeof(ZEUInt32), Indices32.GetCount()))
			return false;
	}

	ZEMLReaderNode DrawsNode = LODNode.GetNode("Draws");
	if (DrawsNode.IsValid())
	{
		ZESize DrawsNodeCount = LODNode.GetNodeCount("Draws");
		for (ZESize I = 0; I < DrawsNodeCount; I++)
		{
			ZEMLReaderNode DrawNode = DrawsNode.GetNode("Draw", I);
			
			ZEMDResourceDraw Draw;
			if (!Draw.Load(DrawNode))
				return false;
			
			Draws.Add(Draw);
		}
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
	MaterialID = -1;
	VertexType = ZEMD_VT_NORMAL;
	IndexType = ZEMD_VIT_NONE;
}

ZEModelResourceMeshLOD::~ZEModelResourceMeshLOD()
{

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
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	BoundingBox = ZEAABBox::Zero;
	Visible = true;
	Skinned = false;
}

ZEModelResourceMesh::~ZEModelResourceMesh()
{
	ze_for_each(LOD, LODs)
		delete &LOD.GetItem();
}
