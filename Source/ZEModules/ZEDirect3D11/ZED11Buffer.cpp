//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED11Buffer.cpp
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

#include "ZED11Buffer.h"

#include "ZEPointer\ZEPointer.h"

UINT ConvertBufferType(ZEGRBufferType BufferType)
{
	switch (BufferType)
	{
		case ZEGR_BT_STRUCTURED_BUFFER:
			return D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		case ZEGR_BT_RAW_BUFFER:
			return D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		default:
			return 0;
	}
};

bool ZED11Buffer::Initialize(ZEGRBufferType BufferType, ZESize SizeInBytes, ZESize StrideInBytes, ZEGRResourceUsage Usage, ZEGRResourceBindFlags BindFlags, ZEGRFormat Format, const void* Data)
{
	if (!ZEGRBuffer::Initialize(BufferType, SizeInBytes, StrideInBytes, Usage, BindFlags, Format, Data))
		return false;

	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = (UINT)SizeInBytes;
	BufferDesc.StructureByteStride = (BufferType == ZEGR_BT_STRUCTURED_BUFFER) ? (UINT)StrideInBytes : 0;
	BufferDesc.BindFlags = ConvertBindFlags(BindFlags);
	BufferDesc.Usage = ConvertUsage(Usage);
	BufferDesc.CPUAccessFlags = ConvertUsageToCpuAccessFlags(Usage);
	BufferDesc.MiscFlags = ConvertBufferType(BufferType) | (BindFlags.GetFlags(ZEGR_RBF_INDIRECT_ARGS) ? D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0);

	ZEPointer<D3D11_SUBRESOURCE_DATA> SubresourceData;
	if (Data != NULL)
	{
		SubresourceData = new D3D11_SUBRESOURCE_DATA();
		SubresourceData->pSysMem = Data;
		SubresourceData->SysMemPitch = SizeInBytes;
	}

	HRESULT Result = GetDevice()->CreateBuffer(&BufferDesc, SubresourceData, &Buffer);
	if (FAILED(Result))
	{
		zeError("Buffer creation failed. Error: 0x%X.", Result);
		return false;
	}

	if (BindFlags.GetFlags(ZEGR_RBF_SHADER_RESOURCE))
	{
		if (Type == ZEGR_BT_BUFFER)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc;
			SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			SrvDesc.Format = ConvertFormat(Format);
			SrvDesc.Buffer.ElementOffset = 0;
			SrvDesc.Buffer.ElementWidth = ElementCount;
			
			Result = GetDevice()->CreateShaderResourceView(Buffer, &SrvDesc, &ShaderResourceView);
		}
		else
		{
			Result = GetDevice()->CreateShaderResourceView(Buffer, NULL, &ShaderResourceView);
		}

		if (FAILED(Result))
		{
			zeError("Buffer shader resource view creation failed. Error: 0x%X.", Result);
			return false;
		}
	}

	if (BindFlags.GetFlags(ZEGR_RBF_UNORDERED_ACCESS))
	{
		Result = GetDevice()->CreateUnorderedAccessView(Buffer, NULL, &UnorderedAccessView);
		if (FAILED(Result))
		{
			zeError("Buffer unordered access view creation failed. Error: 0x%X.", Result);
			return false;
		}
	}

	return true;
}

void ZED11Buffer::Deinitialize()
{
	ZEGR_RELEASE(Buffer);
	ZEGR_RELEASE(ShaderResourceView);
	ZEGR_RELEASE(UnorderedAccessView);

	ZEGRBuffer::Deinitialize();
}

bool ZED11Buffer::Map(ZEGRResourceMapType MapType, void** Data, ZESize* SizeInBytes)
{
	zeDebugCheck(Buffer == NULL, "Buffer is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_STATIC, "Buffer is not mappable");

	D3D11_MAP D11MapType = ConvertMapType(MapType);
	D3D11_MAPPED_SUBRESOURCE MapData = {};

	LockContext();
	HRESULT Result = GetMainContext()->Map(Buffer, 0, D11MapType, 0, &MapData);
	UnlockContext();
	if (FAILED(Result))
	{
		zeError("Mapping buffer failed. Error: 0x%X", Result);
		return false;
	}

	if (Data != NULL)
		*Data = MapData.pData;

	if (SizeInBytes != NULL)
		*SizeInBytes = MapData.RowPitch;

	return true;
}

void ZED11Buffer::Unmap()
{
	zeDebugCheck(Buffer == NULL, "Buffer is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_STATIC, "Buffer is not mappable");

	LockContext();
	GetMainContext()->Unmap(Buffer, 0);
	UnlockContext();
}

void ZED11Buffer::Update(const void* SrcData, ZESize SrcSizeInBytes, ZEUInt DestOffsetInBytes, ZESize DestSizeInBytes)
{
	zeDebugCheck(Buffer == NULL, "Buffer is not initialized.");
	zeDebugCheck(GetResourceUsage() == ZEGR_RU_IMMUTABLE || GetResourceUsage() == ZEGR_RU_DYNAMIC || GetResourceUsage() == ZEGR_RU_STAGING, "Buffer is not non-mappable");
	zeCheckError(((ZESize)DestOffsetInBytes + DestSizeInBytes) > SizeInBytes, ZE_VOID, "Out-of-range access");
	zeCheckError(DestOffsetInBytes > 0 && SrcSizeInBytes > DestSizeInBytes, ZE_VOID, "Source size doesn't fit destination size");

	D3D11_BOX* D11Box = NULL;
	if (DestOffsetInBytes > 0 || DestSizeInBytes > 0)
	{
		D3D11_BOX DestBox = {};
		DestBox.left = DestOffsetInBytes;
		DestBox.right = DestOffsetInBytes + DestSizeInBytes;
		DestBox.top = 0;
		DestBox.bottom = 1;
		DestBox.front = 0;
		DestBox.back = 1;

		D11Box = &DestBox;
	}

	LockContext();
	GetMainContext()->UpdateSubresource(Buffer, 0, D11Box, SrcData, SrcSizeInBytes, 0);
	UnlockContext();
}

ZED11Buffer::ZED11Buffer()
{
	Buffer = NULL;
	ShaderResourceView = NULL;
	UnorderedAccessView = NULL;
}

ZED11Buffer::~ZED11Buffer()
{
	Deinitialize();
}
