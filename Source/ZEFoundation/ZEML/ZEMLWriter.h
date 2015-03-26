//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLWriter.h
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
#ifndef	__ZEML_WRITER_H__
#define __ZEML_WRITER_H__

#include "ZECommon.h"
#include "ZETypes.h"
#include "ZEMLCommon.h"
#include "ZEDS/ZEValue.h"
#include "ZEFile/ZEFile.h"

class ZEMLWriterNode
{
	friend class ZEMLWriter;
	private:
		ZEMLWriterNode*		Parent;
		ZEFile*				File;
		ZEString			Name;
		ZEUInt64			NodeDataOffset;
		ZEUInt64			SubElementCount;
		bool				SubNodeIsOpen;

		bool				WriteElementHeader(const char* Name, ZEMLElementType Type);

	public:
		ZEMLWriterNode		OpenSubNode(const char* Name);

		bool				WriteValue(const char* Name, const ZEValue& Value);
		bool				WriteFloat(const char* Name, float Value);
		bool				WriteDouble(const char* Name, double Value);
		bool				WriteInt8(const char* Name, ZEInt8 Value);
		bool				WriteInt16(const char* Name, ZEInt16 Value);
		bool				WriteInt32(const char* Name, ZEInt32 Value);
		bool				WriteInt64(const char* Name, ZEInt64 Value);
		bool				WriteUInt8(const char* Name, ZEUInt8 Value);
		bool				WriteUInt16(const char* Name, ZEUInt16 Value);
		bool				WriteUInt32(const char* Name, ZEUInt32 Value);
		bool				WriteUInt64(const char* Name, ZEUInt64 Value);
		bool				WriteBool(const char* Name, bool Value);
		bool				WriteString(const char* Name, const char* Value);
		bool				WriteQuaternion(const char* Name, const ZEQuaternion& Value);
		bool				WriteVector2(const char* Name, const ZEVector2& Value);
		bool				WriteVector3(const char* Name, const ZEVector3& Value);
		bool				WriteVector4(const char* Name, const ZEVector4& Value);
		bool				WriteMatrix3x3(const char* Name, const ZEMatrix3x3& Value);
		bool				WriteMatrix4x4(const char* Name, const ZEMatrix4x4& Value);
		bool				WriteData(const char* Name, const void* Data, ZEUInt64 DataSize);

		void				CloseNode();

							ZEMLWriterNode();
							~ZEMLWriterNode();
};

class ZEMLWriter
{
	private:
		ZEFile				OwnedFile;
		ZEFile*				File;

		void				WriteHeader();

	public:
		ZEMLWriterNode		WriteRootNode(const char* Name);

		bool				Open(const char* FileName);
		bool				Open(ZEFile* File);
		void				Close();

							ZEMLWriter();
							~ZEMLWriter();
};


#endif
