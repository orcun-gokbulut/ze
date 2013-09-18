//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEConstantBuffer.cpp
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
#include "ZEShader.h"
#include "ZEGraphicsModule.h"
#include "ZEConstantBuffer.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZEShaderMeta.h"

inline static ZESSize FindConstantIndex(const ZEArray<ZEShaderConstant>& Constants, const char* Name)
{
	ZEUInt64 Hash = ZEHashGenerator::Hash(Name);
	ZESize ConstCount = Constants.GetCount();
	
	for (ZESize I = 0; I < ConstCount; ++I)
		if (Constants[I].Hash == Hash)
			if (Constants[I].Name.Equals(Name))
				return I;

	return -1;
}

inline static void CopyFrom(const ZEShaderConstant& Variable, void* DestData, const void* SourceData)
{
	ZEUInt32* Source = (ZEUInt32*)SourceData;
	ZEUInt32* Destination = (ZEUInt32*)DestData;

	for (ZESize RowN = 0; RowN < (ZESize)Variable.RowCount; ++RowN, Destination += 4)
		for (ZESize ColN = 0; ColN < (ZESize)Variable.ColumnCount; ++ColN)
			Destination[ColN]  = *Source++;
}

inline static void CopyTo(const ZEShaderConstant& Variable, void* DestData, const void* SourceData)
{
	ZEUInt32* Source = (ZEUInt32*)SourceData;
	ZEUInt32* Destination = (ZEUInt32*)DestData;

	for (ZESize RowN = 0; RowN < (ZESize)Variable.RowCount; ++RowN, Source += 4)
		for (ZESize ColN = 0; ColN < (ZESize)Variable.ColumnCount; ++ColN)
			Source[ColN]  = *Destination++;
}

bool ZEConstantBuffer::UpdateWith(ZEUInt ShadowIndex)
{
	ShadowCopy.SetChanged(ShadowIndex, false);
	return true;
}

ZESize ZEConstantBuffer::GetSize() const
{
	return Size;
}

const ZEShaderBuffer* ZEConstantBuffer::GetInfo() const
{
	return Info;
}

ZEGraphicsResourceType ZEConstantBuffer::GetResourceType() const
{
	return ZE_GRT_BUFFER;
}

void ZEConstantBuffer::SetZero()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");

	void* Data = ShadowCopy.GetData();
	ZESize Size = ShadowCopy.GetSliceCount() * ShadowCopy.GetSliceSize();

	memset(Data, 0, Size);
}

void ZEConstantBuffer::SetData(void* ConstantData)
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(ConstantData == NULL, "Null pointer.");

	void* Data = ShadowCopy.GetData();
	ZESize Size = ShadowCopy.GetSliceCount() * ShadowCopy.GetSliceSize();

	memcpy(Data, ConstantData, Size);
}

bool ZEConstantBuffer::SetConstant(ZEUInt Index, const void* SourceData)
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	void* Data = ShadowCopy.GetData();

	const ZEShaderConstant& Constant = Info->Constants[(ZESize)Index];
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	return true;
}

bool ZEConstantBuffer::SetConstant(const char* Name, const void* SourceData)
{
	zeDebugCheck(Name == NULL, "NUll pointer.");
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	zeDebugCheck(Index < 0, "No constant found with specified name");

	void* Data = ShadowCopy.GetData();

	const ZEShaderConstant& Constant = Info->Constants[Index]; 
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	return true;
}

bool ZEConstantBuffer::GetConstant(ZEUInt Index, void* DestinationData) const
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	const void* Data = ShadowCopy.GetConstData();

	const ZEShaderConstant& Constant = Info->Constants[(ZESize)Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

bool ZEConstantBuffer::GetConstant(const char* Name, void* DestinationData) const
{
	zeDebugCheck(Name == NULL, "NUll pointer.");
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	zeDebugCheck(Index < 0, "No constant found with specified name");

	const void* Data = ShadowCopy.GetConstData();

	const ZEShaderConstant& Constant = Info->Constants[Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

void ZEConstantBuffer::Unlock()
{
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(State.IsStatic, "Static buffer not be locked/unlocked.");

	State.IsLocked = false;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer unlocked. ConstantBuffer: %p", this);
#endif
}

bool ZEConstantBuffer::Lock(void** Data)
{
	zeDebugCheck(Data == NULL, "Null pointer.");
	zeDebugCheck(GetIsLocked(), "Already locked.");
	zeDebugCheck(!GetIsCreated(), "Buffer not created.");
	zeDebugCheck(GetIsStatic(), "Static buffer not be locked/unlocked.");

	*Data = ShadowCopy.GetData();
	State.IsLocked = true;
	
#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer locked. ConstantBuffer: %p, Data: %p.", this, *Data);
#endif

	return true;
}

bool ZEConstantBuffer::Create(const ZEShaderBuffer* BufferInfo)
{
	zeDebugCheck(BufferInfo == NULL, "NUll pointer.");
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(BufferInfo->Constants.GetCount() == 0, "Cannot create with empty table");
	zeDebugCheck(BufferInfo->Size == 0, "Cannot create zero sized buffer.");
	
	ShadowCopy.CreateBuffer(BufferInfo->Size, NULL);

	this->Info = BufferInfo;
	this->Size = BufferInfo->Size;
	this->State.IsStatic = false;
	this->State.IsCreated = true;

	TotalSize += BufferInfo->Size;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer created by buffer info. Name: %s, ConstantCount: %u, Size: %u.", 
			BufferInfo->Name, BufferInfo->Constants.GetCount(), BufferInfo->Size);
#endif

	return true;
}

bool ZEConstantBuffer::Create(ZESize BufferSize)
{	
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(BufferSize == 0, "Cannot create zero sized buffer.");

	ShadowCopy.CreateBuffer(BufferSize, NULL);

	this->Info = NULL;
	this->Size = BufferSize;
	this->State.IsStatic = false;
	this->State.IsCreated = true;
	
	TotalSize += BufferSize;
	TotalCount++;

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer created by size. Size: %u.", BufferSize);
#endif

	return true;
}

ZESize		ZEConstantBuffer::TotalSize = 0;
ZEUInt16	ZEConstantBuffer::TotalCount = 0;

ZEConstantBuffer::ZEConstantBuffer()
{
	Size = 0;
	Info = NULL;
}

ZEConstantBuffer::~ZEConstantBuffer()
{
	if (GetIsCreated())
	{
		TotalSize -= Size;
		TotalCount--;
	}
}

ZEConstantBuffer* ZEConstantBuffer::CreateInstance()
{
	return zeGraphics->CreateConstantBuffer();
}
