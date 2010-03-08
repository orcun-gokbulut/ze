//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PhysXPhysicsStream.cpp
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

#include "NxPhysics.h"
#include "PhysXPhysicsStream.h"

ZEPhysXPhysicsMemoryWriteBuffer::ZEPhysXPhysicsMemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

ZEPhysXPhysicsMemoryWriteBuffer::~ZEPhysXPhysicsMemoryWriteBuffer()
{
	NX_DELETE_ARRAY(data);
}

void ZEPhysXPhysicsMemoryWriteBuffer::clear()
{
	currentSize = 0;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeByte(NxU8 b)
{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeWord(NxU16 w)
{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeDword(NxU32 d)
{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeFloat(NxReal f)
{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeDouble(NxF64 f)
{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
}

NxStream& ZEPhysXPhysicsMemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
{
	NxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
	{
		maxSize = expectedSize + 4096;

		NxU8* newData = new NxU8[maxSize];
		NX_ASSERT(newData!=NULL);

		if(data)
		{
			memcpy(newData, data, currentSize);
			delete[] data;
		}
		data = newData;
	}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
}

ZEPhysXPhysicsMemoryReadBuffer::ZEPhysXPhysicsMemoryReadBuffer(const NxU8* data) : buffer(data)
{
}

ZEPhysXPhysicsMemoryReadBuffer::~ZEPhysXPhysicsMemoryReadBuffer()
{
	// We don't own the data => no delete
}

NxU8 ZEPhysXPhysicsMemoryReadBuffer::readByte() const
{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer += sizeof(NxU8);
	return b;
}

NxU16 ZEPhysXPhysicsMemoryReadBuffer::readWord() const
{
	NxU16 w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer += sizeof(NxU16);
	return w;
}

NxU32 ZEPhysXPhysicsMemoryReadBuffer::readDword() const
{
	NxU32 d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer += sizeof(NxU32);
	return d;
}

float ZEPhysXPhysicsMemoryReadBuffer::readFloat() const
{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
}

double ZEPhysXPhysicsMemoryReadBuffer::readDouble() const
{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
}

void ZEPhysXPhysicsMemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
{
	memcpy(dest, buffer, size);
	buffer += size;
}
