//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserMethod.cpp
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
#include "ZEPointer\ZEPointer.h"
#include "llvm\Support\raw_ostream.h"

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
		case OO_Exclaim:
			return ZEMC_MOT_LOGICAL_NOT;
		case OO_Call:
			return ZEMC_MOT_FUNCTION_CALL;
		case OO_Subscript:
			return ZEMC_MOT_ARRAY_SUBSCRIPT;
		default:
			return ZEMC_MOT_NONE;
	}
}

bool ZEMCParser::ProcessMethodParameters(ZEMCMethod* Method, CXXMethodDecl* MethodDecl)
{
	for(clang::FunctionDecl::param_iterator CurrentParameter = MethodDecl->param_begin(); CurrentParameter != MethodDecl->param_end(); CurrentParameter++)
	{
		ZEMCType ParameterType;
		if (!ProcessType(ParameterType, (*CurrentParameter)->getType()))
			return false;

		ZEMCMethodParameter MethodParameter;
		MethodParameter.Name = (*CurrentParameter)->getNameAsString();
		MethodParameter.Type = ParameterType;

		Method->Parameters.Add(MethodParameter);
	}

	return true;
}

void ZEMCParser::CheckNonPublicDefaultContructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (isa<CXXConstructorDecl>(MethodDecl))
	{
		CXXConstructorDecl* ConstructorDecl = cast<CXXConstructorDecl>(MethodDecl);
		if (ConstructorDecl->param_size() == 0)
			Class->HasPublicDefaultConstructor = (ConstructorDecl->getAccess() == AccessSpecifier::AS_public);
	}
}

void ZEMCParser::CheckNonPublicDefaultCopyContructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (isa<CXXConstructorDecl>(MethodDecl))
	{
		CXXConstructorDecl* ConstructorDecl = cast<CXXConstructorDecl>(MethodDecl);
		if (ConstructorDecl->isCopyConstructor())
			Class->HasPublicCopyConstructor = (MethodDecl->getAccess() == AccessSpecifier::AS_public);
	}
}

void ZEMCParser::CheckNonPublicDefaultAssignmentOperator(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (MethodDecl->isCopyAssignmentOperator())
		Class->HasPublicAssignmentOperator = (MethodDecl->getAccess() == AccessSpecifier::AS_public);
}

void ZEMCParser::CheckNonPublicDestructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (isa<CXXDestructorDecl>(MethodDecl))
		Class->HasPublicDestructor = (MethodDecl->getAccess() == AccessSpecifier::AS_public);
}

void ZEMCParser::CheckCreateInstanceMethod(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (MethodDecl->getAccess() != AccessSpecifier::AS_public)
		return;

	if (MethodDecl->getNameAsString() != "CreateInstance")
		return;
	
	if (!MethodDecl->isStatic())
		return;

	if (MethodDecl->param_size() != 0)
		return;

	QualType ReturnType = MethodDecl->getReturnType();
	if (!ReturnType->isPointerType() ||
		ReturnType.isConstQualified() ||
		!ReturnType->getPointeeType().getTypePtr()->isClassType() ||
		ReturnType->getPointeeType().getTypePtr()->getAsCXXRecordDecl()->getNameAsString() != Class->Name.ToCString())
	{
		return;
	}

	Class->HasCreateInstanceMethod = true;
}

void ZEMCParser::CheckDestroyMethod(ZEMCClass* Class, CXXMethodDecl* MethodDecl)
{
	if (MethodDecl->getAccess() != AccessSpecifier::AS_public)
		return;

	if (MethodDecl->getNameAsString() != "Destroy")
		return;

	if (!MethodDecl->isVirtual())
		return;

	if (MethodDecl->param_size() != 0)
		return;

	QualType ReturnType = MethodDecl->getReturnType();
	if (!ReturnType->isVoidType())
		return;

	Class->HasPublicDestroyMethod = true;
}

void ZEMCParser::ProcessMethod(ZEMCClass* ClassData, CXXMethodDecl* MethodDecl)
{
	//zeBreak(ClassData->Name == "ZETerrain");
	if (!MethodDecl->getType()->isFunctionType())
		return;

	// C++11 Not Supported
	if (MethodDecl->isMoveAssignmentOperator())
		return;

	CheckNonPublicDefaultContructor(ClassData, MethodDecl);
	CheckNonPublicDefaultCopyContructor(ClassData, MethodDecl);
	CheckNonPublicDefaultAssignmentOperator(ClassData, MethodDecl);
	CheckNonPublicDestructor(ClassData, MethodDecl);
	CheckCreateInstanceMethod(ClassData, MethodDecl);
	CheckDestroyMethod(ClassData, MethodDecl);

	if (MethodDecl->getAccess() != AccessSpecifier::AS_public)
		return;

	ZEMCType ReturnType;
	if (!ProcessType(ReturnType, MethodDecl->getCallResultType()))
		return;

	ZEPointer<ZEMCMethod> MethodData = new ZEMCMethod();
	MethodData->Class = ClassData;
	MethodData->Name = MethodDecl->getNameAsString();
	MethodData->IsConst = MethodDecl->isConst();
	MethodData->IsVirtual = MethodDecl->isVirtual();
	MethodData->IsPure = MethodDecl->isPure();
	MethodData->IsOperator = MethodDecl->isOverloadedOperator();
	MethodData->IsStatic = MethodDecl->isStatic();
	MethodData->IsEvent = false;
	MethodData->Hash = MethodData->Name.Hash();
	MethodData->ReturnValue = ReturnType;

	ParseAttributes(MethodData, MethodDecl);

	if (MethodDecl->isCopyAssignmentOperator())
	{
		MethodData->IsOperator = true;
		MethodData->OperatorType = ZEMC_MOT_ASSIGNMENT;
		
		if (MethodData->ReturnValue.TypeQualifier == ZEMC_TQ_VALUE)
			MethodData->ReturnValue.TypeQualifier = ZEMC_TQ_REFERENCE;
		else if (MethodData->ReturnValue.TypeQualifier == ZEMC_TQ_CONST_VALUE)
			MethodData->ReturnValue.TypeQualifier = ZEMC_TQ_CONST_REFERENCE;
	}

	if (isa<CXXConstructorDecl>(MethodDecl))
	{
		if (ClassData->IsAbstract)
			return;

		if (MethodDecl->param_size() == 0)
			ClassData->HasPublicDefaultConstructor = true;

		MethodData->IsConstructor = true;
	}
			
	if (MethodData->IsOperator)
		MethodData->OperatorType = GetOperatorType(MethodDecl->getOverloadedOperator());

	if (MethodData->IsOperator || MethodData->IsConstructor || isa<CXXDestructorDecl>(MethodDecl))
		return;
	
	if (!ProcessMethodParameters(MethodData, MethodDecl))
		return;

	ClassData->Methods.Add(MethodData.Transfer());
}

bool ZEMCParser::ProcessEvenParameters(ZEMCMethod* Method, CXXRecordDecl* EventTemplate)
{
	for(CXXRecordDecl::decl_iterator CurrentDecl = EventTemplate->decls_begin(); CurrentDecl != EventTemplate->decls_end(); CurrentDecl++)
	{
		if(CurrentDecl->getKind() != Decl::Kind::CXXMethod)
			continue;

		CXXMethodDecl* CurrentMethod = (CXXMethodDecl*)*CurrentDecl;
		if(CurrentMethod->getNameAsString() != "Call")
			continue;

		if (!ProcessType(Method->ReturnValue, CurrentMethod->getCallResultType()))
			return false;

		for(FunctionDecl::param_iterator CurrentParameter = CurrentMethod->param_begin();CurrentParameter != CurrentMethod->param_end(); CurrentParameter++)
		{
			ZEMCType Type;
			if (!ProcessType(Type, (*CurrentParameter)->getType()))
				return false;

			ZEMCMethodParameter MethodParameter;
			MethodParameter.Name = (*CurrentParameter)->getNameAsString();
			MethodParameter.Type = Type;
			Method->Parameters.Add(MethodParameter);
		}
	}

	return true;
}

void ZEMCParser::ProcessEvent(ZEMCClass* ClassData, DeclaratorDecl* EventDeclaration)
{
	if(!EventDeclaration->getType()->isClassType() || EventDeclaration->getType()->getAsCXXRecordDecl()->getNameAsString() != "ZEEvent")
		return;

	ZEPointer<ZEMCMethod> MethodData = new ZEMCMethod();
	MethodData->Class = ClassData;
	MethodData->Name = EventDeclaration->getName();
	MethodData->Hash = MethodData->Name.Hash();
	MethodData->IsEvent = true;
	MethodData->IsStatic = llvm::cast<FieldDecl>(EventDeclaration) == NULL;


	if (!ProcessEvenParameters(MethodData, EventDeclaration->getType()->getAsCXXRecordDecl()))
		return;

	ClassData->Methods.Add(MethodData.Transfer());
}
