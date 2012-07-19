//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPartialFile.h
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
#ifndef __ZE_PARTIAL_FILE_H__
#define __ZE_PARTIAL_FILE_H__

#include "ZEFile.h"

class ZEPartialFile : public ZEFile
{
	protected:
		bool					EoF;
		ZEInt64					EndPosition;
		ZEInt64					StartPosition;
		
	public:
								ZEPartialFile();
		virtual					~ZEPartialFile();

		virtual ZEInt			Close();
		virtual bool			Open(const ZEFile* ParentFile, const ZEInt64 Offset, const ZEInt64 Size);
		virtual bool			Open(const ZEString& FilePath, const ZEFileOpenMode OpenType, const ZEFileCreationMode CreationType);

		virtual ZESize			Read(void* Buffer, const ZESize Size, const ZESize Count);
		virtual ZESize			Write(const void* Buffer, const ZESize Size, const ZESize Count);

		virtual ZEInt64			Tell() const;
		virtual ZEInt			Seek(const ZEInt64 Offset, const ZESeekFrom Origin);
		
		virtual ZEInt64			GetSize();

		virtual ZEInt64			GetEndPosition() const;
		virtual void			SetEndPosition(const ZEInt64 Position);
		
		virtual ZEInt64			GetStartPosition() const;
		virtual void			SetStartPosition(const ZEInt64 Position);

};

#endif
