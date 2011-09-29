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

#include "ZEDS/ZEString.h"
#include "ZESerialization/ZESerializer.h"
#include "ZESerialization/ZEUnserializer.h"
#include "ZEToolSDK/ZETTypes.h"


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
//int __cdecl _tinput_l(FILE*, const TCHAR*, _locale_t, va_list);


class ZEFile : public ZESerializer, public ZEUnserializer
{
protected:

	void*				File;
	ZEString			FileName;
	ZEQWORD				FileCursor;

public:

	ZEFile();
	virtual				~ZEFile();

	const ZEString&		GetFileName() const;
	void*				GetFileHandle() const;

	virtual bool		Open(const ZEString FileName, ZEFileMode Mode = ZE_FM_READ_WRITE, bool Binary = true);
	virtual bool		Seek(ZEINT64 Offset, ZESeekFrom Origin);
	virtual ZEQWORD		Tell();

	virtual ZEQWORD		Read(void* Buffer, ZEQWORD Size, ZEQWORD Count);
	virtual ZEQWORD		ReadFormated(const char* Format, ...);

	virtual ZEQWORD		Write(const void* Buffer, ZEQWORD Size, ZEQWORD Count);
	virtual ZEQWORD		WriteFormated(const char* Format, ...);

	static ZEQWORD		GetFileSize(const ZEString FileName);
	virtual ZEQWORD		GetFileSize();

	virtual void		Close();
	virtual bool		Eof();

	virtual void		Flush();
	virtual bool		IsOpen();

	static bool			ReadFile(const ZEString FileName, void* Buffer, ZEQWORD BufferSize);
	static bool			ReadTextFile(const ZEString FileName, char* Buffer, ZEQWORD BufferSize);

	static ZEString		GetAbsolutePath(const ZEString Path);

	static bool			IsDirectoryExists(const ZEString Path);
	static ZEString		GetParentDirectory(const ZEString Path);

	static bool			IsFileExists(const ZEString Path);
	static ZEString		GetFileName(const ZEString Path);
	static ZEString		GetFileExtension(const ZEString Path);

	static ZEFile*		Open(const ZEString FilePath);
};


class ZEPartialFile : public ZEFile
{
protected:

	ZEQWORD				StartPosition;
	ZEQWORD				EndPosition;
	bool				IsEof;

public:
	ZEPartialFile();
	virtual				~ZEPartialFile();


	virtual bool		Open(ZEFile* ParentFile, ZEQWORD Offset, ZEQWORD Size);
	virtual bool		Open(const ZEString FileName, ZEFileMode Mode, bool Binary);
	virtual void		Close();

	virtual ZEQWORD		Read(void* Buffer, ZEQWORD Size, ZEQWORD Count);
	virtual ZEQWORD		ReadFormated(const char* Format, ...);

	virtual ZEQWORD		Write(void* Buffer, ZEQWORD Size, ZEQWORD Count);
	virtual ZEQWORD		WriteFormated(const char* Format, ...);


	/* If the seek operation tends to go beyond end position or below start position
	the file cursor remains as it is and the return will be false */
	virtual bool		Seek(ZEINT64 Offset, ZESeekFrom Origin);
	virtual ZEQWORD		Tell();

	virtual ZEQWORD		GetFileSize();
	virtual bool		Eof();


};

#endif
