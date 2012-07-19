//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundResourceOGG.cpp
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

#include "ZESoundResourceOGG.h"
#include "ZEError.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <Memory.h>

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	ZESize ConstLength = strlen("resources\\") - 1;

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

// MEMORY SEEK
static ZESize OggMemory_Read(void *ptr, ZESize size, ZESize nmemb, void *datasource)
{
	ZESoundResourceOGG* Resource = (ZESoundResourceOGG*)datasource;

	if (Resource->DataSize < Resource->MemoryCursor + size * nmemb)
	{
		ZESize ItemCount = (Resource->DataSize - Resource->MemoryCursor) / size;
		memcpy(ptr, (ZEUInt8*)Resource->Data + Resource->MemoryCursor, size * ItemCount);
		Resource->MemoryCursor += ItemCount * size;

		return ItemCount;
	}
	else
	{
		memcpy(ptr, (ZEUInt8*)Resource->Data + Resource->MemoryCursor, size * nmemb);
		Resource->MemoryCursor += nmemb * size;

		return nmemb;
	}
}

static ZEInt OggMemory_Seek(void *datasource, ogg_int64_t offset, ZEInt whence)
{
	ZESoundResourceOGG* Resource = (ZESoundResourceOGG*)datasource;
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

	return 0;
}

static long OggMemory_Tell(void *datasource)
{
	return (long)((ZESoundResourceOGG*)datasource)->MemoryCursor;
}

ZESoundResourceOGG::ZESoundResourceOGG()
{
	Data = NULL;
}

ZESoundResourceOGG::~ZESoundResourceOGG()
{
	if (Data != NULL)
		delete Data;
}

ZESize ZESoundResourceOGG::GetDataSize() const
{
	return DataSize;
}

const void* ZESoundResourceOGG::GetData() const
{
	return Data;
}

void ZESoundResourceOGG::Decode(void* Buffer, ZESize SampleIndex, ZESize Count)
{
	ov_pcm_seek_lap(&OggFile, SampleIndex);	

	long BytesRead	= 1;
	ZEInt Section	= 0;
	ZESize Position	= 0;		
	

	while(Position < (Count * BlockAlign))
	{	
		BytesRead = ov_read(&OggFile, ((char*)(Buffer)) + Position, (int)((Count * BlockAlign) - Position), 0, 2, 1, &Section);
		if(BytesRead < 0)
		{
			zeError("Error decoding ogg. (FileName : \"%s\")", GetFileName().ToCString());
			return;
		}
		Position += (ZESize)BytesRead;
	}
}

ZESoundResource* ZESoundResourceOGG::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	bool Result;
	ZEFile File; 
	
	Result = File.Open(NewPath, ZE_FOM_READ, ZE_FCM_NONE);
	if(!Result)
	{
		zeError("Can not open ogg file. (FileName : \"%s\")", NewPath.ToCString());
		return NULL;
	}

	ZESoundResourceOGG* NewResource = new ZESoundResourceOGG();

	File.Seek(0, ZE_SF_END);
	NewResource->DataSize = File.Tell();
	NewResource->Data = new unsigned char[NewResource->DataSize];
	File.Seek(0, ZE_SF_BEGINING);
	File.Read(NewResource->Data, 1, NewResource->DataSize);
	File.Close();

	NewResource->SetFileName(NewPath);	
	
	NewResource->MemoryCursor = 0;
	
	ov_callbacks Callbacks;
	Callbacks.close_func = NULL;
	Callbacks.read_func = OggMemory_Read;
	Callbacks.seek_func = OggMemory_Seek;
	Callbacks.tell_func = OggMemory_Tell;

	if(!ov_open_callbacks(NewResource, &NewResource->OggFile, NULL, 0, Callbacks)==0)
	{
		zeError("Can not read ogg. (FileName : \"%s\")", NewPath.ToCString());
		delete NewResource;
		return NULL;
	}

	vorbis_info* VorbisInfo			= ov_info(&NewResource->OggFile, -1);
	NewResource->ChannelCount		= VorbisInfo->channels;
	NewResource->BitsPerSample		= 16;
	NewResource->SamplesPerSecond	= (ZESize)VorbisInfo->rate;
	NewResource->BlockAlign			= 2 * (ZESize)VorbisInfo->channels;
	NewResource->SampleCount			= (ZESize)ov_pcm_total(&NewResource->OggFile, -1);
	ov_seekable(&NewResource->OggFile);
	return NewResource;
}




