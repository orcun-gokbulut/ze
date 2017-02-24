//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCContext.h
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
#ifndef __ZE_MC_CONTEXT_H__
#define __ZE_MC_CONTEXT_H__

#include "ZETypes.h"
#include "ZEDS/ZEList.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

class ZEMCClass;

enum ZEMCBaseType
{
	ZEMC_BT_UNDEFINED,
	ZEMC_BT_VOID,
	ZEMC_BT_INTEGER_8,
	ZEMC_BT_INTEGER_16,
	ZEMC_BT_INTEGER_32,
	ZEMC_BT_UNSIGNED_INTEGER_8,
	ZEMC_BT_UNSIGNED_INTEGER_16,
	ZEMC_BT_UNSIGNED_INTEGER_32,
	ZEMC_BT_INTEGER_64,
	ZEMC_BT_UNSIGNED_INTEGER_64,
	ZEMC_BT_FLOAT,
	ZEMC_BT_DOUBLE,
	ZEMC_BT_BOOLEAN,
	ZEMC_BT_STRING,
	ZEMC_BT_QUATERNION,
	ZEMC_BT_VECTOR2,
	ZEMC_BT_VECTOR2D,
	ZEMC_BT_VECTOR3,
	ZEMC_BT_VECTOR3D,
	ZEMC_BT_VECTOR4,
	ZEMC_BT_VECTOR4D,
	ZEMC_BT_MATRIX3X3,
	ZEMC_BT_MATRIX3X3D,
	ZEMC_BT_MATRIX4X4,
	ZEMC_BT_MATRIX4X4D,
	ZEMC_BT_CLASS,
	ZEMC_BT_OBJECT,
	ZEMC_BT_OBJECT_PTR,
	ZEMC_BT_OBJECT_HOLDER,
	ZEMC_BT_ENUMERATOR,
	ZEMC_BT_FLAGS,
};

enum ZEMCContainerType
{
	ZEMC_CT_NONE,
	ZEMC_CT_ARRAY,
	ZEMC_CT_LIST,
	ZEMC_CT_COLLECTION
};

enum ZEMCOperatorType
{
	ZEMC_OT_NONE,
	ZEMC_OT_ADDITION,
	ZEMC_OT_ADDITION_ASSIGNMENT,
	ZEMC_OT_SUBTRACTION,
	ZEMC_OT_SUBSTRACTION_ASSIGNMENT,
	ZEMC_OT_MULTIPLICATION,
	ZEMC_OT_MULTIPLICATION_ASSIGNMENT,
	ZEMC_OT_DIVISION,
	ZEMC_OT_DIVISION_ASSIGNMENT,
	ZEMC_OT_MODULO,
	ZEMC_OT_MODULO_ASSIGNMENT,
	ZEMC_OT_INCREMENT,
	ZEMC_OT_DECREMENT,
	ZEMC_OT_LOGICAL_NOT,
	ZEMC_OT_LOGICAL_AND,
	ZEMC_OT_LOGICAL_OR,
	ZEMC_OT_BITWISE_AND,
	ZEMC_OT_BITWISE_AND_ASSIGNMENT,
	ZEMC_OT_BITWISE_OR,
	ZEMC_OT_BITWISE_OR_ASSIGNMENT,
	ZEMC_OT_BITWISE_XOR,
	ZEMC_OT_BITWISE_XOR_ASSIGNMENT,
	ZEMC_OT_LEFT_SHIFT,
	ZEMC_OT_LEFT_SHIFT_ASSIGNMENT,
	ZEMC_OT_RIGHT_SHIFT,
	ZEMC_OT_RIGHT_SHIFT_ASSIGNMENT,
	ZEMC_OT_ASSIGNMENT,
	ZEMC_OT_EQUAL,
	ZEMC_OT_NOT_EQUAL,
	ZEMC_OT_LESS,
	ZEMC_OT_LESS_EQUAL,
	ZEMC_OT_GREATER,
	ZEMC_OT_GREATER_AND_EQUAL,
	ZEMC_OT_FUNCTION_CALL,
	ZEMC_OT_ARRAY_SUBSCRIPT
};

enum ZEMCPropertyAccess
{
	ZEMC_PA_NONE		= 0,
	ZEMC_PA_READ		= 1,
	ZEMC_PA_WRITE		= 2,
	ZEMC_PA_READ_WRITE	= 3
};

enum ZEMCDeclarationType
{
	ZEMC_DT_NONE		= 0,
	ZEMC_DT_CLASS		= 1,
	ZEMC_DT_ENUMERATOR	= 2,
	ZEMC_DT_FLAGS		= 3
};

enum ZEMCTypeQualifier
{
	ZEMC_TQ_VALUE,
	ZEMC_TQ_CONST_VALUE,
	ZEMC_TQ_REFERENCE,
	ZEMC_TQ_CONST_REFERENCE,
};

class ZEMCDeclaration;
class ZEMCEnumerator;
class ZEMCMethod;

class ZEMCType
{
	public:
		ZEMCBaseType					BaseType;
		ZEMCTypeQualifier				TypeQualifier;
		ZEMCContainerType				CollectionType;
		ZEMCTypeQualifier				CollectionQualifier;
		
		ZEMCClass*						Class;
		ZEMCEnumerator*					Enumerator;

		bool							operator==(const ZEMCType& Other) const;
		bool							operator!=(const ZEMCType& Other) const;

		static bool						Equal(const ZEMCType& A, const ZEMCType& B);

		bool							Check();

										ZEMCType();
										~ZEMCType();
};

class ZEMCAttribute
{
	public:
		ZEMCDeclaration*				Owner;
		ZEString						Name;
		ZEArray<ZEString>				Values;

										ZEMCAttribute();
										~ZEMCAttribute();
};

class ZEMCForwardDeclaration
{
	public:
		ZEMCDeclarationType				Type;							
		ZEString						Name;
		
										ZEMCForwardDeclaration();
										~ZEMCForwardDeclaration();
};

class ZEMCInclude
{
	public:
		ZEString						HeaderFileName;
};

class ZEMCDeclaration
{
	public:
		ZEString						Name;
		ZEString						MetaName;
		ZEUInt32						Hash;

		ZEArray<ZEMCAttribute>			AttributeStack;
		ZEArray<ZEMCAttribute>			Attributes;

		void							NormalizeAttributeStack();

		ZEMCAttribute*					GetAttribute(const char* Name);
		const char*						GetAttributeValue(const char* Name, ZESize Index = 0, const char* DefaultValue = NULL);

		bool							CheckAttribute(const char* Name);
		bool							CheckAttributeValue(const char* Name, const char* TestValue, ZESize Index = 0, const char* DefaultValue = NULL);
		bool							CheckAttributeHasValue(const char* Name, const char* Value);

										ZEMCDeclaration();
		virtual							~ZEMCDeclaration();
};

class ZEMCEnumeratorItem
{
	public:
		ZEString							Name;
		ZEUInt32							Value;

											ZEMCEnumeratorItem();
											~ZEMCEnumeratorItem();
};

class ZEMCEnumerator : public ZEMCDeclaration
{
	public:
		ZEArray<ZEMCEnumeratorItem>			Items;

											ZEMCEnumerator();
											~ZEMCEnumerator();
};

class ZEMCProperty : public ZEMCDeclaration
{
	public:
		ZEMCClass*							Class;

		ZESize								ID;
		ZEMCType							Type;

		bool								HasAccessors;
		ZEMCMethod*							Getter;
		ZEMCMethod*							Setter;
		ZEMCMethod*							Adder;
		ZEMCMethod*							Remover;
		ZEMCMethod*							Counter;

		bool								IsStatic;
		bool								IsContainer;

		ZEMCPropertyAccess					Access;

											ZEMCProperty();
											~ZEMCProperty();
};

class ZEMCMethodParameter
{
	public:
		ZEString							Name;
		ZEMCType							Type;

											ZEMCMethodParameter();
											~ZEMCMethodParameter();
};

class ZEMCMethod : public ZEMCDeclaration
{
	public:
		ZEMCClass*							Class;

		ZESize								ID;

		bool								IsVirtual;
		bool								IsPure;
		bool								IsStatic;
		bool								IsEvent;
		bool								IsConst;
		bool								IsConstructor;
		bool								IsOperator;

		ZEMCOperatorType				OperatorType;

		ZEMCType							ReturnValue;
		ZEArray<ZEMCMethodParameter>		Parameters;

											ZEMCMethod();
		virtual								~ZEMCMethod();
};

class ZEMCClass : public ZEMCDeclaration
{
	public:
		ZEMCClass*							BaseClass;
		ZEArray<ZEMCProperty*>				Properties;
		ZEArray<ZEMCMethod*>				Methods;

		bool								IsForwardDeclared;
		bool								HasScriptBase;

		bool								IsAbstract;
		bool								IsFundamental;

		bool								HasCreateInstanceMethod;
		bool								HasPublicCopyConstructor;
		bool								HasPublicDefaultConstructor;
		bool								HasPublicDestructor;
		bool								HasPublicAssignmentOperator;
		bool								HasPublicDestroyMethod;

											ZEMCClass();
		virtual								~ZEMCClass();
};

class ZEMCContext
{
	public:
		ZEArray<ZEMCClass*>					Classes;
		ZEArray<ZEMCEnumerator*>			Enumerators;

		ZEArray<ZEMCClass*>					TargetClasses;
		ZEArray<ZEMCEnumerator*>			TargetEnumerators;

		ZEArray<ZEMCForwardDeclaration*>	ForwardDeclarations;
		ZEArray<ZEMCInclude*>				Includes;

											ZEMCContext();
											~ZEMCContext();
};

#endif
