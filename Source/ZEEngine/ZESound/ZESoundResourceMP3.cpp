//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundResourceMP3.cpp
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

#include "ZESoundResourceMP3.h"
#include "ZEError.h"
#include "ZEDS/ZEArray.h"

#include <mpg123.h>
#include <stdio.h>
#include "ZEFile/ZEFile.h"

static ZEChunkArray<ZESoundResourceMP3*, 50> Indexes;
static ZELock IndexesLock;

ssize_t ZESoundResourceMP3::MP3Read(ZEInt fd, void *buffer, ZESize nbyte)
{
	IndexesLock.Lock();
	ZESoundResourceMP3* Resource = (ZESoundResourceMP3*)Indexes[fd];
	IndexesLock.Unlock();

	if (Resource->MemoryCursor == Resource->DataSize)
		return 0;

	ZESize BytesRead = Resource->MemoryCursor + nbyte < Resource->DataSize ? nbyte : Resource->DataSize - Resource->MemoryCursor;
	memcpy(buffer, Resource->Data + Resource->MemoryCursor, BytesRead);
	Resource->MemoryCursor += BytesRead;

	return (ssize_t)BytesRead;
}

off_t ZESoundResourceMP3::MP3Seek(ZEInt fd, off_t offset, ZEInt whence)
{
	ZESoundResourceMP3* Resource = (ZESoundResourceMP3*)Indexes[fd];

	switch(whence)
	{
		case SEEK_SET:
			Resource->MemoryCursor = (ZESize)offset;
			break;
		
		case SEEK_CUR:
			Resource->MemoryCursor += (ZESize)offset;
			break;

		case SEEK_END:
			Resource->MemoryCursor = Resource->DataSize + (ZESize)offset;
			break;
	}

	return (off_t)Resource->MemoryCursor;
}

ZETaskResult ZESoundResourceMP3::LoadInternal()
{
	static bool MPG123Initialized = false;
	if (MPG123Initialized)
	{
		mpg123_init();
		MPG123Initialized = true;
	}

	ZEFile File;
	if (!File.Open(GetFileName(), ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot load sound resource. Cannot open mp3 file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	File.Seek(0, ZE_SF_END);
	DataSize = File.Tell();
	Data = new unsigned char[DataSize];
	File.Seek(0, ZE_SF_BEGINING);
	File.Read(Data, 1, DataSize);
	File.Close();

	MemoryCursor = 0;

	mpg123 = mpg123_new(NULL, NULL);
	if (mpg123 == NULL)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (mpg123_param(mpg123, MPG123_RESYNC_LIMIT, -1, 0) != MPG123_OK) /* New in library version 0.0.1 . */
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}

	if (mpg123_replace_reader(mpg123, &ZESoundResourceMP3::MP3Read, &ZESoundResourceMP3::MP3Seek) != MPG123_OK)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}

	IndexesLock.Lock();
	Indexes.Add(this);
	int fd = (int)Indexes.GetCount();
	IndexesLock.Unlock();

	if (mpg123_open_fd(mpg123, fd) != MPG123_OK)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}
	long Rate;
	ZEInt Channels, Encoding;
	if (mpg123_getformat(mpg123, &Rate, &Channels, &Encoding) != MPG123_OK)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}

	FileFormat = ZE_SFF_MP3;
	BitsPerSample = 16;
	ChannelCount = Channels;
	SamplesPerSecond = (ZESize)Rate;
	BlockAlign = (ZESize)ChannelCount * ((ZESize)BitsPerSample / 8);
	
	if (mpg123_scan(mpg123) != MPG123_OK)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}

	SampleCount = (ZESize)mpg123_length(mpg123);
	if (SampleCount == MPG123_ERR)
	{
		zeError("Cannot load sound resource. Decoder error. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;	
	}

	return ZE_TR_DONE;
}

ZETaskResult ZESoundResourceMP3::UnloadInternal()
{
	if (mpg123 != NULL)
	{
		mpg123_delete(mpg123);
		mpg123 = NULL;
	}

	if (Data != NULL)
	{
		delete Data;
		Data = NULL;
	}

	DataSize = 0;
	MemoryCursor = 0;
	ChannelCount = 0;
	BitsPerSample = 0;
	SamplesPerSecond = 0;
	BlockAlign = 0;
	SampleCount = 0;

	return ZE_TR_DONE;
}

ZESoundResourceMP3::ZESoundResourceMP3()
{
	mpg123 = NULL;
	Data = NULL;
}

ZESoundResourceMP3::~ZESoundResourceMP3()
{
	if (Data != NULL)
		delete Data;

	if (mpg123 != NULL)
		mpg123_delete(mpg123);
}

ZESize ZESoundResourceMP3::GetDataSize() const
{
	return DataSize;
}

const void* ZESoundResourceMP3::GetData() const
{
	return Data;
}

bool ZESoundResourceMP3::Decode(void* Buffer, ZESize SampleIndex, ZESize Count) const
{
	if (!IsLoaded())
	{
		memset(Buffer, 0, Count);
		return false;
	}
	
	ZESize BytesRead = 1;
	ZESize Position = 0;
	ZEInt Result;

	DecodeLock.Lock();
	mpg123_seek(mpg123, (off_t)SampleIndex, SEEK_SET);

	while(Position < (Count * BlockAlign))
	{
		Result = mpg123_read(mpg123, (unsigned char*)Buffer, Count * BlockAlign - Position, &BytesRead);
		if (Result != MPG123_OK && Result != MPG123_DONE && Result != MPG123_NEED_MORE)
		{
			DecodeLock.Unlock();
			zeError("Error decoding MP3 packet. File Name : \"%s\", Error Code : %d.", GetFileName().ToCString(), Result);
			return false;
		}
		Position += BytesRead;
	}

	DecodeLock.Unlock();

	return true;
}
