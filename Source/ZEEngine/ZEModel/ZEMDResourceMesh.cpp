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
#include "ZERenderer\ZERNMaterial.h"
#include "ZEMDResourceLOD.h"


void ZEMDResourceMesh::SetParentMesh(ZEInt32 ParentMesh)
{
	this->ParentMesh = ParentMesh;
}

ZEInt32 ZEMDResourceMesh::GetParentMesh() const 
{
	return ParentMesh;
}

void ZEMDResourceMesh::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEMDResourceMesh::GetName() const 
{
	return Name;
}

void ZEMDResourceMesh::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	this->BoundingBox = BoundingBox;
}

const ZEAABBox& ZEMDResourceMesh::GetBoundingBox() const 
{
	return BoundingBox;
}

void ZEMDResourceMesh::SetPosition(const ZEVector3& Position)
{
	this->Position = Position;
}

const ZEVector3& ZEMDResourceMesh::GetPosition() const 
{
	return Position;
}

void ZEMDResourceMesh::SetRotation(const ZEQuaternion& Rotation)
{
	this->Rotation = Rotation;
}

const ZEQuaternion& ZEMDResourceMesh::GetRotation() const 
{
	return Rotation;
}

void ZEMDResourceMesh::SetScale(const ZEVector3& Scale)
{
	this->Scale = Scale;
}

const ZEVector3& ZEMDResourceMesh::GetScale() const 
{
	return Scale;
}

void ZEMDResourceMesh::SetVisible(bool IsVisible)
{
	this->Visible = IsVisible;
}

bool ZEMDResourceMesh::GetVisible() const 
{
	return Visible;
}

void ZEMDResourceMesh::SetSkinned(bool IsSkinned)
{
	this->Skinned = IsSkinned;
}

bool ZEMDResourceMesh::GetSkinned() const 
{
	return Skinned;
}
void ZEMDResourceMesh::SetUserDefinedProperties(const ZEString& UserDefinedProperties)
{
	this->UserDefinedProperties = UserDefinedProperties;
}

const ZEString& ZEMDResourceMesh::GetUserDefinedProperties() const 
{
	return UserDefinedProperties;
}

const ZEList2<ZEMDResourceLOD>& ZEMDResourceMesh::GetLODs()
{
	return LODs;
}

const ZEList2<const ZEMDResourceLOD>& ZEMDResourceMesh::GetLODs() const
{
	return *(const ZEList2<const ZEMDResourceLOD>*)&LODs;
}

void ZEMDResourceMesh::AddLOD(ZEMDResourceLOD* LOD)
{
	zeCheckError(LOD->Link.GetInUse(), ZE_VOID, "LOD is already added to Mesh.");
	LODs.AddEnd(&LOD->Link);
}

void ZEMDResourceMesh::RemoveLOD(ZEMDResourceLOD* LOD)
{
	LODs.Remove(&LOD->Link);
}

void ZEMDResourceMesh::SetGeometry(const ZEArray<ZEVector3>& Vertices)
{
	Geometry = Vertices;
}

bool ZEMDResourceMesh::Load(const ZEMLReaderNode& MeshNode)
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
		
		ZEPointer<ZEMDResourceLOD> LOD = new ZEMDResourceLOD();
		if (!LOD->Load(LODNode))
			return false;

		if (LOD->GetLevel() == 0)
		{
			if (LOD->GetVertexType() == ZEMD_VT_SKINNED)
			{
				Geometry.SetCount(LOD->GetVerticesSkin().GetCount());
				for (ZESize N = 0; N < Geometry.GetCount(); N++)
					Geometry[N] = LOD->GetVerticesSkin()[N].Position;
			}
			else
			{
				Geometry.SetCount(LOD->GetVertices().GetCount());
				for (ZESize N = 0; N < Geometry.GetCount(); N++)
					Geometry[N] = LOD->GetVertices()[N].Position;
			}
		}

		AddLOD(LOD.Transfer());
	}

	return true;
}

bool ZEMDResourceMesh::Save(ZEMLWriterNode& MeshNode) const
{
	return false;
}

ZEMDResourceMesh::ZEMDResourceMesh() : Link(this)
{
	ParentMesh = -1;
	Position = ZEVector3::Zero;
	Rotation = ZEQuaternion::Identity;
	Scale = ZEVector3::One;
	BoundingBox = ZEAABBox::Zero;
	Visible = true;
	Skinned = false;
}

ZEMDResourceMesh::~ZEMDResourceMesh()
{
	ze_for_each(LOD, LODs)
		delete &LOD.GetItem();
}

const ZEArray<ZEVector3>& ZEMDResourceMesh::GetGeometry() const
{
	return Geometry;
}
