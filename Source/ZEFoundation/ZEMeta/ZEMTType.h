//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTType.h
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

enum ZEMTTypeType
{
	ZEMT_TT_UNDEFINED				= 0,
	ZEMT_TT_VOID					= 1,
	ZEMT_TT_INTEGER_8				= 2,
	ZEMT_TT_INTEGER_16				= 3,
	ZEMT_TT_INTEGER_32				= 4,
	ZEMT_TT_INTEGER_64				= 5,
	ZEMT_TT_UNSIGNED_INTEGER_8		= 6,
	ZEMT_TT_UNSIGNED_INTEGER_16		= 7,
	ZEMT_TT_UNSIGNED_INTEGER_32		= 8,
	ZEMT_TT_UNSIGNED_INTEGER_64		= 9,
	ZEMT_TT_FLOAT					= 10,
	ZEMT_TT_DOUBLE					= 11,
	ZEMT_TT_BOOLEAN					= 12,
	ZEMT_TT_STRING					= 13,
	ZEMT_TT_QUATERNION				= 14,
	ZEMT_TT_VECTOR2					= 15,
	ZEMT_TT_VECTOR2D				= 16,
	ZEMT_TT_VECTOR3					= 17,
	ZEMT_TT_VECTOR3D				= 18,
	ZEMT_TT_VECTOR4					= 19,
	ZEMT_TT_VECTOR4D				= 20,
	ZEMT_TT_MATRIX3X3				= 21,
	ZEMT_TT_MATRIX3X3D				= 22,
	ZEMT_TT_MATRIX4X4				= 23,
	ZEMT_TT_MATRIX4X4D				= 24,
	ZEMT_TT_OBJECT					= 25,
	ZEMT_TT_OBJECT_PTR				= 26,
	ZEMT_TT_CLASS					= 27,
	ZEMT_TT_ENUMERATOR				= 28,
	ZEMT_TT_FLAGS					= 29
};

enum ZEMTTypeQualifier
{
	ZEMT_TQ_VALUE,
	ZEMT_TQ_CONST_VALUE,
	ZEMT_TQ_REFERENCE,
	ZEMT_TQ_CONST_REFERENCE
};

enum ZEMTContainerType
{
	ZEMT_CT_NONE,
	ZEMT_CT_ARRAY,
	ZEMT_CT_LIST,
	ZEMT_CT_CONTAINER
};

class ZEClass;
class ZEMTEnumerator;

class ZEMTType
{
	public:
		ZEMTTypeType			Type;
		ZEMTTypeQualifier		TypeQualifier;
		ZEMTContainerType		ContainerType;

		ZEClass*				Class;
		ZEMTEnumerator*			Enumerator;

		bool					operator==(const ZEMTType& Other) const;
		bool					operator!=(const ZEMTType& Other) const;

		static bool				Equal(const ZEMTType& A, const ZEMTType& B);

								ZEMTType();
								ZEMTType(ZEMTTypeType Type, ZEMTTypeQualifier TypeQualifier, 
									ZEMTContainerType ContainerType, 
									ZEClass* Class, ZEMTEnumerator* Enumerator);
};
