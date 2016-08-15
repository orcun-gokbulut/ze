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
#include "ZEMath/ZEAABBox.h"
#include "ZEMath/ZEVector.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZETriangle.h"
#include "ZEOctreeIterator.h"

#define ZE_OO_OUTSIDE	-2
#define ZE_OO_MULTIPLE	-1

template<typename ZEItemType>
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

		void									Expand();
		void									Shrink();
		void									CreateChildNode(ZEInt Octant);

	public:
		ZEOctree*								GetParent();
		const ZEOctree*							GetParent() const;
		ZEUInt									GetParentOctant() const;	
		ZEUInt									GetDepth() const;

		void									SetBoundingBox(const ZEAABBox& BoundingBox);
		const ZEAABBox&							GetBoundingBox() const;

		void									SetMaxDepth(ZEUInt Depth);
		ZEUInt									GetMaxDepth() const;

		ZEOctree*								GetNode(ZESize Octant);
		const ZEOctree*							GetNode(ZESize Octant) const;

		ZEOctree*								GetNode(const ZEVector3& Point);
		const ZEOctree*							GetNode(const ZEVector3& Point) const;

		ZEOctree*								GetNode(const ZEAABBox& BoundingBox);
		const ZEOctree*							GetNode(const ZEAABBox& BoundingBox) const;

		ZEInt									FindOctant(const ZEVector3& Point) const;
		ZEInt									FindOctant(const ZEAABBox& BoundingBox) const;

		ZESize									GetItemCount() const;	
		const ZEArray<ZEItemType>&				GetItems();
		const ZEArray<const ZEItemType>&		GetItems() const;
		ZEItemType&								GetItem(ZESize Index);
		const ZEItemType&						GetItem(ZESize Index) const;

		void									AddItem(const ZEItemType& Item, const ZEVector3& Point);
		void									AddItem(const ZEItemType& Item, const ZEAABBox& Volume);

		void									RemoveItem(ZESize Index);
		bool									RemoveItem(const ZEItemType& Item);
		bool									RemoveItem(const ZEItemType& Item, const ZEVector3& Point);
		bool									RemoveItem(const ZEItemType& Item, const ZEAABBox& Volume);

		void									Clear();

		ZEOctreeIterator<ZEItemType>			Traverse(const ZEViewVolume* Volume);

												ZEOctree();
												~ZEOctree();
};


// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////

template<typename ZEItemType>
ZEInt ZEOctree<ZEItemType>::FindOctant(const ZEVector3& Point) const
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

template<typename ZEItemType>
ZEInt ZEOctree<ZEItemType>::FindOctant(const ZEAABBox& BoundingBox) const
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

template<typename ZEItemType>
void ZEOctree<ZEItemType>::Expand()
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
		ZEOctree<ZEItemType>* Temp = Nodes[I];
		Nodes[I] = NULL;
		
		// Generate New Child
		CreateChildNode(I);
		Nodes[I]->Depth = Depth;
		Nodes[I]->MaxDepth = MaxDepth;

		// Add Old Child to New Child
		Temp->Parent = Nodes[I];
		Temp->ParentOctant = (~I) & 0x07;
		Nodes[I]->Nodes[Temp->ParentOctant] = Temp;
	}

	// Uplift
	Depth = Depth - 1;
	MaxDepth = MaxDepth + 1;
}


template<typename ZEItemType>
void ZEOctree<ZEItemType>::Shrink()
{
	if (Items.GetCount() != 0)
		return;

	for (ZEInt I = 0; I < 8; I++)
	{
		if (Nodes[I] != NULL)
			return;
	}

	if (Parent != NULL)
	{
		ZEOctree<ZEItemType>* ParentPointer = Parent;

		ParentPointer->Nodes[ParentOctant] = NULL;
		delete this;

		ParentPointer->Shrink();
	}
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::CreateChildNode(ZEInt Octant)
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
}

template<typename ZEItemType>
ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetParent()
{
	return Parent;
}

template<typename ZEItemType>
const ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetParent() const
{
	return Parent;
}

template<typename ZEItemType>
ZEUInt ZEOctree<ZEItemType>::GetDepth() const
{
	return Depth;
}

template<typename ZEItemType>
	ZEUInt ZEOctree<ZEItemType>::GetParentOctant() const
{
	return ParentOctant;
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::SetBoundingBox(const ZEAABBox& BoundingBox)
{
	zeCheckError(Parent != NULL, ZE_VOID, "You can only change bounding box of a root octree.");
	this->BoundingBox = BoundingBox;
}

template<typename ZEItemType>
const ZEAABBox& ZEOctree<ZEItemType>::GetBoundingBox()  const
{
	return BoundingBox;
}

template<typename ZEItemType>
ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(ZESize OctantIndex)
{
	zeDebugCheck(OctantIndex >= 8, "Index out of bounds.");
	return Nodes[OctantIndex];
}

template<typename ZEItemType>
const ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(ZESize OctantIndex) const 
{
	zeDebugCheck(OctantIndex >= 8, "Index out of bounds.");
	return Nodes[OctantIndex];
}

template<typename ZEItemType>
ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(const ZEVector3& Point)
{
	if (!ZEAABBox::IntersectionTest(BoundingBox, Point))
		return NULL;

	ZEInt OctantIndex = FindOctant(Point);
	if (Nodes[OctantIndex] != NULL)
		return Nodes[OctantIndex]->GetNode(Point);
	else
		return this;
}

template<typename ZEItemType>
const ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(const ZEVector3& Point) const
{
	ZEInt OctantIndex = FindOctant(Point);
	if (OctantIndex == ZE_OO_OUTSIDE)
	{
		return NULL;
	}
	else
	{
		if (Nodes[OctantIndex] != NULL)
			return Nodes[OctantIndex]->GetNode(Point);
		else
			return this;
	}
}

template<typename ZEItemType>
ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(const ZEAABBox& SearchVolume)
{
	ZEInt OctantIndex = FindOctant(SearchVolume);
	if (OctantIndex == ZE_OO_OUTSIDE)
	{
		return NULL;
	}
	else if (OctantIndex == ZE_OO_MULTIPLE)
	{
		return this;
	}
	else 
	{
		if (Nodes[OctantIndex] != NULL)
			return Nodes[OctantIndex]->GetNode(SearchVolume);
		else
			return this;
	}
}

template<typename ZEItemType>
const ZEOctree<ZEItemType>* ZEOctree<ZEItemType>::GetNode(const ZEAABBox& SearchVolume) const
{
	ZEInt OctantIndex = FindOctant(SearchVolume);
	if (OctantIndex == ZE_OO_OUTSIDE)
	{
		return NULL;
	}
	else if (OctantIndex == ZE_OO_MULTIPLE)
	{
		return this;
	}
	else 
	{
		if (Nodes[OctantIndex] != NULL)
			return Nodes[OctantIndex]->GetNode(SearchVolume);
		else
			return this;
	}
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::SetMaxDepth(ZEUInt Depth)
{
	MaxDepth = Depth;
}

template<typename ZEItemType>
ZEUInt ZEOctree<ZEItemType>::GetMaxDepth()  const
{
	return MaxDepth;
}

template<typename ZEItemType>
ZESize ZEOctree<ZEItemType>::GetItemCount()  const
{
	return Items.GetCount();
}

template<typename ZEItemType>
const ZEItemType& ZEOctree<ZEItemType>::GetItem(ZESize Index) const
{
	return Items[Index];
}

template<typename ZEItemType>
ZEItemType& ZEOctree<ZEItemType>::GetItem(ZESize Index)
{
	zeBreak(Items[Index] == (void*)0xDDDDDDDDDDDDDDDD);
	return Items[Index];
}

template<typename ZEItemType>
const ZEArray<ZEItemType>& ZEOctree<ZEItemType>::GetItems() 
{
	return Items;
}

template<typename ZEItemType>
const ZEArray<const ZEItemType>& ZEOctree<ZEItemType>::GetItems() const
{
	return reinterpret_cast<const ZEArray<const ZEItemType>&>(Items);
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::AddItem(const ZEItemType& Item, const ZEVector3& Point)
{
	ZEInt ItemOctant = FindOctant(Point);
	if (ItemOctant == ZE_OO_MULTIPLE)
	{
		Items.Add(Item);
	}
	else if (ItemOctant == ZE_OO_OUTSIDE)
	{
		if (Parent == NULL)
		{
			Expand();
			AddItem(Item, Point);
		}
		else
		{
			Parent->AddItem(Item, Point);
		}
	}
	else
	{
		if (MaxDepth == 0)
		{
			Items.Add(Item);
		}
		else
		{
			CreateChildNode(ItemOctant);
			Nodes[ItemOctant]->AddItem(Item, Point);
		}
	}
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::AddItem(const ZEItemType& Item, const ZEAABBox& Volume)
{
	ZEInt ItemOctant = FindOctant(Volume);
	if (ItemOctant == ZE_OO_MULTIPLE)
	{
		Items.Add(Item);
	}
	else if (ItemOctant == ZE_OO_OUTSIDE)
	{
		if (Parent == NULL)
		{
			Expand();
			AddItem(Item, Volume);
		}
		else
		{
			Parent->AddItem(Item, Volume);
		}
	}
	else
	{
		if (MaxDepth == 0)
		{
			Items.Add(Item);
		}
		else
		{
			CreateChildNode(ItemOctant);
			Nodes[ItemOctant]->AddItem(Item, Volume);
		}
	}
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::RemoveItem(ZESize Index)
{
	Items.Remove(Index);
	Shrink();
}

template<typename ZEItemType>
bool ZEOctree<ZEItemType>::RemoveItem(const ZEItemType& Item)
{
	ZEInt Index = Items.FindIndex(Item);
	if (Index != -1)
	{
		Items.Remove(Index);
		Shrink();
		return true;
	}
	else
	{
		for (ZESize I = 0; I < 8; I++)
		{
			if (Nodes[I] != NULL)
			{
				bool Result = Nodes[I]->RemoveItem(Item);
				if (Result)
					return true;
			}
		}

		return false;
	}
}

template<typename ZEItemType>
bool ZEOctree<ZEItemType>::RemoveItem(const ZEItemType& Item, const ZEVector3& Point)
{
	ZEInt ItemOctant = FindOctant(Point);
	if (ItemOctant == ZE_OO_OUTSIDE)
	{
		return false;
	}
	else if (ItemOctant == ZE_OO_MULTIPLE)
	{
		ZEInt Index = Items.FindIndex(Item);
		if (Index != -1)
		{
			Items.Remove(Index);
			Shrink();
			return true;
		}
	}
	else
	{
		if (Nodes[ItemOctant] != NULL)
			return Nodes[ItemOctant]->RemoveItem(Item, Point);		
	}
}

template<typename ZEItemType>
bool ZEOctree<ZEItemType>::RemoveItem(const ZEItemType& Item, const ZEAABBox& Volume)
{
	ZEInt ItemOctant = FindOctant(Volume);
	if (ItemOctant == ZE_OO_OUTSIDE)
	{
		return false;
	}
	else if (ItemOctant == ZE_OO_MULTIPLE)
	{
		ZEInt Index = Items.FindIndex(Item);
		if (Index != -1)
		{
			Items.Remove(Index);
			Shrink();
			return true;
		}

		return false;
	}
	else
	{
		if (Nodes[ItemOctant] != NULL)
			return Nodes[ItemOctant]->RemoveItem(Item, Volume);		
	}
}

template<typename ZEItemType>
void ZEOctree<ZEItemType>::Clear()
{
	Items.Clear();
	for (ZEInt I = 0; I < 8; I++)
	{
		if (Nodes[I] != NULL)
		{
			delete Nodes[I];
			Nodes[I] = NULL;
		}
	}
}

template<typename ZEItemType>
ZEOctreeIterator<ZEItemType> ZEOctree<ZEItemType>::Traverse(const ZEViewVolume* Volume)
{
	return ZEOctreeIterator<ZEItemType>(this, Volume);
}

template<typename ZEItemType>
ZEOctree<ZEItemType>::ZEOctree()
{
	Parent = NULL;
	for (ZEInt I = 0; I < 8; I++)
		Nodes[I] = NULL;

	Depth = 0;
	MaxDepth = 1;
	ParentOctant = ZE_OO_MULTIPLE;
	BoundingBox.Min = -2.0f * ZEVector3::One;
	BoundingBox.Max =  2.0f * ZEVector3::One;
}

template<typename ZEItemType>
ZEOctree<ZEItemType>::~ZEOctree()
{
	for (ZEInt I = 0; I < 8; I++)
	{
		if (Nodes[I] != NULL)
			delete Nodes[I];
	}
}
