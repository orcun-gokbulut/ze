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

#include "ZETypes.h"
#include "ZEDS/ZEString.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"

enum ZEFileType
{
	ZE_FT_FILE		= 0,
	ZE_FT_PARTIAL	= 1,
};

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


////For formatted read however it does not work on CRT DLL
//#ifdef _DLL
//#error "In order to compile this code, /MT(d) instead of /MD(d) must be used.
//#endif
//
//#ifdef _UNICODE
//#define _tinput_l _winput_l
//#else
//#define _tinput_l _input_l
//#endif 
//
//extern "C"
//ZEInt __cdecl _tinput_l(FILE*, const TCHAR*, _locale_t, va_list);

class ZEPartialFile;
class ZECacheDataIdentifier;

class ZEFile : public ZESerializer, public ZEUnserializer
{
	friend class ZEPartialFile;

	protected:
		void*					File;				// C file pointer
		ZEString				FilePath;			
		ZEUInt64				FileCursor;

		ZEFileType				FileType;
		bool					AutoClose;			// Closes the file when reference count goes zero
		ZEUInt					ReferenceCount;		// Count of how many files are open on this file

		virtual ZEUInt			IncreaseReferenceCount();
		virtual ZEUInt			DecreaseReferenceCount();

	public:
		virtual bool			Open(const ZEString& FilePath, ZEFileMode Mode, bool Binary);
		virtual bool			Seek(ZEInt64 Offset, ZESeekFrom Origin);
		virtual ZEUInt64		Tell();
		virtual void			Close();
		virtual bool			Eof();
		virtual void			Flush();
		virtual bool			IsOpen();

		void					SetAutoClose(bool AutoClose);
		bool					GetAutoClose();
		
		virtual ZEUInt64		Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count);
		virtual ZEUInt64		ReadFormated(const char* Format, ...);

		virtual ZEUInt64		Write(const void* Buffer, ZEUInt64 Size, ZEUInt64 Count);
		virtual ZEUInt64		WriteFormated(const char* Format, ...);

		static ZEUInt64			GetFileSize(const ZEString& FilePath);
		virtual ZEUInt64		GetFileSize();

		static bool				ReadFile(const ZEString& FilePath, void* Buffer, ZEUInt64 BufferSize);
		static bool				ReadTextFile(const ZEString& FilePath, char* Buffer, ZEUInt64 BufferSize);

		ZEFileType				GetFileType() const;
		void*					GetFileHandle() const;
		const ZEString			GetFilePath() const;

		virtual ZEUInt64		GetStartPosition();
		virtual ZEUInt64		GetEndPosition();

		ZEUInt					GetReferenceCount() const;

		static ZEString			GetFileName(const ZEString& FilePath);
		static ZEString			GetAbsolutePath(const ZEString& FilePath);
		static ZEString			GetFileExtension(const ZEString& FilePath);
		static ZEString			GetParentDirectory(const ZEString& FilePath);
		
		static bool				IsDirectoryExists(const ZEString& FilePath);
		static bool				IsFileExists(const ZEString& FilePath);

		static ZEFile*			Open(const ZEString& FilePath, bool AutoClose = true);

		ZEFile&					operator = (ZEFile& OtherFile);

								ZEFile();
		virtual					~ZEFile();

};




#endif
