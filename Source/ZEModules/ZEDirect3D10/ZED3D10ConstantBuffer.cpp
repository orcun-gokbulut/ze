//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10ConstantBuffer.cpp
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
#include "ZED3D10ConstantBuffer.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEGraphics/ZEShaderMetaInfo.h"

#include <d3d10.h>

inline static ZESSize FindConstantIndex(const ZEArray<ZEShaderConstantInfo>& Constants, const ZEString& Name)
{
	ZEUInt64 Hash = Name.Hash();
	ZESize ConstCount = Constants.GetCount();
	
	for (ZESize I = 0; I < ConstCount; ++I)
		if (Constants[I].Hash == Hash)
			if (Constants[I].Name.Equals(Name))
				return I;

	return -1;
}

inline static void CopyFrom(const ZEShaderConstantInfo& Variable, void* DestData, const void* SourceData)
{
	ZEUInt32* Source = (ZEUInt32*)SourceData;
	ZEUInt32* Destination = (ZEUInt32*)DestData;

	for (ZESize RowN = 0; RowN < (ZESize)Variable.RowCount; ++RowN, Destination += 4)
		for (ZESize ColN = 0; ColN < (ZESize)Variable.ColumnCount; ++ColN)
			Destination[ColN]  = *Source++;
}

inline static void CopyTo(const ZEShaderConstantInfo& Variable, void* DestData, const void* SourceData)
{
	ZEUInt32* Source = (ZEUInt32*)SourceData;
	ZEUInt32* Destination = (ZEUInt32*)DestData;

	for (ZESize RowN = 0; RowN < (ZESize)Variable.RowCount; ++RowN, Source += 4)
		for (ZESize ColN = 0; ColN < (ZESize)Variable.ColumnCount; ++ColN)
			Source[ColN]  = *Destination++;
}

void ZED3D10ConstantBuffer::UpdateData()
{
	if (NeedUpdate)
	{
		void* Mapped = NULL;
		if (FAILED(D3D10Buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &Mapped)))
		{
			zeError("Cannot update constant buffer data.");
			return;
		}

		memcpy(Mapped, Data, Size);

		D3D10Buffer->Unmap();
		NeedUpdate = false;
	}
}

ZED3D10ConstantBuffer::ZED3D10ConstantBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D10ConstantBuffer::~ZED3D10ConstantBuffer()
{
	ZED3D_RELEASE(D3D10Buffer);

	if (Data != NULL)
		delete [] Data;
}

ID3D10Buffer* ZED3D10ConstantBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D10ConstantBuffer::Lock(void** ConstantData)
{
	zeDebugCheck(ConstantData == NULL, "Null pointer.");
	
	*ConstantData = Data;
	
	return true;
}

void ZED3D10ConstantBuffer::Unlock()
{
	NeedUpdate = true;
}

bool ZED3D10ConstantBuffer::SetConstant(ZESize Index, const void* SourceData)
{
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	const ZEShaderConstantInfo& Constant = Info->Constants[Index]; 
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	NeedUpdate = true;

	return true;
}

bool ZED3D10ConstantBuffer::GetConstant(ZESize Index, void* DestinationData) const
{
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");
	zeDebugCheck(Index >= Info->Constants.GetCount(), "Index out of range.");
	
	const ZEShaderConstantInfo& Constant = Info->Constants[Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

bool ZED3D10ConstantBuffer::SetConstant(const ZEString& Name, const void* SourceData)
{
	zeDebugCheck(SourceData == NULL, "Null pointer.");
	zeDebugCheck(Name.IsEmpty(), "Index out of range.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	if (Index < 0)
		return false;

	const ZEShaderConstantInfo& Constant = Info->Constants[Index]; 
	CopyFrom(Constant, (ZEUInt8*)Data + Constant.Offset, SourceData);

	NeedUpdate = true;

	return true;
}

bool ZED3D10ConstantBuffer::GetConstant(const ZEString& Name, void* DestinationData) const
{
	zeDebugCheck(Name.IsEmpty(), "Index out of range.");
	zeDebugCheck(DestinationData == NULL, "Null pointer.");
	zeDebugCheck(Info == NULL, "Buffer not created with a ZEShaderBufferInfo");

	ZESSize Index = FindConstantIndex(Info->Constants, Name);

	if (Index < 0)
		return false;

	const ZEShaderConstantInfo& Constant = Info->Constants[Index]; 
	CopyTo(Constant, DestinationData, (ZEUInt8*)Data + Constant.Offset);

	return true;
}

bool ZED3D10ConstantBuffer::Create(ZESize BufferSize)
{
	zeDebugCheck(Data != NULL, "Already created");
	zeDebugCheck(BufferSize == 0, "Zero size.");
	
	// Create buffer
	D3D10_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D10_USAGE_DYNAMIC;
	Desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferSize;
	
	if (FAILED(D3D10Device->CreateBuffer(&Desc, NULL, &D3D10Buffer)))
	{
		zeError("Cannot create constant buffer");
		return false;
	}

	Data = new ZEUInt8[BufferSize];
	Size = BufferSize;
	Info = NULL;

	return true;
}

bool ZED3D10ConstantBuffer::Create(const ZEShaderBufferInfo* BufferInfo)
{
	zeDebugCheck(Data != NULL, "Already created");
	zeDebugCheck(BufferInfo == NULL, "NUll pointer");
	zeDebugCheck(BufferInfo->Constants.GetCount() == 0, "Empty table");

	// Create buffer
	D3D10_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D10_USAGE_DYNAMIC;
	Desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferInfo->Size;
	
	if (FAILED(D3D10Device->CreateBuffer(&Desc, NULL, &D3D10Buffer)))
	{
		zeError("Cannot create constant buffer");
		return false;
	}

	Data = new ZEUInt8[BufferInfo->Size];
	Size = BufferInfo->Size;
	Info = BufferInfo;

	return true;
}
