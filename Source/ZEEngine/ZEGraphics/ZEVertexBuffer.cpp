//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexBuffer.cpp
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


#include "ZEVertexBuffer.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZECore/ZECore.h"

ZEVertexBuffer::ZEVertexBuffer()
{
}

ZEVertexBuffer::~ZEVertexBuffer()
{
}

const ZEArray<ZEVertexElement>& ZEVertexBuffer::GetVertexElements() const
{
	return VertexElements;
}

void ZEVertexBuffer::AddVertexElements(const ZEVertexElement& NewElement)
{
	VertexElements.Add(NewElement);
}

void ZEVertexBuffer::GenerateVertexSize()
{
	VertexSize = 0;
	for(int i = 0; i < VertexElements.GetCount(); i++)
	{
		ZEVertexElement E = VertexElements[i];
		switch(E.Type)
		{
			case ZE_VET_FLOAT:
				VertexSize += sizeof(float);
				break;
			case ZE_VET_FLOAT2:
				VertexSize += sizeof(float) * 2;
				break;
			case ZE_VET_FLOAT3:
				VertexSize += sizeof(float) * 3;
				break;
			case ZE_VET_FLOAT4:
				VertexSize += sizeof(float) * 4;
				break;
			case ZE_VET_SHORT2:
				VertexSize += sizeof(short) * 2;
				break;
			case ZE_VET_SHORT4:
				VertexSize += sizeof(short) * 4;
				break;
			case ZE_VET_BYTE4:
				VertexSize += sizeof(char) * 4;
				break;
			case ZE_VET_COLOR:
				VertexSize += sizeof(char) * 4;
				break;
		}
	}
}

ZESize ZEVertexBuffer::GetVertexSize() const
{
	return VertexSize;
}

ZEStaticVertexBuffer::ZEStaticVertexBuffer()
{
}

ZEStaticVertexBuffer::~ZEStaticVertexBuffer()
{
}

bool ZEStaticVertexBuffer::IsStatic()
{
	return true;
}

void ZEStaticVertexBuffer::Destroy()
{
	delete this;
}


ZEStaticVertexBuffer* ZEStaticVertexBuffer::CreateInstance()
{
	return zeGraphics->CreateStaticVertexBuffer();
}

bool ZEDynamicVertexBuffer::IsStatic()
{
	return false;
}
