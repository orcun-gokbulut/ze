//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAStar.h
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

#include "ZEDS/ZEGraph.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEMinHeap.h"

template <typename Type>
class ZEAStarNode
{
	public:
		size_t ActualNodeIndex;
		ptrdiff_t PrevNodeIndex;

		float SummedCost;
		float EstimatedCost;

		bool operator==(const Type& Other)
		{
			return (Type == ActualNodeIndex);
		}

		bool operator <(const ZEAStarNode<Type>& Other)
		{
			return EstimatedCost < Other.EstimatedCost;
		}

		bool operator >(const ZEAStarNode<Type>& Other)
		{
			return EstimatedCost > Other.EstimatedCost;
		}
};

class ZEAIAStar
{
	private:
		template <typename Type>
		static bool CheckLists(size_t Index, 
			ZEMinHeap<ZEAStarNode<Type>, ZEChunkAllocator<ZEAStarNode<Type>, 100>>& OpenList, 
			const ZEArray<ZEAStarNode<Type>, ZESmartAllocator<ZEAStarNode<Type>, 2>>& ClosedList)
		{
			for (size_t I = 0; I < ClosedList.GetCount(); I++)
				if (ClosedList[I].ActualNodeIndex == Index)
					return true;

			for (size_t I = 0; I < OpenList.GetCount(); I++)
				if (OpenList.GetArray()[I].ActualNodeIndex == Index)
					return true;

			return false;
		}

		template <typename Type>
		static ZEArray<const Type*> GeneratePath(const ZEGraph<Type>& Graph, const ZEArray<ZEAStarNode<Type>>& ClosedList)
		{

		}

	public:
		template <typename Type>
		static ZEArray<const Type*> Process(const ZEGraph<Type>& Graph, size_t StartIndex, size_t DestinationIndex, float (*Cost)(const ZEGraph<Type>&, size_t, size_t), float (*Heuristic)(const ZEGraph<Type>&, size_t, size_t))
		{
			ZEMinHeap<ZEAStarNode<Type>, ZEChunkAllocator<ZEAStarNode<Type>, 100> > OpenList;
			ZEArray<ZEAStarNode<Type>, ZESmartAllocator<ZEAStarNode<Type>, 2> > ClosedList;

			OpenList.Clear();
			ClosedList.Clear();

			ZEAStarNode<Type> Node;
			Node.SummedCost = 0;
			Node.EstimatedCost = Heuristic(Graph, StartIndex, DestinationIndex);
			Node.ActualNodeIndex = StartIndex;
			Node.PrevNodeIndex = -1;

			OpenList.Insert(Node);

			while (OpenList.GetCount() != 0)
			{
				const ZEAStarNode<Type> CurrentNode = OpenList.GetMin();
				OpenList.RemoveMin();
				ClosedList.Add(CurrentNode);

				size_t LinkIndex = 0;
				ptrdiff_t AdjacentNodeIndex = Graph.GetAdjacentNode(CurrentNode.ActualNodeIndex, LinkIndex);
				while(AdjacentNodeIndex != -1)
				{
					if (AdjacentNodeIndex == DestinationIndex)
					{
						ZEArray<const Type*> Output;
						Output.Add(&Graph.GetNode(DestinationIndex));
						const ZEAStarNode<Type>* CurrentNode = &ClosedList.GetItem(ClosedList.GetCount() - 1);
						do
						{
							Output.Add(&Graph.GetNode(CurrentNode->ActualNodeIndex));
							CurrentNode = &ClosedList.GetItem(CurrentNode->PrevNodeIndex);
						}
						while(CurrentNode->PrevNodeIndex != -1);

						Output.Traverse();

						return Output;
					}

					if (!CheckLists(AdjacentNodeIndex, OpenList, ClosedList))
					{
						ZEAStarNode<Type> NewNode;
						NewNode.ActualNodeIndex = AdjacentNodeIndex;
						NewNode.PrevNodeIndex = ClosedList.GetCount() - 1;
						NewNode.SummedCost = CurrentNode.SummedCost + Cost(Graph, CurrentNode.ActualNodeIndex, AdjacentNodeIndex);
						NewNode.EstimatedCost = NewNode.SummedCost + Heuristic(Graph, CurrentNode.ActualNodeIndex, DestinationIndex);
						OpenList.Insert(NewNode);
					}
					LinkIndex++;
					AdjacentNodeIndex = Graph.GetAdjacentNode(CurrentNode.ActualNodeIndex, LinkIndex);
				}
			} 

			return ZEArray<const Type*>();
		}
};
