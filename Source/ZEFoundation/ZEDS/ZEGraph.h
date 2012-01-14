//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGraph.h
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
#ifndef __ZE_GRAPH_H__
#define __ZE_GRAPH_H__

#include "ZEArray.h"

struct ZEGraphLink
{
	size_t Nodes[2];
};

template<typename Type, typename NodeAllocator = ZEAllocatorBase<Type>, typename LinkAllocator = ZEAllocatorBase<ZEGraphLink>>
class ZEGraph
{
	private:
		ZEArray<Type, NodeAllocator> Nodes;
		ZEArray<ZEGraphLink, LinkAllocator> Links;

	public:
		bool CheckAdjacency(size_t NodeIndex1, size_t NodeIndex2) const
		{
			for (size_t I = 0; I < Links.GetCount(); I++)
				if ((Links[I].Nodes[0] == NodeIndex1 && Links[I].Nodes[1] == NodeIndex2) || 
					(Links[I].Nodes[0] == NodeIndex2 && Links[I].Nodes[1] == NodeIndex1))
					return true;

			return false;
		}

		int GetAdjacentNode(size_t NodeIndex, size_t& LastLinkIndex) const
		{
			for (size_t I = LastLinkIndex; I < Links.GetCount(); I++)
			{
				if (Links[I].Nodes[0] == NodeIndex)
				{
					LastLinkIndex = I;
					return Links[I].Nodes[1];
				}
				else if (Links[I].Nodes[1] == NodeIndex)
				{
					LastLinkIndex = I;
					return Links[I].Nodes[0];
				}
			}

			return -1;
		}

		ZEArray<size_t> GetAdjacentNodes(size_t NodeIndex) const
		{
			ZEArray<size_t> Indexes;

			for (size_t I = 0; I < Links.GetCount(); I++)
				if (Links[I].Nodes[0] == NodeIndex || Links[I].Nodes[1] == NodeIndex)
					Indexes.Add(I);

			return Indexes;
		}

		void Clear()
		{
			Nodes.Clear();
			Links.Clear();
		}

		const ZEArray<Type>& GetNodes() const
		{
			return Nodes;
		}

		Type& GetNode(size_t Index)
		{
			return Nodes[Index];
		}

		const Type& GetNode(size_t Index) const
		{
			return Nodes[Index];
		}

		size_t GetNodeCount() const
		{
			return Nodes.GetCount();
		}

		void AddNode(const Type& Node)
		{
			Nodes.Add(Node);
		}

		void MassAddNode(size_t Count)
		{
			Nodes.MassAdd(Count);
		}

		void RemoveNode(size_t NodeIndex)
		{
			for (size_t I = 0; I < Links.GetCount(); I++)
				if (Links[I].Nodes[0] == NodeIndex || Links[I].Nodes[1] == NodeIndex)
				{
					Links.DeleteAt(I);
					I--;
				}
		}
		
		void ClearNodes() const
		{
			Nodes.Clear();
			Links.Clear();
		}

		const ZEArray<ZEGraphLink>& GetLinks() const
		{
			return Links;
		}

		Type& GetLink(size_t Index)
		{
			return Links[Index];
		}

		const Type& GetLink(size_t Index) const
		{
			return Links[Index];
		}

		size_t GetLinkCount() const
		{
			return Links.GetCount();
		}

		void AddLink(size_t NodeIndex1, size_t NodeIndex2)
		{
			ZEGraphLink Link;
			Link.Nodes[0] = NodeIndex1;
			Link.Nodes[1] = NodeIndex2;
			Links.Add(Link);
		}

		void RemoveLink(size_t LinkIndex)
		{
			Links.DeleteAt(LinkIndex);
		}

		void RemoveLink(size_t NodeIndex1, size_t NodeIndex2)
		{
			for (size_t I = 0; I < Links.GetCount(); I++)
				if ((Links[I].Nodes[0] == NodeIndex1 && Links[I].Nodes[1] == NodeIndex2) || 
					(Links[I].Nodes[0] == NodeIndex2 && Links[I].Nodes[1] == NodeIndex1))
				{
					Links.DeleteAt(I);
					break;
				}
		}

		void ClearLinks()
		{
			Links.Clear();
		}
};

#endif
