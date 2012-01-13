//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECompressedFile.h
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
#ifndef	__ZE_COMPRESSED_FILE_H__
#define __ZE_COMPRESSED_FILE_H__

#include "ZETypes.h"
#include "ZEFile/ZEFile.h"


class ZECompressedFile
{
	friend class ZEPartialCompressedFile;
	protected:
		void*				File;
		char				FileName[256];
		ZEUInt64				FileCursor;
		
	public:
		const char*			GetFileName() const;
		void*				GetFileHandle() const;

		virtual bool		Open(const char* FileName, ZEFileMode Mode = ZE_FM_READ_WRITE, bool Binary = true); 

		virtual bool		Seek(ZEInt64 Offset, ZESeekFrom Origin);		

		virtual ZEUInt64		Tell();										

		virtual ZEUInt64		Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count);				
		virtual ZEUInt64		Write(const void* Buffer, ZEUInt64 Size, ZEUInt64 Count);	
		
		static ZEUInt64		GetFileSize(const char* FileName);
		virtual ZEUInt64		GetFileSize();
		
		virtual void		Close();
		virtual bool		Eof();

		virtual void		Flush();
		virtual bool		IsOpen();

							ZECompressedFile();
		virtual				~ZECompressedFile();
};


class ZEPartialCompressedFile : public ZECompressedFile
{
	protected:
		ZEUInt64				StartPosition;
		ZEUInt64				EndPosition;
		bool				IsEof;
		ZECompressedFile*	BaseFile;

	public:
		virtual bool		Open(ZECompressedFile* ParentFile, ZEUInt64 Offset, ZEUInt64 Size);
		virtual bool		Open(const char* FileName, ZEFileMode Mode, bool Binary);
		virtual void		Close();

		virtual ZEUInt64		Read(void* Buffer, ZEUInt64 Size, ZEUInt64 Count);
		virtual ZEUInt64		Write(void* Buffer, ZEUInt64 Size, ZEUInt64 Count);

		virtual bool		Seek(ZEInt64 Offset, ZESeekFrom Origin);
		virtual ZEUInt64		Tell();

		virtual ZEUInt64		GetFileSize();
		virtual bool		Eof();				

							ZEPartialCompressedFile();
		virtual				~ZEPartialCompressedFile();
};

#endif
