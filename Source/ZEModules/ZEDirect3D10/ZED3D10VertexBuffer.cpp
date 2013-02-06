//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10VertexBuffer.cpp
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
#include "ZED3D10VertexBuffer.h"
#include "ZED3D10GraphicsModule.h"

#include <D3D10.h>

const ID3D10Buffer* ZED3D10VertexBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D10VertexBuffer::CreateDynamic(ZEUInt VertexCount, ZESize VertexSize)
{	
	zeDebugCheck(VertexSize == 0, "Zero vertex size");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");
	zeDebugCheck(D3D10Buffer != NULL, "Buffer already created.");
	zeDebugCheck(VertexSize * VertexCount > 134217728, "Too big to create.");
	
	D3D10_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = VertexCount * (UINT)VertexSize;

	if (FAILED(D3D10Device->CreateBuffer(&BufferDesc, NULL, &D3D10Buffer)))
	{
		zeError("Can not create dynamic vertex buffer.");
		return false;
	}
	
	this->Static = false;
	this->VertexSize = VertexSize;
	this->VertexCount = VertexCount;
	this->BufferSize = VertexCount * VertexSize;
	
	return true;
}

bool ZED3D10VertexBuffer::CreateStatic(ZEUInt VertexCount, ZESize VertexSize, const void* VertexData)
{	
	zeDebugCheck(VertexData == NULL, "NULL pointer.");
	zeDebugCheck(VertexSize == 0, "Zero vertex size");
	zeDebugCheck(VertexCount == 0, "Zero vertex count.");
	zeDebugCheck(D3D10Buffer != NULL, "Buffer already created.");
	zeDebugCheck(VertexSize * VertexCount > 134217728, "Too big to create.");
	
	D3D10_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.CPUAccessFlags = 0;
	BufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	BufferDesc.ByteWidth = VertexCount * (UINT)VertexSize;
	
	D3D10_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = VertexData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	if (FAILED(D3D10Device->CreateBuffer(&BufferDesc, &InitialData, &D3D10Buffer)))
	{
		zeError("Can not create static vertex buffer.");
		return false;
	}
	
	this->Static = true;
	this->VertexSize = VertexSize;
	this->VertexCount = VertexCount;
	this->BufferSize = VertexCount * VertexSize;
	
	return true;
}

bool ZED3D10VertexBuffer::Lock(void** Data)
{
	zeDebugCheck(D3D10Buffer == NULL, "Buffer not created.");
	zeDebugCheck(Static, "Static buffer not be locked/unlocked.");

	if (FAILED(D3D10Buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, Data)))
	{
		zeError("Can not lock dynamic vertex buffer.");
		return false;
	}

	return true;
}

bool ZED3D10VertexBuffer::Unlock()
{
	zeDebugCheck(D3D10Buffer == NULL, "Buffer not created.");
	zeDebugCheck(Static, "Static buffer not be locked/unlocked.");

	D3D10Buffer->Unmap();
	return true;
}

ZED3D10VertexBuffer::ZED3D10VertexBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D10VertexBuffer::~ZED3D10VertexBuffer()
{
	ZED3D_RELEASE(D3D10Buffer);
}




