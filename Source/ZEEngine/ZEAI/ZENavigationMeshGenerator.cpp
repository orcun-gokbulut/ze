//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENavigationMeshGenerator.cpp
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

#include "ZENavigationMeshGenerator.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZESpatial/ZEOctree.h"

struct ZECheckAdjacentResult
{
	bool Swap;
	ZESize Vertex1Index;
	ZESize Vertex2Index;
};

ZESize ZENavigationMeshGenerator::AddVertex(const ZEVector3& Point)
{
	for (ZESize I = 0; I < Mesh.Vertices.GetCount(); I++)
		if (Mesh.Vertices[I] == Point)
			return I;

	Mesh.Vertices.Add(Point);
	return Mesh.Vertices.GetCount() - 1;
}

ZEVector3& ZENavigationMeshGenerator::GetVertex(const ZENavigationMeshPolygon& Polygon, ZESSize Index)
{
	return Mesh.Vertices[Polygon.VertexIndices[Polygon.VertexIndices.Circular(Index)]];
}

ZESize ZENavigationMeshGenerator::GetVertexIndex(const ZENavigationMeshPolygon& Polygon, ZESSize Index)
{
	return Polygon.VertexIndices[Polygon.VertexIndices.Circular(Index)];
}

ZEVector3& ZENavigationMeshGenerator::GetNormal(const ZENavigationMeshPolygon& Polygon)
{
	return Mesh.Normals[Polygon.NormalIndex];
}

void ZENavigationMeshGenerator::GetBoundingBox(ZEAABBox& Output, ZENavigationMeshPolygon& Polygon)
{
	ZEAABBox Box;
	Output.Max = Output.Min = Mesh.Vertices[Polygon.VertexIndices[0]];
	for (ZESize I = 1; I < Polygon.VertexIndices.GetCount(); I++)
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

bool ZENavigationMeshGenerator::CheckStraight(const ZEVector3& A, const ZEVector3& B, const ZEVector3& C)
{
	return ZEVector3::DotProduct((B - A).Normalize(), (C - B).Normalize()) > 0.9999;
}

bool ZENavigationMeshGenerator::CheckInternalAngle(const ZEVector3& Center, const ZEVector3& Normal, const ZEVector3& A, const ZEVector3& B)
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
	if (ZEVector3::DotProduct(Normal, V1CV2) >= 0.0f)
		return true;
	else
		return false;
}

bool ZENavigationMeshGenerator::CheckParalel(const ZEVector3& A1, const ZEVector3& A2, const ZEVector3& B1, const ZEVector3& B2, float& LengthA, float& LengthB)
{
	ZEVector3 DirectionA(A2, A1);
	ZEVector3 DirectionB(B2, B1);
	LengthA = DirectionA.Length();
	LengthB = DirectionB.Length();
	DirectionA /= LengthA;
	DirectionB /= LengthB;
	return ZEVector3::DotProduct((B2 - B1).Normalize(), (A2 - A1).Normalize()) > 0.9999;
}

bool ZENavigationMeshGenerator::CheckAdjacent(const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B, ZECheckAdjacentResult* Result)
{
	if (&A == &B)
		return false;

	if (ZEVector3::DotProduct(GetNormal(A), GetNormal(B)) < 0.9999)
		return false;

	for (ZESSize I = 0; I < (ZESSize)A.VertexIndices.GetCount(); I++)
	{
		for (ZESSize N = 0; N < (ZESSize)B.VertexIndices.GetCount(); N++)
		{
			if (GetVertexIndex(A, I) == GetVertexIndex(B, N))
			{

				float LengthA, LengthB;				
				if (CheckParalel(GetVertex(A, I), GetVertex(A, I - 1), GetVertex(B, N), GetVertex(B, N + 1), LengthA, LengthB))
				{
					if (LengthA < LengthB)
					{
						Result->Swap = false;
						Result->Vertex1Index = I;
						Result->Vertex2Index = A.VertexIndices.Circular(I - 1);
					}
					else
					{
						Result->Swap = true;
						Result->Vertex1Index = N;
						Result->Vertex2Index = B.VertexIndices.Circular(N + 1);
					}

					return true;
				}
				
				if (CheckParalel(GetVertex(A, I), GetVertex(A, I + 1), GetVertex(B, N), GetVertex(B, N - 1), LengthA, LengthB))
				{
					if (LengthA < LengthB)
					{
						Result->Swap = false;
						Result->Vertex1Index = I;
						Result->Vertex2Index = A.VertexIndices.Circular(I + 1);
					}
					else
					{
						Result->Swap = true;
						Result->Vertex1Index = N;
						Result->Vertex2Index = B.VertexIndices.Circular(N - 1);
					}

					return true;
				}
			}
		}
	}

	return false;
}

bool ZENavigationMeshGenerator::CheckAdjacent(const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B, const ZENavigationMeshPolygon& C)
{
	if (&C == &A || &C == &B)
		return false;

	if (ZEVector3::DotProduct(GetNormal(A), GetNormal(C)) < 0.9999)
		return false;

	ZECheckAdjacentResult Result;
	if (CheckAdjacent(C, A, &Result) && CheckAdjacent(C, B, &Result))
		return true;

	return false;
}

bool ZENavigationMeshGenerator::SlicePolygon(ZENavigationMeshPolygon& Output1, ZENavigationMeshPolygon& Output2, ZENavigationMeshPolygon& Input, const ZELine& Cut)
{
	ZESize State = 0;
	for (ZESize I = 0; I < Input.VertexIndices.GetCount(); I++)
	{
		if (State % 2 == 0)
			Output1.VertexIndices.Add(Input.VertexIndices[I]);
		else
			Output2.VertexIndices.Add(Input.VertexIndices[I]);

		ZELineSegment Segment;
		Segment.Create(Segment, GetVertex(Input, I), GetVertex(Input, I + 1));
		float TLineSegment, TLine;
		float Dist = ZELineSegment::MinimumDistance(Segment, Cut, TLineSegment, TLine);
		if (Dist < 0.0001)
		{
			ZESize NewVertexIndex = AddVertex(Cut.GetPointOn(TLine));

			Output1.VertexIndices.Add(NewVertexIndex);
			Output2.VertexIndices.Add(NewVertexIndex);

			State++;
		}
	}

	Output1.NormalIndex = Input.NormalIndex;
	Output2.NormalIndex = Input.NormalIndex;

	return (State == 2);
}

void ZENavigationMeshGenerator::RemoveUnwalkable(ZEArray<ZEPolygon>& Output, const ZEArray<ZEPolygon>& Input, const ZEVector3& Up, float MaxDegree)
{
	float CosMaxDegree = ZEAngle::Cos(MaxDegree);
	ZESize Count = 0;
	for (ZESize I = 0; I < Input.GetCount(); I++)
		if (ZEVector3::DotProduct(Input[I].GetNormal(), Up) > CosMaxDegree)
			Count++;

	Output.MassAdd(Count);
	ZESize Index = 0;
	for (ZESize I = 0; I < Input.GetCount(); I++)
		if (ZEVector3::DotProduct(Input[I].GetNormal(), Up) > CosMaxDegree)
		{
			Output[Index] = Input[I];
			Index++;
		}
}

template<typename Type>
void Swap(Type& A, Type& B)
{
	Type Temp;
	Temp = A;
	A = B;
	B = Temp;
}

void ZENavigationMeshGenerator::RemovePolygonsByOrder(ZESize PolygonAIndex, ZESize PolygonBIndex)
{
	if (PolygonAIndex > PolygonBIndex)
	{
		Mesh.Polygons.Remove(PolygonAIndex);
		Mesh.Polygons.Remove(PolygonBIndex);
	}
	else
	{
		Mesh.Polygons.Remove(PolygonBIndex);
		Mesh.Polygons.Remove(PolygonAIndex);
	}
}

void ZENavigationMeshGenerator::RemovePolygonsByOrder(ZESize PolygonAIndex, ZESize PolygonBIndex, ZESize PolygonCIndex)
{
	if (PolygonAIndex > PolygonBIndex && PolygonAIndex > PolygonCIndex)
	{
		Mesh.Polygons.Remove(PolygonAIndex);
		RemovePolygonsByOrder(PolygonBIndex, PolygonCIndex);
	}
	if (PolygonBIndex > PolygonAIndex && PolygonBIndex > PolygonCIndex)
	{
		Mesh.Polygons.Remove(PolygonBIndex);
		RemovePolygonsByOrder(PolygonAIndex, PolygonCIndex);	
	}
	else
	{
		Mesh.Polygons.Remove(PolygonCIndex);
		RemovePolygonsByOrder(PolygonAIndex, PolygonBIndex);
	}
}

bool ZENavigationMeshGenerator::MergePolygons3to2(ZESize PolygonAIndex, ZESize PolygonBIndex, ZESize PolygonCIndex, ZECheckAdjacentResult& Result)
{
	ZENavigationMeshPolygon* A = &Mesh.Polygons[PolygonAIndex];
	ZENavigationMeshPolygon* B = &Mesh.Polygons[PolygonBIndex];
	ZENavigationMeshPolygon* C = &Mesh.Polygons[PolygonCIndex];
	
	if (!CheckAdjacent(*A, *B, *C))
		return false;

	ZELine Slicer;
	if (A->VertexIndices.Circular(Result.Vertex2Index - 1) == Result.Vertex1Index)
		ZELine::Create(Slicer, GetVertex(*A, Result.Vertex2Index), GetVertex(*A, Result.Vertex2Index + 1));
	else
		ZELine::Create(Slicer, GetVertex(*A, Result.Vertex2Index), GetVertex(*A, Result.Vertex2Index - 1));


	ZENavigationMeshPolygon B1, B2;
	if (!SlicePolygon(B1, B2, *B, Slicer))
		return false;

	ZENavigationMeshPolygon SubOutput;
	if (MergePolygons2to1(SubOutput, *A, B1))
	{
		SubOutput.NormalIndex = B->NormalIndex;
		ZENavigationMeshPolygon Output;
		if (!MergePolygons2to1(Output, SubOutput, *C))
			return false;

		RemovePolygonsByOrder(PolygonAIndex, PolygonBIndex, PolygonCIndex);
		
		Output.NormalIndex = B->NormalIndex;
		Mesh.Polygons.Add(Output);
		Mesh.Polygons.Add(B2);
	}
	else if (MergePolygons2to1(SubOutput, *A, B2))
	{
		SubOutput.NormalIndex = B->NormalIndex;
		ZENavigationMeshPolygon Output;
		if (!MergePolygons2to1(Output, SubOutput, *C))
			return false;

		RemovePolygonsByOrder(PolygonAIndex, PolygonBIndex, PolygonCIndex);
		
		Output.NormalIndex = B->NormalIndex;
		Mesh.Polygons.Add(Output);
		Mesh.Polygons.Add(B1);
	}
	else
		return false;
	

	return true;
}

bool ZENavigationMeshGenerator::MergePolygons3to2(ZESize PolygonAIndex, ZESize PolygonBIndex)
{
	ZECheckAdjacentResult Result;

	if (!CheckAdjacent(Mesh.Polygons[PolygonAIndex], Mesh.Polygons[PolygonBIndex], &Result))
		return false;

	if (Result.Swap)
		Swap(PolygonAIndex, PolygonBIndex);

	for (ZESize I = 0; I < Mesh.Polygons.GetCount(); I++)
	{
		if (I == PolygonAIndex || I == PolygonBIndex)
			continue;

		if (MergePolygons3to2(PolygonAIndex, PolygonBIndex, I, Result))
			return true;
	}

	return false;
}

bool ZENavigationMeshGenerator::MergePolygons3to2(ZESize PolygonAIndex)
{
	for (ZESize I = 0; I < Mesh.Polygons.GetCount(); I++)
	{
		if (I == PolygonAIndex)
			continue;

		if (MergePolygons3to2(PolygonAIndex, I))
			return true;
	}

	return false;
}

bool ZENavigationMeshGenerator::MergePolygons2to1(ZENavigationMeshPolygon& Output, const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B)
{
	if (ZEVector3::DotProduct(Mesh.Normals[A.NormalIndex], Mesh.Normals[B.NormalIndex]) < 0.99f)
		return false;

	for (ZESSize I = 0; I < (ZESSize)A.VertexIndices.GetCount(); I++)
	{
		for (ZESSize N = 0; N < (ZESSize)B.VertexIndices.GetCount(); N++)
		{
			if (GetVertex(A, I) == GetVertex(B, N) && 
				GetVertex(A, I + 1) == GetVertex(B, N - 1))
			{
				if (CheckInternalAngle(GetVertex(A, I), Mesh.Normals[A.NormalIndex], GetVertex(A, I - 1), GetVertex(B, N + 1)) &&
					CheckInternalAngle(GetVertex(A, I + 1), Mesh.Normals[A.NormalIndex], GetVertex(B, N - 2), GetVertex(A, I + 2)))
				{
					Output.VertexIndices.MassAdd(A.VertexIndices.GetCount() + B.VertexIndices.GetCount() - 2);
					Output.NormalIndex = A.NormalIndex;

					ZESize Index = 0;

					for (ZESSize K = 0; K < (ZESSize)B.VertexIndices.GetCount(); K++)
					{
						Output.VertexIndices[Index] = GetVertexIndex(B, N + K);
						Index++;
					}

					for (ZESSize K = 0; K < (ZESSize)A.VertexIndices.GetCount() - 2; K++)
					{
						Output.VertexIndices[Index] = GetVertexIndex(A, I + 2 + K);
						Index++;
					}

					for (ZESSize K = 0; K < (ZESSize)Output.VertexIndices.GetCount(); K++)
					{
						if (GetVertex(Output, K - 1) == GetVertex(Output, K) || CheckStraight(GetVertex(Output, K - 1), GetVertex(Output, K), GetVertex(Output, K + 1)))
						{
							Output.VertexIndices.Remove(K);
							K--;
						}
					}

					return true;
				}
			}
		}
	}
	return false;
}


bool ZENavigationMeshGenerator::MergePolygons2to1(ZESize PolygonIndex1, ZESize PolygonIndex2)
{
	const ZENavigationMeshPolygon& A = Mesh.Polygons[PolygonIndex1];
	const ZENavigationMeshPolygon& B = Mesh.Polygons[PolygonIndex2];
	ZENavigationMeshPolygon Output;

	if (!MergePolygons2to1(Output, A, B))
		return false;

	if (PolygonIndex1 > PolygonIndex2)
	{
		Mesh.Polygons.Remove(PolygonIndex1);
		Mesh.Polygons.Remove(PolygonIndex2);
	}
	else
	{
		Mesh.Polygons.Remove(PolygonIndex2);
		Mesh.Polygons.Remove(PolygonIndex1);
	}

	Mesh.Polygons.Add(Output);

	return true;
}

void ZENavigationMeshGenerator::TraverseOctree(ZENavigationMeshOctree* Octree)
{
	for (ZESize I = 0; I < Octree->GetItemCount(); I++)
		Mesh.Polygons.Add(Octree->GetItem(I));

	for (ZEInt I = 0; I < 8; I++)
		if (Octree->GetNode(I) != NULL)
			TraverseOctree(Octree->GetNode(I));
}

ZENavigationMesh&  ZENavigationMeshGenerator::GetOutput()
{
	return Mesh;
}

bool ZENavigationMeshGenerator::OptimizePolygons2to1()
{
	ZESize PolygonCulled = 0;
	for (ZESize I = 0; I < Mesh.Polygons.GetCount(); I++)
	{
		for (ZESize N = 0; N < Mesh.Polygons.GetCount(); N++)
		{
			if (I == N)
				continue;

			if (MergePolygons2to1(I, N))
			{
				I--;
				PolygonCulled++;
				break;
			}
		}
	}

	return PolygonCulled != 0;
}

bool ZENavigationMeshGenerator::OptimizePolygons3to2()
{
	ZESize PolygonCulled = 0;
	for (ZESize I = 0; I < Mesh.Polygons.GetCount(); I++)
		if (MergePolygons3to2(I))
		{
			PolygonCulled++;
			I = 0;
		}

	return PolygonCulled != 0;
}

void ZENavigationMeshGenerator::Generate(const ZEArray<ZEPolygon>& Input, ZENavigationMeshGeneratorOptions* Options)
{
	ZEArray<ZEPolygon> Temp;
	RemoveUnwalkable(Temp, Input, Options->UpDirection, Options->MaxWalkableSlope);
	Mesh.Generate(Temp);

	TraverseOctree(&Mesh.Octree);

	bool PolygonCulled;
	while(true) //for (ZEInt I = 0; I < Options->MaxConvexSimplicationPass; I++)
	{
		PolygonCulled = false;
		PolygonCulled |= OptimizePolygons2to1();
		PolygonCulled |= OptimizePolygons3to2();
		if (!PolygonCulled)
			PolygonCulled |= OptimizePolygons2to1();
		if (!PolygonCulled)
			break;
	}

	Mesh.Octree.Shrink();
}

ZENavigationMeshGenerator::ZENavigationMeshGenerator()
{

}
