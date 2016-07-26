//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRConstantBuffer.cpp
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

#include "ZEGRConstantBuffer.h"

#include "ZEPointer\ZEPointer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRCounter.h"
#include "ZEResource\ZERSTemplates.h"

ZEGRResourceType ZEGRConstantBuffer::GetResourceType() const
{
	return ZEGR_RT_CONSTANT_BUFFER;
}

bool ZEGRConstantBuffer::Initialize(ZESize BufferSize)
{
	SetSize(BufferSize);
	ZEGR_COUNTER_RESOURCE_INCREASE(this, ConstantBuffer, Pipeline);
	return true;
}

void ZEGRConstantBuffer::Deinitialize()
{
	ZEGR_COUNTER_RESOURCE_DECREASE(this, ConstantBuffer, Pipeline);
	SetSize(0);
}

void ZEGRConstantBuffer::SetData(const void* Data)
{
	void* Buffer;
	if (!Lock(&Buffer))
		return;

	memcpy(Buffer, Data, GetSize());

	Unlock();
}

ZEGRConstantBuffer::ZEGRConstantBuffer()
{

}

static ZERSResource* Instanciator(const void*)
{
	return ZEGRGraphicsModule::GetInstance()->CreateConstantBuffer();
}

ZEHolder<ZEGRConstantBuffer> ZEGRConstantBuffer::CreateResource(ZESize BufferSize)
{
	const ZEHolder<ZEGRConstantBuffer> Resource = ZERSTemplates::CreateResource<ZEGRConstantBuffer>(Instanciator);
	if (Resource == NULL)
		return NULL;

	if (!Resource->Initialize(BufferSize))
		return NULL;
	
	return Resource;
}

ZEHolder<const ZEGRConstantBuffer> ZEGRConstantBuffer::CreateResourceShared(const ZEGUID& GUID, ZESize BufferSize, ZEGRConstantBuffer** StagingResource)
{
	ZEGRConstantBuffer* StagingResourceTemp;
	ZEHolder<const ZEGRConstantBuffer> Resource = ZERSTemplates::CreateResourceShared<ZEGRConstantBuffer>(GUID, &StagingResourceTemp, Instanciator);

	if (Resource == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(BufferSize))
			return NULL;

		if (StagingResource != NULL)
			*StagingResource = StagingResourceTemp;
		else
			StagingResourceTemp->StagingRealized();
	}

	return Resource;
}
