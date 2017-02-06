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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZETriangle.h"
#include "ZEThread/ZELockRW.h"
#include "ZEOctreeIterator.h"

#define ZE_OO_OUTSIDE	-2
#define ZE_OO_MULTIPLE	-1

#define ZE_OCTREE_TEMPLATE template<typename ZEItemType, typename ZELockType>
#define ZE_OCTREE_SPEC ZEItemType, ZELockType

template<typename ZEItemType, typename ZELockType = ZELockRW>
class ZEOctree
{
	private:
		ZEOctree*								Parent;
		ZEInt									ParentOctant;
		ZEInt									Depth;
		ZEUInt									MaxDepth;
		ZEArray<ZEItemType>						Items;
		ZEOctree*								Nodes[8];
		ZEAABBox								BoundingBox;
		ZESize									TotalNodeCount;
		ZESize									TotalItemCount;
		ZELockRW								Lock;

		void									UpdateNodeCount(ZESSize Delta);
		void									UpdateItemCount(ZESSize Delta);

		void									Expand();
		void									Shrink();

		void									CreateChildNode(ZEInt Octant);
		void									RemoveChildNode(ZEOctree* Octree);

	public:
		typedef ZEOctreeIterator<ZE_OCTREE_SPEC> Iterator;

		ZEOctree*								GetParent();
		const ZEOctree*							GetParent() const;
		ZEUInt									GetParentOctant() const;	
		ZEUInt									GetDepth() const;

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox&							GetBoundingBox() const;

		void									SetMaxDepth(ZEUInt Depth);
		ZEUInt									GetMaxDepth() const;

		ZESize									GetTotalNodeCount() const;
		ZESize									GetTotalItemCount() const;

		ZEOctree*								GetNode(ZESize Octant);
		const ZEOctree*							GetNode(ZESize Octant) const;

		ZEOctree*								GetNode(const ZEVector3& Point);
		const ZEOctree*							GetNode(const ZEVector3& Point) const;

		ZEOctree*								GetNode(const ZEAABBox& BoundingBox);
		const ZEOctree*							GetNode(const ZEAABBox& BoundingBox) const;

		ZEInt									FindOctant(const ZEVector3& Point) const;
		ZEInt									FindOctant(const ZEAABBox& BoundingBox) const;

		const ZEArray<ZEItemType>&				GetItems();
		const ZEArray<const ZEItemType>&		GetItems() const;

		ZEOctree*								AddItem(const ZEItemType& Item, const ZEVector3& Point);
		ZEOctree*								AddItem(const ZEItemType& Item, const ZEAABBox& Volume);

		void									RemoveItem(ZESize Index);
		bool									RemoveItem(const ZEItemType& Item);
		bool									RemoveItem(const ZEItemType& Item, const ZEVector3& Point);
		bool									RemoveItem(const ZEItemType& Item, const ZEAABBox& Volume);

		bool									Check() const;

		void									Clear();

		ZEOctreeIterator<ZE_OCTREE_SPEC>		Traverse(const ZEViewVolume* Volume);

		void									LockRead() const;
		void									UnlockRead() const;

		void									LockWrite();
		void									LockWriteNested();
		void									UnlockWrite();

												ZEOctree();
												~ZEOctree();
};

// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::UpdateNodeCount(ZESSize Delta)
{
	Lock.LockWriteNested();
	{
		TotalNodeCount += Delta;

		if (Parent != NULL)
			Parent->UpdateNodeCount(Delta);
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::UpdateItemCount(ZESSize Delta)
{
	Lock.LockWriteNested();
	{
		TotalItemCount += Delta;

		if (Parent != NULL)
			Parent->UpdateItemCount(Delta);
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
ZEInt ZEOctree<ZE_OCTREE_SPEC>::FindOctant(const ZEVector3& Point) const
{
	if (Point.x < BoundingBox.Min.x || Point.x > BoundingBox.Max.x)
		return ZE_OO_OUTSIDE;

	if (Point.y < BoundingBox.Min.y || Point.y > BoundingBox.Max.y)
		return ZE_OO_OUTSIDE;

	if (Point.z < BoundingBox.Min.z || Point.z > BoundingBox.Max.z)
		return ZE_OO_OUTSIDE;

	ZEVector3& Center = BoundingBox.GetCenter();
	return (Point.z > Center.z ? 4 : 0) + (Point.y > Center.y ? 2 : 0) + (Point.x > Center.x ? 1 : 0);
}

ZE_OCTREE_TEMPLATE
ZEInt ZEOctree<ZE_OCTREE_SPEC>::FindOctant(const ZEAABBox& BoundingBox) const
{
	ZEInt MinOctantIndex = FindOctant(BoundingBox.Min);
	if (MinOctantIndex == ZE_OO_OUTSIDE)
		return ZE_OO_OUTSIDE;

	ZEInt MaxOctantIndex = FindOctant(BoundingBox.Max);
	if (MaxOctantIndex == ZE_OO_OUTSIDE)
		return ZE_OO_OUTSIDE;

	if (MinOctantIndex == MaxOctantIndex)
		return MinOctantIndex;
	else
		return ZE_OO_MULTIPLE;
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::Expand()
{
	zeDebugCheck(Parent != NULL, "Uplifiting non root octree.");

	// Calculate New Bounding Box
	float Width_2 = (BoundingBox.Max.x - BoundingBox.Min.x) / 2;
	BoundingBox.Min.x -= Width_2;
	BoundingBox.Max.x += Width_2;	

	float Height_2 = (BoundingBox.Max.y - BoundingBox.Min.y) / 2;
	BoundingBox.Min.y -= Height_2;
	BoundingBox.Max.y += Height_2;

	float Depth_2 = (BoundingBox.Max.z - BoundingBox.Min.z) / 2;
	BoundingBox.Min.z -= Depth_2;
	BoundingBox.Max.z += Depth_2;

	// Create New Child Nodes for Expansion
	for (ZEUInt I = 0; I < 8; I++)
	{
		if (Nodes[I] == NULL)
			continue;

		// Remove Old Child
		ZEOctree<ZE_OCTREE_SPEC>* Temp = Nodes[I];
		Nodes[I] = NULL;
		
		// Generate New Child
		CreateChildNode(I);
		Nodes[I]->Depth = Depth;
		Nodes[I]->MaxDepth = MaxDepth;

		// Add Old Child to New Child
		Temp->LockWriteNested();
		{
			Temp->Parent = Nodes[I];
			Temp->ParentOctant = (~I) & 0x07;
			Nodes[I]->Nodes[Temp->ParentOctant] = Temp;
		}
		Temp->UnlockWrite();
		Nodes[I]->TotalNodeCount = 1;
	}

	// Uplift
	Depth = Depth - 1;
	MaxDepth = MaxDepth + 1;
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::Shrink()
{
	Lock.LockWriteNested();
	{
		if (Depth == 0 || TotalItemCount != 0 || TotalNodeCount != 0)
		{
			Lock.UnlockWrite();
			return;
		}

		ZEOctree<ZE_OCTREE_SPEC>* ParentPointer = Parent;

		if (Parent != NULL)
			Parent->RemoveChildNode(this);

		Lock.UnlockWrite();
		delete this;
	}
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::CreateChildNode(ZEInt Octant)
{
	ZEVector3 Center = BoundingBox.GetCenter();

	if (Nodes[Octant] != NULL || MaxDepth == 0)
		return;

	Nodes[Octant] = new ZEOctree();
	Nodes[Octant]->Parent = this;
	Nodes[Octant]->ParentOctant = Octant;
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

	UpdateNodeCount(1);
}


ZE_OCTREE_TEMPLATE
	void ZEOctree<ZE_OCTREE_SPEC>::RemoveChildNode(ZEOctree<ZE_OCTREE_SPEC>* Octree)
{
	Lock.LockWriteNested();
	Octree->LockWriteNested();
	{
		zeDebugCheck(Octree->Parent != this, "Octree is not child of this octree.");

		Nodes[Octree->ParentOctant] = NULL;
		Octree->Parent = NULL;
		UpdateItemCount(-(ZESSize)Octree->TotalItemCount);
		UpdateNodeCount(-(ZESSize)Octree->TotalNodeCount);
	}
	Octree->UnlockWrite();
	Lock.UnlockWrite();

	Shrink();
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetParent()
{
	return Parent;
}

ZE_OCTREE_TEMPLATE
const ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetParent() const
{
	return Parent;
}

ZE_OCTREE_TEMPLATE
ZEUInt ZEOctree<ZE_OCTREE_SPEC>::GetDepth() const
{
	return Depth;
}

ZE_OCTREE_TEMPLATE
	ZEUInt ZEOctree<ZE_OCTREE_SPEC>::GetParentOctant() const
{
	return ParentOctant;
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	zeCheckError(Parent != NULL || TotalItemCount != 0 || TotalNodeCount != NULL, ZE_VOID, "You can only change bounding box of an empty root octree.");
	this->BoundingBox = BoundingBox;
}

ZE_OCTREE_TEMPLATE
const ZEAABBox& ZEOctree<ZE_OCTREE_SPEC>::GetBoundingBox()  const
{
	return BoundingBox;
}

ZE_OCTREE_TEMPLATE
ZESize ZEOctree<ZE_OCTREE_SPEC>::GetTotalNodeCount() const
{
	return TotalNodeCount;
}

ZE_OCTREE_TEMPLATE
ZESize ZEOctree<ZE_OCTREE_SPEC>::GetTotalItemCount() const
{
	return TotalItemCount;
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(ZESize OctantIndex)
{
	zeDebugCheck(OctantIndex >= 8, "Index out of bounds.");
	return Nodes[OctantIndex];
}

ZE_OCTREE_TEMPLATE
const ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(ZESize OctantIndex) const 
{
	zeDebugCheck(OctantIndex >= 8, "Index out of bounds.");
	return Nodes[OctantIndex];
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(const ZEVector3& Point)
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	Lock.LockRead();
	{
		if (!ZEAABBox::IntersectionTest(BoundingBox, Point))
		{
			Result = NULL;
		}
		else
		{
			ZEInt OctantIndex = FindOctant(Point);
			if (Nodes[OctantIndex] != NULL)
				Result = Nodes[OctantIndex]->GetNode(Point);
			else
				Result = this;
		}
	}
	Lock.UnlockRead();

	return Result;
}

ZE_OCTREE_TEMPLATE
const ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(const ZEVector3& Point) const
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	ZEInt OctantIndex = FindOctant(Point);
	Lock.LockRead();
	{
		if (OctantIndex == ZE_OO_OUTSIDE)
		{
			Result = NULL;
		}
		else
		{
			if (Nodes[OctantIndex] != NULL)
				Result = Nodes[OctantIndex]->GetNode(Point);
			else
				Result = this;
		}
	}
	Lock.UnlockRead();

	return Result;
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(const ZEAABBox& SearchVolume)
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	ZEInt OctantIndex = FindOctant(SearchVolume);
	Lock.LockRead();
	{
		if (OctantIndex == ZE_OO_OUTSIDE)
		{
			Result = NULL;
		}
		else if (OctantIndex == ZE_OO_MULTIPLE)
		{
			Result = this;
		}
		else 
		{
			if (Nodes[OctantIndex] != NULL)
				Result = Nodes[OctantIndex]->GetNode(SearchVolume);
			else
				Result = this;
		}
	}
	Lock.UnlockRead();

	return Result;
}

ZE_OCTREE_TEMPLATE
const ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::GetNode(const ZEAABBox& SearchVolume) const
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	ZEInt OctantIndex = FindOctant(SearchVolume);
	Lock.LockRead();
	{
		if (OctantIndex == ZE_OO_OUTSIDE)
		{
			Result = NULL;
		}
		else if (OctantIndex == ZE_OO_MULTIPLE)
		{
			Result = this;
		}
		else 
		{
			if (Nodes[OctantIndex] != NULL)
				Result = Nodes[OctantIndex]->GetNode(SearchVolume);
			else
				Result = this;
		}
	}
	Lock.UnlockRead();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::SetMaxDepth(ZEUInt Depth)
{
	MaxDepth = Depth;
}

ZE_OCTREE_TEMPLATE
ZEUInt ZEOctree<ZE_OCTREE_SPEC>::GetMaxDepth()  const
{
	return MaxDepth;
}

ZE_OCTREE_TEMPLATE
const ZEArray<ZEItemType>& ZEOctree<ZE_OCTREE_SPEC>::GetItems() 
{
	return Items;
}

ZE_OCTREE_TEMPLATE
const ZEArray<const ZEItemType>& ZEOctree<ZE_OCTREE_SPEC>::GetItems() const
{
	return reinterpret_cast<const ZEArray<const ZEItemType>&>(Items);
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::AddItem(const ZEItemType& Item, const ZEVector3& Point)
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	ZEInt ItemOctant = FindOctant(Point);
	Lock.LockWriteNested();
	{
		if (ItemOctant == ZE_OO_MULTIPLE)
		{
			Items.Add(Item);
			UpdateItemCount(1);
			Result = this;
		}
		else if (ItemOctant == ZE_OO_OUTSIDE)
		{
			if (Parent == NULL)
			{
				Expand();
				Result = AddItem(Item, Point);
			}
			else
			{
				Result = Parent->AddItem(Item, Point);
			}
		}
		else
		{
			if (MaxDepth == 0)
			{
				Items.Add(Item);
				UpdateItemCount(1);
				Result = this;
			}
			else
			{
				CreateChildNode(ItemOctant);
				Result = Nodes[ItemOctant]->AddItem(Item, Point);
			}
		}
	}
	Lock.UnlockWrite();

	return Result;
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>* ZEOctree<ZE_OCTREE_SPEC>::AddItem(const ZEItemType& Item, const ZEAABBox& Volume)
{
	ZEOctree<ZE_OCTREE_SPEC>* Result;
	ZEInt ItemOctant = FindOctant(Volume);
	Lock.LockWriteNested();
	{
		if (ItemOctant == ZE_OO_MULTIPLE)
		{
			Items.Add(Item);
			zeDebugCheck(!ZEAABBox::IntersectionTest(GetBoundingBox(), Volume), "Aha");

			UpdateItemCount(1);
			Result = this;
		}
		else if (ItemOctant == ZE_OO_OUTSIDE)
		{
			if (Parent == NULL)
			{
				Expand();
				Result = AddItem(Item, Volume);
			}
			else
			{
				Result = Parent->AddItem(Item, Volume);
			}
		}
		else
		{
			if (MaxDepth == 0)
			{
				Items.Add(Item);
				zeDebugCheck(!ZEAABBox::IntersectionTest(GetBoundingBox(), Volume), "Aha");
				UpdateItemCount(1);
				Result = this;
			}
			else
			{
				CreateChildNode(ItemOctant);
				Result = Nodes[ItemOctant]->AddItem(Item, Volume);
			}
		}
	}
	Lock.UnlockWrite();

	return Result;
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::RemoveItem(ZESize Index)
{
	Lock.LockWriteNested();
	{
		Items.Remove(Index);
		UpdateItemCount(-1);	
		Shrink();
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
bool ZEOctree<ZE_OCTREE_SPEC>::RemoveItem(const ZEItemType& Item)
{
	Lock.LockWriteNested();
	{
		ZESSize Index = Items.FindIndex(Item);
		if (Index != -1)
		{
			Items.Remove(Index);
			UpdateItemCount(-1);
			Lock.UnlockWrite();

			Shrink();

			return true;
		}
		else
		{
			for (ZESize I = 0; I < 8; I++)
			{
				if (Nodes[I] == NULL)
					continue;

				if (Nodes[I]->RemoveItem(Item))
				{
					Lock.UnlockWrite();
					return true;
				}
			}
		}
	}
	Lock.UnlockWrite();

	return false;
}

ZE_OCTREE_TEMPLATE
bool ZEOctree<ZE_OCTREE_SPEC>::RemoveItem(const ZEItemType& Item, const ZEVector3& Point)
{
	Lock.LockWriteNested();
	{
		ZEInt ItemOctant = FindOctant(Point);
		if (ItemOctant == ZE_OO_OUTSIDE)
		{
			Lock.UnlockWrite();
			return false;
		}
		else if (ItemOctant == ZE_OO_MULTIPLE)
		{
			ZEInt Index = Items.FindIndex(Item);
			if (Index != -1)
			{
				Items.Remove(Index);
				UpdateItemCount(-1);
				Shrink();

				Lock.UnlockWrite();
				return true;
			}
		}
		else
		{
			if (Nodes[ItemOctant] != NULL)
			{
				bool Result = Nodes[ItemOctant]->RemoveItem(Item, Point);

				Lock.UnlockWrite();
				return Result;
			}

			Lock. UnlockWrite();
			return false;
		}
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
bool ZEOctree<ZE_OCTREE_SPEC>::RemoveItem(const ZEItemType& Item, const ZEAABBox& Volume)
{
	Lock.LockWriteNested();
	{
		ZEInt ItemOctant = FindOctant(Volume);
		if (ItemOctant == ZE_OO_OUTSIDE)
		{
			Lock.UnlockWrite();
			return false;
		}
		else if (ItemOctant == ZE_OO_MULTIPLE)
		{
			ZESSize Index = Items.FindIndex(Item);
			if (Index != -1)
			{
				Items.Remove(Index);
				UpdateItemCount(-1);
				Shrink();

				Lock.UnlockWrite();
				return true;
			}

			Lock.UnlockWrite();
			return false;
		}
		else
		{
			if (Nodes[ItemOctant] != NULL)
			{
				bool Result = Nodes[ItemOctant]->RemoveItem(Item, Volume);

				Lock.UnlockWrite();
				return Result;
			}

			Lock.UnlockWrite();
			return false;
		}
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
bool ZEOctree<ZE_OCTREE_SPEC>::Check() const
{
	Lock.LockRead();
	{
		// Check Counts
		ZESize ItemCount = 0;
		ZESize NodeCount = 0;

		for (int I = 0; I < 8; I++)
		{
			if (Nodes[I] == NULL)
				continue;

			if (!Nodes[I]->Check())
			{
				Lock.UnlockRead();
				return false;
			}

			NodeCount++;

			ItemCount += Nodes[I]->GetTotalItemCount();
			NodeCount += Nodes[I]->GetTotalNodeCount();
		}

		ItemCount += Items.GetCount();

		if (TotalNodeCount != NodeCount)
		{
			Lock.UnlockRead();
			return false;
		}

		if (TotalItemCount != ItemCount)
		{
			Lock.UnlockRead();
			return false;
		}
	}
	Lock.UnlockRead();

	return true;
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::Clear()
{
	Lock.LockWriteNested();
	{
		UpdateNodeCount(-(ZESSize)TotalNodeCount);
		UpdateItemCount(-(ZESSize)TotalItemCount);

		Items.Clear();
		for (ZEInt I = 0; I < 8; I++)
		{
			if (Nodes[I] != NULL)
			{
				Nodes[I]->Parent = NULL;
				delete Nodes[I];
				Nodes[I] = NULL;
			}
		}
	}
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::LockRead() const
{
	Lock.LockRead();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::UnlockRead() const
{
	Lock.UnlockRead();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::LockWrite()
{
	Lock.LockWrite();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::LockWriteNested()
{
	Lock.LockWriteNested();
}

ZE_OCTREE_TEMPLATE
void ZEOctree<ZE_OCTREE_SPEC>::UnlockWrite()
{
	Lock.UnlockWrite();
}

ZE_OCTREE_TEMPLATE
ZEOctreeIterator<ZE_OCTREE_SPEC> ZEOctree<ZE_OCTREE_SPEC>::Traverse(const ZEViewVolume* Volume)
{
	return ZEOctreeIterator<ZE_OCTREE_SPEC>(this, Volume);
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>::ZEOctree()
{
	Parent = NULL;
	for (ZEInt I = 0; I < 8; I++)
		Nodes[I] = NULL;

	TotalItemCount = 0;
	TotalNodeCount = 0;
	Depth = 0;
	MaxDepth = 1;
	ParentOctant = ZE_OO_MULTIPLE;
	BoundingBox.Min = -2.0f * ZEVector3::One;
	BoundingBox.Min =  2.0f * ZEVector3::One;
}

ZE_OCTREE_TEMPLATE
ZEOctree<ZE_OCTREE_SPEC>::~ZEOctree()
{
	Clear();
}
