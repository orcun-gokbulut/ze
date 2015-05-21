//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETRPrimitives.cpp
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

#include "ZETRPrimitives.h"

#include "ZEMath/ZEVector.h"
#include "ZEGraphics/ZEVertexBuffer.h"
#include "ZEGraphics/ZEVertexDeclaration.h"
#include "ZERandom.h"

static void CreateQuad(ZETRVertex* Output, ZEInt x, ZEInt y)
{
	float Random = 0.0f; //ZERandom::GetFloatPositive();
	Output[0].Position = ZEVector3(x + 0.0f, Random, y + 0.0f);
	Output[1].Position = ZEVector3(x + 0.0f, Random, y + 1.0f);
	Output[2].Position = ZEVector3(x + 1.0f, Random, y + 0.0f);
	Output[3].Position = Output[2].Position;
	Output[4].Position = Output[1].Position;
	Output[5].Position = ZEVector3(x + 1.0f, Random, y + 1.0f);
}

bool ZETRPrimitiveBuffer::CreateVertexDeclaration()
{
	VertexDeclaration = ZEVertexDeclaration::CreateInstance();
	ZEVertexElement Elements[] = 
	{
		{ZE_VES_POSITION, ZE_VET_FLOAT3, 0}
	};

	if (!VertexDeclaration->Create(Elements, 1))
	{
		zeError("Can not create vertex declaration.");
		return false;
	}

	return true;
}

bool ZETRPrimitiveBuffer::CreateVertexBuffer()
{
	ZEInt PrimitiveSize = this->PrimitiveSize;
	ZEInt HalfSize = PrimitiveSize / 2 ;
	ZESize VertexBufferSize = 0;
	VertexBufferSize += 4 * PrimitiveSize * PrimitiveSize;						// Center Primitive
	VertexBufferSize += 4 * (PrimitiveSize + 2) * PrimitiveSize;				// Vertical/Horizontal Primitives
	VertexBufferSize += 2 * ((PrimitiveSize + 2) * (PrimitiveSize + 2) - 2);	// Corner Primitives
	VertexBufferSize += 4 * (PrimitiveSize + 2) * PrimitiveSize;				// Corner Vertical/Horizontal Primitives
	VertexBufferSize *= 6;

	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();
		
	if (!VertexBuffer->Create(VertexBufferSize * sizeof(ZETRVertex)))
	{
		zeError("Can not create vertex buffer.");
		return false;
	}

	ZETRVertex* Buffer = (ZETRVertex*)VertexBuffer->Lock();
	if (Buffer == NULL)
	{
		zeError("Can not lock vertex buffer.");
		return false;
	}


	// Center Primitive
	for (ZEInt x = -PrimitiveSize; x < PrimitiveSize; x++)
	{
		for (ZEInt y = -PrimitiveSize; y < PrimitiveSize; y++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_CENTER].NegativeExtent2Offset	= 0;
	PrimitiveRange[ZETR_PT_CENTER].NegativeExtent1Offset	= 0;
	PrimitiveRange[ZETR_PT_CENTER].CoreOffset = 0;
	PrimitiveRange[ZETR_PT_CENTER].CoreEndOffset = 4 * PrimitiveSize * PrimitiveSize;
	PrimitiveRange[ZETR_PT_CENTER].PositiveExtent1EndOffset = PrimitiveRange[ZETR_PT_CENTER].CoreEndOffset;
	PrimitiveRange[ZETR_PT_CENTER].PositiveExtent2EndOffset = PrimitiveRange[ZETR_PT_CENTER].CoreEndOffset;


	// Horizontal Primitive
	for (ZEInt x = -HalfSize - 1; x <= HalfSize; x++)
	{
		for (ZEInt y = -PrimitiveSize; y < PrimitiveSize; y++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_HORIZONTAL].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_CENTER].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_HORIZONTAL].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_HORIZONTAL].NegativeExtent2Offset + 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_HORIZONTAL].CoreOffset				= PrimitiveRange[ZETR_PT_HORIZONTAL].NegativeExtent1Offset	+ 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_HORIZONTAL].CoreEndOffset				= PrimitiveRange[ZETR_PT_HORIZONTAL].CoreOffset + (PrimitiveSize - 2) * 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_HORIZONTAL].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_HORIZONTAL].CoreEndOffset + 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_HORIZONTAL].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_HORIZONTAL].PositiveExtent1EndOffset + 2 * PrimitiveSize;

	
	// Vertical Primitive
	for (ZEInt y = -HalfSize - 1 ; y <= HalfSize; y++)
	{
		for (ZEInt x = -PrimitiveSize; x < PrimitiveSize; x++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_VERTICAL].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_HORIZONTAL].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_VERTICAL].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_VERTICAL].NegativeExtent2Offset + 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_VERTICAL].CoreOffset					= PrimitiveRange[ZETR_PT_VERTICAL].NegativeExtent1Offset + 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_VERTICAL].CoreEndOffset				= PrimitiveRange[ZETR_PT_VERTICAL].CoreOffset + (PrimitiveSize - 2) * 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_VERTICAL].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_VERTICAL].CoreEndOffset + 2 * PrimitiveSize;
	PrimitiveRange[ZETR_PT_VERTICAL].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_VERTICAL].PositiveExtent1EndOffset + 2 * PrimitiveSize;


	// Corner
	for (ZEInt x = -HalfSize; x < HalfSize; x++)
	{
		CreateQuad(Buffer, x, -HalfSize - 1);
		Buffer += 6;
	}
	for (ZEInt y = -HalfSize - 1; y < HalfSize; y++)
	{
		CreateQuad(Buffer, -HalfSize - 1, y);
		Buffer += 6;
	}
	for (ZEInt y = -HalfSize; y < HalfSize; y++)
	{
		for (ZEInt x = -HalfSize; x < HalfSize; x++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	for (ZEInt y = -HalfSize; y <= HalfSize; y++)
	{
		CreateQuad(Buffer, HalfSize, y);
		Buffer += 6;
	}
	for (ZEInt x = -HalfSize; x < HalfSize; x++)
	{
		CreateQuad(Buffer, x, HalfSize);
		Buffer += 6;
	}
	PrimitiveRange[ZETR_PT_CORNER].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_VERTICAL].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_CORNER].NegativeExtent2Offset + (2 * PrimitiveSize + 1);
	PrimitiveRange[ZETR_PT_CORNER].CoreOffset				= PrimitiveRange[ZETR_PT_CORNER].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER].CoreEndOffset				= PrimitiveRange[ZETR_PT_CORNER].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER].PositiveExtent1EndOffset + (2 * PrimitiveSize + 1);


	// Corner Flip
	for (ZEInt x = -HalfSize; x < HalfSize; x++)
	{
		CreateQuad(Buffer, x, -HalfSize - 1);
		Buffer += 6;
	}
	for (ZEInt y = -HalfSize - 1; y < HalfSize; y++)
	{
		CreateQuad(Buffer, HalfSize, y);
		Buffer += 6;
	}
	for (ZEInt y = -HalfSize; y < HalfSize; y++)
	{
		for (ZEInt x = HalfSize - 1 ; x >= -HalfSize; x--)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	for (ZEInt y = -HalfSize; y <= HalfSize; y++)
	{
		CreateQuad(Buffer, -HalfSize - 1, y);
		Buffer += 6;
	}
	for (ZEInt x = -HalfSize; x < HalfSize; x++)
	{
		CreateQuad(Buffer, x, HalfSize);
		Buffer += 6;
	}
	PrimitiveRange[ZETR_PT_CORNER_FLIP].NegativeExtent2Offset	= PrimitiveRange[ZETR_PT_CORNER].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER_FLIP].NegativeExtent1Offset	= PrimitiveRange[ZETR_PT_CORNER_FLIP].NegativeExtent2Offset + (2 * PrimitiveSize + 1);
	PrimitiveRange[ZETR_PT_CORNER_FLIP].CoreOffset				= PrimitiveRange[ZETR_PT_CORNER_FLIP].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER_FLIP].CoreEndOffset			= PrimitiveRange[ZETR_PT_CORNER_FLIP].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER_FLIP].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER_FLIP].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER_FLIP].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER_FLIP].PositiveExtent1EndOffset + (2 * PrimitiveSize + 1);


	// Corner Horizontal
	for (ZEInt x = -HalfSize - 1; x <= HalfSize; x++)
	{
		for (ZEInt y = -HalfSize; y < HalfSize; y++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_CORNER_FLIP].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].NegativeExtent2Offset +	PrimitiveSize;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].CoreOffset					= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].CoreEndOffset				= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].PositiveExtent1EndOffset + PrimitiveSize;


	// Corner Horizontal Flip
	for (ZEInt x = HalfSize; x >= -HalfSize - 1; x--)
	{
		for (ZEInt y = -HalfSize; y < HalfSize; y++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].NegativeExtent2Offset +	PrimitiveSize;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].CoreOffset				= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].CoreEndOffset				= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].PositiveExtent1EndOffset + PrimitiveSize;


	// Corner Vertical
	for (ZEInt y = -HalfSize - 1 ; y <= HalfSize; y++)
	{
		for (ZEInt x = -HalfSize; x < HalfSize; x++)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].NegativeExtent2Offset	= PrimitiveRange[ZETR_PT_CORNER_HORIZONTAL_FLIP].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].NegativeExtent1Offset	= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].NegativeExtent2Offset + PrimitiveSize;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].CoreOffset				= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].CoreEndOffset			= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].PositiveExtent1EndOffset + PrimitiveSize;


	// Corner Vertical Flip
	for (ZEInt y = -HalfSize - 1 ; y <= HalfSize; y++)
	{
		for (ZEInt x = HalfSize - 1; x >= -HalfSize; x--)
		{
			CreateQuad(Buffer, x, y);
			Buffer += 6;
		}
	}

	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].NegativeExtent2Offset		= PrimitiveRange[ZETR_PT_CORNER_VERTICAL].PositiveExtent2EndOffset;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].NegativeExtent1Offset		= PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].NegativeExtent2Offset + PrimitiveSize;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].CoreOffset					= PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].NegativeExtent1Offset + 1;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].CoreEndOffset				= PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].CoreOffset + (PrimitiveSize * PrimitiveSize - 2);
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].PositiveExtent1EndOffset	= PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].CoreEndOffset + 1;
	PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].PositiveExtent2EndOffset	= PrimitiveRange[ZETR_PT_CORNER_VERTICAL_FLIP].PositiveExtent1EndOffset + PrimitiveSize;

	VertexBuffer->Unlock();

	return true;
}

void ZETRPrimitiveBuffer::SetPrimitiveSize(ZEUInt Size)
{
	this->PrimitiveSize = Size;
	if (Initialized)
	{
		Deinitialize();
		Initialize();
	}

}

ZEUInt ZETRPrimitiveBuffer::GetPrimitiveSize()
{
	return PrimitiveSize;
}

ZEVertexBuffer* ZETRPrimitiveBuffer::GetVertexBuffer()
{
	return VertexBuffer;
}

ZEVertexDeclaration* ZETRPrimitiveBuffer::GetVertexDeclaration()
{
	return VertexDeclaration;
}

ZETRPrimitive ZETRPrimitiveBuffer::GetPrimitive(ZETRPrimitiveType Type, ZEInt Negative, ZEInt Positive)
{
	ZETRPrimitive Primitive = {0};

	if (Negative == 1)
		Primitive.VertexOffset = PrimitiveRange[Type].NegativeExtent2Offset;
	else if (Negative == 0)
		Primitive.VertexOffset = PrimitiveRange[Type].NegativeExtent1Offset;
	else
		Primitive.VertexOffset = PrimitiveRange[Type].CoreOffset;

	if (Positive == 1)
		Primitive.VertexCount = PrimitiveRange[Type].PositiveExtent2EndOffset - Primitive.VertexOffset;
	else if (Positive == 0)
		Primitive.VertexCount = PrimitiveRange[Type].PositiveExtent1EndOffset - Primitive.VertexOffset;
	else
		Primitive.VertexCount = PrimitiveRange[Type].CoreEndOffset - Primitive.VertexOffset;

	Primitive.VertexOffset *= 6;
	Primitive.VertexCount *= 6;

	return Primitive;
}

bool ZETRPrimitiveBuffer::Initialize()
{
	if (Initialized)
		return true;

	if (!CreateVertexBuffer())
		return false;

	if (!CreateVertexDeclaration())
		return false;

	Initialized = true;
	
	return true;
}

void ZETRPrimitiveBuffer::Deinitialize()
{
	if (!Initialized)
		return;

	if (VertexBuffer != NULL)
	{
		VertexBuffer->Destroy();
		VertexBuffer = NULL;
	}

	if (VertexDeclaration != NULL)
	{
		VertexDeclaration->Destroy();
		VertexDeclaration = NULL;
	}

	Initialized = false;
}

ZETRPrimitiveBuffer::ZETRPrimitiveBuffer()
{
	Initialized = false;
	PrimitiveSize = 8;
	VertexBuffer = NULL;
	VertexDeclaration = NULL;
}

ZETRPrimitiveBuffer::~ZETRPrimitiveBuffer()
{
	Deinitialize();
}
