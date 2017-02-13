//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDResourceLOD.cpp
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

#include "ZEMDResourceLOD.h"

#include "ZEMDResource.h"
#include "ZEMDResourceMesh.h"

#include "ZEML\ZEMLReader.h"
#include "ZEGraphics\ZEGRBuffer.h"
#include "ZERenderer\ZERNMaterial.h"

ZEMDResourceLOD::ZEMDResourceLOD() : Link(this)
{
	ResourceMesh = NULL;
	Level = 0;
	StartDistance = 0;
	EndDistance = 1000000;
	VertexType = ZEMD_VT_NORMAL;
	VertexOffset = 0;
	VertexCount = 0;
	IndexType = ZEMD_VIT_NONE;
	IndexOffset = 0;
	IndexCount = 0;
}

ZEMDResourceLOD::~ZEMDResourceLOD()
{
	if (GetResourceMesh() != NULL)
		GetResourceMesh()->RemoveLOD(this);
}

ZEMDResource* ZEMDResourceLOD::GetResource()
{
	if (ResourceMesh == NULL)
		return NULL;
	else
		return ResourceMesh->GetResource();
}

const ZEMDResource* ZEMDResourceLOD::GetResource() const
{
	if (ResourceMesh == NULL)
		return NULL;
	else
		return ResourceMesh->GetResource();
}

ZEMDResourceMesh* ZEMDResourceLOD::GetResourceMesh()
{
	return ResourceMesh;
}

const ZEMDResourceMesh* ZEMDResourceLOD::GetResourceMesh() const
{
	return ResourceMesh;
}

void ZEMDResourceLOD::SetLevel(ZEInt32 LODLevel)
{
	this->Level = LODLevel;
}

ZEInt32 ZEMDResourceLOD::GetLevel() const 
{
	return Level;
}

void ZEMDResourceLOD::SetStartDistance(float StartDistance)
{
	this->StartDistance = StartDistance;
}

float ZEMDResourceLOD::GetStartDistance() const 
{
	return StartDistance;
}

void ZEMDResourceLOD::SetEndDistance(float EndDistance)
{
	this->EndDistance = EndDistance;
}

float ZEMDResourceLOD::GetEndDistance() const 
{
	return EndDistance;
}

void ZEMDResourceLOD::SetVertexType(ZEMDVertexType VertexType)
{
	this->VertexType = VertexType;
}

ZEMDVertexType ZEMDResourceLOD::GetVertexType() const
{
	return VertexType;
}

void ZEMDResourceLOD::SetVertexOffset(ZESize Offset)
{
	VertexOffset = Offset;
}

ZESize ZEMDResourceLOD::GetVertexOffset() const 
{
	return VertexOffset;
}

void ZEMDResourceLOD::SetVertexCount(ZESize Count)
{
	VertexCount = Count;
}

ZESize ZEMDResourceLOD::GetVertexCount() const 
{
	return VertexCount;
}

void ZEMDResourceLOD::SetIndexType(ZEMDVertexIndexType IndexType)
{
	this->IndexType = IndexType;
}

ZEMDVertexIndexType ZEMDResourceLOD::GetIndexType() const
{
	return IndexType;
}

void ZEMDResourceLOD::SetIndexOffset(ZESize Offset)
{
	IndexOffset = Offset;
}

ZESize ZEMDResourceLOD::GetIndexOffset() const
{
	return IndexOffset;
}

void ZEMDResourceLOD::SetIndexCount(ZESize Count)
{
	IndexCount = Count;
}

ZESize ZEMDResourceLOD::GetIndexCount() const
{
	return IndexCount;
}

const ZEArray<ZEMDResourceDraw>& ZEMDResourceLOD::GetDraws() const
{
	return Draws;
}

void ZEMDResourceLOD::AddDraw(const ZEMDResourceDraw& Draw)
{
	Draws.Add(Draw);
	Draws.GetLastItem().LOD = this;
}

void ZEMDResourceLOD::RemoveDraw(ZESize Index)
{
	Draws.Remove(Index);
}

bool ZEMDResourceLOD::Unserialize(const ZEMLReaderNode& LODNode)
{
	zeCheckError(!LODNode.IsValid(), false, "Invalid LOD node.");
	zeCheckError(LODNode.GetName() != "LOD", false, "Invalid LOD node name.");

	ZEArray<ZEMDVertex>& Vertices = GetResource()->Vertices;
	ZEArray<ZEMDVertexSkin>& VerticesSkin = GetResource()->VerticesSkin;
	ZEArray<ZEUInt16>& Indices = GetResource()->Indices;
	ZEArray<ZEUInt32>& Indices32 = GetResource()->Indices32;

	SetLevel(LODNode.ReadInt32("Level", LODNode.ReadInt32("LODLevel", 0)));
	SetStartDistance(LODNode.ReadFloat("StartDistance", (float)LODNode.ReadInt32("LODStartDistance", GetLevel() * 30)));
	SetEndDistance(LODNode.ReadFloat("EndDistance", (float)LODNode.ReadInt32("LODEndDistance", 100000)));
	SetVertexType((ZEMDVertexType)LODNode.ReadUInt8("VertexType", ZEMD_VT_NORMAL));
	SetIndexType((ZEMDVertexIndexType)(LODNode.ReadUInt8("IndexType", ZEMD_VIT_NONE) / 16));

	if (GetVertexType() == ZEMD_VT_NORMAL)
	{
		VertexOffset = Vertices.GetCount();
		VertexCount = LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertex);
		Vertices.Resize(VertexOffset + VertexCount);
		if (!LODNode.ReadDataItems("Vertices", Vertices.GetCArray() + VertexOffset, sizeof(ZEMDVertex), VertexCount))
			return false;

	}
	else if (GetVertexType() == ZEMD_VT_SKINNED)
	{
		VertexOffset = VerticesSkin.GetCount();
		VertexCount = LODNode.ReadDataSize("Vertices") / sizeof(ZEMDVertexSkin);
		VerticesSkin.Resize(VertexOffset + VertexCount);
		if (!LODNode.ReadDataItems("Vertices", VerticesSkin.GetCArray() + VertexOffset, sizeof(ZEMDVertexSkin), VertexCount))
			return false;
	}

	if (GetIndexType() == ZEMD_VIT_16BIT)
	{
		IndexOffset = Indices.GetCount();
		IndexCount = LODNode.ReadDataSize("Indices") / sizeof(ZEUInt16);
		Indices.Resize(IndexOffset + IndexCount);
		if (!LODNode.ReadDataItems("Indices", Indices.GetCArray() + IndexOffset, sizeof(ZEUInt16), IndexCount))
			return false;
	}
	else if (GetIndexType() == ZEMD_VIT_32BIT)
	{
		IndexOffset = Indices32.GetCount();
		IndexCount = LODNode.ReadDataSize("Indices") / sizeof(ZEUInt32);
		Indices32.Resize(IndexOffset + IndexCount);
		if (!LODNode.ReadDataItems("Indices", Indices32.GetCArray() + IndexOffset, sizeof(ZEUInt32), IndexCount))
			return false;
	}

	ZEMLReaderNode DrawsNode = LODNode.GetNode("Draws");
	if (DrawsNode.IsValid())
	{
		ZESize DrawsNodeCount = LODNode.GetNodeCount("Draws");
		Draws.SetCount(DrawsNodeCount);
		for (ZESize I = 0; I < DrawsNodeCount; I++)
		{
			Draws[I].LOD = this;
			if (!Draws[I].Unserialize(DrawsNode.GetNode("Draw", I)))
				return false;
		}
	}

	return true;
}

bool ZEMDResourceLOD::Serialize(ZEMLWriterNode& LODNode) const
{
	return false; //Implementation required.
}

ZEMDResourceLOD* ZEMDResourceLOD::CreateInstance()
{
	return new ZEMDResourceLOD();
}
