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

static ZESize SearchIndex = 0;
static bool ActiveInstances[ZE_MAX_CONSTANT_BUFFER_INSTANCE] = {false};

ZESSize FindFirstEmptyIndex()
{
	ZEUInt32 SearchCount = 0;
	while (ZE_MAX_CONSTANT_BUFFER_INSTANCE > SearchCount++)
	{
		SearchIndex = ++SearchIndex % ZE_MAX_CONSTANT_BUFFER_INSTANCE;
		
		if (!ActiveInstances[SearchIndex])
			return (ZESSize)SearchIndex;
	}

	return -1;
}

/************************************************************************/
/*                      ZEConstantBufferLink                            */
/************************************************************************/
const ZEShaderConstant* ZEConstantBufferLink::GetShaderConstant() const
{
	return &Buffer->GetInfo()->Constants[Index];
}

bool ZEConstantBufferLink::SetValue(const void* Source)
{
	zeDebugCheck(!ActiveInstances[BufferId], "Requested buffer is not available, probably destroyed.");

	return Buffer->SetConstant(Index, Source);
}

bool ZEConstantBufferLink::GetValue(void* Destination) const
{
	zeDebugCheck(!ActiveInstances[BufferId], "Requested buffer is not available, probably destroyed.");

	return Buffer->GetConstant(Index, Destination);
}

ZEConstantBufferLink::ZEConstantBufferLink(ZEConstantBuffer* ConstantBuffer, ZESSize ConstantBufferId, ZESize ConstantIndex)
{
	Index = ConstantIndex;
	Buffer = ConstantBuffer;
	BufferId = ConstantBufferId;
}

ZEConstantBufferLink::~ZEConstantBufferLink()
{

}

/************************************************************************/
/*                       ZEConstantBuffer                               */
/************************************************************************/
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

bool ZEConstantBuffer::NeedUpdate() const
{
	return (State.InUse || State.Locked || State.Deleted || !State.Changed) ? false : true;
}

bool ZEConstantBuffer::UpdateBuffer()
{
	State.Changed = false;

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

bool ZEConstantBuffer::Lock(void** ConstantData)
{
	zeDebugCheck(ConstantData == NULL, "Null pointer.");
	
	*ConstantData = Data;
	State.Locked = true;
	
	return true;
}

void ZEConstantBuffer::Unlock(bool NotChanged)
{
	if (!State.Locked)
		return;

	State.Locked = false;
	State.Changed = NotChanged ? false : true;
}

ZEConstantBufferLink ZEConstantBuffer::GetConstantLink(ZESize Index)
{
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");

	ZEConstantBufferLink Link(this, Id, Index);
	return Link;
}

ZEConstantBufferLink ZEConstantBuffer::GetConstantLink(const char* Name)
{
	zeDebugCheck(Name == NULL, "NUll pointer.");
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	
	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	zeDebugCheck(Index < 0, "No constant found with specified name");

	ZEConstantBufferLink Link(this, Id, Index);
	return Link;
}

bool ZEConstantBuffer::SetConstant(ZESize Index, const void* SourceData)
{
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	const ZEShaderConstant& Constant = Info->Constants[Index];
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	State.Changed = true;

	return true;
}

bool ZEConstantBuffer::SetConstant(const char* Name, const void* SourceData)
{
	zeDebugCheck(Name == NULL, "NUll pointer.");
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	zeDebugCheck(Index < 0, "No constant found with specified name");

	const ZEShaderConstant& Constant = Info->Constants[Index]; 
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	State.Changed = true;

	return true;
}

bool ZEConstantBuffer::GetConstant(ZESize Index, void* DestinationData) const
{
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	const ZEShaderConstant& Constant = Info->Constants[Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

bool ZEConstantBuffer::GetConstant(const char* Name, void* DestinationData) const
{
	zeDebugCheck(Name == NULL, "NUll pointer.");
	zeDebugCheck(Data == NULL, "Buffer not created.");
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	zeDebugCheck(Index < 0, "No constant found with specified name");

	const ZEShaderConstant& Constant = Info->Constants[Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

bool ZEConstantBuffer::Create(const ZEShaderBuffer* BufferInfo)
{
	zeDebugCheck(BufferInfo == NULL, "NUll pointer.");
	zeDebugCheck(BufferInfo->Size == 0, "Cannot create zero sized buffer.");
	zeDebugCheck(Id < 0, "Maximum constant buffer instance reached.");

	Data = new ZEUInt8[BufferInfo->Size];
	Size = BufferInfo->Size;
	Info = BufferInfo;

	return true;
}

bool ZEConstantBuffer::Create(ZESize BufferSize)
{	
	zeDebugCheck(BufferSize == 0, "Cannot create zero sized buffer.");
	zeDebugCheck(Id < 0, "Maximum constant buffer instance reached.");

	Data = new ZEUInt8[BufferSize];
	Size = BufferSize;
	Info = NULL;

	return true;
}

void ZEConstantBuffer::Destroy()
{
	if (State.InUse)
	{
		State.Deleted = true;
		return;
	}

	delete this;
}

ZEConstantBuffer::ZEConstantBuffer()
{
	Size = 0;
	Data = NULL;
	Info = NULL;
	Id = FindFirstEmptyIndex();

	State.InUse = false;
	State.Changed = false;
	State.Deleted = false;
	State.Locked = false;

	ActiveInstances[Id] = true;
}

ZEConstantBuffer::~ZEConstantBuffer()
{
	if (Data != NULL)
	{
		delete [] Data;
		Data = NULL;
	}

	ActiveInstances[Id] = false;
}

ZEConstantBuffer* ZEConstantBuffer::CreateInstance()
{
	return zeGraphics->CreateConstantBuffer();
}
