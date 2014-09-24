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
	for (int I = 0; I < Context->Declarations.GetCount(); I++)
	{
		if (Context->Declarations[I]->Name == ClassName)
			return Context->Declarations[I];
	}

	for (int I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
	{
		if (Context->ForwardDeclarations[I]->ClassName == ClassName)
		{
			ZEMCClass* ForwardDeclaredClass = *Context->Declarations.Add();
			ForwardDeclaredClass->Name = ClassName;
			ForwardDeclaredClass->IsForwardDeclared = true;
			return ForwardDeclaredClass;
		}
	}

	return NULL;
}

ZEMCEnumerator* ZEMCParser::FindEnumurator(const char* EnumName)
{
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

			case BuiltinType::Char_S:
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
				return false;

			default:
				return false;
		}
	}
	else if (ClangType->isClassType()) // ZE Builtin Types
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
		else if (ClassDecl->getName() == "ZEVector3")
		{
			Output.BaseType = ZEMC_BT_VECTOR3;
		}
		else if (ClassDecl->getName() == "ZEVector4")
		{
			Output.BaseType = ZEMC_BT_VECTOR4;
		}
		else if (ClassDecl->getName() == "ZEQuaternion")
		{
			Output.BaseType = ZEMC_BT_QUATERNION;
		}
		else if (ClassDecl->getName() == "ZEMatrix3x3")
		{
			Output.BaseType = ZEMC_BT_MATRIX3X3;
		}
		else if (ClassDecl->getName() == "ZEMatrix4x4")
		{
			Output.BaseType = ZEMC_BT_MATRIX4X4;
		}
		else
		{
			if (ClassDecl->getNameAsString() == "ZEClass")
			{
				Output.BaseType = ZEMC_BT_CLASS;
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

			if (!Output.Class->HasPublicCopyConstructor || !Output.Class->HasPublicDestructor || Output.Class->IsForwardDeclared)
				return false;
		}
	}
	else if(ClangType->isEnumeralType())
	{
		Output.BaseType = ZEMC_BT_ENUMERATOR;
		Output.Enumurator = FindEnumurator(ClangType->getAs<EnumType>()->getDecl()->getNameAsString().c_str());
		if (Output.Enumurator == NULL)
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
	ZEMCType TempType;
	QualType CanonicalType = ClangType.getCanonicalType();
	const Type* TypePtr = CanonicalType.getTypePtr();
	const Type* BaseTypePtr = NULL;
	
	// Check Qualifier
	if (TypePtr->isReferenceType())
	{
		TempType.TypeQualifier = CanonicalType.isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
		if (TypePtr->getPointeeType()->isPointerType() || TypePtr->getPointeeType()->isReferenceType())
			return false;
		BaseTypePtr = TypePtr->getPointeeType().getTypePtr();
	}
	else
	{
		TempType.TypeQualifier = ClangType.isConstQualified() ? ZEMC_TQ_CONST_VALUE : ZEMC_TQ_VALUE;
		BaseTypePtr = TypePtr;
	}

	if (BaseTypePtr->isClassType() && isa<ClassTemplateSpecializationDecl>(BaseTypePtr->getAsCXXRecordDecl()))
	{
		const ClassTemplateSpecializationDecl* TemplateType = llvm::cast<ClassTemplateSpecializationDecl>(BaseTypePtr->getAsCXXRecordDecl());
		if (TemplateType != NULL)
		{
			if (TemplateType->getNameAsString() == "ZEArray")
			{
				TempType.ContainerType = ZEMC_CT_ARRAY;
			}
			else if (TemplateType->getNameAsString() == "ZEList")
			{
				TempType.ContainerType = ZEMC_CT_LIST;
			}
			else if (TemplateType->getNameAsString() == "ZEContainer")
			{
				TempType.ContainerType = ZEMC_CT_CONTAINER;
			}

			if (TemplateType->getTemplateArgs().size() < 1)
				return false;

			QualType Argument = TemplateType->getTemplateArgs().get(0).getAsType();

			// Const does not allowed in containers
			if (Argument.isConstQualified())
				return false;

			if(Argument.getTypePtr()->isReferenceType())
				return false;

			BaseTypePtr = Argument.getTypePtr();
		}
	}

	ZEString NameOfTheThing = BaseTypePtr->getTypeClassName();
	if (!ProcessBaseType(TempType,  BaseTypePtr))
		return false;

	Output = TempType;
	return true;
}

/*
ZEMCType ZEMCParser::ProcessInnerType(ZEString MainClassName, const Type* ClangType)
{
	ZEMCType TempType;
	if (ClangType->isBuiltinType())
	{
		switch(((BuiltinType*)ClangType)->getKind())
		{
		case BuiltinType::Char_U:
		case BuiltinType::UChar:
			TempType.BaseType = ZEMC_BT_UNSIGNED_INTEGER_8;
			break;

		case BuiltinType::UShort:
			TempType.BaseType = ZEMC_BT_UNSIGNED_INTEGER_16;
			break;

		case BuiltinType::UInt:
			TempType.BaseType = ZEMC_BT_UNSIGNED_INTEGER_32;
			break;

		case BuiltinType::ULongLong:
			TempType.BaseType = ZEMC_BT_UNSIGNED_INTEGER_64;
			break;

		case BuiltinType::Char_S:
		case BuiltinType::SChar:
			TempType.BaseType = ZEMC_BT_INTEGER_8;
			break;

		case BuiltinType::Short:
			TempType.BaseType = ZEMC_BT_INTEGER_16;
			break;

		case BuiltinType::Int:
			TempType.BaseType = ZEMC_BT_INTEGER_32;
			break;

		case BuiltinType::LongLong:
			TempType.BaseType = ZEMC_BT_UNSIGNED_INTEGER_64;
			break;

		case BuiltinType::Float:
			TempType.BaseType = ZEMC_BT_FLOAT;
			break;

		case BuiltinType::Double:
			TempType.BaseType = ZEMC_BT_DOUBLE;
			break;

		case BuiltinType::Bool:
			TempType.BaseType = ZEMC_BT_BOOLEAN;
			break;

		case BuiltinType::Void:
			TempType.BaseType = ZEMC_BT_VOID;
			break;

		default:
			TempType.BaseType = ZEMC_BT_UNDEFINED;
		}
	}
	else if (ClangType->isClassType())
	{
		CXXRecordDecl* ClassPtr = ClangType->getAsCXXRecordDecl();
		if (ClassPtr->getName() == "ZEString")
		{
			TempType.BaseType = ZEMC_BT_STRING;
		}
		else if (ClassPtr->getName() == "ZEArray")
		{
			TempType.BaseType = ZEMC_BT_ARRAY;
		}
		else if (ClassPtr->getName() == "ZEList")
		{
			TempType.BaseType = ZEMC_BT_LIST;
		}
		else if (ClassPtr->getName() == "ZEVector2")
		{
			TempType.BaseType = ZEMC_BT_VECTOR2;
		}
		else if (ClassPtr->getName() == "ZEVector3")
		{
			TempType.BaseType = ZEMC_BT_VECTOR3;
		}
		else if (ClassPtr->getName() == "ZEVector4")
		{
			TempType.BaseType = ZEMC_BT_VECTOR4;
		}
		else if (ClassPtr->getName() == "ZEQuaternion")
		{
			TempType.BaseType = ZEMC_BT_QUATERNION;
		}
		else if (ClassPtr->getName() == "ZEMatrix3x3")
		{
			TempType.BaseType = ZEMC_BT_MATRIX3X3;
		}
		else if (ClassPtr->getName() == "ZEMatrix4x4")
		{
			TempType.BaseType = ZEMC_BT_MATRIX4X4;
		}
		else
		{
			TempType.BaseType = ZEMC_BT_OBJECT_PTR;
		
			if(ClassPtr->isCompleteDefinition())
			{
				
				for (int I = 0; I < Context->Declarations.GetCount(); I++)
				{
					if (Context->Declarations[I]->Name == ClassPtr->getNameAsString())
					{
						TempType.Class = Context->Declarations[I];
					}
					return TempType;
				}

				return ZEMCType();
			}
			else
			{
				if(Context->ForwardDeclarations.GetCount() > 0)
				{
					for(int I = Context->ForwardDeclarations.GetCount() - 1; I >= 0; I--)
					{
						if(Context->ForwardDeclarations[I]->ClassName == TempType.Class->Name)
						{
							Context->ForwardDeclarations[I]->HeaderFileDeclaredIn = MainClassName;
							return TempType;
						}
					}
				}

				return ZEMCType();
			}
		}
	}
	else if(ClangType->isEnumeralType())
	{
		TempType.BaseType = ZEMC_BT_ENUMERATOR;
	}
	else
		TempType.BaseType = ZEMC_BT_UNDEFINED;

	return TempType;
}

ZEMCType ZEMCParser::ProcessType(ZEString MainClassName, QualType& ClangType)
{
	QualType CanonicalType = ClangType.getCanonicalType();
	const Type* TypePtr = ClangType.getTypePtr();

	ZEMCType TempType = ProcessInnerType(MainClassName, TypePtr);
	
	if (CanonicalType.getTypePtr()->isReferenceType())
	{
		const ReferenceType* ReferenceTypePtr = TypePtr->castAs<ReferenceType>();
		QualType ReferenceePtr = ReferenceTypePtr->getPointeeType();

		TempType = ProcessInnerType(MainClassName, ReferenceePtr.getTypePtr());
		if (TempType.BaseType == ZEMC_BT_UNDEFINED)
			return ZEMCType();

		TempType.TypeQualifier = (ReferenceePtr.isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE);

		if (TempType.BaseType == ZEMC_BT_ARRAY)
		{
			const clang::TemplateSpecializationType* TemplateType = ClangType->getPointeeType()->getAs<TemplateSpecializationType>();

			if(TemplateType->getNumArgs() < 1)
				return ZEMCType();

			TemplateArgument Argument = TemplateType->getArg(0);

			if(Argument.getAsType().getTypePtr()->isPointerType())
			{
				ZEMCType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr()->getPointeeType().getTypePtr());
				TempType.SubType = SubTypeData.BaseType;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
				return TempType;
			}
			else
			{
				if(Argument.getAsType().getTypePtr()->isClassType())
					return ZEMCType();

				ZEMCType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr());
				TempType.SubType = SubTypeData.BaseType;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
				return TempType;
			}

			return TempType;
		}

		return TempType;
	}
	else if (CanonicalType.getTypePtr()->isPointerType())
	{
		if(CanonicalType->getPointeeType()->isBuiltinType())
			return ZEMCType();

		if (CanonicalType->getPointeeType()->isClassType())
		{
			TempType.BaseType = ZEMC_BT_OBJECT_PTR;
			TempType.TypeQualifier = CanonicalType->getPointeeType().isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
			TempType.Class->Name = CanonicalType.getBaseTypeIdentifier()->getName().str();

			CXXRecordDecl* ClassPtr = ClangType->getPointeeType().getTypePtr()->getAsCXXRecordDecl();
			
			if(ClassPtr->isCompleteDefinition())
			{
				if(CheckClass(ClassPtr))
					return TempType;
				else
					return ZEMCType();
			}
			else
			{
				if(Context->ForwardDeclarations.GetCount() > 0)
				{
					for(int I = Context->ForwardDeclarations.GetCount() - 1; I >= 0; I--)
					{
						if(Context->ForwardDeclarations[I]->ClassName == TempType.Class->Name)
						{
							Context->ForwardDeclarations[I]->HeaderFileDeclaredIn = MainClassName;
							return TempType;
						}
					}
				}

				return ZEMCType();
			}
		}

		return TempType;
	}
	else
	{
		if (CanonicalType.isConstQualified())
			return ZEMCType();

		TempType = ProcessInnerType(MainClassName, CanonicalType.getTypePtr());

		if (TempType.BaseType == ZEMC_BT_ARRAY)
		{
			const clang::TemplateSpecializationType* TemplateType = ClangType->getAs<TemplateSpecializationType>();

			if(TemplateType->getNumArgs() < 1)
				return ZEMCType();

			TemplateArgument Argument = TemplateType->getArg(0);

			if(Argument.getAsType().getTypePtr()->isPointerType())
			{
				ZEMCType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr()->getPointeeType().getTypePtr());
				TempType.SubType = SubTypeData.BaseType;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
				return TempType;
			}
			else
			{
				if(Argument.getAsType().getTypePtr()->isClassType())
					return ZEMCType();

				ZEMCType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr());
				TempType.SubType = SubTypeData.BaseType;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZEMC_TQ_CONST_REFERENCE : ZEMC_TQ_REFERENCE;
				return TempType;
			}

			return TempType;
		}
		else if (TempType.BaseType == ZEMC_BT_LIST)
		{
			//TO DO : Handle List type
		}
		else if (TempType.BaseType == ZEMC_BT_ENUMERATOR)
		{
			TempType.EnumName = ClangType->getAs<EnumType>()->getDecl()->getNameAsString();

			EnumDecl* EnumDeclaration = ClangType->getAs<EnumType>()->getDecl();

			ZEMCEnumerator* EnumData = new ZEMCEnumerator();
			EnumData->Name = TempType.EnumName;

			for(EnumDecl::enumerator_iterator CurrentEnum = EnumDeclaration->enumerator_begin(), EnumEnd = EnumDeclaration->enumerator_end(); CurrentEnum != EnumEnd; ++CurrentEnum)
			{
				ZEEnumeratorItem* EnumParameterData = new ZEEnumeratorItem();
				EnumParameterData->Name = CurrentEnum->getNameAsString();
				EnumParameterData->Value = *CurrentEnum->getInitVal().getRawData();
				EnumData->Items.Add(EnumParameterData);
			}

			Context->Enumurators.Add(EnumData);
		}

		return TempType;
	}
}*/
