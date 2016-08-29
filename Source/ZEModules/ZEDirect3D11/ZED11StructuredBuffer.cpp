//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11StructuredBuffer.cpp
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

#include "ZED11StructuredBuffer.h"

#include "ZEError.h"
#include "ZED11Module.h"

bool ZED11StructuredBuffer::Initialize(ZESize ElementCount, ZESize ElementSize, ZEGRResourceUsage Usage, ZEFlags BindFlags)
{	
	ZESize BufferSize = ElementCount * ElementSize;
	zeDebugCheck(BufferSize == 0, "Cannot create zero sized buffer.");
	//zeDebugCheck((BufferSize % 16) != 0, "Buffer size must be multiple of 16.");

	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = (UINT)BufferSize;
	Desc.Usage = ConvertUsage(Usage);
	Desc.BindFlags = ConvertBindFlags(BindFlags);
	Desc.CPUAccessFlags = (Usage == ZEGR_RU_CPU_READ_WRITE) ? (D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE) : (Usage == ZEGR_RU_GPU_READ_CPU_WRITE) ? D3D11_CPU_ACCESS_WRITE : 0;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.StructureByteStride = ElementSize;

	HRESULT Result = GetDevice()->CreateBuffer(&Desc, NULL, &Buffer);
	if (FAILED(Result))
	{
		zeError("Structured buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		Result = GetDevice()->CreateShaderResourceView(Buffer, NULL, &ShaderResourceView);
		if (FAILED(Result))
		{
			zeError("Structured buffer shader resource view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		Result = GetDevice()->CreateUnorderedAccessView(Buffer, NULL, &UnorderedAccessView);
		if (FAILED(Result))
		{
			zeError("Structured buffer unordered access view creation failed. ErrorCode: %d.", Result);
			return false;
		}
	}

	return ZEGRStructuredBuffer::Initialize(ElementCount, ElementSize, Usage, BindFlags);
}

void ZED11StructuredBuffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	ZEGRStructuredBuffer::Deinitialize();
}

ZED11StructuredBuffer::ZED11StructuredBuffer()
{
	Buffer = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11StructuredBuffer::~ZED11StructuredBuffer()
{
	Deinitialize();
}

ID3D11Buffer* ZED11StructuredBuffer::GetBuffer() const
{
	return Buffer;
}

ID3D11ShaderResourceView* ZED11StructuredBuffer::GetShaderResourceView() const
{
	return ShaderResourceView;
}

ID3D11UnorderedAccessView* ZED11StructuredBuffer::GetUnorderedAccessView() const
{
	return UnorderedAccessView;
}

bool ZED11StructuredBuffer::Lock(void** Buffer)
{
	zeCheckError(this->Buffer == NULL, false, "Structured buffer is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_GPU_READ_ONLY || GetResourceUsage() == ZEGR_RU_GPU_READ_WRITE_CPU_WRITE, "Structured buffer has not been created for mapping");

	D3D11_MAP MapType;
	if (GetResourceUsage() == ZEGR_RU_CPU_READ_WRITE)
		MapType = D3D11_MAP_READ_WRITE;
	else if (GetResourceUsage() == ZEGR_RU_GPU_READ_CPU_WRITE)
		MapType = D3D11_MAP_WRITE_DISCARD;
	else
		return false;

	LockContext();

	D3D11_MAPPED_SUBRESOURCE Map;
	HRESULT Result = GetMainContext()->Map(this->Buffer, 0, MapType, 0, &Map);

	if (FAILED(Result))
	{
		zeError("Cannot lock structured buffer.");
		return false;
	}

	*Buffer = Map.pData;

	memset(*Buffer, 0, GetSize());

	return true;
}
void ZED11StructuredBuffer::Unlock()
{
	zeCheckError(this->Buffer == NULL, ZE_VOID, "Structured buffer is not initialized.");

	GetMainContext()->Unmap(Buffer, 0);

	UnlockContext();
}
