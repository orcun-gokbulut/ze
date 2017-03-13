//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRBuffer.cpp
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

#include "ZEGRBuffer.h"

#include "ZEGRGraphicsModule.h"
#include "ZEResource\ZERSTemplates.h"

static ZERSResource* Instanciator(const void*)
{
	ZEGRGraphicsModule* GraphicsModule = ZEGRGraphicsModule::GetInstance();
	if (GraphicsModule == NULL)
		return NULL;

	return GraphicsModule->CreateBuffer();
}

bool ZEGRBuffer::Initialize(ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format, const void* Data)
{
	this->SizeInBytes = SizeInBytes;
	this->StrideInBytes = StrideInBytes;
	this->ElementCount = (BufferType != ZEGR_BT_CONSTANT_BUFFER) ? (SizeInBytes / StrideInBytes) : 0;
	this->Type = BufferType;

	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);
	SetFormat(Format);
	SetSize(SizeInBytes);

	return true;
}

void ZEGRBuffer::Deinitialize()
{
	SizeInBytes = 0;
	StrideInBytes = 0;
	ElementCount = 0;
	Type = ZEGR_BT_NONE;
}

ZEGRBuffer::ZEGRBuffer()
{
	SizeInBytes = 0;
	StrideInBytes = 0;
	ElementCount = 0;
	Type = ZEGR_BT_NONE;

	Register();
}

ZEGRBuffer::~ZEGRBuffer()
{
	Unregister();
}

ZESize ZEGRBuffer::GetSizeInBytes() const
{
	return SizeInBytes;
}

ZESize ZEGRBuffer::GetStrideInBytes() const
{
	return StrideInBytes;
}

ZEUInt ZEGRBuffer::GetElementCount() const
{
	return ElementCount;
}

ZEGRBufferType ZEGRBuffer::GetBufferType() const
{
	return Type;
}

ZEGRResourceType ZEGRBuffer::GetResourceType() const
{
	return ZEGR_RT_BUFFER;
}

void ZEGRBuffer::SetData(const void* Data, ZESize DataSize)
{
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE, "Immutable buffer cannot be modified.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_STAGING, "Staging buffer cannot be implicitly modified. Instead use Map with appropriate map-type.");
	zeDebugCheck(DataSize > GetSizeInBytes(), "Out-of-range access");
	
	DataSize = (DataSize != 0) ? DataSize : GetSizeInBytes();

	if (GetResourceUsage() == ZEGR_RU_DYNAMIC)
	{
		void* Buffer;
		if (!Map(ZEGR_RMT_WRITE_DISCARD, &Buffer))
		{
			zeError("SetData failed. Mapping buffer failed.");
			return;
		}

		memcpy(Buffer, Data, DataSize);

		Unmap();
	}
	else if (GetResourceUsage() == ZEGR_RU_STATIC)
	{
		Update(Data, DataSize);
	}
}

ZEHolder<ZEGRBuffer> ZEGRBuffer::CreateResource(ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format, const void* Data)
{
	zeDebugCheck(SizeInBytes == 0, "SizeInBytes cannot be zero.");
	zeDebugCheck(BufferType != ZEGR_BT_CONSTANT_BUFFER && StrideInBytes == 0, "StrideInBytes cannot be zero for non-constant buffers.");
	zeDebugCheck(BufferType == ZEGR_BT_INDEX_BUFFER && Format == ZEGR_TF_NONE, "Index buffer format cannot be none");
	zeDebugCheck(Usage == ZEGR_RU_IMMUTABLE && Data == NULL, "Data cannot be NULL on immutable buffer");

	ZEHolder<ZEGRBuffer> Buffer = ZERSTemplates::CreateResource<ZEGRBuffer>(Instanciator);
	if (Buffer == NULL)
		return NULL;

	if (!Buffer->Initialize(BufferType, SizeInBytes, StrideInBytes, Usage, BindFlags, Format, Data))
		return NULL;

	return Buffer;
}

ZEHolder<const ZEGRBuffer> ZEGRBuffer::CreateResourceShared(const ZEGUID& GUID, ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format, const void* Data, ZEGRBuffer** StagingResource)
{
	zeDebugCheck(SizeInBytes == 0, "SizeInBytes cannot be zero.");
	zeDebugCheck(BufferType != ZEGR_BT_CONSTANT_BUFFER && StrideInBytes == 0, "StrideInBytes cannot be zero for non-constant buffers.");
	zeDebugCheck(BufferType == ZEGR_BT_INDEX_BUFFER && Format == ZEGR_TF_NONE, "Index buffer format cannot be none");
	zeDebugCheck(Usage == ZEGR_RU_IMMUTABLE && Data == NULL, "Data cannot be NULL on immutable buffer");

	ZEGRBuffer* StagingResourceTemp;
	ZEHolder<const ZEGRBuffer> Buffer = ZERSTemplates::CreateResourceShared<ZEGRBuffer>(GUID, &StagingResourceTemp, Instanciator);
	if (Buffer == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(BufferType, SizeInBytes, StrideInBytes, Usage, BindFlags, Format, Data))
			return NULL;

		if (StagingResource != NULL)
			*StagingResource = StagingResourceTemp;
		else
			StagingResourceTemp->StagingRealized();
	}

	return Buffer;
}
