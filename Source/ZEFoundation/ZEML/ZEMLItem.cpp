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
	Type = (ZEUInt8)ZEML_IT_UNDEFINED;
	DataSize = 0;
	Parent = NULL;
	FilePosition = 0;
}

ZEMLItem::~ZEMLItem()
{

}

void ZEMLItem::SetType(ZEMLItemType Type)
{
	this->Type = (ZEUInt8)Type;
}

ZEMLItemType ZEMLItem::GetType() const
{
	return (ZEMLItemType)Type;
}

ZEString ZEMLItem::GetTypeText()
{
	switch (GetType())
	{
		case ZEML_IT_UNDEFINED:
			return "ZEML_IT_UNDEFINED";
			break;
		case ZEML_IT_FLOAT:
			return "ZEML_IT_FLOAT";
			break;
		case ZEML_IT_DOUBLE:
			return "ZEML_IT_DOUBLE";
			break;
		case ZEML_IT_INT8:
			return "ZEML_IT_INT8";
			break;
		case ZEML_IT_INT16:
			return "ZEML_IT_INT16";
			break;
		case ZEML_IT_INT32:
			return "ZEML_IT_INT32";
			break;
		case ZEML_IT_INT64:
			return "ZEML_IT_INT64";
			break;
		case ZEML_IT_UINT8:
			return "ZEML_IT_UINT8";
			break;
		case ZEML_IT_UINT16:
			return "ZEML_IT_UINT16";
			break;
		case ZEML_IT_UINT32:
			return "ZEML_IT_UINT32";
			break;
		case ZEML_IT_UINT64:
			return "ZEML_IT_UINT64";
			break;
		case ZEML_IT_BOOLEAN:
			return "ZEML_IT_BOOLEAN";
		case ZEML_IT_STRING:
			return "ZEML_IT_STRING";
			break;
		case ZEML_IT_QUATERNION:
			return "ZEML_IT_QUATERNION";
			break;
		case ZEML_IT_VECTOR2:
			return "ZEML_IT_VECTOR2";
			break;
		case ZEML_IT_VECTOR3:
			return "ZEML_IT_VECTOR3";
			break;
		case ZEML_IT_VECTOR4:
			return "ZEML_IT_VECTOR4";
			break;
		case ZEML_IT_MATRIX3X3:
			return "ZEML_IT_MATRIX3X3";
			break;
		case ZEML_IT_MATRIX4X4:
			return "ZEML_IT_MATRIX4X4";
			break;
		case ZEML_IT_INLINE_DATA:
			return "ZEML_IT_INLINE_DATA";
			break;
		case ZEML_IT_OFFSET_DATA:
			return "ZEML_IT_OFFSET_DATA";
			break;
		case ZEML_IT_NODE:
			return "ZEML_IT_NODE";
			break;
	}	
}

ZEMLItemType ZEMLItem::GetTypeFromText(ZEString TypeText)
{
	if(TypeText == "ZEML_IT_FLOAT")
		return ZEML_IT_FLOAT;
	else if(TypeText == "ZEML_IT_DOUBLE")
		return ZEML_IT_DOUBLE;
	else if(TypeText == "ZEML_IT_INT8")
		return ZEML_IT_INT8;
	else if(TypeText == "ZEML_IT_INT16")
		return ZEML_IT_INT16;
	else if(TypeText == "ZEML_IT_INT32")
		return ZEML_IT_INT32;
	else if(TypeText == "ZEML_IT_INT64")
		return ZEML_IT_INT64;
	else if(TypeText == "ZEML_IT_UINT8")
		return ZEML_IT_UINT8;
	else if(TypeText == "ZEML_IT_UINT16")
		return ZEML_IT_UINT16;
	else if(TypeText == "ZEML_IT_UINT32")
		return ZEML_IT_UINT32;
	else if(TypeText == "ZEML_IT_UINT64")
		return ZEML_IT_UINT64;
	else if(TypeText == "ZEML_IT_BOOLEAN")
		return ZEML_IT_BOOLEAN;
	else if(TypeText == "ZEML_IT_STRING")
		return ZEML_IT_STRING;
	else if(TypeText == "ZEML_IT_QUATERNION")
		return ZEML_IT_QUATERNION;
	else if(TypeText == "ZEML_IT_VECTOR2")
		return ZEML_IT_VECTOR2;
	else if(TypeText == "ZEML_IT_VECTOR3")
		return ZEML_IT_VECTOR3;
	else if(TypeText == "ZEML_IT_VECTOR4")
		return ZEML_IT_VECTOR4;
	else if(TypeText == "ZEML_IT_MATRIX3X3")
		return ZEML_IT_MATRIX3X3;
	else if(TypeText == "ZEML_IT_MATRIX4X4")
		return ZEML_IT_MATRIX4X4;
	else if(TypeText == "ZEML_IT_INLINE_DATA")
		return ZEML_IT_INLINE_DATA;
	else if(TypeText == "ZEML_IT_OFFSET_DATA")
		return ZEML_IT_OFFSET_DATA;
	else if(TypeText == "ZEML_IT_NODE")
		return ZEML_IT_NODE;
	else
		return ZEML_IT_UNDEFINED;
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
