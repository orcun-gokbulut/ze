//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11IndexBuffer.cpp
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
#include "ZED11IndexBuffer.h"
#include "ZED11Module.h"
#include <d3d11.h>
#include "ZEPointer\ZEPointer.h"

inline static ZESize GetIndexSize(ZEGRIndexBufferFormat Format)
{
	static const ZESize FormatToIndexSize[3] = 
	{
		0,	// ZE_IBF_NONE
		2,	// ZE_IBF_INDEX16
		4	// ZE_IBF_INDEX32
	};
	return FormatToIndexSize[Format];
}

ID3D11Buffer* ZED11IndexBuffer::GetBuffer() const
{
	return Buffer;
}

bool ZED11IndexBuffer::Initialize(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, ZEGRResourceUsage Usage, const void* Data)
{
	zeDebugCheck(Buffer != NULL, "Index buffer is already initialized.");

	ZESize Size = GetIndexSize(Format) * (ZESize)IndexCount;
	zeDebugCheck(Size > 134217728, "Buffer too large");
	
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = ConvertUsage(Usage);
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);
	BufferDesc.ByteWidth = (UINT)Size;

	ZEPointer<D3D11_SUBRESOURCE_DATA> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA();
		SubresourceData->pSysMem = Data;
		SubresourceData->SysMemPitch = Size;
	}

	HRESULT Result = GetDevice()->CreateBuffer(&BufferDesc, SubresourceData, &Buffer);
	if (FAILED(Result))
	{
		zeError("Index buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}
	
	return ZEGRIndexBuffer::Initialize(IndexCount, Format, Usage, Data);
}

void ZED11IndexBuffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);

	ZEGRIndexBuffer::Deinitialize();
}

ZED11IndexBuffer::ZED11IndexBuffer()
{
	Buffer = NULL;
}

bool ZED11IndexBuffer::Lock(void** Data)
{
	D3D11_MAPPED_SUBRESOURCE Map;
	LockContext();
	HRESULT Result = GetMainContext()->Map(Buffer, 0, D3D11_MAP_WRITE, 0, &Map);
	UnlockContext();
	if (FAILED(Result))
		return false;
	
	*Data = Map.pData;

	memset(*Data, 0, GetSize());

	return true;
}

void ZED11IndexBuffer::Unlock()
{
	LockContext();
	GetMainContext()->Unmap(Buffer, 0);
	UnlockContext();
}

ZED11IndexBuffer::~ZED11IndexBuffer()
{
	Deinitialize();
}
