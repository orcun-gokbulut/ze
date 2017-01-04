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
#include "ZEFile/ZEPathInfo.h"
#include "ZESoundResourceMP3.h"
#include "ZESoundResourceOGG.h"
#include "ZESoundResourceWAV.h"
#include "ZEResource/ZERSTemplates.h"


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

ZERSResource* ZESoundResource::Instanciator(const void* Parameters)
{
	ZEPathInfo FileInfo(*reinterpret_cast<const ZEString*>(Parameters));

	if (FileInfo.GetExtension().EqualsIncase(".mp3"))
		return new ZESoundResourceMP3();
	else if(FileInfo.GetExtension().EqualsIncase(".wav"))
		return  new ZESoundResourceWAV();
	else if(FileInfo.GetExtension().EqualsIncase(".ogg"))
		return  new ZESoundResourceOGG();
	
	return  NULL;
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

ZEHolder<ZESoundResource> ZESoundResource::LoadResource(const ZEString& FileName)
{
	return ZERSTemplates::LoadResource<ZESoundResource>(FileName, Instanciator, &FileName);
}

ZEHolder<const ZESoundResource> ZESoundResource::LoadResourceShared(const ZEString& FileName)
{
	return ZERSTemplates::LoadResourceShared<ZESoundResource>(FileName, Instanciator, &FileName);
}
