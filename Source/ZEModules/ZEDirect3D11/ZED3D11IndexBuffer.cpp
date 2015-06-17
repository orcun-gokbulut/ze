//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11IndexBuffer.cpp
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
#include "ZED3D11IndexBuffer.h"
#include "ZED3D11GraphicsModule.h"

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

bool ZED3D11IndexBuffer::UpdateWith(ZEUInt ShadowIndex)
{
	if (!ShadowCopy.GetChanged(ShadowIndex))
		return true;

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT Result = D3DContexes[0]->Map(D3D10Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	if (FAILED(Result))
	{
		zeError("D3D10 Index buffer mapping failed. ErrorCode: %d.", Result);
		return false;
	}

	ZESize Size = ShadowCopy.GetSliceCount() * ShadowCopy.GetSliceSize();
	const void* Data = ShadowCopy.GetConstData(ShadowIndex);

	memcpy(Mapped.pData, Data, Size);

	D3DContexes[0]->Unmap(D3D10Buffer, 0);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Index buffer contents updated. IndexBuffer: %p, ShadowCOpyIdnex: %u.", this, ShadowIndex);
#endif

	return ZEGRIndexBuffer::UpdateWith(ShadowIndex);
}

const ID3D11Buffer* ZED3D11IndexBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D11IndexBuffer::CreateDynamic(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, const void* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(IndexCount == 0, "Cannot create empty buffer.");
	zeDebugCheck(Format == ZEGR_IBF_NONE, "Unknown buffer format.");

	ZESize Size = GetIndexSize(Format) * (ZESize)IndexCount;
	zeDebugCheck(Size > 134217728, "Buffer too large");

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = (UINT)Size;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = InitialData;
	Data.SysMemPitch = 0;
	Data.SysMemSlicePitch = 0;

	HRESULT Result = D3DDevices[0]->CreateBuffer(&BufferDesc, InitialData == NULL ? NULL : &Data, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Dynamic index buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEGRIndexBuffer::CreateDynamic(IndexCount, Format, InitialData);
}

bool ZED3D11IndexBuffer::CreateStatic(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, const void* InitialData)
{
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(IndexCount == 0, "Cannot create empty buffer.");
	zeDebugCheck(InitialData == NULL, "Buffer must have initial data.");
	zeDebugCheck(Format == ZEGR_IBF_NONE, "Unknown buffer format");

	ZESize Size = GetIndexSize(Format) * (ZESize)IndexCount;
	zeDebugCheck(Size > 134217728, "Buffer too large");
	
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = (UINT)Size;

	D3D11_SUBRESOURCE_DATA Data;
	Data.pSysMem = InitialData;
	Data.SysMemPitch = 0;
	Data.SysMemSlicePitch = 0;

	HRESULT Result = D3DDevices[0]->CreateBuffer(&BufferDesc, &Data, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Static index buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}
	
	return ZEGRIndexBuffer::CreateStatic(IndexCount, Format, InitialData);
}

ZED3D11IndexBuffer::ZED3D11IndexBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D11IndexBuffer::~ZED3D11IndexBuffer()
{
	ZED3D_RELEASE(D3D10Buffer);
}
