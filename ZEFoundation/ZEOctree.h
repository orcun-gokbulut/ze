//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOctree.h
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
#ifndef __ZE_OCTREE_H__
#define __ZE_OCTREE_H__

#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEArray.h"

template <typename Type, typename Allocator = ZEAllocatorBase<Type>>
class ZEOctree
{
	private:
		ZEOctree* Parent;
		ZEUInt Depth;
		ZEUInt MaxDepth;
		ZEUInt Octant;
		ZEArray<Type, Allocator> Items;
		ZEOctree* Nodes[8];
		ZEAABBox BoundingBox;

	public:
		ZEOctree* GetParent()
		{
			return Parent;
		}

		ZEOctree** GetNodes()
		{
			return Nodes;
		}

		ZEUInt GetDepth()
		{
			return Depth;
		}

		ZEUInt GetOctant()
		{
			return Octant;
		}

		void SetMaxDepth(ZEUInt Depth)
		{
			MaxDepth = Depth;
		}

		ZEUInt GetMaxDepth()
		{
			return MaxDepth
		}

		const ZEAABBox& GetBoundingBox()
		{
			return BoundingBox;
		}

		void SetBoundingBox(const ZEAABBox& BoundingBox)
		{
			this->BoundingBox = BoundingBox;
		}

		size_t GetItemCount()
		{
			return Items.GetCount();
		}

		const Type& GetItem(size_t Index) const
		{
			return Items[Index];
		}

		Type& GetItem(size_t Index)
		{
			return Items[Index];
		}

		const ZEArray<Type, Allocator>& GetItems()
		{
			return Items;
		}

		void CreateNode(int Octant)
		{
			ZEVector3 Center = BoundingBox.GetCenter();

			if (Nodes[Octant] == NULL && MaxDepth != 0)
			{
				Nodes[Octant] = new ZEOctree();
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

				if (Octant & 0x02) // Up
				{
					Nodes[Octant]->BoundingBox.Min.y = Center.y;
					Nodes[Octant]->BoundingBox.Max.y = BoundingBox.Max.y;		
				}
				else
				{
					Nodes[Octant]->BoundingBox.Min.y = BoundingBox.Min.y;
					Nodes[Octant]->BoundingBox.Max.y = Center.y;
				}

				if (Octant & 0x04) // Front
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

		void AddItem(const Type& Item, const ZEVector3& Point)
		{
			if (MaxDepth == 0)
				Items.Add(Item);
			else
			{
				int ItemOctant = GetOctantIndex(Point);
				if (ItemOctant == -1)
					Items.Add(Item);
				else
				{
					CreateNode(ItemOctant);
					Nodes[ItemOctant]->AddItem(Item, Point);
				}
			}
		}

		void AddItem(const Type& Item, const ZEAABBox& Volume)
		{
			if (MaxDepth == 0)
				Items.Add(Item);
			else
			{
				int ItemOctant = GetOctantIndex(Volume);
				if (ItemOctant == -1)
					Items.Add(Item);
				else
				{
					CreateNode(ItemOctant);
					Nodes[ItemOctant]->AddItem(Item, Volume);
				}
			}
		}
		
		void RemoveItem(size_t Index)
		{
			Items.DeleteAt(Index);
		}

		void Clear()
		{
			Items.Clear();
			for (int I = 0; I < 8; I++)
				if (Nodes[I] != NULL)
				{
					delete Nodes[I];
					Nodes[I] = NULL;
				}
		}

		int GetOctantIndex(const ZEVector3& Point)
		{
			ZEVector3& Center = BoundingBox.GetCenter();
			return (Point.z > Center.z ? 4 : 0) + (Point.y > Center.y ? 2 : 0) + (Point.x > Center.x ? 1 : 0);
		}

		int GetOctantIndex(const ZEAABBox& BoundingBox)
		{
			int MinOctantIndex = GetOctantIndex(BoundingBox.Min);
			int MaxOctantIndex = GetOctantIndex(BoundingBox.Max);
			if (MinOctantIndex == MaxOctantIndex)
				return MinOctantIndex;
			else
				return -1;
		}

		ZEOctree* GetNode(size_t OctantIndex)
		{
			return Nodes[OctantIndex];
		}

		ZEOctree* GetNode(const ZEVector3& Point)
		{
			if (!ZEAABBox::IntersectionTest(BoundingBox, Point))
				return NULL;

			int OctantIndex = GetOctantIndex(Point);
			if (Nodes[OctantIndex] != NULL)
				return Nodes[OctantIndex]->GetNode(Point);
			else
				return this;
		}

		ZEOctree* GetNode(const ZEAABBox& SearchVolume)
		{
			if (!ZEAABBox::IntersectionTest(BoundingBox, SearchVolume))
				return NULL;

			int OctantIndex = GetOctantIndex(SearchVolume);
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
			for (int I = 0; I < 8; I++)
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

		ZEOctree()
		{
			Parent = NULL;
			for (int I = 0; I < 8; I++)
				Nodes[I] = NULL;

			Depth = 0;
			MaxDepth = 0;
			Octant = 0;
		}

		~ZEOctree()
		{
			for (int I = 0; I < 8; I++)
				if (Nodes[I] != NULL)
					delete Nodes[I];
		}
};

#endif
