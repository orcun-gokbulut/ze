//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEQuadTree.h
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
#ifndef __ZE_QUAD_TREE_H__
#define __ZE_QUAD_TREE_H__

#include "ZETypes.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEArray.h"

template < typename ZEItemType, typename Allocator = ZEAllocatorBase<ZEItemType> >
class ZEQuadTree
{
	private:
		ZEQuadTree* Parent;
		ZEUInt Depth;
		ZEUInt MaxDepth;
		ZEUInt Quadrant;
		ZEArray<ZEItemType, Allocator> Items;
		ZEQuadTree* Nodes[4];
		ZEAABBox BoundingBox;

	public:
		ZEQuadTree* GetParent()
		{
			return Parent;
		}

		ZEQuadTree** GetNodes()
		{
			return Nodes;
		}

		ZEUInt GetDepth()
		{
			return Depth;
		}

		ZEUInt GetQuadrant()
		{
			return Quadrant;
		}

		void SetMaxDepth(ZEUInt Depth)
		{
			MaxDepth = Depth;
		}

		ZEUInt GetMaxDepth()
		{
			return MaxDepth;
		}

		const ZEAABBox& GetBoundingBox()
		{
			return BoundingBox;
		}

		void SetBoundingBox(const ZEAABBox& BoundingBox)
		{
			this->BoundingBox = BoundingBox;
		}

		ZESize GetItemCount()
		{
			return Items.GetCount();
		}

		const ZEItemType& GetItem(ZESize Index) const
		{
			return Items[Index];
		}

		ZEItemType& GetItem(ZESize Index)
		{
			return Items[Index];
		}

		const ZEArray<ZEItemType, Allocator>& GetItems()
		{
			return Items;
		}

		void CreateNode(ZEInt Octant)
		{
			ZEVector3 Center = BoundingBox.GetCenter();

			if (Nodes[Octant] == NULL && MaxDepth != 0)
			{
				Nodes[Octant] = new ZEQuadTree();
				Nodes[Octant]->Parent = this;
				Nodes[Octant]->Octant = Octant;
				Nodes[Octant]->Depth = Depth + 1;
				Nodes[Octant]->MaxDepth = MaxDepth - 1;

				if (Octant & 0x01) // Right
				{
					Nodes[Octant]->BoundingBox.Min.x = Center.x;
					Nodes[Octant]->BoundingBox.Max.x = BoundingBox.Max.x;		
				}
				else
				{
					Nodes[Octant]->BoundingBox.Min.x = BoundingBox.Min.x;
					Nodes[Octant]->BoundingBox.Max.x = Center.x;
				}

				if (Octant & 0x02) // Top
				{
					Nodes[Octant]->BoundingBox.Min.z = Center.z;
					Nodes[Octant]->BoundingBox.Max.z = BoundingBox.Max.z;		
				}
				else
				{
					Nodes[Octant]->BoundingBox.Min.z = BoundingBox.Min.z;
					Nodes[Octant]->BoundingBox.Max.z = Center.z;
				}
			}
		}

		void AddItem(const ZEItemType& Item, const ZEVector3& Point)
		{
			if (MaxDepth == 0)
				Items.Add(Item);
			else
			{
				ZEInt ItemOctant = GetOctantIndex(Point);
				if (ItemOctant == -1)
					Items.Add(Item);
				else
				{
					CreateNode(ItemOctant);
					Nodes[ItemOctant]->AddItem(Item, Point);
				}
			}
		}

		void AddItem(const ZEItemType& Item, const ZEAABBox& Volume)
		{
			if (MaxDepth == 0)
				Items.Add(Item);
			else
			{
				ZEInt ItemOctant = GetOctantIndex(Volume);
				if (ItemOctant == -1)
					Items.Add(Item);
				else
				{
					CreateNode(ItemOctant);
					Nodes[ItemOctant]->AddItem(Item, Volume);
				}
			}
		}
		
		void RemoveItem(ZESize Index)
		{
			Items.Remove(Index);
		}

		void Clear()
		{
			Items.Clear();
			for (ZEInt I = 0; I < 4; I++)
				if (Nodes[I] != NULL)
				{
					delete Nodes[I];
					Nodes[I] = NULL;
				}
		}

		ZEInt GetOctantIndex(const ZEVector3& Point)
		{
			ZEVector3& Center = BoundingBox.GetCenter();
			return (Point.z > Center.z ? 2 : 0) + (Point.x > Center.x ? 1 : 0);
		}

		ZEInt GetOctantIndex(const ZEAABBox& BoundingBox)
		{
			ZEInt MinOctantIndex = GetOctantIndex(BoundingBox.Min);
			ZEInt MaxOctantIndex = GetOctantIndex(BoundingBox.Max);
			if (MinOctantIndex == MaxOctantIndex)
				return MinOctantIndex;
			else
				return -1;
		}

		ZEQuadTree* GetNode(ZESize OctantIndex)
		{
			return Nodes[OctantIndex];
		}

		ZEQuadTree* GetNode(const ZEVector3& Point)
		{
			if (!ZEAABBox::IntersectionTest(BoundingBox, Point))
				return NULL;

			ZEInt OctantIndex = GetOctantIndex(Point);
			if (Nodes[OctantIndex] != NULL)
				return Nodes[OctantIndex]->GetNode(Point);
			else
				return this;
		}

		ZEQuadTree* GetNode(const ZEAABBox& SearchVolume)
		{
			if (!ZEAABBox::IntersectionTest(BoundingBox, SearchVolume))
				return NULL;

			ZEInt OctantIndex = GetOctantIndex(SearchVolume);
			if (OctantIndex == -1)
				return this;
			else
			{
				if (Nodes[OctantIndex] != NULL)
					return Nodes[OctantIndex]->GetNode(SearchVolume);
				else
					return this;
			}
		}

		bool Shrink()
		{
			bool Found = false;
			for (ZEInt I = 0; I < 4; I++)
				if (Nodes[I] != NULL)
				{
					if (Nodes[I]->Shrink())
					{
						delete Nodes[I];
						Nodes[I] = NULL;
					}
					else
						Found = true;
				}

			if (Items.GetCount() != 0)
				return false;

			if (Found)
				return false;

			return true;
		}

		ZEQuadTree()
		{
			Parent = NULL;
			for (ZEInt I = 0; I < 4; I++)
				Nodes[I] = NULL;

			Depth = 0;
			MaxDepth = 0;
			Quadrant = 0;
		}

		~ZEQuadTree()
		{
			for (ZEInt I = 0; I < 4; I++)
				if (Nodes[I] != NULL)
					delete Nodes[I];
		}
};

#endif
