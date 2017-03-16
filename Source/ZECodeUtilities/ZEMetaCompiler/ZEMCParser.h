//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParser.h
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
#ifndef __ZE_MC_PARSER_H__
#define __ZE_MC_PARSER_H__

#include "ZEMCOptions.h"
#include "ZEMCContext.h"

#include <clang/Frontend/CompilerInstance.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Attr.h>
#include "ZEMeta/ZEObject.h"

using namespace clang;

enum ZEMCAccessorType
{
	ZEMC_AT_SETTER,
	ZEMC_AT_GETTER,
	ZEMC_AT_ADDER,
	ZEMC_AT_REMOVER,
	ZEMC_AT_COUNTER
};

struct ZEMCAccessor
{
	ZEString							Name;
	ZEMCAccessorType					Type;
	ZEMCType							PropertyType;
	ZEMCMethod*							Method;
};

class ZEMCParser
{
	friend class ZEMetaCompilerASTConsumer;
	private:
		CompilerInstance*				Compiler;
		ZEMCContext*					Context;
		ZEMCOptions*					Options;

		// Error Management
		void							RaiseNote(SourceLocation& Location, const char* WarningText);
		void							RaiseWarning(SourceLocation& Location, const char* WarningText);
		void							RaiseError(SourceLocation& Location, const char* ErrorText);
		void							RaiseCriticalError(SourceLocation& Location, const char* ErrorText);

		// Checks
		bool							CheckClassHasDerivedFromZEObject(CXXRecordDecl* Class, bool& IsReferenceCounted);
		bool							CheckClassHasZEObjectMacro(CXXRecordDecl* Class);
		bool							CheckClass(CXXRecordDecl* Class, bool& IsReferenceCounted);
		bool							CheckFundamentalClass(CXXRecordDecl* Class);
		bool							CheckTargetDeclaration(Decl* Declaration);

		// Memory Management
		void							CheckPublicDefaultContructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl);
		void							CheckPublicCopyContructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl);
		void							CheckPublicAssignmentOperator(ZEMCClass* Class, CXXMethodDecl* MethodDecl);
		void							CheckPublicDestructor(ZEMCClass* Class, CXXMethodDecl* MethodDecl);
		void							CheckCreateInstanceMethod(ZEMCClass* Class, CXXMethodDecl* MethodDecl);
		void							CheckDestroyMethod(ZEMCClass* Class, CXXMethodDecl* MethodDecl);

		// Attributes
		bool							ParseAttribute(ZEMCAttribute& Attribute, const AnnotateAttr* ClangAttribute);
		void							ParseAttributes(ZEMCDeclaration* Decleration, Decl* ClangDecl);

		// Type
		ZEMCClass*						FindClass(const char* ClassName);
		ZEMCEnumerator*					FindEnumurator(const char* EnumName);
		bool							ProcessType(ZEMCType& Output, const QualType& ClangType);
		bool							ProcessBaseType(ZEMCType& Output, const Type* ClangType);
		bool							IsContainer();
		bool							IsEvent();

		// Processors
		ZEMCOperatorType				GetOperatorType(OverloadedOperatorKind OperatorKind);
		void							ProcessDeclaration(Decl* BaseDeclaration);
		bool							ProcessCommands(Decl* Declaration);
		void							ProcessEnumerator(EnumDecl* EnumDeclaration);
		void							ProcessClass(CXXRecordDecl* ClassDeclaration);
		void							ProcessClassAttributes(ZEMCClass* ClassData, CXXRecordDecl* ClassDeclaration);

		void							ProcessProperty(ZEMCClass* ClassData, DeclaratorDecl* PropertyDeclaration);
		void							ProcessMethod(ZEMCClass* ClassData, CXXMethodDecl* MethodDeclaration);
		bool							ProcessMethodParameters(ZEMCMethod* Method, CXXMethodDecl* MethodDecl);
		void							ProcessEvent(ZEMCClass* ClassData, DeclaratorDecl* EventDeclaration);
		bool							ProcessEvenParameters(ZEMCMethod* Method, CXXRecordDecl* EventTemplate);
		void							ProcessMemberAttributes(ZEMCDeclaration* Declaration, Decl* ClangDeclaration, bool IsMethod, ZEMCDeclaration* Overriden);
		
		// Accessors
		void							FilterPropertyAccessors(ZEMCClass* Class, ZEArray<ZEMCAccessor>& Accessors);
		void							ProcessPropertyAccessor(ZEArray<ZEMCAccessor>& Accessors, ZEMCMethod* MethodData);
		void							ProcessPropertyAccessors(ZEMCClass* ClassData);
		
		void							RemoveMetaCompilerAttributes(ZEMCClass* Class);

	public:
		void							SetOptions(ZEMCOptions* options);
		void							SetMetaContext(ZEMCContext* context);

		bool							Parse();
};

#endif
