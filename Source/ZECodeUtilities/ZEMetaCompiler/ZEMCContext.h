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
	ZEMC_BT_ENUMERATOR,
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
	ZEMC_BT_CONTAINER
};

enum ZEMCMetaOperatorType
{
	ZEMC_MOT_NONE,
	ZEMC_MOT_ADDITION,
	ZEMC_MOT_ADDITION_ASSIGNMENT,
	ZEMC_MOT_SUBTRACTION,
	ZEMC_MOT_SUBSTRACTION_ASSIGNMENT,
	ZEMC_MOT_MULTIPLICATION,
	ZEMC_MOT_MULTIPLICATION_ASSIGNMENT,
	ZEMC_MOT_DIVISION,
	ZEMC_MOT_DIVISION_ASSIGNMENT,
	ZEMC_MOT_MODULO,
	ZEMC_MOT_MODULO_ASSIGNMENT,
	ZEMC_MOT_INCREMENT,
	ZEMC_MOT_DECREMENT,
	ZEMC_MOT_LOGICAL_NOT,
	ZEMC_MOT_LOGICAL_AND,
	ZEMC_MOT_LOGICAL_OR,
	ZEMC_MOT_BITWISE_AND,
	ZEMC_MOT_BITWISE_AND_ASSIGNMENT,
	ZEMC_MOT_BITWISE_OR,
	ZEMC_MOT_BITWISE_OR_ASSIGNMENT,
	ZEMC_MOT_BITWISE_XOR,
	ZEMC_MOT_BITWISE_XOR_ASSIGNMENT,
	ZEMC_MOT_LEFT_SHIFT,
	ZEMC_MOT_LEFT_SHIFT_ASSIGNMENT,
	ZEMC_MOT_RIGHT_SHIFT,
	ZEMC_MOT_RIGHT_SHIFT_ASSIGNMENT,
	ZEMC_MOT_ASSIGNMENT,
	ZEMC_MOT_EQUAL,
	ZEMC_MOT_NOT_EQUAL,
	ZEMC_MOT_LESS,
	ZEMC_MOT_LESS_EQUAL,
	ZEMC_MOT_GREATER,
	ZEMC_MOT_GREATER_AND_EQUAL,
	ZEMC_MOT_FUNCTION_CALL,
	ZEMC_MOT_ARRAY_SUBSCRIPT
};

enum ZEMCPropertyAccess
{
	ZEMC_PA_NONE		= 0,
	ZEMC_PA_READ		= 1,
	ZEMC_PA_WRITE		= 2,
	ZEMC_PA_READ_WRITE	= 3
};

enum ZEMCTypeQualifier
{
	ZEMC_TQ_VALUE,
	ZEMC_TQ_CONST_VALUE,
	ZEMC_TQ_REFERENCE,
	ZEMC_TQ_CONST_REFERENCE
};

enum ZEMCContainerType
{
	ZEMC_CT_NONE,
	ZEMC_CT_ARRAY,
	ZEMC_CT_LIST,
	ZEMC_CT_CONTAINER
};

class ZEMCEnumerator;
class ZEMCMethod;

class ZEMCType
{
	public:
		ZEMCBaseType BaseType;
		ZEMCTypeQualifier TypeQualifier;
		ZEMCContainerType ContainerType;

		ZEMCClass* Class;
		ZEMCEnumerator* Enumurator;

		bool operator==(const ZEMCType& Other) const;
		bool operator!=(const ZEMCType& Other) const;

		static bool Equal(const ZEMCType& A, const ZEMCType& B);

		ZEMCType();
		~ZEMCType();
};

class ZEMCAttribute
{
	public:
		ZEString Name;
		ZEString MemberOf;
		ZEArray<ZEString> Parameters;

		ZEMCAttribute();
		~ZEMCAttribute();
};

class ZEMCForwardDeclaration
{
	public:
		ZEString HeaderFileDeclaredIn;
		ZEString ClassName;
		ZEString HeaderName;
		
		ZEMCForwardDeclaration();
		~ZEMCForwardDeclaration();
};

class ZEMCDeclaration
{
	public:
		ZEString Name;
		ZEUInt32 Hash;
		ZEArray<ZEMCAttribute> Attributes;

		bool CheckAttribute(const char* Name);
		ZEMCAttribute* GetAttribute(const char* Name);

		ZEMCDeclaration();
		virtual ~ZEMCDeclaration();
};

class ZEMCEnumeratorItem
{
	public:
		ZEString Name;
		ZEUInt32 Value;

		ZEMCEnumeratorItem();
		~ZEMCEnumeratorItem();
};

class ZEMCEnumerator : public ZEMCDeclaration
{
	public:
		ZEArray<ZEMCEnumeratorItem> Items;

		ZEMCEnumerator();
		~ZEMCEnumerator();
};

class ZEMCProperty : public ZEMCDeclaration
{
	public:
		ZEMCClass* Class;

		ZESize ID;
		ZEMCType Type;

		bool HasAccessors;
		ZEMCMethod* Getter;
		ZEMCMethod* Setter;
		ZEMCMethod* Adder;
		ZEMCMethod* Remover;

		bool IsStatic;
		bool IsContainer;

		ZEMCPropertyAccess Access;

		ZEMCProperty();
		~ZEMCProperty();
};

class ZEMCMethodParameter
{
	public:
		ZEString Name;
		ZEMCType Type;

		ZEMCMethodParameter();
		~ZEMCMethodParameter();
};

class ZEMCMethod : public ZEMCDeclaration
{
	public:
		ZEMCClass* Class;

		ZESize ID;
		ZEString MemberOf;

		bool IsVirtual;
		bool IsPure;
		bool IsStatic;
		bool IsEvent;
		bool IsConst;
		bool IsConstructor;
		bool IsOperator;

		ZEMCMetaOperatorType OperatorType;

		ZEMCType ReturnValue;
		ZEArray<ZEMCMethodParameter> Parameters;

		ZEMCMethod();
		virtual ~ZEMCMethod();
};

class ZEMCClass : public ZEMCDeclaration
{
	public:
		ZEMCClass* BaseClass;
		ZEArray<ZEMCProperty*> Properties;
		ZEArray<ZEMCMethod*> Methods;

		bool IsForwardDeclared;
		bool HasScriptBase;

		bool IsAbstract;
		bool IsBuiltInClass;

		bool HasCreateInstanceMethod;
		bool HasPublicCopyConstructor;
		bool HasPublicDefaultConstructor;
		bool HasPublicDestructor;
		bool HasPublicAssignmentOperator;
		bool HasPublicDestroyMethod;

		ZEMCClass();
		virtual ~ZEMCClass();
};

class ZEMCContext
{
	public:
		ZEArray<ZEMCClass*> Classes;
		ZEArray<ZEMCEnumerator*> Enumerators;

		ZEArray<ZEMCClass*> TargetClasses;
		ZEArray<ZEMCEnumerator*> TargetEnumerators;

		ZEArray<ZEMCForwardDeclaration*> ForwardDeclarations;

		ZEMCContext();
		~ZEMCContext();
};

#endif
