//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11ConstantBuffer.cpp
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

#include "ZED11ConstantBuffer.h"

#include "ZEError.h"
#include "ZED11Direct3D11Module.h"

#include <d3d11.h>

const ID3D11Buffer* ZED11ConstantBuffer::GetBuffer() const
{
	return Buffer;
}

bool ZED11ConstantBuffer::Initialize(ZESize BufferSize)
{	
	zeDebugCheck(BufferSize == 0, "Cannot create zero sized buffer.");
	zeDebugCheck((BufferSize % 16) != 0, "Buffer size must be multiple of 16.");
	zeDebugCheck(BufferSize > 65536, "Buffer too large");

	D3D11_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferSize;
	
	HRESULT Result = GetDevice()->CreateBuffer(&Desc, NULL, &Buffer);
	if (FAILED(Result))
	{
		zeError("Constant buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

	SetSize(BufferSize);

	return true;
}

void ZED11ConstantBuffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);
	ZEGRConstantBuffer::Deinitialize();
}

bool ZED11ConstantBuffer::Lock(void** Buffer)
{
	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT Result = GetMainContext()->Map(this->Buffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &Map);
	if (FAILED(Result))
	{
		zeError("Cannot lock constant buffer.");
		return false;
	}

	*Buffer = Map.pData;

	return true;
}
void ZED11ConstantBuffer::Unlock()
{
	GetMainContext()->Unmap(Buffer, 0);
}

ZED11ConstantBuffer::ZED11ConstantBuffer()
{
	Buffer = NULL;
}
