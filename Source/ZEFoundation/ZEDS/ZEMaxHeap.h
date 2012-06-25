//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaxHeap.h
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
#ifndef __ZE_MAX_HEAP_H__
#define __ZE_MAX_HEAP_H__

#include "ZETypes.h"
#include "ZEHeapBase.h"

template< typename Type, typename Allocator_ = ZEChunkAllocator<Type, 100> >
class  ZEMaxHeap : public ZEHeapBase<Type, Allocator_>
{
	private:
		void Swap(Type& One, Type& Two)
		{
			Type Temp = One;
			One = Two;
			Two = Temp;
		}

		void MaxHeapify(ZESize Index)
		{
			ZESize Largest = Index;

			ZESize LeftIndex = this->GetFirstChildIndex(Index);
			if (LeftIndex < this->Heap.GetCount() && this->Heap[LeftIndex] > this->Heap[Index])
				Largest = LeftIndex;

			ZESize RightIndex = this->GetSecondChildIndex(Index);
			if (RightIndex < this->Heap.GetCount() && this->Heap[RightIndex] > this->Heap[Largest])
				Largest = RightIndex;

			if (Largest != Index)
			{
				Swap(this->Heap[Index], this->Heap[Largest]);
				MaxHeapify(Largest);
			}
		}

	public:
		void Create(Type* Array, ZESize Size)
		{
			this->Heap.CopyFrom(Array, Size);

			for (ZESSize I = this->Heap.GetSize() / 2; I >= 0; I--)
				MaxHeapify(I);
		}

		void Create(const ZEArray<Type>& Array)
		{
			Create(Array.GetCArray(), Array.GetCount());
		}

		Type& GetMax()
		{
			return this->Heap[0];
		}

		const Type& GetMax() const
		{
			return this->Heap[0];
		}

		void InsertValue(Type Value)
		{
			Insert(Value);
		}

		void Insert(Type& Value)
		{
			this->Heap.Add(Value);

			ZESize Index = this->Heap.GetCount() - 1;
			while(Index != 0)
			{
				ZESize ParentIndex = this->GetParentIndex(Index);

				if (this->Heap[Index] < this->Heap[ParentIndex])
					break;

				Swap(this->Heap[Index], this->Heap[ParentIndex]);
				Index = ParentIndex;
			}
		}

		void Remove(ZESize Index)
		{
			this->Heap[Index] = this->Heap.GetLastItem();
			this->Heap.DeleteAt(this->Heap.GetCount() - 1);

			MaxHeapify(Index);
		}

		void RemoveMax()
		{
			Remove(0);
		}

		ZEMaxHeap<Type, Allocator_>& operator=(const ZEMaxHeap<Type, Allocator_>& Other)
		{
			this->Heap.CopyFrom(Other.Heap);
			return *this;
		}

		ZEMaxHeap(const ZEMaxHeap<Type, Allocator_>& Other)
		{
			this->Heap.CopyFrom(Other.Heap);
		}

		ZEMaxHeap()
		{

		}
};
#endif
