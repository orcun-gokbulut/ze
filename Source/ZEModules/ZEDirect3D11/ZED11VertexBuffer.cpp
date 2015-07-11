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

#include "ZEError.h"
#include "ZED11VertexBuffer.h"
#include "ZED11Direct3D11Module.h"

ID3D11Buffer* ZED11VertexBuffer::GetBuffer() const
{
	return Buffer;
}

bool ZED11VertexBuffer::Initialize(ZEUInt VertexCount, ZESize VertexSize)
{
	zeDebugCheck(Buffer != NULL, "Vertex buffer is already initialized.");
	zeDebugCheck(VertexSize == 0, "Zero vertex size.");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");

	ZESize Size = VertexSize * (ZESize)VertexCount;
	zeDebugCheck(Size > 134217728, "Buffer too large.");

	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = VertexCount * (UINT)VertexSize;

	HRESULT Result = GetDevice()->CreateBuffer(&BufferDesc, NULL, &Buffer);
	if (FAILED(Result))
	{
		zeError("Can not create dynamic vertex buffer.");
		return false;
	}

	return ZEGRVertexBuffer::Initialize(VertexCount, VertexSize);
}

bool ZED11VertexBuffer::Lock(void** Data)
{
	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT Result = GetMainContext()->Map(Buffer, 0, D3D11_MAP_WRITE, 0, &Map);
	if (FAILED(Result))
		return false;

	return true;
}

void ZED11VertexBuffer::Unlock()
{
	GetMainContext()->Unmap(Buffer, 0);
}

void ZED11VertexBuffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);
}

ZED11VertexBuffer::ZED11VertexBuffer()
{
	Buffer = NULL;
}
