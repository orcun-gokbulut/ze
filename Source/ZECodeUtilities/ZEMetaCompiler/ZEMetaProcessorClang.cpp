//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaProcessorClang.cpp
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

/*#include "ZEMetaProcessorInternal.h"

#include "ZEDS/ZEFormat.h"
#include "ZEMetaGenerator.h"

#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <clang/AST/ASTConsumer.h>
#include "ZEFile/ZEFileInfo.h"

class ZEMetaCompilerASTConsumer : public clang::ASTConsumer
{
	public:
		virtual void Initialize(ASTContext &Context) 
		{

		}

		virtual bool HandleTopLevelDecl(DeclGroupRef DR) 
		{
			for (DeclGroupRef::iterator Iterator = DR.begin(); Iterator != DR.end(); Iterator++)
			{
				ZEMetaCompilerParser::ProcessDeclaration(*Iterator);
			}

			return true;
		}

		virtual void HandleTranslationUnit(ASTContext &Ctx)
		{

		}
};

class ZEMetaCompilerFrontendAction : public clang::ASTFrontendAction
{
	public:
		virtual clang::ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef InFile)
		{
			ZEMetaCompilerParser::Compiler = &CI;
			return new ZEMetaCompilerASTConsumer();
		}
};

class ZEMetaCompilerFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
	public:
		virtual clang::FrontendAction *create()
		{
			return new ZEMetaCompilerFrontendAction();
		}
};

void ZEMetaCompilerParser::Parse()
{
	std::vector<std::string> Arguments;
	Arguments.push_back("ZEMetaCompiler.exe");
	Arguments.push_back(Options.InputFileName);
	
	Arguments.push_back("--");
	Arguments.push_back("-fms-extensions");
	Arguments.push_back("-fms-compatibility");
	Arguments.push_back("-x");	Arguments.push_back("c++");
	Arguments.push_back("-fsyntax-only");
	Arguments.push_back("-w");
	//Arguments.push_back("-std=c++11");

	for (int I = 0; I < Options.IncludeDirectories.GetCount(); I++)
	{
		Arguments.push_back(ZEFormat::Format("-I{0}", Options.IncludeDirectories[I]));
	}

	for (int I = 0; I < Options.Definitions.GetCount(); I++)
	{
		Arguments.push_back(ZEFormat::Format("-D{0}", Options.Definitions[I]));
	}	
	
	const char* argv[1024];
	int argc = Arguments.size();
	for (int I = 0; I < argc; I++)
	{
		argv[I] = Arguments[I].c_str();
	}

	clang::tooling::CommonOptionsParser OptionsParser(argc, argv);
	clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),	OptionsParser.getSourcePathList());
	int Result = Tool.run(new ZEMetaCompilerFrontendActionFactory());
	if (Result != EXIT_SUCCESS)
	{
		exit(EXIT_FAILURE);
	}

	if (!ZEMetaGenerator::Generate(Options, MetaData))
		exit(EXIT_FAILURE);

	if(Options.IsRegisterSession)
	{
		FILE* File;
		File = fopen(Options.RegisterFileName.ToCString(), "w");

		for(ZESize I = 0; I < MetaData->TargetTypes.GetCount(); I++)
		{
			fprintf(File, "%s,%s;", 
				MetaData->TargetTypes[I]->Name.ToCString(), 
				ZEFileInfo::GetFileName(Options.InputFileName).ToCString());
		}

		fclose(File);
	}

	exit(EXIT_SUCCESS);
}*/
	/*
#include <clang\AST\DeclCXX.h>
#include <clang\Tooling\Tooling.h>
#include <clang\AST\DeclBase.h>
#include <clang\Frontend\FrontendAction.h>
#include <clang\Frontend\CompilerInstance.h>
#include <clang\Basic\LLVM.h>
#include <clang\AST\ASTConsumer.h>

using namespace clang;
using namespace llvm;

bool IsInstantiatable(const CXXMethodDecl* Method)
{
	if (!isa<CXXConstructorDecl>(Method))
		return false;

	// For regular C++ member functions Decl::getAccess() works fine for regular member functions
	// however, in this case, if member function is constructor it allways returns AccessSpecifier::AS_public.
	if (Method->getAccess() == AccessSpecifier::AS_public)
		return true;

	return false;
}

void ProcessClass(CXXRecordDecl* Class)
{
	for(CXXRecordDecl::decl_iterator Current = Class->decls_begin(), End = Class->decls_end(); Current != End; ++Current)
	{
		if(isa<CXXMethodDecl>(*Current))
		{
			if (IsInstantiatable(cast<CXXMethodDecl>(*Current)))
			{
				printf("Class %s has a public constructor so it is instantiatable.", Class->getName());
				return;
			}
		}
	}

	printf("Class %s doesn't have a public constructor so it is not instantiatable.", Class->getName());
}

class TestASTConsumer : public ASTConsumer
{
	public:
		virtual bool HandleTopLevelDecl(DeclGroupRef DR) 
		{
			for (DeclGroupRef::iterator Iterator = DR.begin(); Iterator != DR.end(); Iterator++)
			{
				if (isa<CXXRecordDecl>(*Iterator))
				{
					ProcessClass(cast<CXXRecordDecl>(*Iterator));
				}
			}

			return true;
		}
};

class TestFrontendAction : public ASTFrontendAction
{
	public:
		virtual clang::ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef InFile)
		{
			return new TestASTConsumer();
		}
};

void main(int argc, char** argv)
{
	char* Code = 
		"class TestClass\n"
		"{\n"
		"	private:\n"
		"		// It's access type is identified as public. WRONG\n"
		"		\n"
		"		void DoPrivateStuff();\n"
		"\n"	
		"	public:\n"
		"TestClass();"
		"		void DoStuff();\n"
		"};\n";

		clang::tooling::runToolOnCode(
			new TestFrontendAction(), 
			Code);
}*/
