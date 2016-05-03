//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11VertexBuffer.cpp
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

#include "ZED11VertexBuffer.h"

#include "ZEError.h"
#include "ZED11Module.h"

ID3D11Buffer* ZED11VertexBuffer::GetBuffer() const
{
	return Buffer;
}

bool ZED11VertexBuffer::Initialize(ZESize VertexCount, ZEUInt VertexStride, ZEGRResourceUsage Usage, void* Data)
{
	zeDebugCheck(Buffer != NULL, "Vertex buffer is already initialized.");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");
	zeDebugCheck(VertexStride == 0, "Zero vertex stride.");
	zeDebugCheck(Usage == ZEGR_RU_GPU_READ_ONLY && Data == NULL, "Data cannot be NULL on static vertex buffer");

	ZESize Size = VertexCount * VertexStride;
	zeDebugCheck(Size > 134217728, "Buffer too large.");

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = VertexCount * VertexStride;
	BufferDesc.Usage = ConvertUsage(Usage);
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = (Usage == ZEGR_RU_GPU_READ_CPU_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0;
	BufferDesc.StructureByteStride = VertexStride;
	BufferDesc.MiscFlags = 0;

	HRESULT Result;
	if (BufferDesc.Usage == D3D11_USAGE_IMMUTABLE)
	{
		D3D11_SUBRESOURCE_DATA SubresourceData = {};
		SubresourceData.pSysMem = Data;
		SubresourceData.SysMemPitch = Size;

		Result = GetDevice()->CreateBuffer(&BufferDesc, &SubresourceData, &Buffer);
	}
	else if (BufferDesc.Usage == D3D11_USAGE_DYNAMIC)
	{
		Result = GetDevice()->CreateBuffer(&BufferDesc, NULL, &Buffer);
	}

	if (FAILED(Result))
	{
		zeError("Can not create vertex buffer.");
		return false;
	}

	return ZEGRVertexBuffer::Initialize(VertexCount, VertexStride, Usage, Data);
}

void ZED11VertexBuffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);

	ZEGRVertexBuffer::Deinitialize();
}

ZED11VertexBuffer::ZED11VertexBuffer()
{
	Buffer = NULL;
}

ZED11VertexBuffer::~ZED11VertexBuffer()
{
	Deinitialize();
}

bool ZED11VertexBuffer::Lock(void** Data)
{
	zeDebugCheck(GetResourceUsage() != ZEGR_RU_GPU_READ_CPU_WRITE, "Vertex buffer is not dynamic");

	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT Result = GetMainContext()->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Map);
	if (FAILED(Result))
		return false;

	*Data = Map.pData;

	memset(*Data, 0, GetSize());

	return true;
}

void ZED11VertexBuffer::Unlock()
{
	zeDebugCheck(GetResourceUsage() != ZEGR_RU_GPU_READ_CPU_WRITE, "Vertex buffer is not dynamic");

	GetMainContext()->Unmap(Buffer, 0);
}
