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
#include "ZEFile/ZEFile.h"

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <Memory.h>


ZESize ZESoundResourceOGG::OggRead(void *ptr, ZESize size, ZESize nmemb, void *datasource)
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

ZEInt ZESoundResourceOGG::OggSeek(void *datasource, ogg_int64_t offset, ZEInt whence)
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

long ZESoundResourceOGG::OggTell(void *datasource)
{
	return (long)((ZESoundResourceOGG*)datasource)->MemoryCursor;
}

ZETaskResult ZESoundResourceOGG::LoadInternal()
{
	ZEFile File; 
	if(!File.Open(GetFileName(), ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot load sound resource. Cannot open ogg file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	File.Seek(0, ZE_SF_END);
	DataSize = File.Tell();
	Data = new unsigned char[DataSize];
	File.Seek(0, ZE_SF_BEGINING);
	File.Read(Data, 1, DataSize);
	File.Close();

	MemoryCursor = 0;

	ov_callbacks Callbacks;
	Callbacks.close_func = NULL;
	Callbacks.read_func = &ZESoundResourceOGG::OggRead;
	Callbacks.seek_func = &ZESoundResourceOGG::OggSeek;
	Callbacks.tell_func = &ZESoundResourceOGG::OggTell;

	if(!ov_open_callbacks(this, &OggFile, NULL, 0, Callbacks)==0)
	{
		zeError("Cannot load sound resource. Cannot open ogg file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	vorbis_info* VorbisInfo = ov_info(&OggFile, -1);
	ChannelCount = VorbisInfo->channels;
	BitsPerSample = 16;
	SamplesPerSecond = (ZESize)VorbisInfo->rate;
	BlockAlign = 2 * (ZESize)VorbisInfo->channels;
	SampleCount = (ZESize)ov_pcm_total(&OggFile, -1);
	ov_seekable(&OggFile);

	return ZE_TR_DONE;
}

ZETaskResult ZESoundResourceOGG::UnloadInternal()
{
	if (Data != NULL)
		delete Data;

	Data = NULL;
	DataSize = 0;
	MemoryCursor = 0;
	memset(&OggFile, 0, sizeof(OggVorbis_File));

	ChannelCount = 0;
	BitsPerSample = 0;
	SamplesPerSecond = 0;
	BlockAlign = 0;
	SampleCount = 0;

	return ZE_TR_DONE;
}

ZESoundResourceOGG::ZESoundResourceOGG()
{
	Data = NULL;
	DataSize = 0;
	MemoryCursor = 0;
	memset(&OggFile, 0, sizeof(OggVorbis_File));
	Register();
}

ZESoundResourceOGG::~ZESoundResourceOGG()
{

}

ZESize ZESoundResourceOGG::GetDataSize() const
{
	return DataSize;
}

const void* ZESoundResourceOGG::GetData() const
{
	return Data;
}

bool ZESoundResourceOGG::Decode(void* Buffer, ZESize SampleIndex, ZESize Count) const
{
	if (!IsLoaded())
	{
		memset(Buffer, 0, Count);
		return false;
	}
	
	DecodeLock.Lock();

	ov_pcm_seek_lap(&OggFile, SampleIndex);	

	long BytesRead	= 1;
	ZEInt Section	= 0;
	ZESize Position	= 0;		
	

	while(Position < (Count * BlockAlign))
	{	
		BytesRead = ov_read(&OggFile, ((char*)(Buffer)) + Position, (int)((Count * BlockAlign) - Position), 0, 2, 1, &Section);
		if(BytesRead < 0)
		{
			DecodeLock.Unlock();
			zeError("Error decoding ogg. File Name : \"%s\".", GetFileName().ToCString());
			return false;
		}
		Position += (ZESize)BytesRead;
	}

	DecodeLock.Unlock();

	return true;
}
