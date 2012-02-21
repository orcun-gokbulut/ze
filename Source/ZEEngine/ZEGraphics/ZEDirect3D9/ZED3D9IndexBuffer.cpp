//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9IndexBuffer.cpp
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


#include "ZED3D9IndexBuffer.h"
#include "ZED3D9Module.h"
#include "ZEError.h"


ZESize ZED3D9StaticIndexBuffer::GetBufferSize()
{
	return BufferSize;
}

bool ZED3D9StaticIndexBuffer::Create(ZESize BufferSize, ZEIndexBufferFormat Format)
{
	this->Release();
	this->BufferSize = BufferSize;

	D3DFORMAT D3D9Format;
	switch (Format)
	{
		case ZE_IBF_INDEX16:
			D3D9Format = D3DFMT_INDEX16;
			break;
		case ZE_IBF_INDEX32:
			D3D9Format = D3DFMT_INDEX32;
			break;
		default:
			D3D9Format = D3DFMT_UNKNOWN;
			zeAssert(true, "Unknown Index Buffer Format");
			break;
	}

	if (GetDevice()->CreateIndexBuffer(BufferSize, D3DUSAGE_WRITEONLY, D3D9Format, D3DPOOL_MANAGED, &StaticBuffer, NULL) != D3D_OK)
	{
		zeCriticalError("Can not create static index buffer.");
		return false;
	}

	return true;
}

void* ZED3D9StaticIndexBuffer::Lock()
{
	if (StaticBuffer != NULL)
	{
		char* Data;
		HRESULT Result = StaticBuffer->Lock(0, 0, (void**)&Data, 0);
		
		if (Result != D3D_OK)
		{
			zeCriticalError("Can not lock static vertex buffer.");
			return NULL;
		}

		return (void*)Data;
	}

	return NULL;
}

void ZED3D9StaticIndexBuffer::Unlock()
{
	if (StaticBuffer != NULL)
	{
		HRESULT Result;
		Result = StaticBuffer->Unlock();

		if (Result != D3D_OK)
			zeError("Can not unlock static index buffer.");
	}

}

void ZED3D9StaticIndexBuffer::Release()
{
	if (StaticBuffer != NULL)
	{
		StaticBuffer->Release();
		StaticBuffer = NULL;
	}

	BufferSize = 0;
}


void ZED3D9StaticIndexBuffer::Destroy()
{
	GetModule()->IndexBuffers.DeleteValue((ZED3D9StaticIndexBuffer*)this);

	delete this;
}

ZED3D9StaticIndexBuffer::ZED3D9StaticIndexBuffer()
{
	BufferSize = 0;
	StaticBuffer = NULL;
}

ZED3D9StaticIndexBuffer::~ZED3D9StaticIndexBuffer()
{
	this->Release();
}
