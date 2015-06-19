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

const ID3D11Buffer* ZED11IndexBuffer::GetBuffer() const
{
	return Buffer;
}

bool ZED11IndexBuffer::CreateStatic(ZEUInt IndexCount, ZEGRIndexBufferFormat Format, const void* InitialData)
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

	HRESULT Result = D3DDevices[0]->CreateBuffer(&BufferDesc, &Data, &Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Static index buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}
	
	return ZEGRIndexBuffer::CreateStatic(IndexCount, Format, InitialData);
}

ZED11IndexBuffer::ZED11IndexBuffer()
{
	Buffer = NULL;
}

ZED11IndexBuffer::~ZED11IndexBuffer()
{
	ZEGR_RELEASE(Buffer);
}
