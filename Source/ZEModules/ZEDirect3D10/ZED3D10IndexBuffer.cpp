//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10IndexBuffer.cpp
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


#include "ZED3D10IndexBuffer.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEError.h"

#include <D3D10.h>

inline static ZESize GetIndexSize(ZEIndexBufferFormat Format)
{
	switch(Format)
	{
		case ZE_IBF_INDEX16:
			return 2;
			break;
		case ZE_IBF_INDEX32:
			return 4;
			break;
	}
}

const ID3D10Buffer* ZED3D10IndexBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D10IndexBuffer::CreateDynamic(ZESize IndexCount, ZEIndexBufferFormat Format)
{
	zeDebugCheck(IndexCount == 0, "Zero vertex count.");
	zeDebugCheck(Format == ZE_IBF_NONE, "Format not set");
	zeDebugCheck(D3D10Buffer != NULL, "Buffer already created.");
	
	D3D10_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = (UINT)(IndexCount * GetIndexSize(Format));

	if (FAILED(D3D10Device->CreateBuffer(&BufferDesc, NULL, &D3D10Buffer)))
	{
		zeError("Can not create index buffer.");
		return false;
	}
	
	this->Static = false;
	this->Format = Format;
	this->IndexCount = IndexCount;
	this->BufferSize = IndexCount * GetIndexSize(Format);

	return true;
}

bool ZED3D10IndexBuffer::CreateStatic(ZESize IndexCount, ZEIndexBufferFormat Format, const void* IndexData)
{
	zeDebugCheck(IndexData == NULL, "NULL pointer.");
	zeDebugCheck(IndexCount == 0, "Zero vertex count.");
	zeDebugCheck(Format == ZE_IBF_NONE, "Format not set");
	zeDebugCheck(D3D10Buffer != NULL, "Buffer already created.");

	D3D10_BUFFER_DESC BufferDesc;
	BufferDesc.MiscFlags = 0;
	BufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	BufferDesc.ByteWidth = (UINT)(IndexCount * GetIndexSize(Format));

	D3D10_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = IndexData;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	if (FAILED(D3D10Device->CreateBuffer(&BufferDesc, &InitialData, &D3D10Buffer)))
	{
		zeError("Can not create static index buffer.");
		return false;
	}
	
	this->Static = true;
	this->Format = Format;
	this->IndexCount = IndexCount;
	this->BufferSize = IndexCount * GetIndexSize(Format);

	return true;
}

bool ZED3D10IndexBuffer::Lock(void** Data)
{
	zeDebugCheck(D3D10Buffer == NULL, "Buffer not created.");
	zeDebugCheck(Static, "Static buffer not be locked/unlocked.");
	
	if (FAILED(D3D10Buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, Data)))
	{
		zeError("Can not lock dynamic index buffer.");
		return false;
	}

	return true;
}

bool ZED3D10IndexBuffer::Unlock()
{
	zeDebugCheck(D3D10Buffer == NULL, "Buffer not created.");
	zeDebugCheck(Static, "Static buffer not be locked/unlocked.");
		
	D3D10Buffer->Unmap();
	return true;
}

ZED3D10IndexBuffer::ZED3D10IndexBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D10IndexBuffer::~ZED3D10IndexBuffer()
{
	ZED3D_RELEASE(D3D10Buffer);
}
