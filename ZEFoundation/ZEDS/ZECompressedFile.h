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
#include "../Source/ZEDefinitions.h"

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


class ZECompressedFile
{
	protected:

		void*				File;
		char				FileName[256];
		
	public:

							ZECompressedFile();
		virtual				~ZECompressedFile();

		ZEQWORD				FileCursor;


		const char*			GetFileName() const;
		void*				GetFileHandle() const;

		virtual bool		Open(const char* FileName, ZEFileMode Mode = ZE_FM_READ_WRITE, bool Binary = true); 

		virtual bool		Seek(ZEINT64 Offset, ZESeekFrom Origin);		

		virtual ZEQWORD		Tell();										

		virtual ZEQWORD		Read(void* Buffer, ZEQWORD Size, ZEQWORD Count);		
		
		virtual ZEQWORD		Write(const void* Buffer, ZEQWORD Size, ZEQWORD Count);	
		
		static ZEQWORD		GetFileSize(const char* FileName);
		virtual ZEQWORD		GetFileSize();
		
		virtual void		Close();
		virtual bool		Eof();

		virtual void		Flush();
		virtual bool		IsOpen();

};


class ZEPartialCompressedFile : public ZECompressedFile
{
	protected:

		ZEQWORD				StartPosition;
		ZEQWORD				EndPosition;
		bool				IsEof;
		ZECompressedFile*	BaseFile;

	public:

		ZEPartialCompressedFile();
		virtual				~ZEPartialCompressedFile();


		virtual bool		Open(ZECompressedFile* ParentFile, ZEQWORD Offset, ZEQWORD Size);
		virtual bool		Open(const char* FileName, ZEFileMode Mode, bool Binary);
		virtual void		Close();

		virtual ZEQWORD		Read(void* Buffer, ZEQWORD Size, ZEQWORD Count);

		virtual ZEQWORD		Write(void* Buffer, ZEQWORD Size, ZEQWORD Count);


		/* If the seek operation tends to go beyond end position or below start position
		the file cursor remains as it is and the return will be false */
		virtual bool		Seek(ZEINT64 Offset, ZESeekFrom Origin);
		virtual ZEQWORD		Tell();

		virtual ZEQWORD		GetFileSize();
		virtual bool		Eof();				
};

#endif
