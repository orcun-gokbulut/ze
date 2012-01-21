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
#include <mpg123.h>
#include <stdio.h>

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZEUInt ConstLength = strlen("resources\\") - 1;

	if (Path[0] == '\\' || Path[0] == '/')
		NewString = NewString.SubString(1, Path.GetLength() - 1);

	// If it is guaranteed that there is no "resources\\" string in beginning
	if (NewString.GetLength() - 1 < ConstLength)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}
	// Else check if there is "resources\\" in the beginning
	else if (_stricmp("resources\\", Path.SubString(0, ConstLength)) != 0)
	{
		NewString.Insert(0, "resources\\");
		return NewString;
	}

	return NewString;
}

static ssize_t Memory_Read(ZEInt fd, void *buffer, ZESize nbyte)
{
	ZESoundResourceMP3* Resource = (ZESoundResourceMP3*)fd;

	if (Resource->MemoryCursor == Resource->DataSize)
		return 0;

	ZESize BytesRead = Resource->MemoryCursor + nbyte < Resource->DataSize ? nbyte : Resource->DataSize - Resource->MemoryCursor;
	memcpy(buffer, Resource->Data + Resource->MemoryCursor, BytesRead);
	Resource->MemoryCursor += BytesRead;

	return BytesRead;
}

static off_t Memory_Seek(ZEInt fd, off_t offset, ZEInt whence)
{
	ZESoundResourceMP3* Resource = (ZESoundResourceMP3*)fd;

	switch(whence)
	{
		case SEEK_SET:
			Resource->MemoryCursor = offset;
			break;
		
		case SEEK_CUR:
			Resource->MemoryCursor += offset;
			break;

		case SEEK_END:
			Resource->MemoryCursor = Resource->DataSize + offset;
			break;
	}

	return Resource->MemoryCursor;
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

void ZESoundResourceMP3::Decode(void* Buffer, ZESize SampleIndex, ZESize Count)
{
	mpg123_seek(mpg123, SampleIndex, SEEK_SET);
	
	ZESize BytesRead = 1;
	ZESize Position = 0;
	ZEInt Result;

	while(Position < (Count * BlockAlign))
	{
		Result = mpg123_read(mpg123, (unsigned char*)Buffer, Count * BlockAlign - Position, &BytesRead);
		if (Result != MPG123_OK && Result != MPG123_DONE && Result != MPG123_NEED_MORE)
		{
			zeError("Error decoding mp3. (FileName : \"%s\", Error Code : %d)", GetFileName(), Result);
			return;
		}
		Position += BytesRead;
	}
}

void ZESoundResourceMP3::BaseInitialize()
{
	mpg123_init();
}

void ZESoundResourceMP3::BaseDeinitialize()
{
	mpg123_exit();
}

ZESoundResource* ZESoundResourceMP3::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile* File = ZEFile::Open(NewPath);
	if(File == NULL || !File->IsOpen())
	{
		zeError("Can not open ogg file. (FileName : \"%s\")", NewPath);
		return NULL;
	}

	ZESoundResourceMP3* NewResource = new ZESoundResourceMP3();

	File->Seek(0, ZE_SF_END);
	NewResource->DataSize = File->Tell();
	NewResource->Data = new unsigned char[NewResource->DataSize];
	File->Seek(0, ZE_SF_BEGINING);
	File->Read(NewResource->Data, 1, NewResource->DataSize);
	File->Close();
	delete File;

	NewResource->SetFileName(NewPath);	
	NewResource->MemoryCursor = 0;

	NewResource->mpg123 = mpg123_new(NULL, NULL);
	if (NewResource->mpg123 == NULL)
	{
		zeError("Can not create MP3 handle. (FileName : \"%s\")", NewPath);
		return NULL;
	}
	
	mpg123_param(NewResource->mpg123, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */

	mpg123_replace_reader(NewResource->mpg123, Memory_Read, Memory_Seek);
	mpg123_open_fd(NewResource->mpg123, (ZEInt)NewResource);

	long Rate;
	ZEInt Channels, Encoding;
	mpg123_getformat(NewResource->mpg123, &Rate, &Channels, &Encoding);

	/*switch(Encoding)
	{
		case MPG123_ENC_8:
			NewResource->BitsPerSample = 8;
			break;
		case MPG123_ENC_16:
			NewResource->BitsPerSample = 16;
			break;
		case MPG123_ENC_32:
			NewResource->BitsPerSample = 32;
			break;
		default:
			zeError("Unsupported encoding. Only 8bit, 16bit and 32bit unsigned integer encoding supported. (Filename : \"%s\")", NewResource->GetFileName());
			delete NewResource;
			return NULL;
	}*/

	NewResource->FileFormat = ZE_SFF_MP3;
	NewResource->BitsPerSample = 16;
	NewResource->ChannelCount = Channels;
	NewResource->SamplesPerSecond = Rate;
	NewResource->BlockAlign = NewResource->ChannelCount * (NewResource->BitsPerSample / 8);
	mpg123_scan(NewResource->mpg123);
	NewResource->SampleCount = mpg123_length(NewResource->mpg123);

	return NewResource;
}




