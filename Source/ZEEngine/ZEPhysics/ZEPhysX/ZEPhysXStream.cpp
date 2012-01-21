//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPhysXStream.cpp
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

#include "ZEPhysXStream.h"
#include "ZEError.h"
#include <stdio.h>

// ZEPhysXFileStream
///////////////////////////////////////////////////////////////////////////////////////////////
ZEPhysXFileStream::ZEPhysXFileStream()
{
	File = NULL;
}

ZEPhysXFileStream::~ZEPhysXFileStream()
{
	CloseFile();
}
		
bool ZEPhysXFileStream::OpenFile(const char* FileName, bool ReadMode)
{
	File = fopen(FileName, ReadMode ? "rb" : "wb");
	return (File != NULL);
}

void ZEPhysXFileStream::CloseFile()
{
	if(File == NULL)	
		fclose((FILE*)File);	
}

NxU8 ZEPhysXFileStream::readByte() const
{
	NxU8 b;
	ZESize r = fread(&b, sizeof(NxU8), 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
	return b;
}

NxU16 ZEPhysXFileStream::readWord() const
{
	NxU16 w;
	ZESize r = fread(&w, sizeof(NxU16), 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
	return w;
}

NxU32 ZEPhysXFileStream::readDword() const
{
	NxU32 d;
	ZESize r = fread(&d, sizeof(NxU32), 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
	return d;
}

float ZEPhysXFileStream::readFloat() const
{
	NxReal f;
	ZESize r = fread(&f, sizeof(NxReal), 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
	return f;
}

double ZEPhysXFileStream::readDouble() const
{
	NxF64 f;
	ZESize r = fread(&f, sizeof(NxF64), 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
	return f;
}

void ZEPhysXFileStream::readBuffer(void* Buffer, NxU32 Size)	const
{
	ZESize r= fread(Buffer, Size, 1, ((FILE*)File));
	zeAssert(r == 0, "Reading error.");
}

NxStream& ZEPhysXFileStream::storeByte(NxU8 b)
{
	ZESize w = fwrite(&b, sizeof(NxU8), 1, ((FILE*)File));
	zeAssert(w == 0, "Writing error.");
	return *this;
}

NxStream& ZEPhysXFileStream::storeWord(NxU16 w)
{
	ZESize ww = fwrite(&w, sizeof(NxU16), 1, ((FILE*)File));
	zeAssert(ww == 0, "Writing error.");
	return *this;
}

NxStream& ZEPhysXFileStream::storeDword(NxU32 d)
{
	ZESize w = fwrite(&d, sizeof(NxU32), 1, ((FILE*)File));
	zeAssert(w == 0, "Writing error.");
	return *this;
}

NxStream& ZEPhysXFileStream::storeFloat(NxReal f)
{
	ZESize w = fwrite(&f, sizeof(NxReal), 1, ((FILE*)File));
	zeAssert(w == 0, "Writing error.");
	return *this;
}

NxStream& ZEPhysXFileStream::storeDouble(NxF64 f)
{
	ZESize w = fwrite(&f, sizeof(NxF64), 1, ((FILE*)File));
	zeAssert(w == 0, "Writing error.");
	return *this;
}

NxStream& ZEPhysXFileStream::storeBuffer(const void* Buffer, NxU32 Size)
{
	ZESize w = fwrite(Buffer, Size, 1, ((FILE*)File));
	zeAssert(w == 0, "Writing error.");
	return *this;
}


// ZEPhysXMemoryWriteStream
///////////////////////////////////////////////////////////////////////////////////////////////
ZEPhysXMemoryWriteStream::ZEPhysXMemoryWriteStream() : CurrentSize(0), MaxSize(0), Data(NULL)
{
}

ZEPhysXMemoryWriteStream::~ZEPhysXMemoryWriteStream()
{
	if (Data != NULL)
	{
		delete[] Data;
		Data = NULL;
	}
}

const NxU8* ZEPhysXMemoryWriteStream::GetData() const
{
	return Data;
}

const ZESize ZEPhysXMemoryWriteStream::GetDataSize() const
{
	return CurrentSize;
}

void ZEPhysXMemoryWriteStream::Clear()
{
	CurrentSize = 0;
}

NxU8 ZEPhysXMemoryWriteStream::readByte() const 
{	
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams."); 
	return 0;
}

NxU16 ZEPhysXMemoryWriteStream::readWord() const
{	
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams."); 
	return 0;
}

NxU32 ZEPhysXMemoryWriteStream::readDword() const 
{
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams."); 
	return 0;
}

float ZEPhysXMemoryWriteStream::readFloat() const 
{
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams."); 
	return 0.0f;
}

double ZEPhysXMemoryWriteStream::readDouble() const
{
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams."); 
	return 0.0;
}

void ZEPhysXMemoryWriteStream::readBuffer(void* buffer, NxU32 size) const
{
	zeAssert(true, "ZEPhysXMemoryWriteStream can only write to streams.");
}

NxStream& ZEPhysXMemoryWriteStream::storeByte(NxU8 b)
{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
}

NxStream& ZEPhysXMemoryWriteStream::storeWord(NxU16 w)
{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
}

NxStream& ZEPhysXMemoryWriteStream::storeDword(NxU32 d)
{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
}

NxStream& ZEPhysXMemoryWriteStream::storeFloat(NxReal f)
{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
}

NxStream& ZEPhysXMemoryWriteStream::storeDouble(NxF64 f)
{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
}

NxStream& ZEPhysXMemoryWriteStream::storeBuffer(const void* Buffer, NxU32 Size)
{
	NxU32 ExpectedSize = CurrentSize + Size;
	if(ExpectedSize > MaxSize)
	{
		MaxSize = ExpectedSize + 4096;

		NxU8* NewData = new NxU8[MaxSize];
		zeAssert(NewData == NULL, "Can not allocate data.");

		if(Data)
		{
			memcpy(NewData, Data, CurrentSize);
			delete[] Data;
		}
		Data = NewData;
	}

	memcpy(Data + CurrentSize, Buffer, Size);
	CurrentSize += Size;
	return *this;
}


// ZEPhysXMemoryWriteStream
///////////////////////////////////////////////////////////////////////////////////////////////
ZEPhysXMemoryReadStream::ZEPhysXMemoryReadStream()
{
	Buffer = NULL;
}

ZEPhysXMemoryReadStream::~ZEPhysXMemoryReadStream()
{

}

void ZEPhysXMemoryReadStream::SetData(const NxU8* Data)
{
	Buffer = Data;
}

const NxU8* ZEPhysXMemoryReadStream::GetData() const
{
	return Buffer;
}

NxU8 ZEPhysXMemoryReadStream::readByte() const
{
	NxU8 b;
	memcpy(&b, Buffer, sizeof(NxU8));
	Buffer += sizeof(NxU8);
	return b;
}

NxU16 ZEPhysXMemoryReadStream::readWord() const
{
	NxU16 w;
	memcpy(&w, Buffer, sizeof(NxU16));
	Buffer += sizeof(NxU16);
	return w;
}

NxU32 ZEPhysXMemoryReadStream::readDword() const
{
	NxU32 d;
	memcpy(&d, Buffer, sizeof(NxU32));
	Buffer += sizeof(NxU32);
	return d;
}

float ZEPhysXMemoryReadStream::readFloat() const
{
	float f;
	memcpy(&f, Buffer, sizeof(float));
	Buffer += sizeof(float);
	return f;
}

double ZEPhysXMemoryReadStream::readDouble() const
{
	double f;
	memcpy(&f, Buffer, sizeof(double));
	Buffer += sizeof(double);
	return f;
}

void ZEPhysXMemoryReadStream::readBuffer(void* Destination, NxU32 Size) const
{
	memcpy(Destination, Buffer, Size);
	Buffer += Size;
}

NxStream& ZEPhysXMemoryReadStream::storeByte(NxU8 b)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}

NxStream& ZEPhysXMemoryReadStream::storeWord(NxU16 w)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}

NxStream& ZEPhysXMemoryReadStream::storeDword(NxU32 d)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}

NxStream& ZEPhysXMemoryReadStream::storeFloat(NxReal f)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}

NxStream& ZEPhysXMemoryReadStream::storeDouble(NxF64 f)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}

NxStream& ZEPhysXMemoryReadStream::storeBuffer(const void* buffer, NxU32 size)
{
	zeAssert(true, "ZEPhysXMemoryReadStream can only used for reading.");
	return *this;
}




