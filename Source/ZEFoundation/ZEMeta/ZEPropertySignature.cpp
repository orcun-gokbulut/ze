//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPropertySignature.cpp
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

#include "ZEPropertySignature.h"

bool ZEPropertySignature::Compare(const ZEPropertySignature& A, const ZEPropertySignature& B)
{
	if (A.CanonicalType != B.CanonicalType)
		return false;

	if (A.Type != B.Type)
		return false;

	if (A.BaseClass != B.BaseClass)
		return false;

	return true;
}

ZEString ZEPropertySignature::GetName()
{
	ZEString TypeString;
	switch(Type)
	{
		default:
		case ZE_VRT_UNDEFINED:
			TypeString = "<UNDEFINED>";
			break;

		case ZE_VRT_NULL:
			TypeString = "void";
			break;

		case ZE_VRT_FLOAT:
			TypeString = "float";
			break;

		case ZE_VRT_DOUBLE:
			TypeString = "double";
			break;

		case ZE_VRT_INTEGER_8:
			TypeString = "int8";
			break;

		case ZE_VRT_INTEGER_16:
			TypeString = "int16";
			break;

		case ZE_VRT_INTEGER_32:
			TypeString = "int32";
			break;

		case ZE_VRT_INTEGER_64:
			TypeString = "int64";
			break;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			TypeString = "uint8";
			break;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			TypeString = "uint16";
			break;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			TypeString = "uint32";
			break;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			TypeString = "uint64";
			break;

		case ZE_VRT_BOOLEAN:
			TypeString = "bool";
			break;

		case ZE_VRT_STRING:
			TypeString = "string";
			break;

		case ZE_VRT_QUATERNION:
			TypeString = "quaternion";
			break;

		case ZE_VRT_VECTOR2:
			TypeString = "vector2";
			break;

		case ZE_VRT_VECTOR3:
			TypeString = "vector3";
			break;

		case ZE_VRT_VECTOR4:
			TypeString = "vector4";
			break;

		case ZE_VRT_MATRIX3X3:
			TypeString = "matrix3x3";
			break;

		case ZE_VRT_MATRIX4X4:
			TypeString = "matrix4x4";
			break;

		case ZE_VRT_CLASS:
			break;
	}

	if (CanonicalType == ZE_CT_CONST_VALUE)
		return "const " + TypeString;
	else if (CanonicalType == ZE_CT_POINTER)
		return TypeString + "*";
	else if (CanonicalType == ZE_CT_CONST_POINTER)
		return "const " + TypeString + "*";
	else if (CanonicalType == ZE_CT_REFERENCE)
		return TypeString + "&";
	else if (CanonicalType == ZE_CT_CONST_REFERENCE)
		return "const " + TypeString + "&";
}

ZEPropertySignature::ZEPropertySignature()
{
	Type = ZE_VRT_UNDEFINED;
	BaseClass = NULL;
	CanonicalType = ZE_CT_VALUE;
}
