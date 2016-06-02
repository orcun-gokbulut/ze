//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRStructuredBuffer.cpp
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

#include "ZEGRStructuredBuffer.h"

#include "ZEPointer\ZEPointer.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRCounter.h"
#include "ZEGRContext.h"

void ZEGRStructuredBuffer::SetBoundStage(ZEGRShaderType Shader, ZEInt Slot, bool BoundAsShaderResource, bool BoundAsUnorderedAccess)
{
	zeDebugCheck(BoundAsShaderResource && BoundAsUnorderedAccess, "A structured buffer cannot be bound as both shader resource and unordered access");

	BoundStages[Shader].BoundAsShaderResource = BoundAsShaderResource;
	BoundStages[Shader].BoundAsUnorderedAccess = BoundAsUnorderedAccess;
	BoundStages[Shader].Slot = Slot;
}

const ZEArray<ZEGRStructuredBuffer::BoundStage>& ZEGRStructuredBuffer::GetBoundStages() const
{
	return BoundStages;
}

ZEGRResourceType ZEGRStructuredBuffer::GetResourceType() const
{
	return ZEGR_RT_STRUCTURED_BUFFER;
}

bool ZEGRStructuredBuffer::Initialize(ZESize ElementCount, ZESize ElementSize, ZEGRResourceUsage Usage, ZEFlags BindFlags)
{
	SetSize(ElementCount * ElementSize);
	SetResourceUsage(Usage);
	SetResourceBindFlags(BindFlags);
	ZEGR_COUNTER_RESOURCE_INCREASE(this, StructuredBuffer, Pipeline);

	return true;
}

void ZEGRStructuredBuffer::Deinitialize()
{
	ZEGR_COUNTER_RESOURCE_DECREASE(this, StructuredBuffer, Pipeline);
	SetSize(0);
}

void ZEGRStructuredBuffer::SetData(void* Data)
{
	void* Buffer;
	if (!Lock(&Buffer))
		return;

	memcpy(Buffer, Data, GetSize());

	Unlock();
}

ZEGRStructuredBuffer::ZEGRStructuredBuffer()
{
	BoundStages.SetCount(ZEGR_SHADER_TYPE_COUNT);
	BoundStage Stage;
	Stage.BoundAsShaderResource = false;
	Stage.BoundAsUnorderedAccess = false;
	Stage.Slot = -1;
	BoundStages.Fill(Stage);
}

ZEGRStructuredBuffer::~ZEGRStructuredBuffer()
{
	ZEGRContext* Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();
	if (Context != NULL)
	{
		ze_for_each(Stage, BoundStages)
		{
			if (Stage->BoundAsShaderResource)
				Context->ClearShaderResources((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1);

			else if (Stage->BoundAsUnorderedAccess)
				Context->ClearUnorderedAccesses(Stage->Slot, 1);
		}
	}
}

ZEHolder<ZEGRStructuredBuffer> ZEGRStructuredBuffer::Create(ZESize ElementCount, ZESize ElementSize, ZEGRResourceUsage Usage, ZEFlags BindFlags)
{
	ZEHolder<ZEGRStructuredBuffer> StructuredBuffer = ZEGRGraphicsModule::GetInstance()->CreateStructuredBuffer();
	if (!StructuredBuffer->Initialize(ElementCount, ElementSize, Usage, BindFlags))
		return NULL;
	
	return StructuredBuffer;
}
