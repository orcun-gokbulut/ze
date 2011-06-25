//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFile.h
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
#ifndef	__ZE_FILE_H__
#define __ZE_FILE_H__

#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"
#include "ZEDefinitions.h"

enum ZESeekFrom
{
	ZE_SF_BEGINING	= 0,
	ZE_SF_END		= 1,
	ZE_SF_CURRENT	= 2,
};

enum ZEFileMode
{
	ZE_FM_READ_ONLY			= 0,
	ZE_FM_WRITE_ONLY		= 1,
	ZE_FM_APPEND_ONLY		= 2,
	ZE_FM_READ_WRITE		= 3,
	ZE_FM_READ_APPEND		= 4,
};

class ZEPartialFile;
class ZEFile : public ZESerializer, public ZEUnserializer
{
	protected:

		void*				File;
		char				FileName[256];

	public:

							ZEFile();
		virtual				~ZEFile();

		const char*			GetFileName();
		void*				GetFileHandle();

		virtual bool		Open(const char* FileName, ZEFileMode Mode = ZE_FM_READ_WRITE, bool Binary = true);
		virtual bool		Seek(size_t Offset, ZESeekFrom Origin);
		virtual size_t		Tell();

		virtual size_t		Read(void* Buffer, size_t Size, size_t Count);
		virtual size_t		ReadFormated(const char* Format, ...);

		virtual size_t		Write(void* Buffer, size_t Size, size_t Count);
		virtual size_t		WriteFormated(const char* Format, ...);

		static size_t		GetFileSize();
		virtual void		Close();
		virtual bool		Eof();

		
		static bool			ReadFile(const char* FileName, void* Buffer, size_t BufferSize);
		static bool			ReadTextFile(const char* FileName, char* Buffer, size_t BufferSize);

};




class ZEPartialFile : public ZEFile
{
	protected:

		ZEFile*				ParentFile;
		size_t				StartPosition;
		size_t				EndPosition;
		bool				IsEof;

	public:
							
							ZEPartialFile(ZEFile* File, size_t Offset, size_t Size);
		virtual				~ZEPartialFile();

		virtual size_t		Read(void* Buffer, size_t Size, size_t Count);
		virtual size_t		ReadFormated(const char* Format, ...);

		virtual size_t		Write(void* Buffer, size_t Size, size_t Count);
		virtual size_t		WriteFormated(const char* Format, ...);


		/* If the seek operation tends to go beyond end position or below start position
		the file cursor is set to start position and end position respectively and the return will be false */
		virtual bool		Seek(size_t Offset, ZESeekFrom Origin);
		virtual size_t		Tell();

		static size_t		GetFileSize();
		virtual bool		Eof();

							
};

#endif
