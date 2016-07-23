//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRVertexBuffer.cpp
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

#include "ZEGRVertexBuffer.h"

#include "ZEResource/ZERSTemplates.h"
#include "ZEGraphics/ZEGRGraphicsModule.h"

bool ZEGRVertexBuffer::Initialize(ZESize VertexCount, ZEUInt VertexStride, ZEGRResourceUsage Usage, const void* Data)
{
	this->VertexCount = VertexCount;
	this->VertexStride = VertexStride;

	SetResourceUsage(Usage);
	SetResourceBindFlags(ZEGR_RBF_VERTEX_BUFFER);
	SetSize(VertexCount * VertexStride);
	ZEGR_COUNTER_RESOURCE_INCREASE(this, VertexBuffer, Geometry);

	return true;
}

void ZEGRVertexBuffer::Deinitialize()
{
	ZEGR_COUNTER_RESOURCE_INCREASE(this, VertexBuffer, Geometry);
	SetSize(0);
}

ZEGRVertexBuffer::ZEGRVertexBuffer()
{
	VertexCount = 0;
	VertexStride = 0;
}

ZEGRResourceType ZEGRVertexBuffer::GetResourceType() const
{
	return ZEGR_RT_VERTEX_BUFFER;
}

ZESize ZEGRVertexBuffer::GetVertexCount() const
{
	return VertexCount;
}

ZEUInt ZEGRVertexBuffer::GetVertexStride() const
{
	return VertexStride;
}

static ZERSResource* Instanciator()
{
	return ZEGRGraphicsModule::GetInstance()->CreateVertexBuffer();
}

ZEHolder<ZEGRVertexBuffer> ZEGRVertexBuffer::CreateResource(ZESize VertexCount, ZEUInt VertexStride, ZEGRResourceUsage Usage, const void* Data)
{
	zeDebugCheck(VertexCount == 0, "Vertex count cannot be zero.");
	zeDebugCheck(VertexStride == 0, "Vertex stride cannot be zero.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static vertex buffer");

	ZEHolder<ZEGRVertexBuffer> Resource = ZERSTemplates::CreateResource<ZEGRVertexBuffer>(Instanciator);
	if (Resource == NULL)
		return NULL;

	if (!Resource->Initialize(VertexCount, VertexStride, Usage, Data))
		return NULL;

	return Resource;
}

ZEHolder<const ZEGRVertexBuffer> ZEGRVertexBuffer::CreateResourceShared(const ZEGUID& GUID, ZESize VertexCount, ZEUInt VertexStride, ZEGRResourceUsage Usage, const void* Data, ZEGRVertexBuffer** StagingResource)
{
	zeDebugCheck(VertexCount == 0, "Vertex count cannot be zero.");
	zeDebugCheck(VertexStride == 0, "Vertex stride cannot be zero.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static vertex buffer");

	ZEGRVertexBuffer* StagingResourceTemp;
	ZEHolder<const ZEGRVertexBuffer> Resource = ZERSTemplates::CreateResourceShared<ZEGRVertexBuffer>(GUID, &StagingResourceTemp, Instanciator);

	if (Resource == NULL)
		return NULL;

	if (StagingResourceTemp != NULL)
	{
		if (!StagingResourceTemp->Initialize(VertexCount, VertexStride, Usage, Data))
			return NULL;

		if (StagingResource != NULL)
			*StagingResource = StagingResourceTemp;
		else
			StagingResourceTemp->StagingRealized();
	}

	return Resource;
}
