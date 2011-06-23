//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFileCache.h
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
#ifndef __ZE_FILE_CACHE_H__
#define __ZE_FILE_CACHE_H__

#include "ZEArray.h"
#include "ZETypes.h"
#include <stdio.h>
class ZEResourceFile;

class ZECacheChunkIdentifier
{
	public:
		virtual ZEDWORD					GetHash() const = 0;
		virtual size_t					Write(void* File) const = 0;
		virtual bool					Equal(void* File) const = 0;
};

class ZEFileCache
{
	private:
		FILE*							File;
		
	public:
		bool							OpenCache(const char* FileName);
		void							CloseCache();

		bool							OpenChunk(const ZECacheChunkIdentifier* Identifier);
		void							AddToChunk(void* Data, size_t Size);
		void							CloseChunk();
		
		
		void							AddChunk(const ZECacheChunkIdentifier* Identifier, const void* Data, size_t Size);
		bool							GetChunkData(const ZECacheChunkIdentifier* Identifier, void* Buffer, size_t Offset, size_t Size);
		ZEResourceFile&					GetChunkAsFile(const ZECacheChunkIdentifier* Identifier);

		void							ClearCache();

										ZEFileCache();
										~ZEFileCache();
};

#endif
