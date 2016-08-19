//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRIndexBuffer.cpp
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

#include "ZEGRIndexBuffer.h"

#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEResource/ZERSTemplates.h"

inline ZESize SizeOfIndex(ZEGRIndexBufferFormat Format)
{
	switch(Format)
	{
		default:
		case ZEGR_IBF_NONE:
			return 0;

		case ZEGR_IBF_INDEX16:
			return sizeof(ZEUInt16);

		case ZEGR_IBF_INDEX32:
			return sizeof(ZEUInt32);
	}
}

bool ZEGRIndexBuffer::Initialize(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, ZEGRResourceUsage Usage, const void* Data)
{
	zeDebugCheck(IndexCount == 0, "IndexCount count cannot be zero.");
	zeDebugCheck(Format != ZEGR_IBF_INDEX16 && Format != ZEGR_IBF_INDEX32, "Unknown index buffer format.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static index buffer");

	this->IndexCount = IndexCount;
	this->Format = Format;
	SetResourceUsage(Usage);
	SetResourceBindFlags(ZEGR_RBF_INDEX_BUFFER);
	SetSize(IndexCount * SizeOfIndex(Format));

	return true;
}

void ZEGRIndexBuffer::Deinitialize()
{
	SetSize(0);
}

ZEGRResourceType ZEGRIndexBuffer::GetResourceType() const
{
	return ZEGR_RT_INDEX_BUFFER;
}

ZESize ZEGRIndexBuffer::GetIndexCount() const
{
	return IndexCount;
}

ZEGRIndexBufferFormat ZEGRIndexBuffer::GetFormat() const
{
	return Format;
}

ZEGRIndexBuffer::ZEGRIndexBuffer()
{
	Format = ZEGR_IBF_NONE;
	Register();
}

ZEGRIndexBuffer::~ZEGRIndexBuffer()
{
	Unregister();
}

static ZERSResource* Instanciator(const void*)
{
	return ZEGRGraphicsModule::GetInstance()->CreateIndexBuffer();
}

ZEHolder<ZEGRIndexBuffer> ZEGRIndexBuffer::CreateResource(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, ZEGRResourceUsage Usage, const void* Data)
{
	zeDebugCheck(IndexCount == 0, "IndexCount count cannot be zero.");
	zeDebugCheck(Format != ZEGR_IBF_INDEX16 && Format != ZEGR_IBF_INDEX32, "Unknown index buffer format.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static index buffer");

	ZEHolder<ZEGRIndexBuffer> Resource = ZERSTemplates::CreateResource<ZEGRIndexBuffer>(Instanciator);
	if (Resource == NULL)
		return NULL;

	if (!Resource->Initialize(IndexCount, Format, Usage, Data))
		return NULL;

	return Resource;
}

ZEHolder<const ZEGRIndexBuffer> ZEGRIndexBuffer::CreateResourceShared(const ZEGUID& GUID, ZEUInt IndexCount, ZEGRIndexBufferFormat Format, ZEGRResourceUsage Usage, const void* Data, ZEGRIndexBuffer** StagingResource)
{
	zeDebugCheck(IndexCount == 0, "IndexCount count cannot be zero.");
	zeDebugCheck(Format != ZEGR_IBF_INDEX16 && Format != ZEGR_IBF_INDEX32, "Unknown index buffer format.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static index buffer");

	ZEGRIndexBuffer* StagingResourceTemp;
	ZEHolder<const ZEGRIndexBuffer> Resource = ZERSTemplates::CreateResourceShared<ZEGRIndexBuffer>(GUID, &StagingResourceTemp, Instanciator);
	if (Resource == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(IndexCount, Format, Usage, Data))
			return NULL;

		if (StagingResource != NULL)
			*StagingResource = StagingResourceTemp;
		else
			StagingResourceTemp->StagingRealized();
	}

	return Resource;
}
