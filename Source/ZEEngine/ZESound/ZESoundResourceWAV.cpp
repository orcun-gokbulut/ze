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
#include "ZEFile/ZEFile.h"

#include <memory.h>

ZEPackStruct
(
	struct ZEWAVRiffHeader
	{	
		ZEUInt32	Header;
		ZEUInt32	Size;
		ZEUInt32	Format;
	}
);

ZEPackStruct
(
	struct ZEWAVFMTHeader
	{ 
		ZEUInt32 Header;      
		ZEUInt32 Size;    
		ZEUInt16 AudioFormat;
		ZEUInt16 NumChannels;     
		ZEUInt32 SampleRate;       
		ZEUInt32 ByteRate;         
		ZEUInt16 BlockAlign;       
		ZEUInt16 BitsPerSample;
	};
);

ZEPackStruct
(
	struct ZEWAVDataHeader
	{
		ZEUInt32 Header;    
		ZEUInt32 Size;     
	}
);

ZETaskResult ZESoundResourceWAV::LoadInternal()
{
	ZEFile File;
	if (!File.Open(GetFileName(), ZE_FOM_READ, ZE_FCM_NONE))
	{
		zeError("Cannot load sound resource. Cannot open wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	ZEWAVRiffHeader RiffHeader;
	if (File.Read(&RiffHeader, sizeof(RiffHeader), 1) != 1)
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (RiffHeader.Header != 'FFIR')
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (RiffHeader.Format != 'EVAW')
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	ZEWAVFMTHeader FMTHeader;
	if (File.Read(&FMTHeader, sizeof(FMTHeader), 1) != 1)
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (FMTHeader.Header != ' tmf')
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (FMTHeader.AudioFormat != 1)
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	ZEWAVDataHeader DataHeader;
	if (File.Read(&DataHeader, sizeof(DataHeader), 1) != 1)
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	if (DataHeader.Header != 'atad')
	{
		zeError("Cannot load sound resource. Corrupted or unknown wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	BitsPerSample = FMTHeader.BitsPerSample;
	BlockAlign = FMTHeader.BlockAlign;
	ChannelCount = FMTHeader.NumChannels;
	SamplesPerSecond = (ZESize)FMTHeader.SampleRate;
	DataSize = (ZESize)DataHeader.Size;
	SampleCount = DataSize / BlockAlign;
	Data = new ZEBYTE[DataSize];
	
	if (File.Read(Data, DataSize, 1) != 1)
	{
		zeError("Cannot load sound resource. Cannot read data in file. Corrupted wav file. File Name : \"%s\".", GetFileName().ToCString());
		return ZE_TR_FAILED;
	}

	File.Close();

	return ZE_TR_DONE;
}

ZETaskResult ZESoundResourceWAV::UnloadInternal()
{
	if (Data != NULL)
		delete Data;

	Data = NULL;
	DataSize = 0;
	ChannelCount = 0;
	BitsPerSample = 0;
	SamplesPerSecond = 0;
	BlockAlign = 0;
	SampleCount = 0;

	return ZE_TR_DONE;
}

ZESoundResourceWAV::ZESoundResourceWAV()
{
	Data = NULL;
	Register();
}

ZESoundResourceWAV::~ZESoundResourceWAV()
{
	if (Data != NULL)
		delete Data;
}

ZESize ZESoundResourceWAV::GetDataSize() const
{
	return DataSize;
}

const void* ZESoundResourceWAV::GetData() const
{
	return Data;
}

bool ZESoundResourceWAV::Decode(void* Buffer, ZESize SampleIndex, ZESize Count) const
{
	if (!IsLoaded() || SampleIndex + Count > SampleCount)
	{
		memcpy(Buffer, 0, Count);
		return false;
	}

	memcpy(Buffer, (ZEUInt8*)Data + SampleIndex * BlockAlign, Count * BlockAlign);

	return true;
}
