//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLItem.cpp
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

#include "ZEMLItem.h"
#include "ZEError.h"

ZEMLItem::ZEMLItem()
{
	Type = (ZEUInt8)ZEML_ET_UNDEFINED;
	DataSize = 0;
	Parent = NULL;
	FilePosition = 0;
}

ZEMLItem::~ZEMLItem()
{

}

void ZEMLItem::SetType(ZEMLElementType Type)
{
	this->Type = (ZEUInt8)Type;
}

ZEMLElementType ZEMLItem::GetType() const
{
	return (ZEMLElementType)Type;
}

ZEString ZEMLItem::GetTypeText()
{
	switch (GetType())
	{
		case ZEML_ET_UNDEFINED:
			return "ZEML_ET_UNDEFINED";
			break;
		case ZEML_ET_FLOAT:
			return "ZEML_ET_FLOAT";
			break;
		case ZEML_ET_DOUBLE:
			return "ZEML_ET_DOUBLE";
			break;
		case ZEML_ET_INT8:
			return "ZEML_ET_INT8";
			break;
		case ZEML_ET_INT16:
			return "ZEML_ET_INT16";
			break;
		case ZEML_ET_INT32:
			return "ZEML_ET_INT32";
			break;
		case ZEML_ET_INT64:
			return "ZEML_ET_INT64";
			break;
		case ZEML_ET_UINT8:
			return "ZEML_ET_UINT8";
			break;
		case ZEML_ET_UINT16:
			return "ZEML_ET_UINT16";
			break;
		case ZEML_ET_UINT32:
			return "ZEML_ET_UINT32";
			break;
		case ZEML_ET_UINT64:
			return "ZEML_ET_UINT64";
			break;
		case ZEML_ET_BOOLEAN:
			return "ZEML_ET_BOOLEAN";
		case ZEML_ET_STRING:
			return "ZEML_ET_STRING";
			break;
		case ZEML_ET_QUATERNION:
			return "ZEML_ET_QUATERNION";
			break;
		case ZEML_ET_VECTOR2:
			return "ZEML_ET_VECTOR2";
			break;
		case ZEML_ET_VECTOR3:
			return "ZEML_ET_VECTOR3";
			break;
		case ZEML_ET_VECTOR4:
			return "ZEML_ET_VECTOR4";
			break;
		case ZEML_ET_MATRIX3X3:
			return "ZEML_ET_MATRIX3X3";
			break;
		case ZEML_ET_MATRIX4X4:
			return "ZEML_ET_MATRIX4X4";
			break;
		case ZEML_ET_INLINE_DATA:
			return "ZEML_ET_INLINE_DATA";
			break;
		case ZEML_ET_OFFSET_DATA:
			return "ZEML_ET_OFFSET_DATA";
			break;
		case ZEML_ET_NODE:
			return "ZEML_ET_NODE";
			break;
	}	
}

ZEMLElementType ZEMLItem::GetTypeFromText(ZEString TypeText)
{
	if(TypeText == "ZEML_ET_FLOAT")
		return ZEML_ET_FLOAT;
	else if(TypeText == "ZEML_ET_DOUBLE")
		return ZEML_ET_DOUBLE;
	else if(TypeText == "ZEML_ET_INT8")
		return ZEML_ET_INT8;
	else if(TypeText == "ZEML_ET_INT16")
		return ZEML_ET_INT16;
	else if(TypeText == "ZEML_ET_INT32")
		return ZEML_ET_INT32;
	else if(TypeText == "ZEML_ET_INT64")
		return ZEML_ET_INT64;
	else if(TypeText == "ZEML_ET_UINT8")
		return ZEML_ET_UINT8;
	else if(TypeText == "ZEML_ET_UINT16")
		return ZEML_ET_UINT16;
	else if(TypeText == "ZEML_ET_UINT32")
		return ZEML_ET_UINT32;
	else if(TypeText == "ZEML_ET_UINT64")
		return ZEML_ET_UINT64;
	else if(TypeText == "ZEML_ET_BOOLEAN")
		return ZEML_ET_BOOLEAN;
	else if(TypeText == "ZEML_ET_STRING")
		return ZEML_ET_STRING;
	else if(TypeText == "ZEML_ET_QUATERNION")
		return ZEML_ET_QUATERNION;
	else if(TypeText == "ZEML_ET_VECTOR2")
		return ZEML_ET_VECTOR2;
	else if(TypeText == "ZEML_ET_VECTOR3")
		return ZEML_ET_VECTOR3;
	else if(TypeText == "ZEML_ET_VECTOR4")
		return ZEML_ET_VECTOR4;
	else if(TypeText == "ZEML_ET_MATRIX3X3")
		return ZEML_ET_MATRIX3X3;
	else if(TypeText == "ZEML_ET_MATRIX4X4")
		return ZEML_ET_MATRIX4X4;
	else if(TypeText == "ZEML_ET_INLINE_DATA")
		return ZEML_ET_INLINE_DATA;
	else if(TypeText == "ZEML_ET_OFFSET_DATA")
		return ZEML_ET_OFFSET_DATA;
	else if(TypeText == "ZEML_ET_NODE")
		return ZEML_ET_NODE;
	else
		return ZEML_ET_UNDEFINED;
}

ZEUInt64 ZEMLItem::GetFilePosition()
{
	return FilePosition;
}

void ZEMLItem::SetName(const ZEString& Name)
{
	if(Name.GetSize() > 255)
		zeError("ZEMLProperty name too long. Name size must be smaller than 256");

	this->Name = Name;
}

const ZEString& ZEMLItem::GetName() const
{
	return Name;
}

ZEUInt64 ZEMLItem::GetDataSize()
{
	return DataSize;
}
