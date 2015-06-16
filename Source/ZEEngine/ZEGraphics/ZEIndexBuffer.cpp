//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEIndexBuffer.cpp
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

#include "ZEIndexBuffer.h"
#include "ZECore/ZECore.h"
#include "ZEGraphics/ZEGraphicsModule.h"

inline static ZESize GetIndexSize(ZEIndexBufferFormat Format)
{
	static const ZESize FormatToIndexSize[3] = 
	{
		0,	// ZE_IBF_NONE
		2,	// ZE_IBF_INDEX16
		4	// ZE_IBF_INDEX32
	};
	return FormatToIndexSize[Format];
}

bool ZEIndexBuffer::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZEGraphicsResourceType ZEIndexBuffer::GetResourceType() const
{
	return ZE_GRT_BUFFER;
}

ZESize ZEIndexBuffer::GetBufferSize() const
{
	return BufferSize;
}

ZESize ZEIndexBuffer::GetIndexCount() const
{
	return IndexCount;
}

ZEIndexBufferFormat ZEIndexBuffer::GetBufferFormat() const
{
	return Format;
}

void ZEIndexBuffer::Unlock()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic index buffer unlocked. IndexBuffer: %p", this);
#endif
}

bool ZEIndexBuffer::Lock(void** Data)
{
	zeDebugCheck(Data == NULL, "Null pointer.");
	zeDebugCheck(State.IsLocked, "Already locked.");
	zeDebugCheck(*Data == NULL, "Null pointer.");
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	*Data = ShadowCopy.GetData();
	State.IsLocked = true;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic index buffer locked. IndexBuffer: %p, Data: %p.", this, *Data);
#endif

	return true;
}

bool ZEIndexBuffer::CreateDynamic(ZEUInt IndexCount, ZEIndexBufferFormat Format, const void* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(IndexCount == 0, "Cannot create empty buffer.");
	zeDebugCheck(Format == ZE_IBF_NONE, "Unknown buffer format.");

	ZESize Size = GetIndexSize(Format) * (ZESize)IndexCount;
	ShadowCopy.CreateBuffer(Size, InitialData);

	this->Format = Format;
	this->IndexCount = IndexCount;
	this->BufferSize = Size;
	this->State.IsStatic = false;
	this->State.IsCreated = true;

	TotalSize += Size;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Dynamic index buffer created. IndexCount: %u, Format: %u, Size: %u.", 
			IndexCount, Format, (ZEUInt)Size);
#endif

	return true;
}

bool ZEIndexBuffer::CreateStatic(ZEUInt IndexCount, ZEIndexBufferFormat Format, const void* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(IndexCount == 0, "Cannot create empty buffer.");
	zeDebugCheck(InitialData == NULL, "InitialData cannot be null.");
	zeDebugCheck(Format == ZE_IBF_NONE, "Unknown buffer format");

	this->Format = Format;
	this->IndexCount = IndexCount;
	this->BufferSize = GetIndexSize(Format) * (ZESize)IndexCount;
	this->State.IsStatic = true;
	this->State.IsCreated = true;

	TotalSize += BufferSize;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Static index buffer created. IndexCount: %u, Format: %u, Size: %u.", 
			IndexCount, Format, (ZEUInt)BufferSize);
#endif

	return true;
}

ZESize		ZEIndexBuffer::TotalSize = 0;
ZEUInt16	ZEIndexBuffer::TotalCount = 0;

ZEIndexBuffer::ZEIndexBuffer()
{
	BufferSize = 0;
	IndexCount = 0;
	Format = ZE_IBF_NONE;
}

ZEIndexBuffer::~ZEIndexBuffer()
{
	if (GetIsCreated())
	{
		TotalSize -= BufferSize;
		TotalCount--;
	}
}

ZEIndexBuffer* ZEIndexBuffer::CreateInstance()
{
	return zeGraphics->CreateIndexBuffer();
}
