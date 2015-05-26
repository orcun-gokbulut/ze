//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMLCommon.cpp
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

#include "ZEMLCommon.h"

ZEMLValueType ZEMLUtils::ConvertValueType(ZEValueType Type)
{
	switch (Type)
	{
		default:
		case ZE_VRT_UNDEFINED:
		case ZE_VRT_NULL:
		case ZE_VRT_CLASS:
			return ZEML_VT_UNDEFINED;

		case ZE_VRT_FLOAT:
			return ZEML_VT_FLOAT;

		case ZE_VRT_DOUBLE:
			return ZEML_VT_DOUBLE;

		case ZE_VRT_INTEGER_8:
			return ZEML_VT_INT8;

		case ZE_VRT_INTEGER_16:
			return ZEML_VT_INT16;

		case ZE_VRT_INTEGER_32:
			return ZEML_VT_INT32;

		case ZE_VRT_INTEGER_64:
			return ZEML_VT_INT64;

		case ZE_VRT_UNSIGNED_INTEGER_8:
			return ZEML_VT_UINT8;

		case ZE_VRT_UNSIGNED_INTEGER_16:
			return ZEML_VT_UINT16;

		case ZE_VRT_UNSIGNED_INTEGER_32:
			return ZEML_VT_UINT32;

		case ZE_VRT_UNSIGNED_INTEGER_64:
			return ZEML_VT_UINT64;

		case ZE_VRT_BOOLEAN:
			return ZEML_VT_BOOLEAN;

		case ZE_VRT_STRING:
			return ZEML_VT_STRING;

		case ZE_VRT_QUATERNION:
			return ZEML_VT_QUATERNION;

		case ZE_VRT_VECTOR2:
			return ZEML_VT_VECTOR2;

		case ZE_VRT_VECTOR3:
			return ZEML_VT_VECTOR3;

		case ZE_VRT_VECTOR4:
			return ZEML_VT_VECTOR4;

		case ZE_VRT_MATRIX3X3:
			return ZEML_VT_MATRIX3X3;

		case ZE_VRT_MATRIX4X4:
			return ZEML_VT_MATRIX4X4;
	}
}
