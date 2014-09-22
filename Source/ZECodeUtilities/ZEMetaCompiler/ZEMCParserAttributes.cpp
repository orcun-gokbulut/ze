//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserAttributes.cpp
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

	ZEMCClass* ClassData = new ZEMCClass();
	ClassData->Name = Class->getNameAsString();
	ClassData->Hash = ClassData->Name.Hash();
	ClassData->HasCreateInstanceMethod = false;
	ClassData->HasPublicCopyConstructor = false;
	ClassData->HasPublicConstructor = false;
	ClassData->IsAbstract = Class->isAbstract();

	Context->Declarations.Add(ClassData);
	if (Compiler->getSourceManager().getFileID(Class->getLocation()) == Compiler->getSourceManager().getMainFileID())
		Context->TargetDeclarations.Add(ClassData);

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
			VarDecl* PropertyDeclaration = ((VarDecl*)*Current);
			if (PropertyDeclaration->getType()->isClassType() && PropertyDeclaration->getType()->getAsCXXRecordDecl()->getName() == "ZEEvent")
				ProcessEvent(ClassData, PropertyDeclaration);
			else
				ProcessProperty(ClassData, llvm::cast<VarDecl>(Current));
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
