//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZENavigationMeshGenerator.h
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

#pragma once
#ifndef __ZE_NAVIGATION_MESH_GENERATOR_H__
#define __ZE_NAVIGATION_MESH_GENERATOR_H__

#include "ZEDS/ZEGraph.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEPolygon.h"
#include "ZENavigationMesh.h"

class ZENavigationMeshGeneratorOptions 
{
	public:
		ZEVector3 UpDirection;
		float MaxWalkableSlope;
		int MaxConvexSimplicationPass;
		int Max3To1SimplificationPass;

};

struct ZECheckAdjacentResult;

class ZENavigationMeshGenerator
{
	public:
		ZENavigationMesh Mesh;

		// Helpers
		ZEVector3&			GetVertex(const ZENavigationMeshPolygon& Polygon, ptrdiff_t Index);
		size_t				GetVertexIndex(const ZENavigationMeshPolygon& Polygon, ptrdiff_t Index);
		ZEVector3&			GetNormal(const ZENavigationMeshPolygon& Polygon);
		void				GetBoundingBox(ZEAABBox& Output, ZENavigationMeshPolygon& Polygon);

		size_t				AddVertex(const ZEVector3& Point);
		void				RemovePolygonsByOrder(size_t PolygonAIndex, size_t PolygonBIndex);
		void				RemovePolygonsByOrder(size_t PolygonAIndex, size_t PolygonBIndex, size_t PolygonCIndex);

		void				TraverseOctree(ZENavigationMeshOctree* Octree);

		// Checks
		bool				CheckStraight(const ZEVector3& A, const ZEVector3& B, const ZEVector3& C);
		bool				CheckInternalAngle(const ZEVector3& Center, const ZEVector3& Normal, const ZEVector3& A, const ZEVector3& B);
		bool				CheckParalel(const ZEVector3& A1, const ZEVector3& A2, const ZEVector3& B1, const ZEVector3& B2, float& LengthA, float& LengthB);
		bool				CheckAdjacent(const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B, ZECheckAdjacentResult* Result);
		bool				CheckAdjacent(const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B, const ZENavigationMeshPolygon& C);

		// Operations
		bool				SlicePolygon(ZENavigationMeshPolygon& Output, ZENavigationMeshPolygon& Output2, ZENavigationMeshPolygon& Input, const ZELine& Cut);

		// Mergers
		static void			RemoveUnwalkable(ZEArray<ZEPolygon>& Output, const ZEArray<ZEPolygon>& Input, const ZEVector3& Up, float MaxDegree);
		bool				MergePolygons2to1(ZENavigationMeshPolygon& Output, const ZENavigationMeshPolygon& A, const ZENavigationMeshPolygon& B);
		bool				MergePolygons2to1(size_t PolygonIndex1, size_t PolygonIndex2);

		bool				MergePolygons3to2(size_t PolygonAIndex);
		bool				MergePolygons3to2(size_t PolygonAIndex, size_t PolygonBIndex);
		bool				MergePolygons3to2(size_t PolygonAIndex, size_t PolygonBIndex, size_t PolygonCIndex, ZECheckAdjacentResult& Result);

		bool				MergePolygonsNto1(size_t VertexIndex);

		void				FloodFill(const ZEVector3& StartPoint);
		
		bool				OptimizePolygons2to1();
		bool				OptimizePolygons3to2();
		bool				OptimizePolygonsNto1();

	public:
		ZENavigationMesh&	GetOutput();
		void				Optimize(ZENavigationMesh& Mesh, ZENavigationMeshGeneratorOptions* Options);
		void				Generate(const ZEArray<ZEPolygon>& Input, ZENavigationMeshGeneratorOptions* Options);

							ZENavigationMeshGenerator();
};

#endif
