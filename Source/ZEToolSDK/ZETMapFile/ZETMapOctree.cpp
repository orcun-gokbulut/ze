//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETMapOctree.cpp
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

#include "ZETMapOctree.h"

#define IsAtFront(x)				(((x) & 1) == 0)
#define IsAtLeft(x)					(((x) & 4) == 0)
#define IsAtDown(x)					(((x) & 2) == 0)

ZEMapFileOctree::ZEMapFileOctree()
{	
	IsLeaf = false;
	Depth = 0;
	BoundingBox.Max = BoundingBox.Min = ZEVector3(0.0f, 0.0f, 0.0f);
	for (ZEInt I = 0; I < 8; I++)
		SubTrees[I] = NULL;
}

ZEMapFileOctree::~ZEMapFileOctree()
{
	for (ZEInt I = 0; I < 8; I++)
		if (SubTrees[I] != NULL)
			delete SubTrees[I];
}

void AddPolygonToSubSpace(ZEMapFileOctree* Node, ZEOctreeSubspace SubSpace, ZEInt PolygonId)
{
	if (Node->SubTrees[SubSpace] == NULL)
	{
		Node->SubTrees[SubSpace] = new ZEMapFileOctree();
		Node->SubTrees[SubSpace]->Depth = Node->Depth + 1;
		
		ZEAABBox& Box = Node->BoundingBox;
		ZEAABBox& SubBox = Node->SubTrees[SubSpace]->BoundingBox;

		ZEVector3 Center = Box.GetCenter();

		if (IsAtLeft(SubSpace))
		{
			SubBox.Max.x = Center.x;
			SubBox.Min.x = Box.Min.x;
		}
		else
		{
			SubBox.Max.x = Box.Max.x;
			SubBox.Min.x = Center.x;
		}

		if (IsAtDown(SubSpace))
		{
			SubBox.Max.y = Center.y;
			SubBox.Min.y = Box.Min.y;
		}
		else
		{
			SubBox.Max.y = Box.Max.y;
			SubBox.Min.y = Center.y;
		}

		if (IsAtFront(SubSpace))
		{
			SubBox.Max.z = Center.z;
			SubBox.Min.z = Box.Min.z;
		}
		else
		{
			SubBox.Max.z = Box.Max.z;
			SubBox.Min.z = Center.z;
		}
	}
	Node->SubTrees[SubSpace]->PolygonIds.Add(PolygonId);
}

void GenerateOctreeNode(ZEMapFileOctree* Node, ZEInt MaxDepth, ZEArray<ZEMapFilePolygon>* Polygons)
{
	if (Node->Depth == MaxDepth)
	{
		Node->IsLeaf = true;
		return;
	}

	float PlaneX = Node->BoundingBox.Min.x + (Node->BoundingBox.Max.x - Node->BoundingBox.Min.x) / 2;
	float PlaneY = Node->BoundingBox.Min.y + (Node->BoundingBox.Max.y - Node->BoundingBox.Min.y) / 2;
	float PlaneZ = Node->BoundingBox.Min.z + (Node->BoundingBox.Max.z - Node->BoundingBox.Min.z) / 2;
	bool AtLeft, AtRight, AtDown, AtUp, AtFront, AtBack;
	ZEMapFilePolygon* Polygon;

	for (ZESize I = 0; I < Node->PolygonIds.GetCount(); I++)
	{
		AtLeft = AtRight = AtDown = AtUp = AtFront = AtBack = false;
		Polygon = &(*Polygons)[Node->PolygonIds[I]];
		for (ZEInt N = 0; N < 3; N++)
		{

			if (Polygon->Vertices[N].Position.x < PlaneX)
				AtLeft = true;
			else
				AtRight = true;
				
			if (Polygon->Vertices[N].Position.y < PlaneY)
				AtDown = true;
			else
				AtUp = true;

			if (Polygon->Vertices[N].Position.z < PlaneZ)
				AtFront = true;
			else
				AtBack= true;
		}

		if (AtLeft && AtUp && AtFront)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_LEFTUPFRONT, Node->PolygonIds[I]);

		if (AtLeft && AtUp && AtBack)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_LEFTUPBACK, Node->PolygonIds[I]);

		if (AtLeft && AtDown && AtFront)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_LEFTDOWNFRONT, Node->PolygonIds[I]);

		if (AtLeft && AtDown && AtBack)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_LEFTDOWNBACK, Node->PolygonIds[I]);

		if (AtRight && AtUp && AtFront)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_RIGHTUPFRONT, Node->PolygonIds[I]);

		if (AtRight && AtUp && AtBack)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_RIGHTUPBACK, Node->PolygonIds[I]);

		if (AtRight && AtDown && AtFront)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_RIGHTDOWNFRONT, Node->PolygonIds[I]);

		if (AtRight && AtDown && AtBack)
			AddPolygonToSubSpace(Node, ZEOCTREESUBSPACE_RIGHTDOWNBACK, Node->PolygonIds[I]);
	}

	Node->PolygonIds.Clear();

	Node->IsLeaf = false;
	for (ZEInt I = 0; I < 8; I++)
	{
		if (Node->SubTrees[I] != NULL)
			GenerateOctreeNode(Node->SubTrees[I], MaxDepth, Polygons);
	}
}

ZEAABBox CalculateBoundingBox(ZEArray<ZEMapFilePolygon>& Polygons)
{
	ZEAABBox MapBoundingBox;
	MapBoundingBox.Min = Polygons[0].Vertices[0].Position;
	MapBoundingBox.Max = Polygons[0].Vertices[0].Position;

	for(ZESize I = 0; I < Polygons.GetCount(); I++)
	{
		for (ZEInt N = 0; N < 3; N++)
		{			
			if (Polygons[I].Vertices[N].Position.x > MapBoundingBox.Max.x)
				MapBoundingBox.Max.x = Polygons[I].Vertices[N].Position.x;
			if (Polygons[I].Vertices[N].Position.x < MapBoundingBox.Min.x)
				MapBoundingBox.Min.x = Polygons[I].Vertices[N].Position.x;
			
			if (Polygons[I].Vertices[N].Position.y > MapBoundingBox.Max.y)
				MapBoundingBox.Max.y = Polygons[I].Vertices[N].Position.y;
			if (Polygons[I].Vertices[N].Position.y < MapBoundingBox.Min.y)
				MapBoundingBox.Min.y = Polygons[I].Vertices[N].Position.y;
			
			if (Polygons[I].Vertices[N].Position.z > MapBoundingBox.Max.z)
				MapBoundingBox.Max.z = Polygons[I].Vertices[N].Position.z;
			if (Polygons[I].Vertices[N].Position.z < MapBoundingBox.Min.z)
				MapBoundingBox.Min.z = Polygons[I].Vertices[N].Position.z;
		}
	}
	return MapBoundingBox;
}

ZEMapFileOctree* ZEMapFileOctree::GenerateOctree(ZEArray<ZEMapFilePolygon>& Polygons, ZEUInt MaxDepth)
{
	ZEMapFileOctree* Octree = new ZEMapFileOctree();
	Octree->Depth = 0;
	Octree->BoundingBox = CalculateBoundingBox(Polygons);
	Octree->PolygonIds.SetCount(Polygons.GetCount());
	for(ZESize I = 0; I < Polygons.GetCount(); I++)
		Octree->PolygonIds[I] = I;
	
	GenerateOctreeNode(Octree, MaxDepth, &Polygons);

	return Octree;
}
