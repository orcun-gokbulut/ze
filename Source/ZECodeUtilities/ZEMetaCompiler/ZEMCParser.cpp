//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParser.cpp
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
#include "ZEPointer/ZEPointer.h"

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

void ZEMCParser::ProcessDeclaration(Decl* BaseDeclaration)
{
	if (BaseDeclaration == NULL)
		return;

	if (ProcessCommands(BaseDeclaration))
		return;

	if(isa<EnumDecl>(BaseDeclaration))
	{
		ProcessEnumerator(dyn_cast<EnumDecl>(BaseDeclaration));
		return;
	}

	if (isa<CXXRecordDecl>(BaseDeclaration))
	{
		ProcessClass(dyn_cast<CXXRecordDecl>(BaseDeclaration));
		return;
	}
}

bool ZEMCParser::ProcessCommands(Decl* Declaration)
{
	ZEMCAttribute AttributeData;
	for (RecordDecl::attr_iterator CurrentAttr = Declaration->attr_begin(), LastAttr = Declaration->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{

		if (!AnnotateAttr::classof(*CurrentAttr))
			continue;

		ParseAttribute(AttributeData, static_cast<AnnotateAttr*>(*CurrentAttr));

		if (AttributeData.Name == "ZEMC.ForwardDeclare")
		{
			bool Found = false;
			for (ZESize I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
			{
				if (Context->ForwardDeclarations[I]->Name == AttributeData.Values[0])
				{
					Found = true;
					break;
				}
			}

			if (Found)
				continue;

			ZEMCForwardDeclaration* ForwardDeclaredClass = new ZEMCForwardDeclaration();
			if (isa<EnumDecl>(Declaration))
				ForwardDeclaredClass->Type = ZEMC_DT_ENUMERATOR;
			else if (isa<CXXRecordDecl>(Declaration))
				ForwardDeclaredClass->Type = ZEMC_DT_CLASS;

			ForwardDeclaredClass->Name = AttributeData.Values[0].ToCString();
			Context->ForwardDeclarations.Add(ForwardDeclaredClass);

			return true;
		}
		else if (AttributeData.Name == "ZEMC.Include")
		{
			bool Found = false;
			for (ZESize I = 0; I < Context->Includes.GetCount(); I++)
			{
				if (Context->Includes[I]->HeaderFileName == AttributeData.Values[0])
				{
					Found = true;
					break;
				}
			}

			if (Found)
				continue;

			ZEMCInclude* Include = new ZEMCInclude();
			Include->HeaderFileName = AttributeData.Values[0].ToCString();
			Context->Includes.Add(Include);

			return true;
		}
	}

	return false;
}

void ZEMCParser::ProcessEnumerator(EnumDecl* EnumDeclaration)
{
	if (EnumDeclaration->getNameAsString().empty())
		return;

	if (!EnumDeclaration->isCompleteDefinition())
		return;

	ZEPointer<ZEMCEnumerator> Enumerator = new ZEMCEnumerator();
	Enumerator->Name = EnumDeclaration->getNameAsString();
	Enumerator->MetaName = Enumerator->Name + "Enumerator";

	ParseAttributes(Enumerator, EnumDeclaration);
	Enumerator->NormalizeAttributeStack();

	if (!Enumerator->CheckAttribute("ZEMC.Enumerator"))
		return;

	Enumerator->Hash = Enumerator->Name.Hash();

	for (EnumDecl::enumerator_iterator Current = EnumDeclaration->enumerator_begin(), End = EnumDeclaration->enumerator_end(); Current != End; ++Current)
	{
		ZEMCEnumeratorItem EnumeratorItem;
		EnumeratorItem.Name = Current->getNameAsString().c_str();
		EnumeratorItem.Value = *Current->getInitVal().getRawData();
		Enumerator->Items.Add(EnumeratorItem);
	}

	Context->Enumerators.Add(Enumerator.Transfer());

	std::string output;
	llvm::raw_string_ostream stream(output);
	EnumDeclaration->getLocation().print(stream, Compiler->getSourceManager());

	if (CheckTargetDeclaration(EnumDeclaration))
		Context->TargetEnumerators.Add(Context->Enumerators.GetLastItem());
}

void ZEMCParser::RemoveMetaCompilerAttributes(ZEMCClass* Class)
{
	for (ZESize I = 0; I < Class->Attributes.GetCount(); I++)
	{
		if (Class->Attributes[I].Name.Left(5) == "ZEMC.")
		{
			Class->Attributes.Remove(I);
			I--;
		}
	}

	for (ZESize N = 0; N < Class->Properties.GetCount(); N++)
	{
		ZEMCProperty* Property = Class->Properties[N];
		for (ZESize I = 0; I < Property->Attributes.GetCount(); I++)
		{
			if (Property->Attributes[I].Name.Left(5) == "ZEMC.")
			{
				Property->Attributes.Remove(I);
				I--;
			}
		}
	}

	for (ZESize N = 0; N < Class->Methods.GetCount(); N++)
	{
		ZEMCMethod* Method = Class->Methods[N];
		for (ZESize I = 0; I < Method->Attributes.GetCount(); I++)
		{
			if (Method->Attributes[I].Name.Left(5) == "ZEMC.")
			{
				Method->Attributes.Remove(I);
				I--;
			}
		}
	}
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

	Arguments.push_back("-fms-extensions");
	Arguments.push_back("-fms-compatibility");
	Arguments.push_back("-x");	Arguments.push_back("c++");
	Arguments.push_back("-fsyntax-only");
	Arguments.push_back("-w");
	Arguments.push_back("-DZE_META_COMPILER");

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

	return true;
}
