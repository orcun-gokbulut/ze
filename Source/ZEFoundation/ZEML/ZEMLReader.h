//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLReader.h
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
#ifndef	__ZEML_READER_H__
#define __ZEML_READER_H__

#include "ZEMLElement.h"
#include "ZEMLFormat.h"

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEValue.h"
#include "ZEFile/ZEFile.h"
#include "ZEPointer/ZEPointer.h"


struct ZEMLReaderProperty
{
	ZEMLElementType	ElementType;
	ZEString		Name;
	ZEMLValueType	ValueType;
	ZEValue			Value;
	ZEUInt64		DataSize;
	ZEUInt64		DataOffset;

					ZEMLReaderProperty();
};

struct ZEMLReaderSubNode
{
	ZEString		Name;
	ZEUInt64		Offset;
	ZEUInt64		Size;
	
					ZEMLReaderSubNode();
};

class ZEMLReaderNode
{
	friend class ZEMLReader;
	private:
		ZEFile*						File;
		ZEMLFormat*					Format;
		ZEMLFormatElement			Node;

		ZEArray<ZEMLFormatElement>	Elements;
		ZESize						NodeCount;

		const ZEMLFormatElement*	FindElement(const char* Name, ZEMLElementType Type, ZESize Index = 0) const;
		bool						Load();
		 
	public:
		const ZEFile*						GetFile() const;
		const ZEString&						GetName() const;
		const ZEArray<ZEMLFormatElement>&	GetElements() const;

		const ZESmartArray<ZEMLReaderSubNode>& GetSubNodes();
		const ZESmartArray<ZEMLReaderProperty>& GetProperties();	
		ZESize						GetNodeCount() const;
		ZESize						GetNodeCount(const char* Name) const;

		ZEMLReaderNode				GetNode(const char* Name, ZESize Index = 0) const;
		ZEMLReaderNode				GetNode(ZESize Index) const;

		bool						IsValid() const;
		bool						IsPropertyExists(const char* Name) const;
		bool						IsSubNodeExists(const char* Name) const;

		ZEValue						ReadValue(const char* Name) const;
		ZEInt8						ReadInt8(const char* Name, ZEInt8 Default = 0) const;
		ZEInt16						ReadInt16(const char* Name, ZEInt16 Default = 0) const;
		ZEInt32						ReadInt32(const char* Name, ZEInt32 Default = 0) const;
		ZEInt64						ReadInt64(const char* Name, ZEInt64 Default = 0) const;
		ZEUInt8						ReadUInt8(const char* Name, ZEUInt8 Default = 0) const;
		ZEUInt16					ReadUInt16(const char* Name, ZEUInt16 Default = 0) const;
		ZEUInt32					ReadUInt32(const char* Name, ZEUInt32 Default = 0) const;
		ZEUInt64					ReadUInt64(const char* Name, ZEUInt64 Default = 0) const;
		float						ReadFloat(const char* Name, float Default = 0.0f) const;
		double						ReadDouble(const char* Name, double Default = 0.0) const;
		bool						ReadBoolean(const char* Name, bool Default = false) const;
		ZEVector2					ReadVector2(const char* Name, const ZEVector2& Default = ZEVector2::Zero) const;
		ZEVector2d					ReadVector2d(const char* Name, const ZEVector2d& Default = ZEVector2d::Zero) const;
		ZEVector3					ReadVector3(const char* Name, const ZEVector3& Default = ZEVector3::Zero) const;
		ZEVector3d					ReadVector3d(const char* Name, const ZEVector3d& Default = ZEVector3d::Zero) const;
		ZEVector4					ReadVector4(const char* Name, const ZEVector4& Default = ZEVector4::Zero) const;
		ZEVector4d					ReadVector4d(const char* Name, const ZEVector4d& Default = ZEVector4d::Zero) const;
		ZEQuaternion				ReadQuaternion(const char* Name, const ZEQuaternion& Default = ZEQuaternion::Zero) const;
		ZEMatrix3x3					ReadMatrix3x3(const char* Name, const ZEMatrix3x3& Default = ZEMatrix3x3::Zero) const;
		ZEMatrix3x3d				ReadMatrix3x3d(const char* Name, const ZEMatrix3x3d& Default = ZEMatrix3x3d::Zero) const;
		ZEMatrix4x4					ReadMatrix4x4(const char* Name, const ZEMatrix4x4& Default = ZEMatrix4x4::Zero) const;
		ZEMatrix4x4d				ReadMatrix4x4d(const char* Name, const ZEMatrix4x4d& Default = ZEMatrix4x4d::Zero) const;
		ZEString					ReadString(const char* Name, const ZEString& Default = "") const;

		ZESize						ReadDataSize(const char* Name) const;
		bool						ReadData(const char* Name, void* Buffer, ZESize BufferSize, ZESize Offset = 0) const;
		bool						ReadDataItems(const char* Name, void* Buffer, ZESize ItemSize, ZESize ItemCount, ZESize Offset = 0) const;

									ZEMLReaderNode();
};

class ZEMLReader
{
	private:
		ZEFile						OwnedFile;
		ZEFile*						File;
		ZEPointer<ZEMLFormat>		Format;
		ZEMLReaderNode				RootNode;

		bool						Load();

	public:
		ZEMLReaderNode				GetRootNode();

		ZEMLFormat*					GetFormat();

		bool						Open(const char* FileName);
		bool						Open(ZEFile* File);
		void						Close();

									ZEMLReader();
									~ZEMLReader();
};

#endif
