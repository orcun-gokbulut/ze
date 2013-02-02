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

#include "ZEMetaProcessorInternal.h"

#include <cstdio>
#include <string>
#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Driver/Util.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/system_error.h"

#include "ZEMetaGenerator.h"

using namespace clang::driver;
using namespace clang;
using namespace std;

class ZECodeGeneratorASTConsumer : public ASTConsumer
{
	public:
		virtual bool HandleTopLevelDecl(DeclGroupRef DR) 
		{
			for (DeclGroupRef::iterator Iterator = DR.begin(); Iterator != DR.end(); Iterator++)
			{
				ZEMetaProcessorInternal::ProcessDeclaration(*Iterator);
			}

			return true;
		}
};

void ZEMetaProcessorInternal::InitializeClang()
{
	const char* Arguments[] =
	{
		"-cc1",
		
		"-I", ".",
		"-I", "..",
		"-I", "../ZEFoundation",

		"-v",
		"-disable-free",
		
		"-fmath-errno",
		"-ferror-limit", "19",
		"-fmessage-length", "150",

		"-fms-extensions",
		"-fms-compatibility",
		"-fmsc-version=1600",
		"-fdelayed-template-parsing",
		
		"-fno-rtti",
		"-fcolor-diagnostics",
		
		"-x", "c++",
		//"-std=", "c++11", 
		"-fsyntax-only",
	};

	size_t ArgumentCount = sizeof(Arguments) / sizeof(const char*);

	CompilerInstance Compiler;
	ZEMetaProcessorInternal::Compiler = &Compiler;
	Compiler.createDiagnostics(ArgumentCount, Arguments);

	CompilerInvocation Invocation;
	bool Result = CompilerInvocation::CreateFromArgs(Invocation, Arguments, Arguments + ArgumentCount, Compiler.getDiagnostics());
	Compiler.setInvocation(&Invocation);

	if (Invocation.getHeaderSearchOpts().UseBuiltinIncludes && Invocation.getHeaderSearchOpts().ResourceDir.empty())
	{
		llvm::sys::Path BinaryDir = llvm::sys::Path::GetMainExecutable(Options.BinaryPath, ZEMetaProcessorInternal::InitializeClang);
		BinaryDir.eraseComponent();
		Invocation.getHeaderSearchOpts().ResourceDir = BinaryDir.str();
	}
	
	std::vector<std::string> SystemDirs = clang::driver::GetWindowsSystemIncludeDirs();
	for(size_t i = 0; i < SystemDirs.size(); ++i) 
		Invocation.getHeaderSearchOpts().AddPath(SystemDirs[i], frontend::System, false, false, true, true);
	Invocation.getHeaderSearchOpts().AddPath(Invocation.getHeaderSearchOpts().ResourceDir + "/../../../../Source/ZEFoundation", clang::frontend::System, false, false, true, true);
	Invocation.getHeaderSearchOpts().AddPath(Invocation.getHeaderSearchOpts().ResourceDir + "/../../../../Source/ZEEngine", clang::frontend::System, false, false, true, true);

	#ifdef ZE_PLATFORM_WINDOWS
		Invocation.getPreprocessorOpts().addMacroDef("WIN32");
		Invocation.getPreprocessorOpts().addMacroDef("_WINDOWS");
		Invocation.getPreprocessorOpts().addMacroDef("NDEBUG");
	#endif
	
	Invocation.getPreprocessorOpts().addMacroDef("ZE_META_COMPILER");

	TargetOptions TO;
	TO.Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo* TI = TargetInfo::CreateTargetInfo(Compiler.getDiagnostics(), TO);
	TI->setCXXABI(TargetCXXABI::CXXABI_Microsoft);
	Compiler.setTarget(TI);

	Compiler.createDiagnostics(ArgumentCount, Arguments);
	Compiler.createFileManager();
	Compiler.createSourceManager(Compiler.getFileManager());
	Compiler.createPreprocessor();
	Compiler.setASTConsumer(new ZECodeGeneratorASTConsumer());
	Compiler.createASTContext();

	const FileEntry *pFile = Compiler.getFileManager().getFile(Options.InputFileName.ToCString());
	Compiler.getSourceManager().createMainFileID(pFile);
	Compiler.getDiagnosticClient().BeginSourceFile(Compiler.getLangOpts(), &Compiler.getPreprocessor());

	ParseAST(Compiler.getPreprocessor(), &Compiler.getASTConsumer(), Compiler.getASTContext(), false, clang::TranslationUnitKind::TU_Complete, NULL, true);

	ZEMetaGenerator::Generate(Options, MetaData);

	//Compiler.getInvocation().Release();
	Compiler.getDiagnosticClient().EndSourceFile();
}
