//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMethod.h
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
#ifndef __ZE_METHOD_H__
#define __ZE_METHOD_H__

#include "ZETypes.h"
#include "ZEType.h"

class ZEClass;
struct ZEAttribute;

enum ZEMetaOperatorType
{
	ZE_MOT_NONE,
	ZE_MOT_ADDITION,
	ZE_MOT_ADDITION_ASSIGNMENT,
	ZE_MOT_SUBTRACTION,
	ZE_MOT_SUBSTRACTION_ASSIGNMENT,
	ZE_MOT_MULTIPLICATION,
	ZE_MOT_MULTIPLICATION_ASSIGNMENT,
	ZE_MOT_DIVISION,
	ZE_MOT_DIVISION_ASSIGNMENT,
	ZE_MOT_MODULO,
	ZE_MOT_MODULO_ASSIGNMENT,
	ZE_MOT_INCREMENT,
	ZE_MOT_DECREMENT,

	ZE_MOT_LOGICAL_NOT,
	ZE_MOT_LOGICAL_AND,
	ZE_MOT_LOGICAL_OR,

	ZE_MOT_BITWISE_AND,
	ZE_MOT_BITWISE_AND_ASSIGNMENT,
	ZE_MOT_BITWISE_OR,
	ZE_MOT_BITWISE_OR_ASSIGNMENT,
	ZE_MOT_BITWISE_XOR,
	ZE_MOT_BITWISE_XOR_ASSIGNMENT,
	ZE_MOT_LEFT_SHIFT,
	ZE_MOT_LEFT_SHIFT_ASSIGNMENT,
	ZE_MOT_RIGHT_SHIFT,
	ZE_MOT_RIGHT_SHIFT_ASSIGNMENT,

	ZE_MOT_ASSIGNMENT,
	ZE_MOT_EQUAL,
	ZE_MOT_NOT_EQUAL,
	ZE_MOT_LESS,
	ZE_MOT_LESS_EQUAL,
	ZE_MOT_GREATER,
	ZE_MOT_GREATER_AND_EQUAL,

	ZE_MOT_FUNCTION_CALL,
	ZE_MOT_ARRAY_SUBSCRIPT
};

struct ZEMethodParameter
{
	const char*							Name;
	ZEType								Type;
};

struct ZEMethod
{
	ZESize								Id;
	const char*							Name;
	ZEUInt32							Hash;

	ZEClass*							MemberOf;
	void*								MethodPtr;

	bool								IsConst;
	bool								IsEvent;
	bool								IsVirtual;
	bool								IsStatic;
	bool								IsOperator;

	ZEMetaOperatorType					OperatorType;
	ZEType								ReturnType;
	ZEMethodParameter*					Parameters;
	ZESize								ParameterCount;

	ZEAttribute*						Attributes;
	ZESize								AttributeCount;

	const ZEAttribute*					GetAttribute(const char* Name) const;
	const char*							GetAttributeValue(const char* AttributeName, ZESize Index = 0, const char* DefaultValue = NULL) const;

	bool								CheckAttribute(const char* Name) const;
	bool								CheckAttributeHasValue(const char* Name, const char* Value) const;
};

#endif
