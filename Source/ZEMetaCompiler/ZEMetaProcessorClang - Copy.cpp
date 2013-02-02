//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaProcessorClang - Copy.cpp
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

#include "ZEMetaProcessor.h"

//-------------------------------------------------------------------------
//
// rewritersample.cpp: Source-to-source transformation sample with Clang,
// using Rewriter - the code rewriting interface.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//
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

using namespace clang::driver;
using namespace clang;
using namespace std;

class ZECodeGeneratorASTConsumer : public ASTConsumer
{
	public:
		CompilerInstance* Compiler;
		ZEMetaData* Data;

		enum CheckClassResult
		{
			ZE_CCR_OK,
			ZE_CCR_MISSING_MACRO,
			ZE_CCR_INHERITANCE_PATH_NOT_PUBLIC,
			ZE_CCR_MULTIPLE_ZEOBJECT_INHERITANCE


		};

/*		bool IsDerivedFromZEObject(CXXRecordDecl* ClassDeclaration)
		{	
			if (ClassDeclaration->getNameAsString() == "ZEObject")
				return true;

			for (CXXRecordDecl::base_class_const_iterator Iterator = ClassDeclaration->bases_begin(); Iterator != ClassDeclaration->bases_end(); Iterator++)
			{
				CXXRecordDecl* BaseClass = Iterator->getType().getTypePtr()->getAsCXXRecordDecl();
				if (IsDerivedFromObject(BaseClass))
					return true;
			}

			return false;
		}
		*/

		CheckClassResult CheckClass(CXXRecordDecl* ClassDeclaration)
		{
			// Potential Problems
			// - Multi ZEObject Inheritance
			// - ZEObject Inheritance path is not public.
			// - Missing ZE_OBJECT macro or methods.
			bool GetClassMethodFound = false;
			bool ClassFoundMethodFound = false;

			for (CXXRecordDecl::method_iterator Iterator = ClassDeclaration->method_begin(); Iterator != ClassDeclaration->method_end(); Iterator++)
			{/*
				if (Iterator->getNameAsString() == "Class")
				{
					QualType QualifiedReturnType = Iterator->getResultType();
					Type ReturnType = QualifiedReturnType->get;

					if (ReturnType.isConstQualified() || ReturnType.isVolatileQualified())
						continue;
					
					if (!ReturnType->isClassType() || ReturnType->getAsCXXRecordDecl()->getNameAsString() != "ZEObject")
						continue
				}*/
				Iterator->print(llvm::outs());
			}

			return ZE_CCR_OK;
		}

		void ProcessClass(CXXRecordDecl* ClassDeclaration)
		{
			/*if (!IsDerivedFromObject(ClassDeclaration))
				printf("Not Derived.");*/

			CheckClass(ClassDeclaration);
			printf("Record Decl. Name : %s.\n", ClassDeclaration->getNameAsString().c_str());
			ClassDeclaration->dump(llvm::outs());
			/*ClassDeclaration->field_begin();
			ClassDeclaration->method_begin();*/

		}

		void ProcessEnum(EnumDecl* EnumDeclaration)
		{
			printf("Enum Decl. Name : %s.\n", EnumDeclaration->getNameAsString().c_str());
		}

		void ProcessDecl(Decl* Declaration)
		{
			if (isa<CXXRecordDecl>(Declaration))
				printf("Name : %s. \n", dyn_cast<CXXRecordDecl>(Declaration)->getNameAsString().c_str());
			// Process only main file
			if (Declaration->getLocation().isMacroID() || Compiler->getSourceManager().getFileID(Declaration->getLocation()) != Compiler->getSourceManager().getMainFileID())
				return;

			if (isa<CXXRecordDecl>(Declaration))
				ProcessClass(dyn_cast<CXXRecordDecl>(Declaration));
			else if (isa<EnumDecl>(Declaration))
				ProcessEnum(dyn_cast<EnumDecl>(Declaration));
		}

		virtual bool HandleTopLevelDecl(DeclGroupRef DR) 
		{
			for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
				ProcessDecl(*b);

			return true;
		}

		ZECodeGeneratorASTConsumer(CompilerInstance* Compiler)
		{
			this->Compiler = Compiler;
		}
};

int main_compiler_instance(int argc, const char** argv)
{
	const char* Arguments[] =
	{
		"-cc1",
		
		"-I", ".",
		"-I", "..",
		"-I", "../ZEFoundation",

		"-v",
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
		"-std=c++0x", 
		"-fsyntax-only",
		"-nobuiltininc"
	};
	size_t ArgumentCount = sizeof(Arguments) / sizeof(const char*);

	CompilerInstance Compiler;
	Compiler.createDiagnostics(ArgumentCount, Arguments);

	CompilerInvocation Invocation;
	bool Result = CompilerInvocation::CreateFromArgs(Invocation, Arguments, Arguments + ArgumentCount, Compiler.getDiagnostics());
	Compiler.setInvocation(&Invocation);

	CompilerInvocation::setLangDefaults(*Invocation.getLangOpts(), InputKind::IK_CXX, LangStandard::Kind::lang_cxx11);
	Invocation.getLangOpts()->ShortWChar = 1;


	if (Invocation.getHeaderSearchOpts().UseBuiltinIncludes && Invocation.getHeaderSearchOpts().ResourceDir.empty())
	{

		llvm::sys::Path BinaryDir = llvm::sys::Path::GetMainExecutable(argv[0], main_compiler_instance);
		BinaryDir.eraseComponent();
		Invocation.getHeaderSearchOpts().ResourceDir = BinaryDir.str();
	}
	

	std::vector<std::string> SystemDirs = clang::driver::GetWindowsSystemIncludeDirs();
	for(size_t i = 0; i < SystemDirs.size(); ++i) 
		Invocation.getHeaderSearchOpts().AddPath(SystemDirs[i], frontend::System, false, false, true, true);
	Invocation.getHeaderSearchOpts().AddPath(Invocation.getHeaderSearchOpts().ResourceDir + "/include", clang::frontend::System, false, false, true, true);


	#ifdef ZE_PLATFORM_WINDOWS
		Invocation.getPreprocessorOpts().addMacroDef("WIN32");
		Invocation.getPreprocessorOpts().addMacroDef("_WINDOWS");
		Invocation.getPreprocessorOpts().addMacroDef("NDEBUG");
	#endif
	
	Invocation.getPreprocessorOpts().addMacroDef("ZE_META_COMPILER");

	/*Invocation.getPreprocessorOpts().addMacroDef("ZE_ZINEK");
	Invocation.getPreprocessorOpts().addMacroDef(ZEClas"ZE_VERSION_MAJOR="			+ ZEString(ZE_VERSION_MAJOR));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_VERSION_MINOR="			+ ZEString(ZE_VERSION_MINOR));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_VERSION_INTERNAL="			+ ZEString(ZE_VERSION_INTERNAL));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_VERSION_BUILD="			+ ZEString(ZE_VERSION_BUILD));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_"					+ ZEString(ZE_PLATFORM).Upper());
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_ARCHITECTURE_"	+ ZEString(ZE_PLATFORM_ARCHITECTURE).Upper());
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_COMPILER_"		+ ZEString(ZE_PLATFORM_COMPILER).Upper());
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_ENDIANNESS_"		+ ZEString(ZE_PLATFORM_ENDIANNESS).Upper());
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM="					+ ZEString(ZE_PLATFORM));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_ARCHITECTURE="	+ ZEString(ZE_PLATFORM_ARCHITECTURE));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_COMPILER="		+ ZEString(ZE_PLATFORM_COMPILER));
	Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_WORD_SIZE="		+ ZEString(ZE_PLATFORM_WORD_SIZE));
	#ifdef ZE_PLATFORM_ARCHITECTURE_SSE3
		Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_ARCHITECTURE_SSE3");
	#endif
	#ifdef ZE_PLATFORM_ARCHITECTURE_SSE4
		Invocation.getPreprocessorOpts().addMacroDef("ZE_PLATFORM_ARCHITECTURE_SSE3");
	#endif*/

	// Target Options
	TargetOptions TO;
	TO.Triple = llvm::sys::getDefaultTargetTriple();
	TargetInfo* TI = TargetInfo::CreateTargetInfo(Compiler.getDiagnostics(), TO);
	TI->setCXXABI(TargetCXXABI::CXXABI_Microsoft);
	Compiler.setTarget(TI);

	Compiler.createDiagnostics(ArgumentCount, Arguments);
	Compiler.createFileManager();
	Compiler.createSourceManager(Compiler.getFileManager());
	Compiler.createPreprocessor();
	Compiler.setASTConsumer(new ZECodeGeneratorASTConsumer(&Compiler));
	Compiler.createASTContext();

	const FileEntry *pFile = Compiler.getFileManager().getFile(argv[1]);
	Compiler.getSourceManager().createMainFileID(pFile);
	Compiler.getDiagnosticClient().BeginSourceFile(Compiler.getLangOpts(), &Compiler.getPreprocessor());

	ParseAST(Compiler.getPreprocessor(), &Compiler.getASTConsumer(), Compiler.getASTContext(), false, clang::TranslationUnitKind::TU_Complete, NULL, true);

	Compiler.getDiagnosticClient().EndSourceFile();

	return 1;
}
