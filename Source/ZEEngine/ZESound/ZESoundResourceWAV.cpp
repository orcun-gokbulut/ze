//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundResourceWAV.cpp
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

#include "ZESoundResourceWAV.h"
#include "ZEError.h"
#include <memory.h>

static ZEString ConstructResourcePath(const ZEString& Path)
{
	ZEString NewString = Path;
	unsigned int ConstLength = strlen("resources\\") - 1;

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

ZESoundResourceWAV::ZESoundResourceWAV()
{
	Data = NULL;
}

ZESoundResourceWAV::~ZESoundResourceWAV()
{
	if (Data != NULL)
		delete Data;
}

size_t ZESoundResourceWAV::GetDataSize() const
{
	return DataSize;
}

const void* ZESoundResourceWAV::GetData() const
{
	return Data;
}

void ZESoundResourceWAV::Decode(void* Buffer, size_t SampleIndex, size_t Count)
{
	zeAssert(SampleIndex + Count > SampleCount, "Sample decoding range (SampleIndex + Count) exceed sample count.");
	memcpy(Buffer, (ZEUInt8*)Data + SampleIndex * BlockAlign, Count * BlockAlign);
}

ZESoundResource* ZESoundResourceWAV::LoadResource(const ZEString& FileName)
{
	ZEString NewPath = ConstructResourcePath(FileName);

	ZEFile* File = ZEFile::Open(NewPath);
	if (File == NULL || !File->IsOpen())
	{
		zeError("Can not load WAV resource. Can not open file. (Filename : \"%s\")", NewPath);
		return NULL;
	}

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

	File->Read(&Riff, sizeof(Riff), 1);
	if (Riff.Header != 'FFIR')
	{
		zeError("Wrong wave file. (FileName : \"%s\")", NewPath);
		return NULL;
	}

	if (Riff.Format != 'EVAW')
	{
		zeError("Wave file format it not supported. (FileName : \"%s\")", NewPath);
		return NULL;
	}

	File->Read(&Fmt, sizeof(Fmt), 1);
	if (Fmt.Header != ' tmf')
	{
		zeError("Wrong wave file. (FileName : \"%s\")", NewPath);
		return NULL;
	}
	
	if (Fmt.AudioFormat != 1)
	{
		zeError("Wave file audio format it not supported. (FileName : \"%s\")", NewPath);
		return NULL;
	}

	File->Read(&Data, sizeof(Data), 1);
	if (Data.Header != 'atad')
	{
		zeError("Wrong wave file. (FileName : \"%s\")", NewPath);
		return NULL;
	}

	ZESoundResourceWAV* NewResource = new ZESoundResourceWAV();
	NewResource->SetFileName(NewPath);
	NewResource->BitsPerSample = Fmt.BitsPerSample;
	NewResource->BlockAlign = Fmt.BlockAlign;
	NewResource->ChannelCount = Fmt.NumChannels;
	NewResource->SamplesPerSecond = Fmt.SampleRate;
	NewResource->DataSize = Data.Size;
	NewResource->SampleCount = NewResource->DataSize / NewResource->BlockAlign;
	NewResource->Data = new unsigned char[Data.Size];
	File->Read(NewResource->Data, 1, Data.Size);	
	File->Close();
	delete File;

	return NewResource;
}




