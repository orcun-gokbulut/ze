//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundResource.cpp
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

#include "SoundResource.h"
#include "Core/Core.h"
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

size_t OggFile_Read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return ((ZEResourceFile*)datasource)->Read(ptr, size, nmemb);
}

int OggFile_Seek(void *datasource, ogg_int64_t offset, int whence)
{
	return ((ZEResourceFile*)datasource)->Seek(offset, (ZESeekFrom)whence);
}

long OggFile_Tell(void *datasource)
{
	return((ZEResourceFile*)datasource)->Tell();
}

int OggFile_Close(void *datasource)
{
	((ZEResourceFile*)datasource)->Close();
	return 0;
}

const char* ZESoundResource::GetResourceType() const
{
	return "Sound Resource";
}

ZESoundResource*  ZESoundResource::LoadOggFile(const char* FileName)
{
	ZESoundResource* Temp =(ZESoundResource*) zeResources->GetResource(FileName);
	if(Temp == NULL)
	{
		Temp = new ZESoundResource();
		ZEResourceFile File;
		
		if(File.Open(FileName))
		{
			vorbis_info*      VorbisInfo;
			OggVorbis_File    OggFile;

			ov_callbacks Callbacks;
			
			Callbacks.close_func = OggFile_Close;
			Callbacks.read_func = OggFile_Read;
			Callbacks.seek_func = OggFile_Seek;
			Callbacks.tell_func = OggFile_Tell;

			if(ov_open_callbacks(&File, &OggFile, NULL, 0, Callbacks)==0)
			{
				VorbisInfo =ov_info(&OggFile, -1);
				Temp->ChannelCount = VorbisInfo->channels;
				Temp->BitsPerSample = 16;
				Temp->BufferSize = ov_pcm_total(&OggFile, -1) * Temp->BitsPerSample / 8;
   				Temp->SamplesPerSecond = VorbisInfo->rate;
				Temp->BlockAlign = 2 * VorbisInfo->channels;
			
				
				long  BytesRead = 1;
				int   Position = 0;		
				int	  Section = 0;
				Temp->Buffer = new unsigned char[Temp->BufferSize];
				while(BytesRead > 0 && Temp->BufferSize > Position)
				{	
					BytesRead = ov_read(&OggFile, (char*)(Temp->Buffer + Position), Temp->BufferSize - Position, 0, 2, 1, &Section);
					Position =  Position + BytesRead;
					if(BytesRead < 0)
					{
						zeError("Ogg Sound Resource", "Error decoding ogg. FileName : \"%s\"", FileName);
						delete Temp;					
						return NULL;
					}
				}
				Temp->SetFileName(FileName);
		
			}
			else
			{
				zeError("Ogg Sound Resource", "Can not read ogg. FileName : \"%s\"", FileName);
				return NULL;
			}
			return Temp;
		}
		else
		{
			zeError("Ogg Sound Resource", "Can not open ogg file. FileName : \"%s\"", FileName);
			return NULL;
		}
	}
	else
		return NULL;
}

ZESoundResource* ZESoundResource::LoadWaveFile(const char* FileName)
{
	ZESoundResource* Temp =(ZESoundResource*) zeResources->GetResource(FileName);
	if (Temp == NULL)
	{	
		ZEResourceFile File;
		
		
		if (File.Open(FileName))
		{
			struct
			{	
				unsigned int	Header;
				unsigned int	Size;
				unsigned int	Format;
			} Riff;

			struct
			{ 
				unsigned int	Header;      
				unsigned int	Size;    
				unsigned short	AudioFormat;
				unsigned short	NumChannels;     
				unsigned int	SampleRate;       
				unsigned int	ByteRate;         
				unsigned short	BlockAlign;       
				unsigned short	BitsPerSample;
			}Fmt;

			struct
			{
				unsigned int	Header;    
				unsigned int	Size;     
			}Data;

						

			File.Read(&Riff, sizeof(Riff), 1);
			if (Riff.Header != 'FFIR')
			{
				zeError("Sound Resource", "Wrong wave file. FileName : \"%s\"", FileName);
				return NULL;
			}

			if (Riff.Format != 'EVAW')
			{
				zeError("Sound Resource", "Wave file format it not supported. FileName : \"%s\"", FileName);
				return NULL;
			}

			File.Read(&Fmt, sizeof(Fmt), 1);
			if (Fmt.Header != ' tmf')
			{
				zeError("Sound Resource", "Wrong wave file. FileName : \"%s\"", FileName);
				return NULL;
			}
			
			if (Fmt.AudioFormat != 1)
			{
				zeError("Sound Resource", "Wave file audio format it not supported. FileName : \"%s\"", FileName);
				return NULL;
			}

			File.Read(&Data, sizeof(Data), 1);
			if (Data.Header != 'atad')
			{
				zeError("Sound Resource", "Wrong wave file. FileName : \"%s\"", FileName);
				return NULL;
			}

			Temp = new ZESoundResource();

			Temp->SetFileName(FileName);
			Temp->BitsPerSample = Fmt.BitsPerSample;
			Temp->BlockAlign = Fmt.BlockAlign;
			Temp->ChannelCount = Fmt.NumChannels;
			Temp->SamplesPerSecond = Fmt.SampleRate;
			
			Temp->BufferSize = Data.Size;
			Temp->Buffer = new unsigned char[Data.Size];
			File.Read(Temp->Buffer, 1, Data.Size);	

			File.Close();
		}
		return Temp;
	}
		
	else
	{
		Temp->Release();
		return NULL;
	}		
}
const ZESoundOutputFormat& ZESoundResource::GetOutputFormat()
{
	return this->OutputFormat;
	
	/*ZESoundOutputFormat Temp;
	Temp.BitsPerSample = this->BitsPerSample;
	Temp.BufferSize   = this->BufferSize;
	Temp.ChannelCount = this->ChannelCount;
	Temp.SamplesPerSecond = this->SamplesPerSecond;

	return Temp;*/
	
}
size_t ZESoundResource::GetBufferSize()
{
	return BufferSize;
	
}
void ZESoundResource::FillBuffer(size_t BufferPosition, unsigned char* Buffer, size_t BufferSize)
{
	FileFormat=GetResourceFormat();
	switch(FileFormat)
	{
		case SOUNDFILEFORMAT_WAVE:
			break;
		case SOUNDFILEFORMAT_OGG:
			oggpack_buffer OggBuffer;
			int Bytes;
			oggpack_writeinit(&OggBuffer);
			oggpack_readinit(&OggBuffer, Buffer, Bytes);
			OggBuffer.ptr = (unsigned char*)&BufferPosition;
			while(BufferSize == oggpack_bytes(&OggBuffer))
			{
				Bytes = oggpack_read(&OggBuffer,Bytes);
				oggpack_write(&OggBuffer,BufferSize,Bytes);
			}
			break;
		case SOUNDFILEFORMAT_NONE:
			zeError("Sound Resource", "SoundFileFormat Unknown");
			break;
	}
}

void  ZESoundResource::CacheResource(const char *FileName)
{
	ZESoundResource* NewResource = (ZESoundResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName);
		if (NewResource != NULL)
		{
			NewResource->Cached = true;
			NewResource->ReferenceCount = 0;
			zeResources->AddResource(NewResource);
		}
	}

}

ZESoundResource* ZESoundResource::LoadSharedResource(const char *FileName)
{
	ZESoundResource* NewResource =(ZESoundResource*)zeResources->GetResource(FileName);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(FileName);
		if (NewResource != NULL)
		{
			NewResource->Cached = false;
			NewResource->ReferenceCount = 1;
			zeResources->AddResource(NewResource);
			return NewResource;
		}
		else
			return NULL;
	}
	else
		return NewResource;

}

ZESoundResource* ZESoundResource::LoadResource(const char* FileName)
{
	ZESoundResource* Temp =(ZESoundResource*) zeResources->GetResource(FileName);
	if (Temp == NULL)
	{
		ZEResourceFile File;
		switch(GetFileFormat(FileName))
		{
			case SOUNDFILEFORMAT_WAVE:
					
					Temp = ZESoundResource::LoadWaveFile(FileName);
					if(Temp != NULL)
					{
						
						return Temp;
					}
					else
					{
						zeError("Sound Resource", "Could not load sound file. FileName : \"%s\"." , FileName);
						return NULL;
					}
					break;
				
			case SOUNDFILEFORMAT_OGG:
				
					Temp=ZESoundResource::LoadOggFile(FileName);
					if (Temp != NULL)
					{
						
						return Temp;
					}
					else
					{
					zeError("Sound Resource", "Could not load sound file. FileName : \"%s\"." , FileName);
					return NULL;
					}
					break;
				
			default:	
			case SOUNDFILEFORMAT_NONE:
				
					zeError("Sound Resource", "Unknown sound file format. FileName : \"%s\"." , FileName);
					return NULL;
					break;
		}
	}
	else
	{
		
		return Temp;
	}
}

ZESoundFileFormat ZESoundResource::GetFileFormat(const char* FileName)
{
	if(!_strnicmp(&FileName[strlen(FileName) - 3],"mp3",3))
		return SOUNDFILEFORMAT_MP3;
	else if(!_strnicmp(&FileName[strlen(FileName) - 3],"wav",3))
		return  SOUNDFILEFORMAT_WAVE;
	else if(!_strnicmp(&FileName[strlen(FileName) - 3],"ogg",3))
		return  SOUNDFILEFORMAT_OGG;
	else
		return  SOUNDFILEFORMAT_NONE;
}

ZESoundFileFormat ZESoundResource::GetResourceFormat()
{ 
	return FileFormat;
}

ZESoundResource::ZESoundResource()
{
	Buffer = NULL;
}

ZESoundResource::~ZESoundResource()
{
	if (Buffer != NULL)
		delete Buffer;
}
