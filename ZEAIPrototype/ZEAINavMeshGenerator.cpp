//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAINavMeshGenerator.cpp
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

#include "ZEAINavMeshGenerator.h"
#include "ZEOctree.h"
#include "ZEMath/ZEAngle.h"

static void RemoveUnwalkable(ZEArray<ZEPolygon>& Output, const ZEArray<ZEPolygon>& Input, const ZEVector3& Up, float MaxDegree)
{
	float CosMaxDegree = cosf(MaxDegree);
	size_t Count = 0;
	for (size_t I = 0; I < Input.GetCount(); I++)
		if (ZEVector3::DotProduct(Input[I].GetNormal(), Up) > CosMaxDegree)
			Count++;

	Output.MassAdd(Count);
	size_t Index = 0;
	for (size_t I = 0; I < Input.GetCount(); I++)
		if (ZEVector3::DotProduct(Input[I].GetNormal(), Up) > CosMaxDegree)
		{
			Output[Index] = Input[I];
			Index++;
		}
}

static bool CheckStraight(const ZEVector3& A, const ZEVector3& B, const ZEVector3& C)
{
	return ZEVector3::DotProduct((B - A).Normalize(), (C - B).Normalize()) > 0.9999;
}

static bool CheckInternalAngle(const ZEVector3& Center, const ZEVector3& Normal, const ZEVector3& A, const ZEVector3& B)
{
	if (CheckStraight(A, Center, B))
		return true;

	ZEVector3 V1 = A - Center;
	ZEVector3 V2 = B - Center; 
	V1.NormalizeSelf();
	V2.NormalizeSelf();
	float Angle = ZEVector3::DotProduct(V1, V2);
	ZEVector3 V1CV2;
	ZEVector3::CrossProduct(V1CV2, V2, V1);
	V1CV2.NormalizeSelf();
	if (ZEVector3::DotProduct(Normal, V1CV2) >= 0.0f /*|| Angle < 0.0001f*/)
		return true;
	else
		return false;
}

static ZEVector3& GetVertex(const ZENavigationMeshPolygon& Polygon, int Index, ZENavigationMesh& Mesh)
{
	return Mesh.Vertices[Polygon.VertexIndices[Polygon.VertexIndices.Circular(Index)]];
}

static size_t GetVertexIndex(const ZENavigationMeshPolygon& Polygon, int Index)
{
	return Polygon.VertexIndices[Polygon.VertexIndices.Circular(Index)];
}

int Merge_TryCount = 0;
int Merge_Normal_Reject = 0;
int Merge_SharedEdgeFound = 0;
int Merge_ConvexTestFail = 0;
int Merge_Done = 0;

static bool MergePolygons(ZENavigationMeshPolygon& Output, const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B, ZENavigationMesh& Mesh)
{
	if (&A == &B)
		return false;

	Merge_TryCount++;

	if (ZEVector3::DotProduct(Mesh.Normals[A.NormalIndex], Mesh.Normals[B.NormalIndex]) < 0.9999f)
	{
		Merge_Normal_Reject++;
		return false;
	}

	for (int I = 0; I < A.VertexIndices.GetCount(); I++)
	{
		for (int N = 0; N < B.VertexIndices.GetCount(); N++)
		{
			if (GetVertex(A, I, Mesh) == GetVertex(B, N, Mesh) && 
				GetVertex(A, I + 1, Mesh) == GetVertex(B, N - 1, Mesh))
			{
				Merge_SharedEdgeFound++;
				if (CheckInternalAngle(GetVertex(A, I, Mesh), Mesh.Normals[A.NormalIndex], GetVertex(A, I - 1, Mesh), GetVertex(B, N + 1, Mesh)) &&
					CheckInternalAngle(GetVertex(A, I + 1, Mesh), Mesh.Normals[A.NormalIndex], GetVertex(B, N - 2, Mesh), GetVertex(A, I + 2, Mesh)))
				{
					Output.VertexIndices.MassAdd(A.VertexIndices.GetCount() + B.VertexIndices.GetCount() - 2);
					Output.NormalIndex = A.NormalIndex;

					size_t Index = 0;

					for (int K = 0; K < B.VertexIndices.GetCount(); K++)
					{
						Output.VertexIndices[Index] = GetVertexIndex(B, N + K);
						Index++;
					}

					for (int K = 0; K < A.VertexIndices.GetCount() - 2; K++)
					{
						Output.VertexIndices[Index] = GetVertexIndex(A, I + 2 + K);
						Index++;
					}

					for (int K = 0; K < Output.VertexIndices.GetCount(); K++)
						if (GetVertex(Output, K - 1, Mesh) == GetVertex(Output, K, Mesh) || CheckStraight(GetVertex(Output, K - 1, Mesh), GetVertex(Output, K, Mesh), GetVertex(Output, K + 1, Mesh)))
						{
							Output.VertexIndices.DeleteAt(K);
							K--;
						}
						
					return true;
				}
				else
					Merge_ConvexTestFail++;

			}
		}
	}
	return false;
}

static void GetBoundingBox(ZEAABBox& Output, ZENavigationMeshPolygon& Polygon, ZENavigationMesh& Mesh)
{
	ZEAABBox Box;
	Output.Max = Output.Min = Mesh.Vertices[Polygon.VertexIndices[0]];
	for (size_t I = 1; I < Polygon.VertexIndices.GetCount(); I++)
	{
		ZEVector3& Vertex = Mesh.Vertices[Polygon.VertexIndices[I]];

		if (Output.Min.x > Vertex.x)
			Output.Min.x = Vertex.x;

		if (Output.Max.x < Vertex.x)
			Output.Max.x = Vertex.x;

		if (Output.Min.y > Vertex.y)
			Output.Min.y = Vertex.y;

		if (Output.Max.y < Vertex.y)
			Output.Max.y = Vertex.y;

		if (Output.Min.z > Vertex.z)
			Output.Min.z = Vertex.z;

		if (Output.Max.z < Vertex.z)
			Output.Max.z = Vertex.z;
	}
}

ZENavigationMeshOctree* Main;

static bool ScanOctreeChilds(ZENavigationMeshPolygon& Polygon, ZENavigationMesh& Mesh, ZENavigationMeshOctree* Octree)
{
	for (size_t I = 0; I < Octree->GetItemCount(); I++)
	{
		ZENavigationMeshPolygon Output;
		if (MergePolygons(Output, Polygon, Octree->GetItem(I), Mesh))
		{
			Octree->RemoveItem(I);

			ZEAABBox Box;
			GetBoundingBox(Box, Output, Mesh);
			Main->AddItem(Output, Box);

			return true;
		}
	}

	for (size_t I = 0; I < 8; I++)
	{
		if (Octree->GetNode(I) != NULL)
		{
			if (ScanOctreeChilds(Polygon, Mesh, Octree->GetNode(I)))
				return true;
		}
	}

	return false;
}

static bool ScanOctreeParents(ZENavigationMeshPolygon& Polygon, ZENavigationMesh& Mesh, ZENavigationMeshOctree* Octree)
{
	while(Octree != NULL)
	{
		for (size_t I = 0; I < Octree->GetItemCount(); I++)
		{
			ZENavigationMeshPolygon Output;
			if (MergePolygons(Output, Polygon, Octree->GetItem(I), Mesh))
			{
				Octree->RemoveItem(I);

				ZEAABBox Box;
				GetBoundingBox(Box, Output, Mesh);
				Main->AddItem(Output, Box);

				return true;
			}
		}

		Octree = Octree->GetParent();
	}
	
	return false;
}

static bool ScanOctree(ZENavigationMeshPolygon& Polygon, ZENavigationMesh& Mesh)
{
	ZEAABBox Box;
	GetBoundingBox(Box, Polygon, Mesh);
	ZENavigationMeshOctree* Octree = Mesh.Octree.GetNode(Box);

	if (ScanOctreeChilds(Polygon, Mesh, Octree) || ScanOctreeParents(Polygon, Mesh, Octree))
		return true;

	return false;
}

size_t ZENavigationMeshGenerator::OptimizePolygons(ZENavigationMeshOctree* CurrentNode, ZENavigationMesh& Mesh)
{
	size_t CulledPolygonCount = 0;
	for (size_t I = 0; I < CurrentNode->GetItemCount(); I++)
		if (ScanOctree(CurrentNode->GetItem(I), Mesh))
		{
			CulledPolygonCount++;
			CurrentNode->RemoveItem(I);
			I--;
		}

	for (int I = 0; I < 8; I++)
		if (CurrentNode->GetNodes()[I] != NULL)
			CulledPolygonCount += OptimizePolygons(CurrentNode->GetNodes()[I], Mesh);

	return CulledPolygonCount;
}


bool ZENavigationMeshGenerator::OptimizePolygonsStep(ZENavigationMeshOctree* CurrentNode, ZENavigationMesh& Mesh)
{
	for (size_t I = 0; I < CurrentNode->GetItemCount(); I++)
		if (ScanOctree(CurrentNode->GetItem(I), Mesh))
		{
			CurrentNode->RemoveItem(I);
			return true;
		}

		for (int I = 0; I < 8; I++)
			if (CurrentNode->GetNodes()[I] != NULL)
				if (OptimizePolygonsStep(CurrentNode->GetNodes()[I], Mesh))
					return true;

		return false;
}

void ZENavigationMeshGenerator::Generate(ZENavigationMesh& Output, const ZEArray<ZEPolygon>& Input, ZENavigationMeshGeneratorOptions* Options)
{
	ZEArray<ZEPolygon> Temp;
	RemoveUnwalkable(Temp, Input, Options->UpDirection, Options->MaxWalkableSlope);
	Output.Generate(Temp);

	Main = &Output.Octree;
	for (int I = 0; I < Options->MaxConvexSimplicationPass; I++)
	{
		Merge_TryCount = 0;
		Merge_Normal_Reject = 0;
		Merge_SharedEdgeFound = 0;
		Merge_ConvexTestFail = 0;
		Merge_Done = 0;

		/*size_t CulledPolygonCount = OptimizePolygons(&Output.Octree, Output, CulledPolygonCount);
		if (CulledPolygonCount == 0)
			break;*/
	}

	Output.Octree.Shrink();
}
