//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundResourceWAV.cpp
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

#include "SoundResourceWAV.h"
#include "Core/Error.h"

ZESoundResourceWAV::ZESoundResourceWAV()
{
	Data = NULL;
}

ZESoundResourceWAV::~ZESoundResourceWAV()
{
	if (Data != NULL)
		delete Data;
}

unsigned int ZESoundResourceWAV::GetDataSize() const
{
	return DataSize;
}

const unsigned char* ZESoundResourceWAV::GetData() const
{
	return Data;
}

void ZESoundResourceWAV::Decode(void* Buffer, size_t SampleIndex, size_t Count)
{
	ZEASSERT(SampleIndex + Count > SampleCount, "Sample decoding range (SampleIndex + Count) exceed sample count.");
	memcpy(Buffer, Data + SampleIndex * BlockAlign, Count * BlockAlign);
}

ZESoundResource* ZESoundResourceWAV::LoadResource(const char* FileName)
{
	ZEResourceFile File;
	if (!File.Open(FileName))
	{
		zeError("Sound Resource WAV", "Can not load WAV resource. Can not open file. (Filename : \"%s\")");
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

	File.Read(&Riff, sizeof(Riff), 1);
	if (Riff.Header != 'FFIR')
	{
		zeError("Sound Resource", "Wrong wave file. (FileName : \"%s\")", FileName);
		return NULL;
	}

	if (Riff.Format != 'EVAW')
	{
		zeError("Sound Resource", "Wave file format it not supported. (FileName : \"%s\")", FileName);
		return NULL;
	}

	File.Read(&Fmt, sizeof(Fmt), 1);
	if (Fmt.Header != ' tmf')
	{
		zeError("Sound Resource", "Wrong wave file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	
	if (Fmt.AudioFormat != 1)
	{
		zeError("Sound Resource", "Wave file audio format it not supported. (FileName : \"%s\")", FileName);
		return NULL;
	}

	File.Read(&Data, sizeof(Data), 1);
	if (Data.Header != 'atad')
	{
		zeError("Sound Resource", "Wrong wave file. (FileName : \"%s\")", FileName);
		return NULL;
	}

	ZESoundResourceWAV* NewResource = new ZESoundResourceWAV();
	NewResource->SetFileName(FileName);
	NewResource->BitsPerSample = Fmt.BitsPerSample;
	NewResource->BlockAlign = Fmt.BlockAlign;
	NewResource->ChannelCount = Fmt.NumChannels;
	NewResource->SamplesPerSecond = Fmt.SampleRate;
	NewResource->DataSize = Data.Size;
	NewResource->SampleCount = NewResource->DataSize / NewResource->BlockAlign;
	NewResource->Data = new unsigned char[Data.Size];
	File.Read(NewResource->Data, 1, Data.Size);	
	File.Close();

	return NewResource;
}
