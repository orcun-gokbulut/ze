//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - SoundResource.h
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

#pragma once
#ifndef	__ZE_SOUNDRESOURCE_H__
#define __ZE_SOUNDRESOURCE_H__

#include "Core/Resource.h"

enum ZESoundFileFormat
{
	SOUNDFILEFORMAT_NONE,
	SOUNDFILEFORMAT_WAVE,
	SOUNDFILEFORMAT_OGG,
	SOUNDFILEFORMAT_MP3
};

class ZESoundOutputFormat
{
	public:
	unsigned int					SamplesPerSecond;
	short int						ChannelCount;
	short int						BitsPerSample;
	short int						BlockAlign;
	unsigned int					BufferSize;
};

class ZESoundResource : public ZEResource
{
	private:
		ZESoundFileFormat			FileFormat;
		ZESoundOutputFormat			OutputFormat;
		size_t						DataSize;
		unsigned char*				Data;
		static ZESoundFileFormat	GetFileFormat(const char* Filename);
		static ZESoundResource* 	LoadWaveFile(const char* Filename);
		static ZESoundResource* 	LoadOggFile (const char* Filename);
		static ZESoundResource* 	LoadMp3File(const char* Filename);
	
	public:
		const char*					GetResourceType() const;

		ZESoundFileFormat			GetResourceFormat();
		unsigned int				SamplesPerSecond;
		short int					ChannelCount;
		short int					BitsPerSample;
		short int					BlockAlign;
		unsigned int				BufferSize;
		unsigned char*				Buffer;

		const ZESoundOutputFormat&	GetOutputFormat();
		size_t						GetBufferSize();
		void						FillBuffer(size_t BufferPosition, unsigned char* Buffer, size_t BufferSize);

		static void					CacheResource(const char* Filename);
		static ZESoundResource*		LoadSharedResource(const char* Filename); 
		static ZESoundResource*		LoadResource(const char* Filename);
		

									ZESoundResource();
									~ZESoundResource();
};

#endif
