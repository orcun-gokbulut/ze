//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D10ConstantBuffer.cpp
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
#include "ZEDS/ZEHashGenerator.h"
#include "ZED3D10ConstantBuffer.h"
#include "ZED3D10GraphicsModule.h"
#include "ZEGraphics/ZEShaderMeta.h"

#include <d3d10.h>

bool ZED3D10ConstantBuffer::UpdateBuffer()
{
	if (!NeedUpdate())
		return true;

	void* Mapped = NULL;
	HRESULT Result = D3D10Buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &Mapped);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer mapping failed. ErrorCode: %d.", Result);
		return false;
	}

	memcpy(Mapped, Data, Size);

	D3D10Buffer->Unmap();

	return ZEConstantBuffer::UpdateBuffer();
}

ID3D10Buffer* ZED3D10ConstantBuffer::GetD3D10Buffer() const
{
	return D3D10Buffer;
}

bool ZED3D10ConstantBuffer::Create(ZESize Size)
{
	zeDebugCheck(Data != NULL, "Already created");
	zeDebugCheck(Size == 0, "Zero size.");
	
	// Create buffer
	D3D10_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D10_USAGE_DYNAMIC;
	Desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)Size;
	
	HRESULT Result = D3D10Device->CreateBuffer(&Desc, NULL, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer created by size. Size: %u.", Size);
#endif

	GlobalSize += Size;
	GlobalCount++;

	return ZEConstantBuffer::Create(Size);
}

bool ZED3D10ConstantBuffer::Create(const ZEShaderBuffer* BufferInfo)
{
	zeDebugCheck(Data != NULL, "Already created");
	zeDebugCheck(BufferInfo == NULL, "NUll pointer");
	zeDebugCheck(BufferInfo->Constants.GetCount() == 0, "Empty table");

	// Create buffer
	D3D10_BUFFER_DESC Desc;
	Desc.MiscFlags = 0;
	Desc.Usage = D3D10_USAGE_DYNAMIC;
	Desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	Desc.ByteWidth = (UINT)BufferInfo->Size;
	
	HRESULT Result = D3D10Device->CreateBuffer(&Desc, NULL, &D3D10Buffer);
	if (FAILED(Result))
	{
		zeError("D3D10 Constant buffer creation failed. ErrorCode: %d.", Result);
		return false;
	}

#ifdef ZE_GRAPHIC_LOG_ENABLE
	zeLog("Constant buffer created by buffer info. Name: %s, ConstantCount: %u, Size: %u.", 
			BufferInfo->Name, BufferInfo->Constants.GetCount(), BufferInfo->Size);
#endif

	GlobalSize += BufferInfo->Size;
	GlobalCount++;

	return ZEConstantBuffer::Create(BufferInfo);
}

ZESize		ZED3D10ConstantBuffer::GlobalSize = 0;
ZEUInt16	ZED3D10ConstantBuffer::GlobalCount = 0;

ZED3D10ConstantBuffer::ZED3D10ConstantBuffer()
{
	D3D10Buffer = NULL;
}

ZED3D10ConstantBuffer::~ZED3D10ConstantBuffer()
{	
	ZED3D_RELEASE(D3D10Buffer);

	GlobalSize -= Size;
	GlobalCount--;
}
