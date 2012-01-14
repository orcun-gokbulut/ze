//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENavigationMesh.cpp
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

#include "ZENavigationMesh.h"

ZEPolygon ZENavigationMesh::ToPolygon(size_t Index)
{
	ZEPolygon Output;
	Output.Vertices.SetCount(Polygons[Index].VertexIndices.GetCount());

	for (size_t I = 0; I < Output.Vertices.GetCount(); I++)
		Output.Vertices[I] = Vertices[Polygons[Index].VertexIndices[I]];

	return Output;
}

static ZEAABBox GetBoundingBox(const ZEArray<ZEPolygon>& Input)
{
	ZEAABBox TotalBox;
	const ZEPolygon& Polygon = Input[0];
	ZEAABBox::Generate(TotalBox, Polygon.Vertices.GetConstCArray(), Polygon.Vertices.GetCount());

	for(int I = 0; I < Input.GetCount(); I++)
	{
		ZEAABBox Box;
		const ZEPolygon& Polygon = Input[I];
		ZEAABBox::Generate(Box, Polygon.Vertices.GetConstCArray(), Polygon.Vertices.GetCount());

		ZEAABBox::Combine(TotalBox, TotalBox, Box);
	}

	return TotalBox;
}

static size_t GetNormalIndex(ZENavigationMesh& Mesh, const ZEPolygon& Polygon)
{
	ZEVector3 PolygonNormal = Polygon.GetNormal();

	for (int N = 0; N < Mesh.Normals.GetCount(); N++)
		if (Mesh.Normals[N] == PolygonNormal)
			return N;

	Mesh.Normals.Add(PolygonNormal);
	return Mesh.Normals.GetCount() - 1;
}

static int GetVertexIndexParents(ZENavigationMesh& Mesh, const ZEVector3& Point, ZENavigationMeshOctree* CurrentNode)
{
	while(CurrentNode != NULL)
	{
		for (size_t I = 0; I < CurrentNode->GetItemCount(); I++)
		{
			ZEArray<size_t>& VertexIndices = CurrentNode->GetItem(I).VertexIndices;
			for (size_t N = 0; N < VertexIndices.GetCount(); N++)
				if (Point == Mesh.Vertices[VertexIndices[N]])
					return VertexIndices[N];
		}

		CurrentNode = CurrentNode->GetParent();
	}

	return -1;
}

static int GetVertexIndexChilds(ZENavigationMesh& Mesh, const ZEVector3& Point, ZENavigationMeshOctree* Octree)
{
	for (size_t I = 0; I < Octree->GetItemCount(); I++)
	{
		ZEArray<size_t>& VertexIndices = Octree->GetItem(I).VertexIndices;
		for (size_t N = 0; N < VertexIndices.GetCount(); N++)
			if (Point == Mesh.Vertices[VertexIndices[N]])
				return VertexIndices[N];
	}

	for (size_t I = 0; I < 8; I++)
		if (Octree->GetNode(I) != NULL)
		{
			int Result = GetVertexIndexChilds(Mesh, Point, Octree->GetNode(I));
			if (Result != -1)
				return Result;
		}

	return -1;
}


static size_t GetVertexIndex(ZENavigationMesh& Mesh, const ZEVector3& Point, ZENavigationMeshOctree* Octree, size_t& VertexIndex)
{
	if (Octree != NULL)
	{
		ZENavigationMeshOctree* CurrentNode = Octree->GetNode(Point);
		if (CurrentNode != NULL)
		{
			int Result = GetVertexIndexChilds(Mesh, Point, CurrentNode);
			if (Result != -1)
				return Result;

			Result = GetVertexIndexParents(Mesh, Point, CurrentNode->GetParent());
			if (Result != -1)
				return Result;
		}
	}

	Mesh.Vertices[VertexIndex] = Point;
	VertexIndex++;
	return VertexIndex - 1;
}

void ZENavigationMesh::Clear()
{
	Vertices.Clear();
	Normals.Clear();
	Octree.Clear();
}

void ZENavigationMesh::Generate(const ZEArray<ZEPolygon>& Input)
{
	Octree.SetBoundingBox(GetBoundingBox(Input));
	Octree.SetMaxDepth(3);

	size_t VertexCount = 0;
	size_t VertexIndex = 0;
	for(int I = 0; I < Input.GetCount(); I++)
		VertexCount += Input[I].Vertices.GetCount();

	Vertices.SetCount(VertexCount);

	for(int I = 0; I < Input.GetCount(); I++)
	{
		const ZEPolygon& Polygon = Input[I];
		ZENavigationMeshPolygon MeshPolygon;

		MeshPolygon.Deleted = false;
		MeshPolygon.NormalIndex = GetNormalIndex(*this, Polygon);

		ZEAABBox Box;
		ZEAABBox::Generate(Box, Polygon.Vertices.GetConstCArray(), Polygon.Vertices.GetCount());
		ZENavigationMeshOctree* Node = Octree.GetNode(Box);

		for (size_t N = 0; N < Polygon.Vertices.GetCount(); N++)
			MeshPolygon.VertexIndices.Add(GetVertexIndex(*this, Polygon.Vertices[N], Node, VertexIndex));

		Octree.AddItem(MeshPolygon, Box);
	}
	Vertices.Resize(VertexIndex);
}
