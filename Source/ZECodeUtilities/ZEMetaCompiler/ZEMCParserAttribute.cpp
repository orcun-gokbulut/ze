//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMCParserAttribute.cpp
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
#include "ZEMCContext.h"

bool ZEMCParser::ParseAttribute(ZEMCAttribute& Data, const AnnotateAttr* Attribute)
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
						Data.Name = ParameterText;
					else
						Data.Parameters.Add(ParameterText);

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
		if (Data.Name.IsEmpty())
			Data.Name = ParameterText;
		else
			Data.Parameters.Add(ParameterText);

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

void ZEMCParser::ParseAttributes(ZEMCDeclaration* Decleration, Decl* ClangDecl)
{
	for(Decl::attr_iterator CurrentAttr = ClangDecl->attr_begin(); CurrentAttr != ClangDecl->attr_end(); CurrentAttr++)
	{
		ZEMCAttribute Attribute;
		if (ParseAttribute(Attribute, ((AnnotateAttr*)(*CurrentAttr))))
			Decleration->Attributes.Add(Attribute);
	}
}

bool ZEMCParser::CheckAttribute(ZEMCDeclaration* Declaration, const char* AttributeName)
{
	for (int I = 0; I < Declaration->Attributes.GetCount(); I++)
		if (Declaration->Attributes[I].Name == AttributeName)
			return true;

	return false;
}

const ZEArray<ZEString>* ZEMCParser::GetAttribute(ZEMCDeclaration* Declaration, const char* AttributeName)
{
	for (int I = 0; I < Declaration->Attributes.GetCount(); I++)
		if (Declaration->Attributes[I].Name == AttributeName)
			return &Declaration->Attributes[I].Parameters;

	return NULL;
}
