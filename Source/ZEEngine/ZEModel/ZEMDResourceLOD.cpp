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

#include "ZEML\ZEMLReader.h"
#include "ZEGraphics\ZEGRVertexBuffer.h"
#include "ZEGraphics\ZEGRIndexBuffer.h"
#include "ZERenderer\ZERNMaterial.h"

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

void ZEModelResourceMeshLOD::SetVerticesSkin(const ZEArray<ZEMDVertexSkin>& VerticesSkin)
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

void ZEModelResourceMeshLOD::SetMaterialFileName(const ZEString& MaterialFilePath)
{
	//Implementation required.
}

const ZEString& ZEModelResourceMeshLOD::GetMaterialFileName()
{
	return ZEString::Empty; //Implementation required.
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

void ZEModelResourceMeshLOD::GenerateBuffers()
{
	if (GetVertexType() == ZEMD_VT_NORMAL)
		VertexBufferBase = ZEGRVertexBuffer::Create(Vertices.GetCount(), sizeof(ZEMDVertex), ZEGR_RU_GPU_READ_ONLY, Vertices.GetCArray());
	else
		VertexBufferBase = ZEGRVertexBuffer::Create(VerticesSkin.GetCount(), sizeof(ZEMDVertexSkin), ZEGR_RU_GPU_READ_ONLY, VerticesSkin.GetCArray());
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
	MaterialID = LODNode.ReadInt32("MaterialId", 0);

	ZEString MaterialFileName = LODNode.ReadString("MaterialFilePath");
	if (!MaterialFileName.IsEmpty())
		SetMaterialFileName(MaterialFileName);

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

	if (Draws.GetCount() == 0)
	{
		ZEMDResourceDraw Draw;
		if (GetVertexType() == ZEMD_VT_NORMAL)
			Draw.SetCount(Vertices.GetCount());
		else
			Draw.SetCount(VerticesSkin.GetCount());

		Draws.Add(Draw);
	}

	GenerateBuffers();

	return true;
}

bool ZEModelResourceMeshLOD::Save(ZEMLWriterNode& LODNode) const
{
	return false; //Implementation required.
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
