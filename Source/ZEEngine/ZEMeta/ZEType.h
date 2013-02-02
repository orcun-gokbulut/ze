//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEType.h
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
#ifndef __ZE_TYPE_H__
#define __ZE_TYPE_H__

enum ZETypeType
{
	ZE_TT_UNDEFINED					= 0,
	ZE_TT_NULL						= 1,

	ZE_TT_INTEGER_8					= 2,
	ZE_TT_INTEGER_16				= 3,
	ZE_TT_INTEGER_32				= 4,
	ZE_TT_UNSIGNED_INTEGER_8		= 5,
	ZE_TT_UNSIGNED_INTEGER_16		= 6,
	ZE_TT_UNSIGNED_INTEGER_32		= 7,

	ZE_TT_INTEGER_64				= 16,
	ZE_TT_UNSIGNED_INTEGER_64		= 17,

	ZE_TT_FLOAT						= 32,
	ZE_TT_DOUBLE					= 33,

	ZE_TT_BOOLEAN					= 64,
	ZE_TT_ENUMERATOR				= 65,
	
	ZE_TT_STRING					= 128,
	ZE_TT_QUATERNION				= 129,
	ZE_TT_VECTOR2					= 130,
	ZE_TT_VECTOR3					= 131,
	ZE_TT_VECTOR4					= 132,
	ZE_TT_MATRIX3X3					= 133,
	ZE_TT_MATRIX4X4					= 134,

	ZE_TT_CLASS						= 135,
	ZE_TT_OBJECT					= 136,
	ZE_TT_OBJECT_PTR				= 137,
	ZE_TT_ARRAY						= 138,
	ZE_TT_LIST						= 139
};


#define ZE_TYPE_MASK_INTEGER		14	
#define ZE_TYPE_MASK_FLOAT			32
#define ZE_TYPE_MASK_SCALAR_NUMERIC	ZE_TT_BOOLEAN + ZE_TYPE_MASK_FLOAT + ZE_TYPE_MASK_INTEGER

enum ZETypeQualifier
{
	ZE_TQ_VALUE,
	ZE_TQ_REFERENCE,
	ZE_TQ_CONST_REFERENCE
};

class ZEClass;

class ZEType
{
	public:
		ZETypeType			Type;
		ZETypeQualifier		TypeQualifier;

		ZETypeType			SubType;
		ZETypeQualifier		SubTypeQualifier;

		ZEClass*			Class;

		bool				operator==(const ZEType& Other) const;
		bool				operator!=(const ZEType& Other) const;

		static bool			Equal(const ZEType& A, const ZEType& B);

							ZEType();
							ZEType(ZETypeType Type, ZETypeQualifier TypeQualifier = ZE_TQ_VALUE, ZETypeType SubType = ZE_TT_UNDEFINED, ZETypeQualifier SubTypeQualifier = ZE_TQ_VALUE, ZEClass* Class = 0);
							ZEType(ZETypeType Type, ZETypeQualifier TypeQualifier, ZEClass* Class);
};

#endif
