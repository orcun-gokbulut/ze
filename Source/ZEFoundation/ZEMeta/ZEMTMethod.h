//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTMethod.h
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

#include "ZETypes.h"
#include "ZEMTType.h"
#include "ZEDS/ZEFlags.h"

class ZEClass;
struct ZEMTAttribute;

enum ZEMTOperatorType
{
	ZEMT_OT_NONE,
	ZEMT_OT_ADDITION,
	ZEMT_OT_ADDITION_ASSIGNMENT,
	ZEMT_OT_SUBTRACTION,
	ZEMT_OT_SUBSTRACTION_ASSIGNMENT,
	ZEMT_OT_MULTIPLICATION,
	ZEMT_OT_MULTIPLICATION_ASSIGNMENT,
	ZEMT_OT_DIVISION,
	ZEMT_OT_DIVISION_ASSIGNMENT,
	ZEMT_OT_MODULO,
	ZEMT_OT_MODULO_ASSIGNMENT,
	ZEMT_OT_INCREMENT,
	ZEMT_OT_DECREMENT,
	ZEMT_OT_LOGICAL_NOT,
	ZEMT_OT_LOGICAL_AND,
	ZEMT_OT_LOGICAL_OR,
	ZEMT_OT_BITWISE_AND,
	ZEMT_OT_BITWISE_AND_ASSIGNMENT,
	ZEMT_OT_BITWISE_OR,
	ZEMT_OT_BITWISE_OR_ASSIGNMENT,
	ZEMT_OT_BITWISE_XOR,
	ZEMT_OT_BITWISE_XOR_ASSIGNMENT,
	ZEMT_OT_LEFT_SHIFT,
	ZEMT_OT_LEFT_SHIFT_ASSIGNMENT,
	ZEMT_OT_RIGHT_SHIFT,
	ZEMT_OT_RIGHT_SHIFT_ASSIGNMENT,
	ZEMT_OT_ASSIGNMENT,
	ZEMT_OT_EQUAL,
	ZEMT_OT_NOT_EQUAL,
	ZEMT_OT_LESS,
	ZEMT_OT_LESS_EQUAL,
	ZEMT_OT_GREATER,
	ZEMT_OT_GREATER_AND_EQUAL,
	ZEMT_OT_FUNCTION_CALL,
	ZEMT_OT_ARRAY_SUBSCRIPT
};

enum ZEMTMethodFlag
{
	ZEMT_MF_NONE			= 0x00,
	ZEMT_MF_EVENT			= 0x01,
	ZEMT_MF_CONST			= 0x02,
	ZEMT_MF_VIRTUAL			= 0x04,
	ZEMT_MF_STATIC			= 0x08,
	ZEMT_MF_OPERATOR		= 0x10,
	ZEMT_MF_CONSTRUCTOR		= 0x20,
	ZEMT_MF_DECONSTRUCTOR	= 0x40
};
typedef ZEFlags ZEMTMethodFlags;

struct ZEMTMethodParameter
{
	const char*							Name;
	ZEMTType							Type;
};

struct ZEMTMethod
{
	ZESize								Id;
	const char*							Name;
	ZEUInt32							Hash;

	ZEClass*							MemberOf;
	void*								MethodPtr;

	ZEMTMethodFlags						Flags;
	ZEMTOperatorType					OperatorType;

	ZEMTType							ReturnType;
	ZEMTMethodParameter*				Parameters;
	ZESize								ParameterCount;

	ZEMTAttribute*						Attributes;
	ZESize								AttributeCount;

	bool								IsEvent() const;
	bool								IsConst() const;
	bool								IsStatic() const;
	bool								IsOperator() const;
	bool								IsConstructor() const;
	bool								IsDeconstructor() const;

	const ZEMTAttribute*				GetAttribute(const char* Name) const;
	const char*							GetAttributeValue(const char* AttributeName, ZESize Index = 0, const char* DefaultValue = NULL) const;

	bool								CheckAttribute(const char* Name) const;
	bool								CheckAttributeHasValue(const char* Name, const char* Value) const;
};
