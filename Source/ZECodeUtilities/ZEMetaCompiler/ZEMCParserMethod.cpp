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
#include "ZEDS\ZEPointer.h"

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

void ZEMCParser::ProcessMethod(ZEMCClass* ClassData, CXXMethodDecl* MethodDecl)
{
	//zeBreak(ClassData->Name == "ZETerrain");
	if (!MethodDecl->getType()->isFunctionType())
		return;

	// C++11 Not Supported
	if (MethodDecl->isMoveAssignmentOperator())
		return;

	if (MethodDecl->getAccess() != AccessSpecifier::AS_public)
	{
		if (isa<CXXDestructorDecl>(MethodDecl))
			ClassData->HasPublicDestructor = false;

		if (isa<CXXConstructorDecl>(MethodDecl))
		{
			CXXConstructorDecl* ConstructorDecl = cast<CXXConstructorDecl>(MethodDecl);
			if (ConstructorDecl->isCopyConstructor())
				ClassData->HasPublicCopyConstructor = false;
		}

		if (isa<CXXConstructorDecl>(MethodDecl))
		{
			if (MethodDecl->param_size() == 0)
				ClassData->HasPublicDefaultConstructor = false;
		}

		return;
	}

	ZEMCType ReturnType;
	if (!ProcessType(ReturnType, MethodDecl->getCallResultType()))
		return;
		
	ZEPointer<ZEMCMethod> Method = new ZEMCMethod();
	Method->Name = MethodDecl->getNameAsString();
	Method->IsConst = MethodDecl->isConst();
	Method->IsVirtual = MethodDecl->isVirtual();
	Method->IsPure = MethodDecl->isPure();
	Method->IsOperator = MethodDecl->isOverloadedOperator();
	Method->IsStatic = MethodDecl->isStatic();
	Method->IsEvent = false;
	Method->Hash = Method->Name.Hash();
	Method->ReturnValue = ReturnType;

	ParseAttributes(Method, MethodDecl);

	if (MethodDecl->isCopyAssignmentOperator())
	{
		Method->IsOperator = true;
		Method->OperatorType = ZEMC_MOT_ASSIGNMENT;
		
		if (Method->ReturnValue.TypeQualifier == ZEMC_TQ_VALUE)
			Method->ReturnValue.TypeQualifier = ZEMC_TQ_REFERENCE;
		else if (Method->ReturnValue.TypeQualifier == ZEMC_TQ_CONST_VALUE)
			Method->ReturnValue.TypeQualifier = ZEMC_TQ_CONST_REFERENCE;
	}

	if (isa<CXXConstructorDecl>(MethodDecl))
	{
		if (MethodDecl->param_size() == 0)
			ClassData->HasPublicDefaultConstructor = true;

		Method->IsConstructor = true;
	}

	if(MethodDecl->getNameAsString() == "CreateInstance" && MethodDecl->param_size() == 0)
		ClassData->HasCreateInstanceMethod = true;

	if (Method->IsOperator)
		Method->OperatorType = GetOperatorType(MethodDecl->getOverloadedOperator());

	if (!ProcessMethodParameters(Method, MethodDecl))
		return;

	ClassData->Methods.Add(Method.Transfer());
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

	ZEPointer<ZEMCMethod> Method = new ZEMCMethod();
	Method->Name = EventDeclaration->getName();
	Method->Hash = Method->Name.Hash();
	Method->IsEvent = true;
	Method->IsStatic = llvm::cast<FieldDecl>(EventDeclaration) == NULL;


	if (!ProcessEvenParameters(Method, EventDeclaration->getType()->getAsCXXRecordDecl()))
		return;

	ClassData->Methods.Add(Method.Transfer());
}
