//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexBuffer.cpp
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

#include "ZEVertexBuffer.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZEGraphicsModule.h"

#include <memory.h>

/************************************************************************/
/*						ZEVertexBufferElement							*/
/************************************************************************/
ZESize ZEVertexBufferElement::GetHash(const char* Semantic, ZEUInt8 Index)
{
	ZESize Hash = 0;
	ZEHashGenerator::Hash(Hash, (const char*)Semantic);
	ZEHashGenerator::Hash(Hash, (void*)&Index, sizeof(ZEUInt8));

	return Hash;
}

/************************************************************************/
/*							ZEVertexBuffer								*/
/************************************************************************/
bool ZEVertexBuffer::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZEGraphicsResourceType ZEVertexBuffer::GetResourceType() const
{
	return ZE_GRT_BUFFER;
}

ZESize ZEVertexBuffer::GetBufferSize() const
{
	return BufferSize;
}

ZESize ZEVertexBuffer::GetVertexSize() const
{
	return VertexSize;
}

ZEUInt ZEVertexBuffer::GetVertexCount() const
{
	return VertexCount;
}

const ZEVertexBufferElement* ZEVertexBuffer::GetElements() const
{
	return Elements;
}

ZESize ZEVertexBuffer::GetElementCount() const
{
	return ElementCount;
}

void ZEVertexBuffer::ClearElements()
{
	ElementCount = 0;

	memset(Hashes, 0, sizeof(ZESize) * ZE_MAX_VERTEX_LAYOUT_ELEMENT);
	memset(Elements, 0, sizeof(ZEVertexBufferElement) * ZE_MAX_VERTEX_LAYOUT_ELEMENT);
}

void ZEVertexBuffer::RegisterElements(const ZEVertexBufferElement* ElementArr, ZESize Count)
{
	zeDebugCheck(ElementArr == NULL, "NULL pointer");
	zeDebugCheck(Count == 0, "Zero element count");
	zeDebugCheck(Count >= ZE_MAX_VERTEX_LAYOUT_ELEMENT, "Too many elements");

	ClearElements();

	ElementCount = Count;
	for (ZESize I = 0; I < Count; ++I)
	{
		sprintf(Elements[I].Semantic, "%s", ElementArr[I].Semantic);
		Elements[I].Index = ElementArr[I].Index;
		Elements[I].Type = ElementArr[I].Type;
		Elements[I].Offset = ElementArr[I].Offset;
		Elements[I].Usage = ElementArr[I].Usage;
		Elements[I].InstanceCount = ElementArr[I].InstanceCount;

		Hashes[I] = ZEVertexBufferElement::GetHash(Elements[I].Semantic, Elements[I].Index);
	}
}

void ZEVertexBuffer::Unlock()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic vertex buffer unlocked. VertexBuffer: %p", this);
#endif
}

bool ZEVertexBuffer::Lock(void** Data)
{
	zeDebugCheck(Data == NULL, "Null pointer.");
	zeDebugCheck(*Data == NULL, "Null pointer.");
	zeDebugCheck(State.IsLocked, "Already locked.");
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	*Data = ShadowCopy.GetData();
	State.IsLocked = true;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic vartex buffer locked. VertexBuffer: %p, Data: %p.", this, *Data);
#endif

	return true;
}

bool ZEVertexBuffer::CreateDynamic(ZEUInt VertexCount, ZESize VertexSize, const void* VertexData)
{
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(VertexSize == 0, "Zero vertex size.");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");

	ZESize Size = (ZESize)VertexCount * VertexSize;
	ShadowCopy.CreateBuffer(Size, VertexData);

	this->VertexSize = VertexSize;
	this->VertexCount = VertexCount;
	this->BufferSize = (ZESize)VertexCount * VertexSize;
	this->State.IsStatic = false;
	this->State.IsCreated = true;

	TotalCount++;
	TotalSize += BufferSize;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic vertex buffer created. BufferSize: %u, VertexCount: %u, VertexSize: %u", 
			VertexCount * (ZEUInt)VertexSize, VertexCount, (ZEUInt)VertexSize);
#endif

	return true;
}

bool ZEVertexBuffer::CreateStatic(ZEUInt VertexCount, ZESize VertexSize, const void* VertexData)
{
	zeDebugCheck(VertexSize == 0, "Zero vertex size");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(VertexData == NULL, "Buffer must have initial data.");

	this->VertexSize = VertexSize;
	this->VertexCount = VertexCount;
	this->BufferSize = (ZESize)VertexCount * VertexSize;
	this->State.IsStatic = true;
	this->State.IsCreated = true;

	TotalCount++;
	TotalSize += BufferSize;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static vertex buffer created. BufferSize: %u, VertexCount: %u, VertexSize: %u", 
			BufferSize, VertexCount, (ZEUInt)VertexSize);
#endif

	return true;
}

ZESize		ZEVertexBuffer::TotalSize = 0;
ZEUInt16	ZEVertexBuffer::TotalCount = 0;

ZEVertexBuffer::ZEVertexBuffer()
{
	BufferSize = 0;
	VertexSize = 0;
	VertexCount = 0;

	ClearElements();
}

ZEVertexBuffer::~ZEVertexBuffer()
{
	if (GetIsCreated())
	{
		TotalSize -= BufferSize;
		TotalCount--;
	}
}

ZEVertexBuffer*	ZEVertexBuffer::CreateInstance()
{
	return zeGraphics->CreateVertexBuffer();
}
