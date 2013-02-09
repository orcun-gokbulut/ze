//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexLayout.cpp
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

#include "ZEError.h"
#include "ZEFile/ZEFile.h"
#include "ZEDefinitions.h"
#include "ZEVertexLayout.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZEGraphicsModule.h"

void ZEVertexLayout::UpdateHash()
{
	if (Dirty)
	{
		Hash = 0;
		Dirty = false;
		for (ZESize I = 0; I < StateData.ElementCount; ++I)
		{
			ZEHashGenerator::Hash(Hash, StateData.VertexElements[I].Semantic);
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].SemanticIndex, sizeof(ZEUInt));
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].Type, sizeof(ZEVertexElementType));	
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].StreamSlot, sizeof(ZEUInt));	
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].ByteOffset, sizeof(ZEUInt));	
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].Usage, sizeof(ZEVertexUsage));
			ZEHashGenerator::Hash(Hash, &StateData.VertexElements[I].InstanceCount, sizeof(ZEUInt));
		}
	}
}

bool ZEVertexLayout::IsEmpty() const
{
	return StateData.ElementCount == 0;
}

void ZEVertexLayout::SetToDefault()
{
	Hash = 0;
	Dirty = false;

	StateData.ElementCount = 0;
}

void ZEVertexLayout::SetLayout(const ZEVertexElement* VertexElements, ZEUInt ElementCount)
{
	zeDebugCheck(VertexElements == NULL, "NULL pointer");
	zeDebugCheck(ElementCount == 0, "Zero element count");
	zeDebugCheck(ElementCount >= ZE_MAX_VERTEX_LAYOUT_ELEMENT, "Too many elements");

	StateData.ElementCount = ElementCount;
	for (ZESize I = 0; I < ElementCount; ++I)
	{
		sprintf(StateData.VertexElements[I].Semantic, "%s", VertexElements[I].Semantic);
		StateData.VertexElements[I].SemanticIndex = VertexElements[I].SemanticIndex;
		StateData.VertexElements[I].Type = VertexElements[I].Type;
		StateData.VertexElements[I].StreamSlot = VertexElements[I].StreamSlot;
		StateData.VertexElements[I].ByteOffset = VertexElements[I].ByteOffset;
		StateData.VertexElements[I].Usage = VertexElements[I].Usage;
		StateData.VertexElements[I].InstanceCount = VertexElements[I].InstanceCount;
	}

	Dirty = true;
}

const ZEVertexElement* ZEVertexLayout::GetLayout() const
{
	return StateData.VertexElements;
}

ZEUInt ZEVertexLayout::GetElementCount() const
{
	return StateData.ElementCount;
}

const ZEVertexLayout& ZEVertexLayout::operator=(const ZEVertexLayout& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;

	memcpy(&StateData, &State.StateData, sizeof(ZEVertexLayoutData));

	return *this;
}

bool ZEVertexLayout::operator==(const ZEVertexLayout& State)
{
	if (State.StateData.ElementCount != StateData.ElementCount)
		return false;

	return memcmp(&StateData.VertexElements, &State.StateData.VertexElements, sizeof(ZEVertexElement) * (ZESize)StateData.ElementCount) == 0 ? true : false;
}

bool ZEVertexLayout::operator!=(const ZEVertexLayout& State)
{
	return !operator==(State);
}

ZEVertexLayout::ZEVertexLayout()
{
	memset(&StateData, 0, sizeof(ZEVertexLayoutData));
	SetToDefault();
}

ZEVertexLayout::~ZEVertexLayout()
{

}
