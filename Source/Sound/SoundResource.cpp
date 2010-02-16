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
#include "Core/Error.h"
#include "Core/ResourceManager.h"
#include "SoundResourceMP3.h"
#include "SoundResourceOGG.h"
#include "SoundResourceWAV.h"

ZESoundFileFormat ZESoundResource::GetFileFormat(const char* FileName)
{
	if(!_strnicmp(&FileName[strlen(FileName) - 3],"mp3",3))
		return ZE_SFF_MP3;
	else if(!_strnicmp(&FileName[strlen(FileName) - 3],"wav",3))
		return  ZE_SFF_WAVE;
	else if(!_strnicmp(&FileName[strlen(FileName) - 3],"ogg",3))
		return  ZE_SFF_OGG;
	else
		return  ZE_SFF_NONE;
}

const char* ZESoundResource::GetResourceType() const
{
	return "Sound Resource";
}

ZESoundFileFormat ZESoundResource::GetSoundFileFormat() const
{ 
	return FileFormat;
}

unsigned int ZESoundResource::GetSamplesPerSecond() const
{
	return SamplesPerSecond;
}

short int ZESoundResource::GetChannelCount() const
{
	return ChannelCount;
}

short int ZESoundResource::GetBitsPerSample() const
{
	return BitsPerSample;
}

short int ZESoundResource::GetBlockAlign() const
{
	return BlockAlign;
}

unsigned int ZESoundResource::GetSampleCount() const
{
	return SampleCount;
}

size_t ZESoundResource::GetPCMDataSize() const
{
	return SampleCount * BlockAlign;
}

ZESoundResource* ZESoundResource::LoadResource(const char* FileName)
{
	ZESoundResource* Temp = NULL;
	switch(GetFileFormat(FileName))
	{
		case ZE_SFF_WAVE:		
			Temp = ZESoundResourceWAV::LoadResource(FileName);
			break;
			
		case ZE_SFF_OGG:
			Temp = ZESoundResourceOGG::LoadResource(FileName);
			break;
		
		case ZE_SFF_MP3:
			Temp = ZESoundResourceMP3::LoadResource(FileName);
			break;

		default:	
		case ZE_SFF_NONE:			
			zeError("Sound Resource", "Unknown sound file format. (FileName : \"%s\")" , FileName);
			return NULL;
			break;
	}

	if (Temp == NULL)
	{
		zeError("Sound Resource", "Could not load sound file. (FileName : \"%s\")" , FileName);
		return NULL;
	}

	return Temp;
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

void ZESoundResource::CacheResource(const char *FileName)
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

ZESoundResource::ZESoundResource()
{

}

ZESoundResource::~ZESoundResource()
{

}
