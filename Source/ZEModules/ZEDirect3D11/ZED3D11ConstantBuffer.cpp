//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D11ConstantBuffer.cpp
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

#include <d3d11.h>

#include "ZEError.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZED3D11ConstantBuffer.h"
#include "ZED3D11GraphicsModule.h"
#include "ZEGraphics/ZEShaderMeta.h"

bool ZED3D11ConstantBuffer::UpdateWith(ZEUInt ShadowIndex)
{
	if (!ShadowCopy.GetChanged(ShadowIndex))
		return true;

	D3D11_MAPPED_SUBRESOURCE Mapped;
	HRESULT Result = D3DContexes[0]->Map(D3D10Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer mapping failed. ErrorCode: %d.", Result);
		return false;
	}

	ZESize Size = ShadowCopy.GetSliceCount() * ShadowCopy.GetSliceSize();
	const void* Data = ShadowCopy.GetConstData(ShadowIndex);

	memcpy(Mapped.pData, Data, Size);

	D3DContexes[0]->Unmap(D3D10Buffer, 0);

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer contents updated. ConstantBuffer: %p, ShadowCOpyIdnex: %u.", this, ShadowIndex);
#endif

	return ZEConstantBuffer::UpdateWith(ShadowIndex);
}

const ID3D11Buffer* ZED3D11ConstantBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D11ConstantBuffer::Create(const ZEShaderBuffer* BufferInfo)
{
	zeDebugCheck(BufferInfo == NULL, "NUll pointer.");
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(BufferInfo->Constants.GetCount() == 0, "Cannot create with empty table");
	zeDebugCheck(BufferInfo->Size == 0, "Cannot create zero sized buffer.");

	zeDebugCheck(BufferInfo->Size > 65536, "Buffer too large");

	D3D11_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferInfo->Size;
	
	HRESULT Result = D3DDevices[0]->CreateBuffer(&Desc, NULL, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEConstantBuffer::Create(BufferInfo);
}

bool ZED3D11ConstantBuffer::Create(ZESize BufferSize)
{	
	zeDebugCheck(GetIsCreated(), "Buffer already created.");
	zeDebugCheck(BufferSize == 0, "Cannot create zero sized buffer.");
	zeDebugCheck((BufferSize % 16) != 0, "Buffer size must be multiple of 16.");

	zeDebugCheck(BufferSize > 65536, "Buffer too large");

	D3D11_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferSize;
	
	HRESULT Result = D3DDevices[0]->CreateBuffer(&Desc, NULL, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

	return ZEConstantBuffer::Create(BufferSize);
}

ZED3D11ConstantBuffer::ZED3D11ConstantBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D11ConstantBuffer::~ZED3D11ConstantBuffer()
{	
	ZED3D_RELEASE(D3D10Buffer);
}
