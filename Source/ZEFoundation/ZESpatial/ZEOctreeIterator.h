//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOctreeIterator.h
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

#include "ZEDS/ZEIterator.h"

#include "ZETypes.h"

#define ZE_OCTREE_ITER_TEMPLATE template<typename ZEItemType, typename ZELockType>
#define ZE_OCTREE_ITER_SPEC ZEItemType, ZELockType

ZE_OCTREE_ITER_TEMPLATE class ZEOctree;
class ZEViewVolume;

ZE_OCTREE_ITER_TEMPLATE
class ZEOctreeIterator : public ZEIterator<ZEItemType>
{
	protected:
		ZEOctree<ZE_OCTREE_ITER_SPEC>*		Octree;
		ZEInt								Octant;
		const ZEViewVolume*					Volume;

	public:
		bool								IsValid() const;

		ZEOctree<ZE_OCTREE_ITER_SPEC>&		GetItem() const;
		ZEOctree<ZE_OCTREE_ITER_SPEC>*		GetPointer() const;

		void								Next();

		ZEOctreeIterator&					operator++();

		bool								operator==(const ZEOctreeIterator& Iterator) const;

		ZEOctree<ZE_OCTREE_ITER_SPEC>&		operator*() const;
		ZEOctree<ZE_OCTREE_ITER_SPEC>*		operator->() const;	

											ZEOctreeIterator(ZEOctree<ZE_OCTREE_ITER_SPEC>* Octree, const ZEViewVolume* Volume);

};

// ZEOctreeIterator
/////////////////////////////////////////////////////////////////////////////

ZE_OCTREE_ITER_TEMPLATE
inline bool ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::IsValid() const
{
	return Octree != NULL;
}

ZE_OCTREE_ITER_TEMPLATE
inline ZEOctree<ZE_OCTREE_ITER_SPEC>& ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::GetItem() const
{
	return *Octree;
}

ZE_OCTREE_ITER_TEMPLATE
inline ZEOctree<ZE_OCTREE_ITER_SPEC>* ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::GetPointer() const
{
	return Octree;
}


ZE_OCTREE_ITER_TEMPLATE
void ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::Next()
{	
	while (Octant <= 7)
	{
		ZEOctree<ZE_OCTREE_ITER_SPEC>* ChildOctree = Octree->GetNode(Octant);
		if (ChildOctree == NULL || ChildOctree->GetTotalItemCount() == 0 || !Volume->IntersectionTest(ChildOctree->GetBoundingBox()))
		{
			Octant++;
			continue;
		}

		Octree = ChildOctree;
		Octant = 0;

		return;
	}

	if (Octree->GetParent() == NULL)
	{
		Octree = NULL;
		return;
	}

	Octant = Octree->GetParentOctant() + 1;
	Octree = Octree->GetParent();

	return Next();
}

ZE_OCTREE_ITER_TEMPLATE
inline ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>& ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::operator++()
{
	Next();
	return *this;
}

ZE_OCTREE_ITER_TEMPLATE
inline bool ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::operator==(const ZEOctreeIterator& Iterator) const
{
	return (Iterator.Octree == Octree && Iterator.Octant = Octant);
}

ZE_OCTREE_ITER_TEMPLATE
inline ZEOctree<ZE_OCTREE_ITER_SPEC>& ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::operator*() const
{
	return GetItem();
}

ZE_OCTREE_ITER_TEMPLATE
inline ZEOctree<ZE_OCTREE_ITER_SPEC>* ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::operator->() const
{
	return &GetItem();
}

ZE_OCTREE_ITER_TEMPLATE
ZEOctreeIterator<ZE_OCTREE_ITER_SPEC>::ZEOctreeIterator(ZEOctree<ZE_OCTREE_ITER_SPEC>* Octree, const ZEViewVolume* Volume)
{
	this->Octree = Octree;
	this->Octant = 0;
	this->Volume = Volume;
}
