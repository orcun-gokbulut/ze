//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundResource.cpp
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

#include "ZESoundResource.h"
#include "ZEError.h"
#include "ZECore/ZEConsole.h"
#include "ZECore/ZEResourceManager.h"
#include "ZESoundResourceMP3.h"
#include "ZESoundResourceOGG.h"
#include "ZESoundResourceWAV.h"
#include "ZEFile/ZEPathUtils.h"

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

ZESoundFileFormat ZESoundResource::GetFileFormat(const ZEString& FileName)
{
	if (FileName.Right(3).Lower() == "mp3")
	{
		return ZE_SFF_MP3;
	}
	else if(FileName.Right(3).Lower() == "wav")
	{
		return  ZE_SFF_WAVE;
	}
	else if(FileName.Right(3).Lower() == "ogg")
	{
		return  ZE_SFF_OGG;
	}
	else
	{
		return  ZE_SFF_NONE;
	}
}

const char* ZESoundResource::GetResourceType() const
{
	return "Sound Resource";
}

ZESoundFileFormat ZESoundResource::GetSoundFileFormat() const
{ 
	return FileFormat;
}

ZESize ZESoundResource::GetSamplesPerSecond() const
{
	return SamplesPerSecond;
}

ZEUInt ZESoundResource::GetChannelCount() const
{
	return ChannelCount;
}

ZEUInt ZESoundResource::GetBitsPerSample() const
{
	return BitsPerSample;
}

ZESize ZESoundResource::GetBlockAlign() const
{
	return BlockAlign;
}

ZESize ZESoundResource::GetSampleCount() const
{
	return SampleCount;
}

ZESize ZESoundResource::GetUncompressedDataSize() const
{
	return SampleCount * BlockAlign;
}

ZESoundResource* ZESoundResource::LoadResource(const ZEString& FileName)
{
	zeLog("Loading sound file \"%s\"", FileName.ToCString());

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
			zeError("Unknown sound file format. (FileName : \"%s\")" , FileName.ToCString());
			return NULL;
			break;
	}

	if (Temp == NULL)
	{
		zeError("Could not load sound file. (FileName : \"%s\")" , FileName.ToCString());
		return NULL;
	}

	zeLog("Sound file \"%s\" has been loaded.", FileName.ToCString());

	return Temp;
}

ZESoundResource* ZESoundResource::LoadSharedResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	NewPath = ZEPathUtils::GetSimplifiedPath(NewPath, false);

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

void ZESoundResource::CacheResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	NewPath = ZEPathUtils::GetSimplifiedPath(NewPath, false);

	ZESoundResource* NewResource = (ZESoundResource*)zeResources->GetResource(NewPath);
	if (NewResource == NULL)
	{
		NewResource = LoadResource(NewPath);
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
	FileFormat = ZE_SFF_NONE;
	SampleCount = 0;
	SamplesPerSecond = 0;
	ChannelCount = 0;
	BitsPerSample = 0;
	BlockAlign = 0;
}

ZESoundResource::~ZESoundResource()
{

}
