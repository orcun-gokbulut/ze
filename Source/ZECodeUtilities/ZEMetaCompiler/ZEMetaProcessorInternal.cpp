//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaProcessorInternal.cpp
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

#include "ZEMetaProcessorInternal.h"
#include "clang/AST/DeclTemplate.h"

CompilerInstance* ZEMetaProcessorInternal::Compiler;
ZEMetaData*	ZEMetaProcessorInternal::MetaData;
ZEMetaCompilerOptions ZEMetaProcessorInternal::Options;

void ZEMetaProcessorInternal::RaiseNote(SourceLocation& Location, const char* WarningText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs().getPtr()->getCustomDiagID(DiagnosticIDs::Level::Note, WarningText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMetaProcessorInternal::RaiseWarning(SourceLocation& Location, const char* WarningText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs().getPtr()->getCustomDiagID(DiagnosticIDs::Level::Warning, WarningText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMetaProcessorInternal::RaiseError(SourceLocation& Location, const char* ErrorText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs().getPtr()->getCustomDiagID(DiagnosticIDs::Level::Error, ErrorText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
}

void ZEMetaProcessorInternal::RaiseCriticalError(SourceLocation& Location, const char* ErrorText)
{
	FullSourceLoc FullLocation(Location, Compiler->getSourceManager());
	unsigned Id = Compiler->getDiagnostics().getDiagnosticIDs().getPtr()->getCustomDiagID(DiagnosticIDs::Level::Error, ErrorText);
	DiagnosticBuilder B = Compiler->getDiagnostics().Report(FullLocation, Id);
	exit(EXIT_FAILURE);
}

bool ZEMetaProcessorInternal::CheckClassHasDerivedFromZEObject(CXXRecordDecl* Class)
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

bool ZEMetaProcessorInternal::CheckClassHasZEObjectMacro(CXXRecordDecl* Class)
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

		QualType ReturnType = Iterator->getResultType().getCanonicalType();
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

bool ZEMetaProcessorInternal::CheckClass(CXXRecordDecl* Class)
{
	if (!CheckClassHasDerivedFromZEObject(Class))
		return false;

	if (Class->isTemplateDecl())
		RaiseError(Class->getLocation(), "ZEObject based templates are not supported.");

	RaiseNote(Class->getLocation(), "ZEObject based class has found.");
	
	if (!CheckClassHasZEObjectMacro(Class))
		return false;

	RaiseNote(Class->getLocation(), "ZEObject based class has been checked.");

	return true;
}

ZEMetaType ProcessInnerType(ZEString MainClassName, const Type* ClangType)
{
	ZEMetaType TempType;
	if (ClangType->isBuiltinType())
	{
		switch(((BuiltinType*)ClangType)->getKind())
		{
		case BuiltinType::Char_U:
		case BuiltinType::UChar:
			TempType.Type = ZE_MTT_UNSIGNED_INTEGER_8;
			break;

		case BuiltinType::UShort:
			TempType.Type = ZE_MTT_UNSIGNED_INTEGER_16;
			break;

		case BuiltinType::UInt:
			TempType.Type = ZE_MTT_UNSIGNED_INTEGER_32;
			break;

		case BuiltinType::ULongLong:
			TempType.Type = ZE_MTT_UNSIGNED_INTEGER_64;
			break;

		case BuiltinType::Char_S:
		case BuiltinType::SChar:
			TempType.Type = ZE_MTT_INTEGER_8;
			break;

		case BuiltinType::Short:
			TempType.Type = ZE_MTT_INTEGER_16;
			break;

		case BuiltinType::Int:
			TempType.Type = ZE_MTT_INTEGER_32;
			break;

		case BuiltinType::LongLong:
			TempType.Type = ZE_MTT_UNSIGNED_INTEGER_64;
			break;

		case BuiltinType::Float:
			TempType.Type = ZE_MTT_FLOAT;
			break;

		case BuiltinType::Double:
			TempType.Type = ZE_MTT_DOUBLE;
			break;

		case BuiltinType::Bool:
			TempType.Type = ZE_MTT_BOOLEAN;
			break;

		case BuiltinType::Void:
			TempType.Type = ZE_MTT_NULL;
			break;

		default:
			TempType.Type = ZE_MTT_UNDEFINED;
		}
	}
	else if (ClangType->isClassType())
	{
		CXXRecordDecl* ClassPtr = ClangType->getAsCXXRecordDecl();
		if (ClassPtr->getName() == "ZEString")
		{
			TempType.Type = ZE_MTT_STRING;
			TempType.ClassData->Name = "ZEString";
		}
		else if (ClassPtr->getName() == "ZEArray")
		{
			TempType.Type = ZE_MTT_ARRAY;
			TempType.ClassData->Name = "ZEArray";
		}
		else if (ClassPtr->getName() == "ZEList")
		{
			TempType.Type = ZE_MTT_LIST;
			TempType.ClassData->Name = "ZEList";
		}
		else if (ClassPtr->getName() == "ZEVector2")
		{
			TempType.Type = ZE_MTT_VECTOR2;
			TempType.ClassData->Name = "ZEVector2";
		}
		else if (ClassPtr->getName() == "ZEVector3")
		{
			TempType.Type = ZE_MTT_VECTOR3;
			TempType.ClassData->Name = "ZEVector3";
		}
		else if (ClassPtr->getName() == "ZEVector4")
		{
			TempType.Type = ZE_MTT_VECTOR4;
			TempType.ClassData->Name = "ZEVector4";
		}
		else if (ClassPtr->getName() == "ZEQuaternion")
		{
			TempType.Type = ZE_MTT_QUATERNION;
			TempType.ClassData->Name = "ZEQuaternion";
		}
		else if (ClassPtr->getName() == "ZEMatrix3x3")
		{
			TempType.Type = ZE_MTT_MATRIX3X3;
			TempType.ClassData->Name = "ZEMatrix3x3";
		}
		else if (ClassPtr->getName() == "ZEMatrix4x4")
		{
			TempType.Type = ZE_MTT_MATRIX4X4;
			TempType.ClassData->Name = "ZEMatrix4x4";
		}
		else
		{
			TempType.Type = ZE_MTT_OBJECT_PTR;
			TempType.ClassData->Name = ClassPtr->getNameAsString();

			if(ClassPtr->isCompleteDefinition())
			{
				if(TempType.ClassData->Name == "ZEEvent")
				{
					TempType.Type = ZE_MTT_CLASS;
					return TempType;
				}
				if(ZEMetaProcessorInternal::CheckClass(ClassPtr))
					return TempType;
				else
					return ZEMetaType();
			}
			else
			{
				if(ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses.GetCount() > 0)
				{
					for(int I = ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses.GetCount() - 1; I >= 0; I--)
					{
						if(ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses[I]->ClassName == TempType.ClassData->Name)
						{
							ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses[I]->HeaderFileDeclaredIn = MainClassName;
							return TempType;
						}
					}
				}

				return ZEMetaType();
			}
		}
	}
	else if(ClangType->isEnumeralType())
	{
		TempType.Type = ZE_MTT_ENUMERATOR;
	}
	else
		TempType.Type = ZE_MTT_UNDEFINED;

	return TempType;
}

ZEMetaType ProcessType(ZEString MainClassName, QualType& ClangType)
{
	QualType CanonicalType = ClangType.getCanonicalType();
	const Type* TypePtr = ClangType.getTypePtr();

	ZEMetaType TempType;
	if (CanonicalType.getTypePtr()->isReferenceType())
	{
		const ReferenceType* ReferenceTypePtr = TypePtr->castAs<ReferenceType>();
		QualType ReferenceePtr = ReferenceTypePtr->getPointeeType();

		TempType = ProcessInnerType(MainClassName, ReferenceePtr.getTypePtr());
		if (TempType.Type == ZE_MTT_UNDEFINED)
			return ZEMetaType();

		TempType.TypeQualifier = (ReferenceePtr.isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE);

		if (TempType.Type == ZE_MTT_ARRAY)
		{
			const clang::TemplateSpecializationType* TemplateType = ClangType->getPointeeType()->getAs<TemplateSpecializationType>();

			if(TemplateType->getNumArgs() < 1)
				return ZEMetaType();

			TemplateArgument Argument = TemplateType->getArg(0);

			if(Argument.getAsType().getTypePtr()->isPointerType())
			{
				ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr()->getPointeeType().getTypePtr());
				TempType.SubType = SubTypeData.Type;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
				TempType.SubTypeClassName = SubTypeData.ClassData->Name;
				return TempType;
			}
			else
			{
				if(Argument.getAsType().getTypePtr()->isClassType())
					return ZEMetaType();

				ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr());
				TempType.SubType = SubTypeData.Type;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
				TempType.SubTypeClassName = SubTypeData.ClassData->Name;
				return TempType;
			}

			return TempType;
		}

		return TempType;
	}
	else if (CanonicalType.getTypePtr()->isPointerType())
	{
		if(CanonicalType->getPointeeType()->isBuiltinType())
			return ZEMetaType();

		if (CanonicalType->getPointeeType()->isClassType())
		{
			TempType.Type = ZE_MTT_OBJECT_PTR;
			TempType.TypeQualifier = ZE_MTQ_REFERENCE;
			TempType.ClassData->Name = CanonicalType.getBaseTypeIdentifier()->getName().str();

			CXXRecordDecl* ClassPtr = ClangType->getPointeeType().getTypePtr()->getAsCXXRecordDecl();
			
			if(ClassPtr->isCompleteDefinition())
			{
				for(CXXRecordDecl::base_class_iterator CurrentBaseClass = ClassPtr->bases_begin(), LastBaseClass = ClassPtr->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
				{
					CXXRecordDecl* BaseClassDecl = CurrentBaseClass->getType()->getAsCXXRecordDecl();
					TempType.ClassData->BaseClass->Name = BaseClassDecl->getNameAsString();

					const clang::TemplateSpecializationType* TemplateType = CurrentBaseClass->getType()->getAs<TemplateSpecializationType>();

					if(TemplateType != NULL)
					{
						if(TemplateType->getNumArgs() < 1)
							return ZEMetaType();

						TemplateArgument Argument = TemplateType->getArg(0);

						if(Argument.getAsType().getTypePtr()->isPointerType())
						{
							ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr()->getPointeeType().getTypePtr());
							TempType.SubType = SubTypeData.Type;
							TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
							TempType.SubTypeClassName = SubTypeData.ClassData->Name;
							return TempType;
						}
						else
						{
							if(Argument.getAsType().getTypePtr()->isClassType())
								return ZEMetaType();

							ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr());
							TempType.SubType = SubTypeData.Type;
							TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
							TempType.SubTypeClassName = SubTypeData.ClassData->Name;
							return TempType;
						}
					}
				}

				if(TempType.ClassData->Name == "ZEClass")
				{
					TempType.Type = ZE_MTT_CLASS;
					return TempType;
				}

				if (!ZEMetaProcessorInternal::CheckClass(CanonicalType->getPointeeType().getTypePtr()->getAsCXXRecordDecl()))
					return ZEMetaType();
			}
			else
			{
				if(ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses.GetCount() > 0)
				{
					for(int I = ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses.GetCount() - 1; I >= 0; I--)
					{
						if(ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses[I]->ClassName == TempType.ClassData->Name)
						{
							ZEMetaProcessorInternal::MetaData->ForwardDeclaredClasses[I]->HeaderFileDeclaredIn = MainClassName;
							return TempType;
						}
					}
				}

				return ZEMetaType();
			}
		}

		return TempType;
	}
	else
	{
		if (CanonicalType.isConstQualified())
			return ZEMetaType();

		TempType = ProcessInnerType(MainClassName, CanonicalType.getTypePtr());

		if (TempType.Type == ZE_MTT_ARRAY)
		{
			const clang::TemplateSpecializationType* TemplateType = ClangType->getAs<TemplateSpecializationType>();

			if(TemplateType->getNumArgs() < 1)
				return ZEMetaType();

			TemplateArgument Argument = TemplateType->getArg(0);

			if(Argument.getAsType().getTypePtr()->isPointerType())
			{
				ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr()->getPointeeType().getTypePtr());
				TempType.SubType = SubTypeData.Type;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
				TempType.SubTypeClassName = SubTypeData.ClassData->Name;
				return TempType;
			}
			else
			{
				if(Argument.getAsType().getTypePtr()->isClassType())
					return ZEMetaType();

				ZEMetaType SubTypeData = ProcessInnerType(MainClassName, Argument.getAsType().getTypePtr());
				TempType.SubType = SubTypeData.Type;
				TempType.SubTypeQualifier = Argument.getAsType().isConstQualified() ? ZE_MTQ_CONST_REFERENCE : ZE_MTQ_REFERENCE;
				TempType.SubTypeClassName = SubTypeData.ClassData->Name;
				return TempType;
			}

			return TempType;
		}
		else if (TempType.Type == ZE_MTT_LIST)
		{

		}
		else if (TempType.Type == ZE_MTT_ENUMERATOR)
		{
			TempType.EnumName = ClangType->getAs<EnumType>()->getDecl()->getNameAsString();

			EnumDecl* EnumDeclaration = ClangType->getAs<EnumType>()->getDecl();

			ZEEnumData* EnumData = new ZEEnumData();
			EnumData->Name = TempType.EnumName;

			for(EnumDecl::enumerator_iterator CurrentEnum = EnumDeclaration->enumerator_begin(), EnumEnd = EnumDeclaration->enumerator_end(); CurrentEnum != EnumEnd; ++CurrentEnum)
			{
				ZEEnumParameterData* EnumParameterData = new ZEEnumParameterData();
				EnumParameterData->Name = CurrentEnum->getNameAsString();
				EnumParameterData->Value = *CurrentEnum->getInitVal().getRawData();
				EnumData->Parameters.Add(EnumParameterData);
			}

			ZEMetaProcessorInternal::MetaData->EnumTypes.Add(EnumData);
		}

		return TempType;
	}
}

bool ZEMetaProcessorInternal::ParseAttribute(ZEAttributeData* Data, const AnnotateAttr* Attribute)
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
				ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Wrong identifier name.");
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
				ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Wrong identifier name.");
				return false;
			}
			else
			{
				if (ParameterTextIndex >= 1024)
				{
					ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Max parameter size reached.");
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
				ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Wrong parameter termination character.");
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
					ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Max parameter size reached.");
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
					ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Unknown string escape character.");
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
		ZEMetaProcessorInternal::RaiseError(Attribute->getLocation(), "Cannot parse attribute.");
		return false;
	}

	return true;
}

void ZEMetaProcessorInternal::ProcessDeclaration(Decl* BaseDeclaration)
{
	if(isa<EnumDecl>(BaseDeclaration) && BaseDeclaration)
		ProcessEnumerator(dyn_cast<EnumDecl>(BaseDeclaration));

	if (isa<CXXRecordDecl>(BaseDeclaration) && BaseDeclaration)
		ProcessClass(dyn_cast<CXXRecordDecl>(BaseDeclaration));
}

void ZEMetaProcessorInternal::ProcessEnumerator(EnumDecl* EnumDeclaration)
{
	ZEEnumData* EnumData = new ZEEnumData();
	EnumData->Name = EnumDeclaration->getNameAsString();

	if(EnumData->Name == NULL || EnumData->Name == "")
		return;

	EnumData->Hash = EnumData->Name.Hash();
	EnumData->BaseClass = NULL;

	for(EnumDecl::enumerator_iterator Current = EnumDeclaration->enumerator_begin(), End = EnumDeclaration->enumerator_end(); Current != End; ++Current)
	{
		ZEEnumParameterData* EnumParameterData = new ZEEnumParameterData();
		EnumParameterData->Name = Current->getNameAsString();
		EnumParameterData->Value = *Current->getInitVal().getRawData();
		EnumData->Parameters.Add(EnumParameterData);
	}

	MetaData->EnumTypes.Add(EnumData);
}

void ZEMetaProcessorInternal::ProcessClass(CXXRecordDecl* Class)
{
	if(Class->isClass() && !Class->isCompleteDefinition() && Class->hasAttrs())
	{
		ZEAttributeData* AttributeData = new ZEAttributeData();
		for(CXXRecordDecl::attr_iterator CurrentAttr = Class->attr_begin(), LastAttr = Class->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
		{
			ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		}

		if(AttributeData->Name == "ForwardDeclaration")
		{
			ZEForwardDeclared* ForwardDeclaredClass = new ZEForwardDeclared();
			
			ForwardDeclaredClass->ClassName = AttributeData->Parameters[0].ToCString();
			ForwardDeclaredClass->HeaderName = AttributeData->Parameters[1].ToCString();

			MetaData->ForwardDeclaredClasses.Add(ForwardDeclaredClass);
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

	ZEClassData* ClassData = new ZEClassData();
	ClassData->Name = Class->getNameAsString();
	ClassData->Hash = ClassData->Name.Hash();
	ClassData->HasPublicConstructor = false;
	ClassData->IsAbstract = false;
	ClassData->IsBuiltInClass = IsBuiltInClassFound;

	for(CXXRecordDecl::base_class_iterator CurrentBaseClass = Class->bases_begin(), LastBaseClass = Class->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
	{
		ClassData->BaseClass = new ZEClassData();
		ClassData->BaseClass->Name = CurrentBaseClass->getType()->getAsCXXRecordDecl()->getNameAsString();
	}

	ZEAttributeData* AttributeData = new ZEAttributeData();
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
			ProcessMethod(ClassData, Method);
		}
		else if(isa<RecordDecl>(*Current))
		{
			RecordDecl* AnonymousRecord = ((RecordDecl*)*Current);
			ZEAttributeData* AnonymousRecordAttributeData = new ZEAttributeData();
			for(CXXRecordDecl::attr_iterator CurrentAttr = AnonymousRecord->attr_begin(), LastAttr = AnonymousRecord->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
			{
				ParseAttribute(AnonymousRecordAttributeData, ((AnnotateAttr*)(*CurrentAttr)));
				ClassData->Attributes.Add(AnonymousRecordAttributeData);
			}
		}
	}

	for(ZESize I = 0; I < ClassData->Events.GetCount(); I++)
	{
		ZEMethodData* Method = new ZEMethodData();
		Method->Name = ClassData->Events[I]->Name;
		Method->IsEvent = true;
		Method->IsStatic = ClassData->Events[I]->IsStatic;
		Method->Hash = Method->Name.Hash();
		Method->Attributes = ClassData->Events[I]->Attributes;
		
		ZEMethodParameterData* ReturnParameter = new ZEMethodParameterData();
		Method->ReturnParameter.Name = ClassData->Events[I]->ReturnParameter.Name;
		Method->ReturnParameter.BaseClass = ClassData->Events[I]->ReturnParameter.BaseClass;
		Method->ReturnParameter.EnumData = ClassData->Events[I]->ReturnParameter.EnumData;
		Method->ReturnParameter.Type = ClassData->Events[I]->ReturnParameter.Type;

		for(ZESize J = 0; J < ClassData->Events[I]->Parameters.GetCount(); J++)
		{
			ZEMethodParameterData* MethodParameter = new ZEMethodParameterData();
			MethodParameter->Name = ClassData->Events[I]->Parameters[J]->Name;
			MethodParameter->Type = ClassData->Events[I]->Parameters[J]->Type;
			MethodParameter->BaseClass = ClassData->Events[I]->Parameters[J]->BaseClass;
			MethodParameter->EnumData = ClassData->Events[I]->Parameters[J]->EnumData;

			Method->Parameters.Add(MethodParameter);
		}

		ClassData->Methods.Add(Method);
	}

	MetaData->Types.Add(ClassData);
	if (Compiler->getSourceManager().getFileID(Class->getLocation()) == Compiler->getSourceManager().getMainFileID())
		MetaData->TargetTypes.Add(ClassData);
}

void ZEMetaProcessorInternal::ProcessProperty(ZEClassData* ClassData, VarDecl* StaticProperty)
{
	if (StaticProperty->getAccess() != AccessSpecifier::AS_public)
		return;

	ZEMetaType PropertyType = ProcessType(ClassData->Name, StaticProperty->getType());
	if (PropertyType.Type == ZE_MTT_UNDEFINED)
		return;

	ZEPropertyData* PropertyData = new ZEPropertyData();
	PropertyData->Name = StaticProperty->getNameAsString();
	PropertyData->Hash = PropertyData->Name.Hash();
	PropertyData->IsStatic = true;
	PropertyData->IsContainer = false;
	PropertyData->Type = PropertyType;
	PropertyData->BaseClass = ClassData->BaseClass;

	if(PropertyType.ClassData->BaseClass != NULL && (PropertyType.ClassData->BaseClass->Name != NULL || PropertyType.ClassData->BaseClass->Name != ""))
	{
		if(PropertyType.ClassData->BaseClass->Name == "ZEContainer")
			PropertyData->IsContainer = true;
	}

	if(StaticProperty->getType()->isClassType() && StaticProperty->getType()->getAsCXXRecordDecl()->getNameAsString() == "ZEEvent")
	{
		bool Error = false;

		CXXRecordDecl* TemplateEvent = StaticProperty->getType()->getAsCXXRecordDecl();

		ZEEventData* Event = new ZEEventData();
		Event->Name = PropertyData->Name;
		Event->IsStatic = PropertyData->IsStatic;

		for(CXXRecordDecl::decl_iterator CurrentTemplateEventMethod = TemplateEvent->decls_begin(), LastTemplateEventMethod = TemplateEvent->decls_end(); 
			CurrentTemplateEventMethod != LastTemplateEventMethod; ++CurrentTemplateEventMethod)
		{
			clang::Decl::Kind TemplateEventKind = CurrentTemplateEventMethod->getKind();

			if(TemplateEventKind == Decl::Kind::CXXMethod)
			{
				CXXMethodDecl* TemplateEventMethodDecl = (CXXMethodDecl*)*CurrentTemplateEventMethod;

				Event->ReturnParameter.Type = ProcessType(ClassData->Name, TemplateEventMethodDecl->getResultType());

				if (Event->ReturnParameter.Type == ZE_MTT_UNDEFINED)
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

						if(TemplateEventParameterData->Type.Type == ZE_MTT_ENUMERATOR)
						{
							for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
							{
								if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && TemplateEventParameterData->Type.EnumName == MetaData->EnumTypes[I]->Name)
								{
									TemplateEventParameterData->EnumData = MetaData->EnumTypes[I];
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

		ZEAttributeData* AttributeData = new ZEAttributeData();
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

	if(PropertyType.Type == ZE_MTT_ENUMERATOR)
	{
		for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
		{
			if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && PropertyData->Type.EnumName == MetaData->EnumTypes[I]->Name)
			{
				PropertyData->EnumData = MetaData->EnumTypes[I];
				break;
			}
		}
	}

	ZEAttributeData* AttributeData = new ZEAttributeData();
	for(CXXRecordDecl::attr_iterator CurrentAttr = StaticProperty->attr_begin(), LastAttr = StaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		PropertyData->Attributes.Add(AttributeData);
	}

	ClassData->Properties.Add(PropertyData);
}

void ZEMetaProcessorInternal::ProcessProperty(ZEClassData* ClassData, FieldDecl* NonStaticProperty)
{
	if (NonStaticProperty->getAccess() != AccessSpecifier::AS_public)
		return;

	ZEMetaType PropertyType = ProcessType(ClassData->Name, NonStaticProperty->getType());
	if (PropertyType.Type == ZE_MTT_UNDEFINED)
		return;

	ZEPropertyData* PropertyData = new ZEPropertyData();
	PropertyData->Name = NonStaticProperty->getNameAsString();
	PropertyData->Hash = PropertyData->Name.Hash();
	PropertyData->IsGeneratedByMetaCompiler = false;
	PropertyData->IsStatic = false;
	PropertyData->IsContainer = false;
	PropertyData->Type = PropertyType;
	PropertyData->BaseClass = ClassData->BaseClass;

	if(PropertyType.ClassData->BaseClass != NULL && (PropertyType.ClassData->BaseClass->Name != NULL || PropertyType.ClassData->BaseClass->Name != ""))
	{
		if(PropertyType.ClassData->BaseClass->Name == "ZEContainer")
			PropertyData->IsContainer = true;
	}

	if(NonStaticProperty->getType()->isClassType() && NonStaticProperty->getType()->getAsCXXRecordDecl()->getNameAsString() == "ZEEvent")
	{
		bool Error = false;

		CXXRecordDecl* TemplateEvent = NonStaticProperty->getType()->getAsCXXRecordDecl();

		ZEEventData* Event = new ZEEventData();
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

				Event->ReturnParameter.Type = ProcessType(ClassData->Name, TemplateEventMethodDecl->getResultType());

				if (Event->ReturnParameter.Type.Type == ZE_MTT_UNDEFINED)
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

						if(TemplateEventParameterData->Type.Type == ZE_MTT_ENUMERATOR)
						{
							for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
							{
								if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && TemplateEventParameterData->Type.EnumName == MetaData->EnumTypes[I]->Name)
								{
									TemplateEventParameterData->EnumData = MetaData->EnumTypes[I];
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

		ZEAttributeData* AttributeData = new ZEAttributeData();
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

	if(PropertyType.Type == ZE_MTT_ENUMERATOR)
	{
		for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
		{
			if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && PropertyData->Type.EnumName == MetaData->EnumTypes[I]->Name)
			{
				PropertyData->EnumData = MetaData->EnumTypes[I];
				break;
			}
		}
	}

	ZEAttributeData* AttributeData = new ZEAttributeData();
	for(CXXRecordDecl::attr_iterator CurrentAttr = NonStaticProperty->attr_begin(), LastAttr = NonStaticProperty->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		PropertyData->Attributes.Add(AttributeData);
	}

	ClassData->Properties.Add(PropertyData);
}

void ZEMetaProcessorInternal::ProcessMethod(ZEClassData* ClassData, CXXMethodDecl* Method)
{
	if(Method->isPure())
		ClassData->IsAbstract = true;

	if(Method->getAccess() != AccessSpecifier::AS_public)
		return;

	if(!Method->getType()->isFunctionType())
		return;

	if(Method->getCanonicalDecl()->isCopyAssignmentOperator())
		return;

	if(ClassData->IsBuiltInClass)
	{
		if(!Method->isStatic())
			return;
	}

	if(isa<CXXDestructorDecl>(Method))
		return;

	if(isa<CXXConstructorDecl>(Method))
	{
		ClassData->HasPublicConstructor = true;
		return;
	}

	ZEMetaType ReturnType = ProcessType(ClassData->Name, Method->getResultType());
	if (ReturnType.Type == ZE_MTT_UNDEFINED)
		return;

	ZEString MethodName = Method->getNameAsString();

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
			ZEPropertyData* PropertyData = new ZEPropertyData();
			PropertyData->Name = PropertyName;
			PropertyData->Hash = PropertyName.Hash();
			PropertyData->IsGeneratedByMetaCompiler = true;
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
					ZEMetaType PropertyType = ProcessType(ClassData->Name, (*CurrentParameter)->getType());
					if (PropertyType.Type == ZE_MTT_UNDEFINED)
						return;

					PropertyData->Type = PropertyType;

					if(PropertyType.Type == ZE_MTT_ENUMERATOR)
					{
						for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
						{
							if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && PropertyType.EnumName == MetaData->EnumTypes[I]->Name)
							{
								PropertyData->EnumData = MetaData->EnumTypes[I];
								break;
							}
						}
					}
				}
			}
			else
			{
				ZEMetaType PropertyType = ProcessType(ClassData->Name, Method->getResultType());
				if (PropertyType.Type == ZE_MTT_UNDEFINED)
					return;

				if(PropertyType.Type == ZE_MTT_ENUMERATOR)
				{
					for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
					{
						if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && PropertyType.EnumName == MetaData->EnumTypes[I]->Name)
						{
							PropertyData->EnumData = MetaData->EnumTypes[I];
							break;
						}
					}
				}

				PropertyData->Type = PropertyType;
			}
			
			ZEAttributeData* AttributeData = new ZEAttributeData();
			for(CXXRecordDecl::attr_iterator CurrentAttr = Method->attr_begin(), LastAttr = Method->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
			{
				ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
				PropertyData->Attributes.Add(AttributeData);
			}

			ClassData->Properties.Add(PropertyData);

			return;
		}
		else
		{
			if(MethodName.SubString(0, 2) == "Set")
				ClassData->Properties[FoundPropertyIndex]->Setter = MethodName;
			else if(MethodName.SubString(0, 2) == "Get")
				ClassData->Properties[FoundPropertyIndex]->Getter = MethodName;

			return;
		}
	}

	ZEMethodData* MethodData = new ZEMethodData();
	MethodData->Name = MethodName;
	MethodData->IsStatic = Method->isStatic();
	MethodData->IsEvent = false;
	MethodData->Hash = MethodData->Name.Hash();
	MethodData->ReturnParameter.Type = ReturnType;

	if(MethodData->ReturnParameter.Type.Type == ZE_MTT_ENUMERATOR)
	{
		for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
		{
			if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && MethodData->ReturnParameter.Type.EnumName == MetaData->EnumTypes[I]->Name)
			{
				MethodData->ReturnParameter.EnumData = MetaData->EnumTypes[I];
				break;
			}
		}
	}

	bool Error = false;
	for(clang::FunctionDecl::param_iterator CurrentParameter = Method->param_begin(), LastParameter = Method->param_end(); CurrentParameter != LastParameter; ++CurrentParameter)
	{
		ZEMetaType ParameterType = ProcessType(ClassData->Name, (*CurrentParameter)->getType());
		if (ParameterType.Type == ZE_MTT_UNDEFINED)
		{
			Error = true;
			break;
		}

		ZEMethodParameterData* MethodParameterData = new ZEMethodParameterData();
		MethodParameterData->Name = (*CurrentParameter)->getNameAsString();
		MethodParameterData->Type = ParameterType;

		if(ParameterType.Type == ZE_MTT_ENUMERATOR)
		{
			for(ZESize I = 0; I < MetaData->EnumTypes.GetCount(); I++)
			{
				if(MetaData->EnumTypes[I]->Parameters.GetCount() > 0 && ParameterType.EnumName == MetaData->EnumTypes[I]->Name)
				{
					MethodParameterData->EnumData = MetaData->EnumTypes[I];
					break;
				}
			}
		}

		MethodData->Parameters.Add(MethodParameterData);
	}

	ZEAttributeData* AttributeData = new ZEAttributeData();
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
