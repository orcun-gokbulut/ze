//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserType.cpp
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

#include "ZEMCParser.h"
#include <clang/AST/DeclTemplate.h>

ZEMCClass* ZEMCParser::FindClass(const char* ClassName)
{
	for (int I = 0; I < Context->Classes.GetCount(); I++)
	{
		if (Context->Classes[I]->Name == ClassName)
			return Context->Classes[I];
	}

	for (int I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
	{
		ZEMCForwardDeclaration* ForwardDeclaration = Context->ForwardDeclarations[I];

		if (Context->ForwardDeclarations[I]->Type != ZEMC_DT_CLASS)
			continue;

		if (Context->ForwardDeclarations[I]->Name != ClassName)
			continue;

		ZEMCClass* ForwardDeclaredClass = new ZEMCClass();
		ForwardDeclaredClass->Name = ClassName;
		ForwardDeclaredClass->IsForwardDeclared = true;
		Context->Classes.Add(ForwardDeclaredClass);
		return ForwardDeclaredClass;
	}

	return NULL;
}

ZEMCEnumerator* ZEMCParser::FindEnumurator(const char* EnumName)
{
	for (ZESize I = 0; I < Context->Enumerators.GetCount(); I++)
		if (EnumName == Context->Enumerators[I]->Name)
			return Context->Enumerators[I];

	return NULL;
}

bool ZEMCParser::ProcessBaseType(ZEMCType& Output, const Type* ClangType)
{
	if (ClangType->isBuiltinType()) // C++ Builtin Types
	{
		switch(llvm::cast<BuiltinType>(ClangType)->getKind())
		{
			case BuiltinType::Char_U:
			case BuiltinType::UChar:
				Output.BaseType = ZEMC_BT_UNSIGNED_INTEGER_8;
				break;

			case BuiltinType::UShort:
				Output.BaseType = ZEMC_BT_UNSIGNED_INTEGER_16;
				break;

			case BuiltinType::UInt:
				Output.BaseType = ZEMC_BT_UNSIGNED_INTEGER_32;
				break;

			case BuiltinType::ULongLong:
				Output.BaseType = ZEMC_BT_UNSIGNED_INTEGER_64;
				break;

			case BuiltinType::SChar:
				Output.BaseType = ZEMC_BT_INTEGER_8;
				break;

			case BuiltinType::Short:
				Output.BaseType = ZEMC_BT_INTEGER_16;
				break;

			case BuiltinType::Int:
				Output.BaseType = ZEMC_BT_INTEGER_32;
				break;

			case BuiltinType::LongLong:
				Output.BaseType = ZEMC_BT_UNSIGNED_INTEGER_64;
				break;

			case BuiltinType::Float:
				Output.BaseType = ZEMC_BT_FLOAT;
				break;

			case BuiltinType::Double:
				Output.BaseType = ZEMC_BT_DOUBLE;
				break;

			case BuiltinType::Bool:
				Output.BaseType = ZEMC_BT_BOOLEAN;
				break;

			case BuiltinType::Void:
				Output.BaseType = ZEMC_BT_VOID;
				break;

			default:
				return false;
		}
	}
	else if (ClangType->isClassType()) // ZE Fundamental Types
	{
		CXXRecordDecl* ClassDecl = ClangType->getAsCXXRecordDecl();
		if (ClassDecl->getName() == "ZEString")
		{
			Output.BaseType = ZEMC_BT_STRING;
		}
		else if (ClassDecl->getName() == "ZEVector2")
		{
			Output.BaseType = ZEMC_BT_VECTOR2;
		}
		else if (ClassDecl->getName() == "ZEVector2d")
		{
			Output.BaseType = ZEMC_BT_VECTOR2D;
		}
		else if (ClassDecl->getName() == "ZEVector3")
		{
			Output.BaseType = ZEMC_BT_VECTOR3;
		}
		else if (ClassDecl->getName() == "ZEVector3d")
		{
			Output.BaseType = ZEMC_BT_VECTOR3D;
		}
		else if (ClassDecl->getName() == "ZEVector4")
		{
			Output.BaseType = ZEMC_BT_VECTOR4;
		}
		else if (ClassDecl->getName() == "ZEVector4d")
		{
			Output.BaseType = ZEMC_BT_VECTOR4D;
		}
		else if (ClassDecl->getName() == "ZEQuaternion")
		{
			Output.BaseType = ZEMC_BT_QUATERNION;
		}
		else if (ClassDecl->getName() == "ZEMatrix3x3")
		{
			Output.BaseType = ZEMC_BT_MATRIX3X3;
		}
		else if (ClassDecl->getName() == "ZEMatrix3x3d")
		{
			Output.BaseType = ZEMC_BT_MATRIX3X3D;
		}
		else if (ClassDecl->getName() == "ZEMatrix4x4")
		{
			Output.BaseType = ZEMC_BT_MATRIX4X4;
		}
		else if (ClassDecl->getName() == "ZEMatrix4x4d")
		{
			Output.BaseType = ZEMC_BT_MATRIX4X4D;
		}
		else
		{
			Output.BaseType = ZEMC_BT_OBJECT;
			Output.Class = FindClass(ClassDecl->getNameAsString().c_str());
			if (Output.Class == NULL)
				return false;

			if (!Output.Class->HasPublicCopyConstructor || !Output.Class->HasPublicDestructor || Output.Class->IsForwardDeclared)
				return false;
		}
	}
	else if (ClangType->isPointerType())
	{
		if (!ClangType->getPointeeType().getTypePtr()->isClassType())
			return false;

		CXXRecordDecl* ClassDecl = ClangType->getPointeeType().getTypePtr()->getAsCXXRecordDecl();
		if (ClassDecl->getNameAsString() == "ZEClass")
		{
			Output.BaseType = ZEMC_BT_CLASS;
		}
		else
		{
			Output.BaseType = ZEMC_BT_OBJECT_PTR;
			Output.Class = FindClass(ClassDecl->getNameAsString().c_str());
			if (Output.Class == NULL)
				return false;

			if (Output.Class->IsFundamental)
				return false;

			if (Output.CollectionType == ZEMC_CT_NONE && ClangType->getPointeeType().isConstQualified())
			{
				if (Output.TypeQualifier != ZEMC_TQ_VALUE)
					return false;

				Output.TypeQualifier = ZEMC_TQ_CONST_VALUE;
			}
		}
	}
	else if(ClangType->isEnumeralType())
	{
		Output.BaseType = ZEMC_BT_ENUMERATOR;
		Output.Enumerator = FindEnumurator(ClangType->getAs<EnumType>()->getDecl()->getNameAsString().c_str());
		if (Output.Enumerator == NULL)
			return false;
	}

	else
	{
		return false;
	}

	return true;
}

bool ZEMCParser::ProcessType(ZEMCType& Output, const QualType& ClangType)
{
	QualType CanonicalType = ClangType.getCanonicalType();
	
	ZEMCType FirstLevelType;
	if (CanonicalType->isReferenceType())
	{
		CanonicalType = CanonicalType->getPointeeType();
		FirstLevelType.TypeQualifier = CanonicalType.isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
	}
	else
	{
		FirstLevelType.TypeQualifier = CanonicalType.isConstQualified() ? ZEMC_TQ_CONST_VALUE : ZEMC_TQ_VALUE;
	}

	// Template Types (ZEHolder, ZEArray)
	if (CanonicalType->isClassType() && isa<ClassTemplateSpecializationDecl>(CanonicalType->getAsCXXRecordDecl()))
	{
		const ClassTemplateSpecializationDecl* TemplateType = llvm::cast<ClassTemplateSpecializationDecl>(CanonicalType->getAsCXXRecordDecl());
		if (TemplateType == NULL)
			return false;

		if (TemplateType->getTemplateArgs().size() < 1)
			return false;

		QualType Argument = TemplateType->getTemplateArgs().get(0).getAsType();

		if (TemplateType->getNameAsString() == "ZEArray")
		{
			// References are not allowed in containers
			if (Argument.getTypePtr()->isReferenceType())
				return false;

			ZEMCType SecondLevelType;
			SecondLevelType.TypeQualifier = Argument.isConstQualified() ? ZEMC_TQ_CONST_VALUE : ZEMC_TQ_VALUE;
			
			if (!ProcessBaseType(SecondLevelType,  Argument.getTypePtr()))
				return false;

			// Only ZEObjectPtr can be const inside Array
			if (SecondLevelType.TypeQualifier == ZEMC_TQ_CONST_VALUE &&	SecondLevelType.BaseType != ZEMC_BT_OBJECT_PTR)
				return false;

			Output.CollectionType = ZEMC_CT_ARRAY;
			Output.CollectionQualifier = FirstLevelType.TypeQualifier;
			Output.BaseType = SecondLevelType.BaseType;
			Output.TypeQualifier = SecondLevelType.TypeQualifier;
			Output.Class = SecondLevelType.Class;
			Output.Enumerator = SecondLevelType.Enumerator;

			return true;
		}
		else if (TemplateType->getNameAsString() == "ZEHolder")
		{
			// References are not allowed in holder
			if (Argument.getTypePtr()->isReferenceType())
				return false;

			ZEMCType SecondLevelType;
			SecondLevelType.TypeQualifier = Argument.isConstQualified() ? ZEMC_TQ_CONST_VALUE : ZEMC_TQ_VALUE;

			if (!ProcessBaseType(SecondLevelType,  Argument.getTypePtr()))
				return false;

			if (SecondLevelType.BaseType != ZEMC_BT_OBJECT)
				return false;

			Output.BaseType = ZEMC_BT_OBJECT_HOLDER;
			Output.TypeQualifier = SecondLevelType.TypeQualifier;
			Output.Class = SecondLevelType.Class;

			return true;
		}
	}
	else
	{
		if (!ProcessBaseType(FirstLevelType, CanonicalType.getTypePtr()))
			return false;

		Output = FirstLevelType;
		return true;
	}

	return false;
}
