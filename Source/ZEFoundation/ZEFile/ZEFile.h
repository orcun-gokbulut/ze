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


enum ZESeekFrom
{
	ZE_SF_BEGINING			= 0,
	ZE_SF_END				= 1,
	ZE_SF_CURRENT			= 2
};

// ZE_FOM_READ: Read only, can be seekable
// ZE_FOM_WRITE: Write only, can be seekable
// ZE_FOM_NOTSET: File is not open
// ZE_FOM_READ_WRITE: Both reading and writing
enum ZEFileOpenMode
{
	ZE_FOM_READ				= 0,
	ZE_FOM_WRITE			= 1,
	ZE_FOM_READ_WRITE		= 2
};


// ZE_FCT_OPEN : Opens an existing file. If there is no file, gives error.						
// ZE_FCT_OPEN_CREATE : Creates a new file if file does not exists, otherwise opens the existing file.	
// ZE_FCT_OPEN_CREATE_OVERWRITE : Creates a file, if file exists the file's content is deleted.				
enum ZEFileCreationMode
{
	ZE_FCM_NONE				= 0,	
	ZE_FCM_CREATE			= 1,	
	ZE_FCM_OVERWRITE		= 2

};


class ZEFile : public ZESerializer, public ZEUnserializer
{
	protected:
		void*					File;
		ZEString				Path;
		ZEFileOpenMode			OpenMode;
		ZEFileCreationMode		CreationMode;

	public:
								ZEFile();
		virtual					~ZEFile();

		virtual ZEInt			Close();
		virtual bool			Open(const ZEString& FilePath, const ZEFileOpenMode FileOpenMode, const ZEFileCreationMode FileCreationMode);
		
		virtual ZESize			Read(void* Buffer, const ZESize Size, const ZESize Count);
		virtual ZESize			Write(const void* Buffer, const ZESize Size, const ZESize Count);
		
		virtual ZEInt			Seek(const ZEInt64 Offset, const ZESeekFrom Origin);
		virtual ZEInt64			Tell() const;
		
		virtual ZEInt			Eof() const;
		virtual ZEInt			Flush() const;
		virtual bool			IsOpen() const;
		
		virtual ZEInt64			GetSize();
		const ZEString&			GetPath() const;
		void*					GetHandle() const;
		ZEFileOpenMode			GetOpenMode() const;
		ZEFileCreationMode		GetCreationMode() const;

		ZEFile&					operator = (ZEFile& OtherFile);

		static bool				ReadFile(const ZEString& FilePath, void* Buffer, const ZESize BufferSize);
		static bool				ReadTextFile(const ZEString& FilePath, char* Buffer, const ZESize BufferSize);

};

#endif
