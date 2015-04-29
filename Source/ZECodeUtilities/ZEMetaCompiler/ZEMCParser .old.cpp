//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParser .old.cpp
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
#include "ZEMCOptions.h"

#include "ZEFile/ZEFileInfo.h"
#include "ZEDS/ZEFormat.h"

#include <clang/AST/DeclTemplate.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/AST/ASTConsumer.h>
#include <llvm/Support/CommandLine.h>
#include <fstream>
#include <string>

ZEMCMetaOperatorType ZEMCParser::GetOperatorType(OverloadedOperatorKind OperatorKind)
{
	switch(OperatorKind)
	{
		case OO_Plus:
			return ZEMC_MOT_ADDITION;
		case OO_PlusEqual:
			return ZEMC_MOT_ADDITION_ASSIGNMENT;
		case OO_PlusPlus:
			return ZEMC_MOT_INCREMENT;
		case OO_Minus:
			return ZEMC_MOT_SUBTRACTION;
		case OO_MinusEqual:
			return ZEMC_MOT_SUBSTRACTION_ASSIGNMENT;
		case OO_MinusMinus:
			return ZEMC_MOT_DECREMENT;
		case OO_Star:
			return ZEMC_MOT_MULTIPLICATION;
		case OO_StarEqual:
			return ZEMC_MOT_MULTIPLICATION_ASSIGNMENT;
		case OO_Slash:
			return ZEMC_MOT_DIVISION;
		case OO_SlashEqual:
			return ZEMC_MOT_DIVISION_ASSIGNMENT;
		case OO_Percent:
			return ZEMC_MOT_MODULO;
		case OO_PercentEqual:
			return ZEMC_MOT_MODULO_ASSIGNMENT;
		case OO_Amp:
			return ZEMC_MOT_BITWISE_AND;
		case OO_AmpEqual:
			return ZEMC_MOT_BITWISE_AND_ASSIGNMENT;
		case OO_AmpAmp:
			return ZEMC_MOT_LOGICAL_AND;
		case OO_Pipe:
			return ZEMC_MOT_BITWISE_OR;
		case OO_PipeEqual:
			return ZEMC_MOT_BITWISE_OR_ASSIGNMENT;
		case OO_PipePipe:
			return ZEMC_MOT_LOGICAL_OR;
		case OO_Caret:
			return ZEMC_MOT_BITWISE_XOR;
		case OO_CaretEqual:
			return ZEMC_MOT_BITWISE_XOR_ASSIGNMENT;
		case OO_Equal:
			return ZEMC_MOT_ASSIGNMENT;
		case OO_EqualEqual:
			return ZEMC_MOT_EQUAL;
		case OO_ExclaimEqual:
			return ZEMC_MOT_NOT_EQUAL;
		case OO_Less:
			return ZEMC_MOT_LESS;
		case OO_LessEqual:
			return ZEMC_MOT_LESS_EQUAL;
		case OO_LessLess:
			return ZEMC_MOT_LEFT_SHIFT;
		case OO_LessLessEqual:
			return ZEMC_MOT_LEFT_SHIFT_ASSIGNMENT;
		case OO_Greater:
			return ZEMC_MOT_GREATER;
		case OO_GreaterEqual:
			return ZEMC_MOT_GREATER_AND_EQUAL;
		case OO_GreaterGreater:
			return ZEMC_MOT_RIGHT_SHIFT;
		case OO_GreaterGreaterEqual:
			return ZEMC_MOT_RIGHT_SHIFT_ASSIGNMENT;
		case OO_Call:
			return ZEMC_MOT_FUNCTION_CALL;
		case OO_Subscript:
			return ZEMC_MOT_ARRAY_SUBSCRIPT;
		default:
			return ZEMC_MOT_NOT_OPERATOR;
	}
}

void ZEMCParser::RaiseNote(SourceLocation& Location, const char* WarningText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Level::Note, WarningText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMCParser::RaiseWarning(SourceLocation& Location, const char* WarningText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Level::Warning, WarningText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMCParser::RaiseError(SourceLocation& Location, const char* ErrorText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Level::Error, ErrorText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMCParser::RaiseCriticalError(SourceLocation& Location, const char* ErrorText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Level::Error, ErrorText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
	exit(EXIT_FAILURE);
}

bool ZEMCParser::CheckClassHasDerivedFromZEObject(CXXRecordDecl* Class)
{
	bool Result = false;
	if (Class->getName() == "ZEObject")
		Result = true;

	if(Class->isCompleteDefinition())
	{
		for(CXXRecordDecl::base_class_iterator CurrentBaseClass = Class->bases_begin(), LastBaseClass = Class->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
		{
			if (!CurrentBaseClass->getType()->isClassType())
				continue;

			bool SubResult = CheckClassHasDerivedFromZEObject(CurrentBaseClass->getType()->getAsCXXRecordDecl());

			if (CurrentBaseClass->getAccessSpecifier() != AccessSpecifier::AS_public)
			{
				RaiseError(CurrentBaseClass->getType()->getAsCXXRecordDecl()->getLocation(), "Class has private or protected ZEObject inheritance path. ZEObject must be inherited publicly.");
				return false;
			}

			if (SubResult && Result)
			{
				RaiseError(CurrentBaseClass->getType()->getAsCXXRecordDecl()->getLocation(), "Class s\" has inherited ZEObject multiple times in it's inheritance tree. Check it's parents.");
			}
			else if (SubResult)
				Result = true;
		}
	}

	return Result;
}

bool ZEMCParser::CheckClassHasZEObjectMacro(CXXRecordDecl* Class)
{
	bool HasClassMethodFound = false;
	bool HasGetClassMethodFound = false;

	for (CXXRecordDecl::method_iterator Iterator = Class->method_begin(); Iterator != Class->method_end(); Iterator++)
	{
		if (Iterator->param_size() != 0)
			continue;

		if (!Iterator->getDeclName().isIdentifier())
			continue;

		if (Iterator->getName() == "Class")
		{
			if (!Iterator->isStatic())
			{
				RaiseError(Iterator->getLocation(), "Class() method is not static.");
				continue;
			}
		}
		else if (Iterator->getName() == "GetClass")
		{
			if (!Iterator->isVirtual())
			{
				RaiseError(Iterator->getLocation(), "GetClass() method is not virtual.");
				continue;
			}

			if (Iterator->isPure())
			{
				RaiseError(Iterator->getLocation(), "GetClass() method is pure virtual.");
				continue;
			}

			if ((Iterator->getTypeQualifiers() & Qualifiers::Const) != Qualifiers::Const)
			{
				RaiseError(Iterator->getLocation(), "GetClass() method is not const.");
				continue;
			}
		}
		else
		{
			continue;
		}

		QualType ReturnType = Iterator->getCallResultType().getCanonicalType();
		if (!ReturnType.getTypePtr()->isPointerType() ||
			ReturnType.getQualifiers() != 0 ||
			!ReturnType.getTypePtr()->getPointeeType()->isClassType() ||
			ReturnType.getTypePtr()->getPointeeType().getQualifiers() != 0 ||
			ReturnType.getTypePtr()->getPointeeType()->getAsCXXRecordDecl()->getName() != "ZEClass")
		{
			RaiseError(Iterator->getLocation(), "Return type of method is not 'ZEClass*'.");
			continue;
		}

		if (Iterator->getName() == "Class")
			HasClassMethodFound = true;
		else if (Iterator->getName() == "GetClass")
			HasGetClassMethodFound = true;

		if (HasClassMethodFound && HasGetClassMethodFound)
			break;
	}
	
	if (!HasGetClassMethodFound)
	{
		RaiseError(Class->getLocation(), "virtual GetClass() const method has not found. Please make sure that ZE_OBJECT macro has been defined inside the class.");
		return false;
	}

	if (!HasClassMethodFound)
	{
		RaiseError(Class->getLocation(), "static Class() const method has not found. Please make sure that ZE_OBJECT macro has been defined inside the class.");
		return false;
	}

	return true;
}

bool ZEMCParser::CheckClass(CXXRecordDecl* Class)
{
	if (!CheckClassHasDerivedFromZEObject(Class))
		return false;

	if (Class->isTemplateDecl())
		RaiseError(Class->getLocation(), "ZEObject based templates are not supported.");

	if (!CheckClassHasZEObjectMacro(Class))
		return false;

	return true;
}

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
			TempType.Class->Name = ClassPtr->getNameAsString();

			if(ClassPtr->isCompleteDefinition())
			{
				if(CheckClass(ClassPtr))
					return TempType;
				else
					return ZEMCType();

				if(TempType.Class->Name == "ZEEvent")
				{
					TempType.BaseType = ZEMC_BT_CLASS;
					return TempType;
				}
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

	ZEMCType TempType;
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

				for(CXXRecordDecl::base_class_iterator CurrentBaseClass = ClassPtr->bases_begin(), LastBaseClass = ClassPtr->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
				{
					CXXRecordDecl* BaseClassDecl = CurrentBaseClass->getType()->getAsCXXRecordDecl();
					TempType.Class->BaseClass->Name = BaseClassDecl->getNameAsString();

					const clang::TemplateSpecializationType* TemplateType = CurrentBaseClass->getType()->getAs<TemplateSpecializationType>();

					if(TemplateType != NULL)
					{
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
					}
				}

				if(TempType.Class->Name == "ZEClass")
				{
					TempType.BaseType = ZEMC_BT_CLASS;
					return TempType;
				}
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
}

bool ZEMCParser::ParseAttribute(ZEMCAttribute* Data, const AnnotateAttr* Attribute)
{
	std::string Temp = Attribute->getAnnotation().str();
	const char* AttributeText = Temp.c_str();

	ZESize ParameterIndex = 0;
	char ParameterText[1024];
	ZESize ParameterTextIndex = 0;
	ZESize AttributeIndex = 0;

	enum
	{
		ZE_PAS_PARAMETER_START,
		ZE_PAS_PARAMETER,
		ZE_PAS_PARAMETER_END,
		ZE_PAS_QUATATION,
		ZE_PAS_ESCAPE
	} State = ZE_PAS_PARAMETER_START;

	while(AttributeText[AttributeIndex] != NULL)
	{
		char InputCharacter = AttributeText[AttributeIndex];

		switch(State)
		{
		case ZE_PAS_PARAMETER_START:
			if (InputCharacter == ' ' || InputCharacter == '\t')
			{
				AttributeIndex++;
			}
			else if (isalnum(InputCharacter))
			{
				State = ZE_PAS_PARAMETER;
			}
			else if (InputCharacter == '\"')
			{
				State = ZE_PAS_QUATATION;
				AttributeIndex++;
			}
			else
			{
				RaiseError(Attribute->getLocation(), "Wrong identifier name.");
				return false;
			}				
			break;

		case ZE_PAS_PARAMETER:
			if (InputCharacter == ',')
			{
				State = ZE_PAS_PARAMETER_END;
			}
			else if (InputCharacter == ' ' || InputCharacter == '\t')
			{
				State == ZE_PAS_PARAMETER_END;
				AttributeIndex++;
			}
			else if (!isalnum(InputCharacter))
			{
				RaiseError(Attribute->getLocation(), "Wrong identifier name.");
				return false;
			}
			else
			{
				if (ParameterTextIndex >= 1024)
				{
					RaiseError(Attribute->getLocation(), "Max parameter size reached.");
					return false;
				}

				ParameterText[ParameterTextIndex] = InputCharacter;
				ParameterTextIndex++;
				AttributeIndex++;
			}
			break;

		case ZE_PAS_PARAMETER_END:
			if (InputCharacter == ' ' || InputCharacter == '\t')
			{
				AttributeIndex++;
			}
			else if (InputCharacter == ',')
			{
				ParameterText[ParameterTextIndex] = '\0';
				if (ParameterIndex == 0)
					Data->Name = ParameterText;
				else
					Data->Parameters.Add(ParameterText);

				ParameterIndex++;
				State = ZE_PAS_PARAMETER_START;
				ParameterTextIndex = 0;
				AttributeIndex++;
			}
			else
			{
				RaiseError(Attribute->getLocation(), "Wrong parameter termination character.");
				return false;
			}
			break;


		case ZE_PAS_QUATATION:
			if (InputCharacter == '\\')
			{
				State = ZE_PAS_ESCAPE;
				AttributeIndex++;
			}
			else if (InputCharacter == '\"')
			{
				State = ZE_PAS_PARAMETER_END;
				AttributeIndex++;
			}
			else
			{
				if (ParameterTextIndex >= 1024)
				{
					RaiseError(Attribute->getLocation(), "Max parameter size reached.");
					return false;
				}
				ParameterText[ParameterTextIndex] = InputCharacter;
				ParameterTextIndex++;
				AttributeIndex++;
			}

			break;

		case ZE_PAS_ESCAPE:
			switch(InputCharacter)
			{
				case 'b':
					ParameterText[ParameterTextIndex] = '\b';
					break;
				case 'f':
					ParameterText[ParameterTextIndex] = '\f';
					break;
				case 'n':
					ParameterText[ParameterTextIndex] = '\n';
					break;
				case 'r':
					ParameterText[ParameterTextIndex] = '\r';
					break;
				case 't':
					ParameterText[ParameterTextIndex] = '\t';
					break;
				case '\\':
					ParameterText[ParameterTextIndex] = '\\';
					break;
				case '\'':
					ParameterText[ParameterTextIndex] = '\'';
					break;
				case '\"':
					ParameterText[ParameterTextIndex] = '\"';
					break;
				default:
					RaiseError(Attribute->getLocation(), "Unknown string escape character.");
					return false;
			}

			AttributeIndex++;
			State = ZE_PAS_QUATATION;
		}
	}

	if (State == ZE_PAS_PARAMETER_END || State == ZE_PAS_PARAMETER)
	{
		ParameterText[ParameterTextIndex] = '\0';
		if (Data->Name == "" || Data->Name == NULL)
			Data->Name = ParameterText;
		else
			Data->Parameters.Add(ParameterText);

		ParameterTextIndex = 0;
		AttributeIndex++;
	}
	else
	{
		RaiseError(Attribute->getLocation(), "Cannot parse attribute.");
		return false;
	}

	return true;
}

void ZEMCParser::ProcessDeclaration(Decl* BaseDeclaration)
{
	if(isa<EnumDecl>(BaseDeclaration) && BaseDeclaration)
		ProcessEnumerator(dyn_cast<EnumDecl>(BaseDeclaration));

	if (isa<CXXRecordDecl>(BaseDeclaration) && BaseDeclaration)
		ProcessClass(dyn_cast<CXXRecordDecl>(BaseDeclaration));
}

void ZEMCParser::ProcessEnumerator(EnumDecl* EnumDeclaration)
{
	ZEMCEnumerator* EnumData = new ZEMCEnumerator();
	EnumData->Name = EnumDeclaration->getNameAsString();

	if(EnumData->Name == NULL || EnumData->Name == "")
		return;

	EnumData->Hash = EnumData->Name.Hash();

	for(EnumDecl::enumerator_iterator Current = EnumDeclaration->enumerator_begin(), End = EnumDeclaration->enumerator_end(); Current != End; ++Current)
	{
		ZEEnumeratorItem* EnumParameterData = new ZEEnumeratorItem();
		EnumParameterData->Name = Current->getNameAsString();
		EnumParameterData->Value = *Current->getInitVal().getRawData();
		EnumData->Items.Add(EnumParameterData);
	}

	Context->Enumurators.Add(EnumData);
}

void ZEMCParser::ProcessClass(CXXRecordDecl* Class)
{
	if(Class->isClass() && !Class->isCompleteDefinition() && Class->hasAttrs())
	{
		ZEMCAttribute* AttributeData = new ZEMCAttribute();
		for(CXXRecordDecl::attr_iterator CurrentAttr = Class->attr_begin(), LastAttr = Class->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
		{
			ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		}

		if(AttributeData->Name == "ForwardDeclaration")
		{
			ZEMCForwardDeclaration* ForwardDeclaredClass = new ZEMCForwardDeclaration();
			
			ForwardDeclaredClass->ClassName = AttributeData->Parameters[0].ToCString();
			ForwardDeclaredClass->HeaderName = AttributeData->Parameters[1].ToCString();

			Context->ForwardDeclarations.Add(ForwardDeclaredClass);
		}
	}

	if (!Class->isCompleteDefinition())
		return;

	ZEArray<ZEString> BuiltInClasses;
	BuiltInClasses.Add("ZEVector2");
	BuiltInClasses.Add("ZEVector3");
	BuiltInClasses.Add("ZEVector4");
	BuiltInClasses.Add("ZEMatrix3x3");
	BuiltInClasses.Add("ZEMatrix4x4");
	BuiltInClasses.Add("ZEQuaternion");

	bool IsBuiltInClassFound = false;

	for(ZESize I = 0; I < BuiltInClasses.GetCount(); I++)
	{
		if(BuiltInClasses[I] == Class->getNameAsString().c_str())
			IsBuiltInClassFound = true;
	}

	if(!IsBuiltInClassFound)
	{
		if (!CheckClass(Class))
			return;
	}

	ZEMCClass* ClassData = new ZEMCClass();
	ClassData->Name = Class->getNameAsString();
	ClassData->Hash = ClassData->Name.Hash();
	ClassData->HasCreateInstanceMethod = false;
	ClassData->HasPublicCopyConstructor = false;
	ClassData->HasPublicConstructor = false;
	ClassData->IsAbstract = Class->isAbstract();
	ClassData->IsBuiltInClass = IsBuiltInClassFound;

	for(CXXRecordDecl::base_class_iterator CurrentBaseClass = Class->bases_begin(), LastBaseClass = Class->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
	{
		ClassData->BaseClass = new ZEMCClass();
		ClassData->BaseClass->Name = CurrentBaseClass->getType()->getAsCXXRecordDecl()->getNameAsString();
	}

	ZEMCAttribute* AttributeData = new ZEMCAttribute();
	for(CXXRecordDecl::attr_iterator CurrentAttr = Class->attr_begin(), LastAttr = Class->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		ClassData->Attributes.Add(AttributeData);
	}

	for(CXXRecordDecl::decl_iterator Current = Class->decls_begin(), End = Class->decls_end(); Current != End; ++Current)
	{
		if(isa<VarDecl>(*Current))
		{
			if(!IsBuiltInClassFound)
			{
				VarDecl* StaticProperty = ((VarDecl*)*Current);
				ProcessProperty(ClassData, StaticProperty);
			}
		}
		else if(isa<FieldDecl>(*Current))
		{
			if(!IsBuiltInClassFound)
			{
				FieldDecl* NonStaticProperty = ((FieldDecl*)*Current);
				ProcessProperty(ClassData, NonStaticProperty);
			}
		}
		else if(isa<CXXMethodDecl>(*Current))
		{
			CXXMethodDecl* Method = ((CXXMethodDecl*)*Current);
			//ProcessMethod(ClassData, Method);
		}
		else if(isa<RecordDecl>(*Current))
		{
			RecordDecl* AnonymousRecord = ((RecordDecl*)*Current);
			ZEMCAttribute* AnonymousRecordAttributeData = new ZEMCAttribute();
			for(CXXRecordDecl::attr_iterator CurrentAttr = AnonymousRecord->attr_begin(), LastAttr = AnonymousRecord->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
			{
				ParseAttribute(AnonymousRecordAttributeData, ((AnnotateAttr*)(*CurrentAttr)));
				ClassData->Attributes.Add(AnonymousRecordAttributeData);
			}
		}
	}

	for(ZESize I = 0; I < ClassData->Events.GetCount(); I++)
	{
		ZEMCMethod* Method = new ZEMCMethod();
		Method->Name = ClassData->Events[I]->Name;
		Method->IsEvent = true;
		Method->IsStatic = ClassData->Events[I]->IsStatic;
		Method->Hash = Method->Name.Hash();
		Method->Attributes = ClassData->Events[I]->Attributes;
		
		Method->ReturnValue = ClassData->Events[I]->ReturnValue;

		for(ZESize J = 0; J < ClassData->Events[I]->Parameters.GetCount(); J++)
		{
			ZEMCMethodParameter* MethodParameter = new ZEMCMethodParameter();
			MethodParameter->Name = ClassData->Events[I]->Parameters[J]->Name;
			MethodParameter->Type = ClassData->Events[I]->Parameters[J]->Type;
			MethodParameter->Type.Enumurator = ClassData->Events[I]->Parameters[J]->EnumData;

			Method->Parameters.Add(MethodParameter);
		}

		ClassData->Methods.Add(Method);
	}

	Context->Declarations.Add(ClassData);
	if (Compiler->getSourceManager().getFileID(Class->getLocation()) == Compiler->getSourceManager().getMainFileID())
		Context->TargetDeclarations.Add(ClassData);
}

void ZEMCParser::ProcessProperty(ZEMCClass* ClassData, VarDecl* StaticProperty)
{
	if (StaticProperty->getAccess() != AccessSpecifier::AS_public)
		return;

	ZEMCType PropertyType = ProcessType(ClassData->Name, StaticProperty->getType());
	if (PropertyType.BaseType == ZEMC_BT_UNDEFINED)
		return;

	ZEMCProperty* PropertyData = new ZEMCProperty();
	PropertyData->Name = StaticProperty->getNameAsString();
	PropertyData->Hash = PropertyData->Name.Hash();
	PropertyData->IsStatic = true;
	PropertyData->IsContainer = false;
	PropertyData->Type = PropertyType;

	if(PropertyType.Class->BaseClass != NULL && (PropertyType.Class->BaseClass->Name != NULL || PropertyType.Class->BaseClass->Name != ""))
	{
		if(PropertyType.Class->BaseClass->Name == "ZEContainer")
			PropertyData->IsContainer = true;
	}

	if(StaticProperty->getType()->isClassType() && StaticProperty->getType()->getAsCXXRecordDecl()->getNameAsString() == "ZEEvent")
	{
		bool Error = false;

		CXXRecordDecl* TemplateEvent = StaticProperty->getType()->getAsCXXRecordDecl();

		ZEMCEvent* Event = new ZEMCEvent();
		Event->Name = PropertyData->Name;
		Event->IsStatic = PropertyData->IsStatic;

		for(CXXRecordDecl::decl_iterator CurrentTemplateEventMethod = TemplateEvent->decls_begin(), LastTemplateEventMethod = TemplateEvent->decls_end(); 
			CurrentTemplateEventMethod != LastTemplateEventMethod; ++CurrentTemplateEventMethod)
		{
			clang::Decl::Kind TemplateEventKind = CurrentTemplateEventMethod->getKind();

			if(TemplateEventKind == Decl::Kind::CXXMethod)
			{
				CXXMethodDecl* TemplateEventMethodDecl = (CXXMethodDecl*)*CurrentTemplateEventMethod;

				Event->ReturnValue = ProcessType(ClassData->Name, TemplateEventMethodDecl->getCallResultType());

				if (Event->ReturnValue.BaseType == ZEMC_BT_UNDEFINED)
				{
					Error = true;
					break;
				}

				ZEString TemplateEventMethodName = TemplateEventMethodDecl->getNameAsString();
				if(TemplateEventMethodName == "Call")
				{
					for(FunctionDecl::param_iterator CurrentTemplateMethodParameter = TemplateEventMethodDecl->param_begin(), LastTemplateMethodParameter = TemplateEventMethodDecl->param_end();
						CurrentTemplateMethodParameter != LastTemplateMethodParameter; ++CurrentTemplateMethodParameter)
					{
						ZEEventParameterData* TemplateEventParameterData = new ZEEventParameterData();
						TemplateEventParameterData->Name = (*CurrentTemplateMethodParameter)->getNameAsString();
						TemplateEventParameterData->Type = ProcessType(ClassData->Name, (*CurrentTemplateMethodParameter)->getType());

						if(TemplateEventParameterData->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
							{
								if(Context->Enumurators[I]->Items.GetCount() > 0 && TemplateEventParameterData->Type.EnumName == Context->Enumurators[I]->Name)
								{
									TemplateEventParameterData->EnumData = Context->Enumurators[I];
									break;
								}
							}
						}

						Event->Parameters.Add(TemplateEventParameterData);
					}

					break;
				}
			}
		}

		ZEMCAttribute* AttributeData = new ZEMCAttribute();
		for(CXXRecordDecl::attr_iterator CurrentAttr = StaticProperty->attr_begin(), LastAttr = StaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
		{
			ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
			Event->Attributes.Add(AttributeData);
		}

		if (Error)
			return;
		else
			ClassData->Events.Add(Event);

		return;
	}

	if(PropertyType.BaseType == ZEMC_BT_ENUMERATOR)
	{
		for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
		{
			if(Context->Enumurators[I]->Items.GetCount() > 0 && PropertyData->Type.EnumName == Context->Enumurators[I]->Name)
			{
				PropertyData->Type.Enumurator = Context->Enumurators[I];
				break;
			}
		}
	}

	ZEMCAttribute* AttributeData = new ZEMCAttribute();
	for(CXXRecordDecl::attr_iterator CurrentAttr = StaticProperty->attr_begin(), LastAttr = StaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		PropertyData->Attributes.Add(AttributeData);
	}

	ClassData->Properties.Add(PropertyData);
}

void ZEMCParser::ProcessProperty(ZEMCClass* ClassData, FieldDecl* NonStaticProperty)
{
	if (NonStaticProperty->getAccess() != AccessSpecifier::AS_public)
		return;

	ZEMCType PropertyType = ProcessType(ClassData->Name, NonStaticProperty->getType());
	if (PropertyType.BaseType == ZEMC_BT_UNDEFINED)
		return;

	ZEMCProperty* PropertyData = new ZEMCProperty();
	PropertyData->Name = NonStaticProperty->getNameAsString();
	PropertyData->Hash = PropertyData->Name.Hash();
	PropertyData->HasSetterGetter = false;
	PropertyData->IsStatic = false;
	PropertyData->IsContainer = false;
	PropertyData->Type = PropertyType;

	if(PropertyType.Class->BaseClass != NULL && (PropertyType.Class->BaseClass->Name != NULL || PropertyType.Class->BaseClass->Name != ""))
	{
		if(PropertyType.Class->BaseClass->Name == "ZEContainer")
			PropertyData->IsContainer = true;
	}

	if(NonStaticProperty->getType()->isClassType() && NonStaticProperty->getType()->getAsCXXRecordDecl()->getNameAsString() == "ZEEvent")
	{
		bool Error = false;

		CXXRecordDecl* TemplateEvent = NonStaticProperty->getType()->getAsCXXRecordDecl();

		ZEMCEvent* Event = new ZEMCEvent();
		Event->Name = PropertyData->Name;
		Event->IsStatic = PropertyData->IsStatic;
		Event->ClassName = ClassData->Name;

		for(CXXRecordDecl::decl_iterator CurrentTemplateEventMethod = TemplateEvent->decls_begin(), LastTemplateEventMethod = TemplateEvent->decls_end(); 
			CurrentTemplateEventMethod != LastTemplateEventMethod; ++CurrentTemplateEventMethod)
		{
			clang::Decl::Kind TemplateEventKind = CurrentTemplateEventMethod->getKind();

			if(TemplateEventKind == Decl::Kind::CXXMethod)
			{
				CXXMethodDecl* TemplateEventMethodDecl = (CXXMethodDecl*)*CurrentTemplateEventMethod;

				Event->ReturnValue = ProcessType(ClassData->Name, TemplateEventMethodDecl->getCallResultType());

				if (Event->ReturnValue.BaseType == ZEMC_BT_UNDEFINED)
				{
					Error = true;
					break;
				}

				ZEString TemplateEventMethodName = TemplateEventMethodDecl->getNameAsString();
				if(TemplateEventMethodName == "Call")
				{
					for(FunctionDecl::param_iterator CurrentTemplateMethodParameter = TemplateEventMethodDecl->param_begin(), LastTemplateMethodParameter = TemplateEventMethodDecl->param_end();
						CurrentTemplateMethodParameter != LastTemplateMethodParameter; ++CurrentTemplateMethodParameter)
					{
						ZEEventParameterData* TemplateEventParameterData = new ZEEventParameterData();
						TemplateEventParameterData->Name = (*CurrentTemplateMethodParameter)->getNameAsString();
						TemplateEventParameterData->Type = ProcessType(ClassData->Name, (*CurrentTemplateMethodParameter)->getType());

						if(TemplateEventParameterData->Type.BaseType == ZEMC_BT_ENUMERATOR)
						{
							for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
							{
								if(Context->Enumurators[I]->Items.GetCount() > 0 && TemplateEventParameterData->Type.EnumName == Context->Enumurators[I]->Name)
								{
									TemplateEventParameterData->EnumData = Context->Enumurators[I];
									break;
								}
							}
						}

						Event->Parameters.Add(TemplateEventParameterData);
					}

					break;
				}
			}
		}

		ZEMCAttribute* AttributeData = new ZEMCAttribute();
		for(CXXRecordDecl::attr_iterator CurrentAttr = NonStaticProperty->attr_begin(), LastAttr = NonStaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
		{
			ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
			Event->Attributes.Add(AttributeData);
		}

		if (Error)
			return;
		else
			ClassData->Events.Add(Event);

		return;
	}

	if(PropertyType.BaseType == ZEMC_BT_ENUMERATOR)
	{
		for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
		{
			if(Context->Enumurators[I]->Items.GetCount() > 0 && PropertyData->Type.EnumName == Context->Enumurators[I]->Name)
			{
				PropertyData->Type.Enumurator = Context->Enumurators[I];
				break;
			}
		}
	}

	ZEMCAttribute* AttributeData = new ZEMCAttribute();
	for(CXXRecordDecl::attr_iterator CurrentAttr = NonStaticProperty->attr_begin(), LastAttr = NonStaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		PropertyData->Attributes.Add(AttributeData);
	}

	ClassData->Properties.Add(PropertyData);
}

void ZEMCParser::ProcessMethod(ZEMCClass* ClassData, CXXMethodDecl* Method)
{
	bool IsOperator = false;
	if(Method->isOverloadedOperator())
		IsOperator = true;

	if(Method->getAccess() != AccessSpecifier::AS_public)
		return;

	if(!Method->getType()->isFunctionType())
		return;

	if(Method->isImplicit())
		return;

/*	if(Method->isCopyAssignmentOperator())
		ClassData->HasPublicCopyConstructor = true;*/

	if(isa<CXXDestructorDecl>(Method))
		return;

	if(isa<CXXConstructorDecl>(Method))
	{
		CXXConstructorDecl* constructor = cast<CXXConstructorDecl>(Method);
		if (constructor->isCopyOrMoveConstructor())
			return;
		else if (constructor->isDefaultConstructor())
		{
			ClassData->HasPublicConstructor = true;
			return;
		}
	}

	ZEMCType ReturnType = ProcessType(ClassData->Name, Method->getCallResultType());
	if (ReturnType.BaseType == ZEMC_BT_UNDEFINED)
		return;

	ZEString MethodName = Method->getNameAsString();

	if(MethodName == "CreateInstance")
		ClassData->HasCreateInstanceMethod = true;

	if((MethodName.SubString(0, 2) == "Set" && Method->param_size() == 1) || (MethodName.SubString(0, 2) == "Get" && Method->param_size() == 0))
	{
		ZEString PropertyName = MethodName.SubString(3, MethodName.GetLength() - 1);
		
		bool IsPropertyFound = false;
		ZESize FoundPropertyIndex = -1;

		for(ZESize I = 0; I < ClassData->Properties.GetCount(); I++)
		{
			if(PropertyName == ClassData->Properties[I]->Name)
			{
				IsPropertyFound = true;
				FoundPropertyIndex = I;
				break;
			}
		}

		if(!IsPropertyFound)
		{
			ZEMCProperty* PropertyData = new ZEMCProperty();
			PropertyData->Name = PropertyName;
			PropertyData->Hash = PropertyName.Hash();
			PropertyData->HasSetterGetter = true;
			PropertyData->IsStatic = false;
			PropertyData->IsContainer = false;

			if(MethodName.SubString(0, 2) == "Set")
				PropertyData->Setter = MethodName;
			else if(MethodName.SubString(0, 2) == "Get")
				PropertyData->Getter = MethodName;
			else
				return;

			if(MethodName.SubString(0, 2) != "Get")
			{
				for(clang::FunctionDecl::param_iterator CurrentParameter = Method->param_begin(), LastParameter = Method->param_end(); CurrentParameter != LastParameter; ++CurrentParameter)
				{
					ZEMCType PropertyType = ProcessType(ClassData->Name, (*CurrentParameter)->getType());
					if (PropertyType.BaseType == ZEMC_BT_UNDEFINED)
						return;

					PropertyData->Type = PropertyType;

					if(PropertyType.BaseType == ZEMC_BT_ENUMERATOR)
					{
						for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
						{
							if(Context->Enumurators[I]->Items.GetCount() > 0 && PropertyType.EnumName == Context->Enumurators[I]->Name)
							{
								PropertyData->Type.Enumurator = Context->Enumurators[I];
								break;
							}
						}
					}
				}
			}
			else
			{
				ZEMCType PropertyType = ProcessType(ClassData->Name, Method->getCallResultType());
				if (PropertyType.BaseType == ZEMC_BT_UNDEFINED)
					return;

				if(PropertyType.BaseType == ZEMC_BT_ENUMERATOR)
				{
					for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
					{
						if(Context->Enumurators[I]->Items.GetCount() > 0 && PropertyType.EnumName == Context->Enumurators[I]->Name)
						{
							PropertyData->Type.Enumurator = Context->Enumurators[I];
							break;
						}
					}
				}

				PropertyData->Type = PropertyType;
			}
			
			ZEMCAttribute* AttributeData = new ZEMCAttribute();
			for(CXXRecordDecl::attr_iterator CurrentAttr = Method->attr_begin(), LastAttr = Method->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
			{
				ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
				PropertyData->Attributes.Add(AttributeData);
			}

			ClassData->Properties.Add(PropertyData);
		}
		else
		{
			if(MethodName.SubString(0, 2) == "Set")
				ClassData->Properties[FoundPropertyIndex]->Setter = MethodName;
			else if(MethodName.SubString(0, 2) == "Get")
				ClassData->Properties[FoundPropertyIndex]->Getter = MethodName;
		}
	}

	ZEMCMethod* MethodData = new ZEMCMethod();
	MethodData->Name = MethodName;
	MethodData->IsConst = Method->isConst();
	MethodData->IsVirtual = Method->isVirtual();
	MethodData->IsPure = Method->isPure();
	MethodData->IsOperator = IsOperator;
	MethodData->IsStatic = Method->isStatic();
	MethodData->IsEvent = false;
	MethodData->Hash = MethodData->Name.Hash();
	MethodData->ReturnValue = ReturnType;

	if(IsOperator)
		MethodData->OperatorType = GetOperatorType(Method->getOverloadedOperator());

	if(MethodData->ReturnValue.BaseType == ZEMC_BT_ENUMERATOR)
	{
		for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
		{
			if(Context->Enumurators[I]->Items.GetCount() > 0 && MethodData->ReturnValue.EnumName == Context->Enumurators[I]->Name)
			{
				MethodData->ReturnValue.Enumurator = Context->Enumurators[I];
				break;
			}
		}
	}

	bool Error = false;
	ZESize ParameterCount = 0;
	for(clang::FunctionDecl::param_iterator CurrentParameter = Method->param_begin(), LastParameter = Method->param_end(); CurrentParameter != LastParameter; ++CurrentParameter)
	{
		ZEMCType ParameterType = ProcessType(ClassData->Name, (*CurrentParameter)->getType());
		if (ParameterType.BaseType == ZEMC_BT_UNDEFINED)
		{
			Error = true;
			break;
		}

		ZEMCMethodParameter* MethodParameterData = new ZEMCMethodParameter();
		if(!(*CurrentParameter)->getNameAsString().empty())
			MethodParameterData->Name = (*CurrentParameter)->getNameAsString();
		else
		{
			MethodParameterData->Name = "Arg";
			MethodParameterData->Name.Append(ParameterCount);
		}
		MethodParameterData->Type = ParameterType;

		if(ParameterType.BaseType == ZEMC_BT_ENUMERATOR)
		{
			for(ZESize I = 0; I < Context->Enumurators.GetCount(); I++)
			{
				if(Context->Enumurators[I]->Items.GetCount() > 0 && ParameterType.EnumName == Context->Enumurators[I]->Name)
				{
					MethodParameterData->Type.Enumurator = Context->Enumurators[I];
					break;
				}
			}
		}

		MethodData->Parameters.Add(MethodParameterData);

		ParameterCount++;
	}

	ZEMCAttribute* AttributeData = new ZEMCAttribute();
	for(CXXRecordDecl::attr_iterator CurrentAttr = Method->attr_begin(), LastAttr = Method->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		MethodData->Attributes.Add(AttributeData);
	}

	if (Error)
		return;
	else
		ClassData->Methods.Add(MethodData);
}

class ZEMetaCompilerASTConsumer : public clang::ASTConsumer
{
	private:
		ZEMCParser* Parser;

	public:
		virtual void Initialize(ASTContext &Context) 
		{

		}

		virtual bool HandleTopLevelDecl(DeclGroupRef DR) 
		{
			for (DeclGroupRef::iterator Iterator = DR.begin(); Iterator != DR.end(); Iterator++)
			{
				Parser->ProcessDeclaration(*Iterator);
			}

			return true;
		}

		virtual void HandleTranslationUnit(ASTContext& Ctx)
		{

		}

		ZEMetaCompilerASTConsumer(ZEMCParser* parser, CompilerInstance& CI)
		{
			Parser = parser;
			parser->Compiler = &CI;
		}
};

class ZEMetaCompilerFrontendAction : public clang::ASTFrontendAction
{
	private:
		ZEMCParser* Parser;

	public:
		virtual clang::ASTConsumer* CreateASTConsumer(CompilerInstance& CI, StringRef InFile)
		{
			return new ZEMetaCompilerASTConsumer(Parser, CI);
		}

		ZEMetaCompilerFrontendAction(ZEMCParser* parser)
		{
			Parser = parser;
		}
};

void ZEMCParser::SetOptions(ZEMCOptions* options)
{
	Options = options;
}

void ZEMCParser::SetMetaContext(ZEMCContext* context)
{
	Context = context;
}

bool ZEMCParser::Parse()
{
	std::vector<std::string> Arguments;
	//Arguments.push_back("ZEMetaCompiler.exe");
	//Arguments.push_back(Options.InputFileName);
	
	//Arguments.push_back("--");
	//Arguments.push_back("clang++");
	Arguments.push_back("-fms-extensions");
	Arguments.push_back("-fms-compatibility");
	Arguments.push_back("-x");	Arguments.push_back("c++");
	Arguments.push_back("-fsyntax-only");
	Arguments.push_back("-w");
	//Arguments.push_back("-std=c++11");

	for (int I = 0; I < Options->IncludeDirectories.GetCount(); I++)
	{
		Arguments.push_back(ZEFormat::Format("-I{0}", Options->IncludeDirectories[I]));
	}

	for (int I = 0; I < Options->Definitions.GetCount(); I++)
	{
		Arguments.push_back(ZEFormat::Format("-D{0}", Options->Definitions[I]));
	}	
	
	const char* argv[1024];
	int argc = Arguments.size();
	for (int I = 0; I < argc; I++)
	{
		argv[I] = Arguments[I].c_str();
	}
	
	std::ifstream sourceFile(Options->InputFileName.ToCString());
	std::string sourceCode((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());

	bool Result = clang::tooling::runToolOnCodeWithArgs(
		new ZEMetaCompilerFrontendAction(this), 
		sourceCode,
		Arguments, 
		Options->InputFileName.ToCString());

	if (!Result)
		return false;
}
