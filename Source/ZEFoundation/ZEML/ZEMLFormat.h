//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLFormat.h
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

#include "ZEMLCommon.h"
#include "ZEDS\ZEFlags.h"
#include "ZEDS\ZEValue.h"

class ZEFile;
class ZEMLFormat;

typedef ZEUInt ZEMFormatSupport;
#define ZEML_FS_NONE		0
#define ZEML_FS_READ		1
#define ZEML_FS_WRITE		2

enum ZEMLFormatType
{
	ZEML_FT_BINARY,
	ZEML_FT_XML,
	ZEML_FT_XML_SIMPLIFIED
};

struct ZEMLFormatElement
{
	ZEMLElementType			ElementType;
	ZEString				Name;
	ZEUInt32				NameHash;
	ZEUInt64				Offset;
	ZEUInt64				Count;
	ZESize					Size;
	ZEMLValueType			ValueType;
	ZEValue					Value;

							ZEMLFormatElement();
};

struct ZEMLFormatDescription
{
	public:
		virtual const char*				GetName() const = 0;
		virtual ZEUInt					GetMinorVersion() const = 0;
		virtual ZEUInt					GetMajorVersion() const = 0;
		virtual ZEMLFormatType			GetType() const = 0;
		virtual ZEMFormatSupport		GetSupport() const = 0;
		virtual bool					Determine(ZEFile* File) = 0;
		virtual ZEMLFormat*				CreateInstance() = 0;
};

class ZEMLFormat
{
	public:
		virtual ZEMLFormatDescription*	GetDescription() const = 0;

		virtual bool					ReadHeader(ZEFile* File) = 0;
		virtual bool					ReadGoToNode(ZEFile* File, const ZEMLFormatElement& Node) = 0;
		virtual bool					ReadElement(ZEFile* File, ZEMLFormatElement& Element) = 0;
		virtual bool					ReadData(ZEFile* File, const ZEMLFormatElement& Element, void* Buffer, ZESize Offset, ZESize Size) = 0;

		virtual bool					WriteHeader(ZEFile* File) = 0;
		virtual bool					WriteHeaderClose(ZEFile* File) = 0;
		virtual bool					WriteElement(ZEFile* File, ZEMLFormatElement& Element) = 0;
		virtual bool					WriteElementClose(ZEFile* File, ZEMLFormatElement& Element) = 0;


		static ZESize					GetFormatCount();
		static ZEMLFormatDescription*const* GetFormats();
		static ZEMLFormatDescription*	GetDefaultFormat();
		static ZEMLFormatDescription*	GetDefaultBinaryFormat();
		static ZEMLFormatDescription*	GetDefaultTextFormat();
};
