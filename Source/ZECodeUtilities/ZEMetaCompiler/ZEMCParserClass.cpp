//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserClass.cpp
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

bool ZEMCParser::CheckFundamentalClass(CXXRecordDecl* Class)
{
	for (CXXRecordDecl::method_iterator Iterator = Class->method_begin(); Iterator != Class->method_end(); Iterator++)
	{
		if (Iterator->param_size() != 0 ||
			!Iterator->getDeclName().isIdentifier() ||
			Iterator->getName() != "Class" ||  
			!Iterator->isStatic())
		{
			continue;
		}

		ZEMCDeclaration Temp;
		ParseAttributes(&Temp, *Iterator);

		return CheckAttribute(&Temp, "ZEMeta.Fundamental");
	}

	return false;
}

bool ZEMCParser::CheckTargetDeclaration(Decl* Declaration)
{
	clang::SourceLocation Location = Declaration->getLocEnd();
	clang::FileID CurrentFileID = Compiler->getSourceManager().getFileID(Location);
	clang::FileID MainFileId = Compiler->getSourceManager().getMainFileID();
	return (CurrentFileID == MainFileId);
}

bool ZEMCParser::ProcessForwardDeclaration(CXXRecordDecl* Class)
{
	if (Class->isClass() && !Class->isCompleteDefinition() && Class->hasAttrs())
	{
		ZEMCAttribute AttributeData;
		for (CXXRecordDecl::attr_iterator CurrentAttr = Class->attr_begin(), LastAttr = Class->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
		{
			ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));

			if (AttributeData.Name == "ZEMeta.ForwardDeclaration")
			{
				bool Found = false;
				for (ZESize I = 0; I < Context->ForwardDeclarations.GetCount(); I++)
				{
					if (Context->ForwardDeclarations[I]->ClassName == AttributeData.Parameters[0])
					{
						Found = true;
						break;
					}
				}

				if (!Found)
				{
					ZEMCForwardDeclaration* ForwardDeclaredClass = new ZEMCForwardDeclaration();

					ForwardDeclaredClass->ClassName = AttributeData.Parameters[0].ToCString();
					ForwardDeclaredClass->HeaderName = AttributeData.Parameters[1].ToCString();

					Context->ForwardDeclarations.Add(ForwardDeclaredClass);

					return true;
				}
			}
		}
	}

	return false;
}

void ZEMCParser::ProcessClass(CXXRecordDecl* Class)
{
	if (ProcessForwardDeclaration(Class))
		return;

	if (!Class->isCompleteDefinition())
		return;

	bool FundamentalClass = CheckFundamentalClass(Class);
	if (!FundamentalClass && !CheckClass(Class))
		return;
	
	ZEMCClass* ClassData = FindClass(Class->getNameAsString().c_str());
	if (ClassData == NULL)
	{
		ClassData = new ZEMCClass();
		Context->Classes.Add(ClassData);
	}

	ClassData->Name = Class->getNameAsString();
	ClassData->MetaName = ClassData->Name + "Class";
	ClassData->Hash = ClassData->Name.Hash();

	ClassData->HasCreateInstanceMethod = false;
	ClassData->HasPublicDestroyMethod = false;

	ClassData->HasPublicCopyConstructor = true;
	ClassData->HasPublicDefaultConstructor = true;
	ClassData->HasPublicDestructor = true;
	ClassData->HasPublicAssignmentOperator = true;
	
	ClassData->IsAbstract = Class->isAbstract();
	ClassData->IsFundamental = FundamentalClass;
	ClassData->IsForwardDeclared = false;

	if (!ClassData->IsFundamental)
	{
		for(CXXRecordDecl::base_class_iterator CurrentBaseClass = Class->bases_begin(), LastBaseClass = Class->bases_end(); CurrentBaseClass != LastBaseClass; ++CurrentBaseClass)
		{
			ClassData->BaseClass = FindClass(CurrentBaseClass->getType()->getAsCXXRecordDecl()->getNameAsString().c_str());
			if (ClassData == NULL) // Non-Object Base
			{
				RaiseError(Class->getLocStart(), "Non-object base class.");
			}
		}
	}
	else
	{
		ClassData->BaseClass = NULL;
	}

	// Combine Methods and Properties
	if (ClassData->BaseClass != NULL)
	{
		for (ZESize I = 0; I < ClassData->BaseClass->Methods.GetCount(); I++)
		{
			ZEMCMethod* CloneMethod = new ZEMCMethod();
			*CloneMethod = *ClassData->BaseClass->Methods[I];
			ClassData->Methods.Add(CloneMethod);
		}

		for (ZESize I = 0; I < ClassData->BaseClass->Properties.GetCount(); I++)
		{
			ZEMCProperty* CloneProperty = new ZEMCProperty();
			*CloneProperty = *ClassData->BaseClass->Properties[I];
			ClassData->Properties.Add(CloneProperty);
		}
	}

	ClassData->Attributes = ClassData->BaseClass->Attributes;
	for(CXXRecordDecl::attr_iterator CurrentAttr = Class->attr_begin(), LastAttr = Class->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
	{
		ZEMCAttribute AttributeData;
		ParseAttribute(AttributeData, ((AnnotateAttr*)(*CurrentAttr)));
		ClassData->Attributes.Add(AttributeData);
	}

	NormalizeAttributeData(ClassData->Attributes);

	for(CXXRecordDecl::decl_iterator Current = Class->decls_begin(), End = Class->decls_end(); Current != End; ++Current)
	{
		if(isa<VarDecl>(*Current) || isa<FieldDecl>(*Current))
		{
			VarDecl* PropertyDeclaration = ((VarDecl*)*Current);
			if (PropertyDeclaration->getType()->isClassType() && 
				PropertyDeclaration->getType()->getAsCXXRecordDecl()->getNameAsString() == "ZEEvent")
			{
				ProcessEvent(ClassData, PropertyDeclaration);
			}
			else
			{
				ProcessProperty(ClassData, PropertyDeclaration);
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
			ZEMCAttribute AnonymousRecordAttributeData;
			for(CXXRecordDecl::attr_iterator CurrentAttr = AnonymousRecord->attr_begin(), LastAttr = AnonymousRecord->attr_end(); CurrentAttr != LastAttr; ++CurrentAttr)
			{
				ParseAttribute(AnonymousRecordAttributeData, ((AnnotateAttr*)(*CurrentAttr)));
				ClassData->Attributes.Add(AnonymousRecordAttributeData);
			}
		}
	}

	ProcessPropertyAccessors(ClassData);

	// Give Properties Ordered IDs.
	for (int I = 0; I < ClassData->Properties.GetCount(); I++)
	{
		ClassData->Properties[I]->ID = I;
	}

	for (int I = 0; I < ClassData->Methods.GetCount(); I++)
	{
		ClassData->Methods[I]->ID = I;
	}

	if (CheckTargetDeclaration(Class))
		Context->TargetClasses.Add(ClassData);
}
