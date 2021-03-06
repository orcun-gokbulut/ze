//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEOptionManager.cpp
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

#include "ZEOptionManager.h"
#include "ZECommandManager.h"
#include "ZECore.h"
#include "ZEError.h"
#include "ZEConsole.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ZEFile\ZEPathInfo.h"

#define OPTIONS_FILE_MAX_LINE_LENGTH 1024
#define OPTIONS_FILE_MAX_NAME_LENGTH 256
#define OPTIONS_FILE_MAX_VALUE_LENGTH 768

bool LoadState;
bool ZEOptionManager::LoadCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
	if(Params->GetCount() == 0)
	{
		Load("#E:/options.ini");
		return true;
	}
	else if (Params->GetCount() == 1 &&
			 Params->GetItem(0).GetType() == ZE_VRT_STRING)
	{
		Load(Params->GetItem(0).GetString());
		zeOutput("Options loaded.");
		return true;
	}
	else
	{
		zeOutput("Incorrect parameters. \r\n"
			"Usage :\r\n"
			"  Options.Load() - Loads options from options.ini\r\n"
			"  Options.Load(String FileName) - Load options from a file named 'FileName'\r\n"
			"Example :\r\n"
			"  Options.Load(\"c:\\backup.ini\") - Load options from f:\\backup.ini file\r\n");
		return false;
	 }
}

bool ZEOptionManager::SaveCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
 	 switch(Params->GetCount())
	 {
		case 0:
			Save("#E:/options.ini");
			zeOutput("Options saved.");
			return true;
		case 1:
			if (Params->GetItem(0).GetType() == ZE_VRT_STRING)
			{
				Save(Params->GetItem(0).GetString());
				zeOutput("Options saved to \"%s\" file.", Params->GetItem(0).GetString());
				return true;
			}
		default:
			zeOutput("Incorrect parameters\r\n"
				"Usage :\r\n"
				"  Options.SaveOptions() - Saves current options to options.ini\r\n"
				"  Options.SaveOptions(String FileName) - Saves current options to a file named 'FileName'\r\n"
				"Example :\r\n"
				"  Options.SaveOptions(\"f:\\myoptions.ini\") - Save current options to f:\\myoptions.ini file\r\n");
			return false;
	 }
}

bool ZEOptionManager::ListSectionsCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
	ZESize Index, Count;

	if (Params->GetCount() == 0)
	{
		Index = 0;
		Count = Sections.GetCount();
	}
	else if (Params->GetCount() == 2 && 
			 Params->GetItem(0).GetType() == ZE_VRT_INTEGER_32 && 
			 Params->GetItem(1).GetType() == ZE_VRT_INTEGER_32)
	{
		Index = Params->GetItem(0).GetInt32();
		Count = Params->GetItem(1).GetInt32();
	}
	else
	{
		zeOutput("Incorrect parameters\r\n"
			"Usage :\r\n"
			"  Options.ListSections() - Lists all of the option sections\r\n"
			"  Options.ListSection(Integer From, Integer Count) - Lists 'Count' number of option sections starting from index 'From'\r\n"
			"Example :\r\n"
			"  Options.ListSections(10, 50) - List sections from 10 to 60\r\n");
		return false;
	}

	zeOutput(" Name                           Has Changes   Number Of Options \r\n"
		"----------------------------------------------------------------\r\n");

	ZEOptionSection* CurrSection;
	for (ZESize I = Index; I < Sections.GetCount() && I <= Count; I++)
	{
		CurrSection = Sections.GetItem(I);
		zeOutput(" %-30s   %11s   %Iu\r\n", CurrSection->GetName().ToCString(), 
			(CurrSection->HasChanges() ? "    Yes    " : "   No      "),
			CurrSection->GetNumberOfOptions());
	}
	return true;
}

bool ZEOptionManager::ListOptionsCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
	ZESize Index, Count;
	ZEOptionSection* Sec;

	if (Params->GetCount() == 1 &&
		Params->GetItem(0).GetType() == ZE_VRT_STRING)
	{
		Sec = GetSection(Params->GetItem(0).GetString());
		Index = 0;
		Count = -1;
	}
	else if (Params->GetCount() == 1 &&
			 Params->GetItem(0).GetType() == ZE_VRT_STRING &&
			 Params->GetItem(1).GetType() == ZE_VRT_INTEGER_32 &&
			 Params->GetItem(2).GetType() == ZE_VRT_INTEGER_32)
	{
		Sec = GetSection(Params->GetItem(0).GetString());
		Index = Params->GetItem(1).GetInt32();
		Count = Params->GetItem(2).GetInt32();
	}
	else
	{
		zeOutput("Incorrect parameters. \r\n"
			"Usage :\r\n"
			" Options.ListOptions(String Section) - Lists all of the options in the section named 'Section'\r\n"
			" Options.ListOptions(String Section, Integer Index, Integer Count) - Lists 'Count' number of options in the option section named 'Section' starting from index 'Index'\r\n"
			"Example :\r\n"
			"  Options.ListOptions(\"Core\", 1, 5) - List options in the Core option section from 1 to 5\r\n");
		return false;
	}

	if (Sec != NULL)
	{
		zeOutput(" Name                            Type           Changed    Value           \r\n"
			"---------------------------------------------------------------------------\r\n");
		
		ZEOption* Opt;
		for (ZESize I = 0; I < Sec->GetNumberOfOptions() && I <= Count; I++)
		{
			Opt = Sec->GetOption(I);	
			zeOutput(" %-30s  ", Sec->GetOption(I)->GetName().ToCString());
			switch(Opt->GetValueType())
			{
				case ZE_VRT_UNDEFINED:
					zeOutput(" Undefined  ");
					break;
				case ZE_VRT_INTEGER_32:
					zeOutput(" Integer    ");
					break;
				case ZE_VRT_FLOAT:
					zeOutput(" Float      ");
					break;
				case ZE_VRT_BOOLEAN:
					zeOutput(" Boolean    ");
					break;
				case ZE_VRT_NULL:
					zeOutput(" NULL       ");
					break;
				case ZE_VRT_STRING:
					zeOutput(" String     ");
					break;
				default:
					zeOutput(" Error      ");
			}
			zeOutput(" %s  ", (Opt->IsChanged() ? "  Yes   " : "   No   "));

   			switch(Opt->GetValueType())
			{
				case ZE_VRT_UNDEFINED:
					zeOutput("UNDEFINED (Probably Internal Error)\r\n");
					break;
				case ZE_VRT_INTEGER_32:
					zeOutput("%d\r\n", Opt->GetValue().GetInt32());
					break;
				case ZE_VRT_FLOAT:
					zeOutput("%lf\r\n", Opt->GetValue().GetFloat());
					break;
				case ZE_VRT_BOOLEAN:
					zeOutput("%s\r\n", Opt->GetValue().GetBoolean() ? "true" : "false");
					break;
				case ZE_VRT_NULL:
					zeOutput("NULL\r\n");
					break;
				case ZE_VRT_STRING:
					zeOutput("\"%s\"\r\n", Opt->GetValue().GetString().ToCString());
					break;
				default:
					zeOutput("Error\r\n");
			}
		}
		return true;
	}
	else
	{
		zeOutput("There is no such an option section. \r\n\r\n");
		return false;
	}
}

bool ZEOptionManager::CommitChangesCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
	zeOutput("Committing changes in options to the modules and components. This may take a while. Please wait.\r\n");
	CommitChanges();
	zeOutput("Changes committed.\r\n");
	return true;
}

bool ZEOptionManager::ResetChangesCommand(ZECommand* Command, const ZECommandParameterList* Params)
{
	zeOutput("Option changes resetted.\r\n");
	ResetChanges();
	return true;
}

bool ZEOptionManager::RegisterSection(ZEOptionSection* Ref)
{
	if (GetSection(Ref->GetName()) != NULL)
	{
		zeError("Can not register option section. An option section with same name is already registered. (Option Section Name : \"%s\")", 
			Ref->GetName().ToCString());
		return false;
	}
	Sections.Add(Ref);
	return true;
}

bool ZEOptionManager::UnregisterSection(ZEOptionSection* Ref)
{
	if (GetSection(Ref->GetName()) == NULL)
		return false;

	Sections.RemoveValue(Ref);
	return true;
}

ZEOptionSection* ZEOptionManager::GetSection(const ZEString& Name)
{
	for(ZESize I = 0; I < Sections.GetCount(); I++)
		if (Sections[I]->GetName() == Name)
			return Sections[I];
	return NULL;
}

ZEOptionSection* ZEOptionManager::GetSection(ZESize Index)
{
	if (Index < Sections.GetCount())
			return Sections[Index];
	else
		return NULL;
}

ZESize ZEOptionManager::GetNumberOfSections()
{
	return Sections.GetCount();
}

void ZEOptionManager::Save(const ZEString& FileName)
{
	ZERealPath Path = ZEPathInfo(FileName).GetRealPath();
	FILE* File = fopen(Path.Path, "w");
	ZEOptionSection*	CurrentSet;
	ZEOption*		Current;
	ZESize I, N;
	if (File)
	{
		for (I = 0; I < Sections.GetCount(); I++)
		{
			CurrentSet = Sections.GetItem(I);
			fprintf(File, "[%s]\n", CurrentSet->GetName().ToCString());
			for(N = 0; N < CurrentSet->GetNumberOfOptions(); N++)
			{
				Current = CurrentSet->GetOption(N);
				if (Current->GetAttribute() != ZE_OA_HIDDEN && Current->GetAttribute() != ZE_OA_INTERNAL)
					switch(Current->GetValueType())
					{
						case ZE_VRT_NULL:
							fprintf(File, "\n");
							break;
						case ZE_VRT_STRING:
							fprintf(File, "%s = %s\n", Current->GetName().ToCString(), Current->GetValue().GetString().ToCString());
							break;
						case ZE_VRT_INTEGER_32:
							fprintf(File, "%s = %d\n", Current->GetName().ToCString(), Current->GetValue().GetInt32());
							break;			
						case ZE_VRT_FLOAT:
							fprintf(File, "%s = %f\n", Current->GetName().ToCString(), Current->GetValue().GetFloat());
							break;
						case ZE_VRT_BOOLEAN:
							fprintf(File, "%s = %s\n", Current->GetName().ToCString(), Current->GetValue().GetBoolean() == true ? "true" : "false");
							break;
					}
			}
			fprintf(File, "\n");
		}
		fclose(File);
	}
	else
		zeError("Can not open option file. Could not write options to file.\r\n");
}  

void ZEOptionManager::MatchOption(char* Line, char* MatchName, char* MatchValue)
{
	enum
	{
		Begin,
		Name,
		Equal,
		Value
	} State = Begin;

	*MatchName = * MatchValue = '\0';
	while(*Line != '\n')
	{
		if (State == Begin && isalpha(*Line))
			State = Name;


		if (State == Name && *Line == '=')
		{
			MatchName--;
			while(*MatchName == ' ' || *MatchName == '\t')
				MatchName--;
			*(++MatchName) = '\0';

			State = Equal;
			Line++;
			continue;
		}

		if (State == Equal && !(*Line == ' ' || *Line == '\t'))
			State = Value;

		if (State == Name)
		{
			*MatchName = *Line;
			MatchName++;
		}
		else if(State == Value)
		{
			*MatchValue = *Line;
			MatchValue++;
		}
		Line++;
	}

	MatchValue--;
	while(*MatchValue == ' ' || *MatchValue == '\t')
		MatchValue--;
	*(++MatchValue) = '\0';
}

bool ZEOptionManager::MatchSet(char* Line, char* Match)
{
	enum
	{
		Begin,
		Inside
	} State = Begin;

	*Match = '\0';

	while(*Line != '\n')
	{
		if (State == Begin && *Line == '[')
		{
			State = Inside;
			Line++;
			continue;
		}

		if (State == Begin && !(*Line == ' ' || *Line == '\t'))
			return false;



		if (State == Inside && *Line == ']')
		{
			*Match = '\0';
			return true;
		}

		if (State == Inside)
			*Match++ = *Line;

		Line++;
	}
	return false;
}

ZEOption* ZEOptionManager::GetOption(const ZEString& SectionName, const ZEString& Name)
{
	ZEOptionSection* Sec = GetSection(SectionName);
	if (Sec != NULL)
		return Sec->GetOption(Name);
	else
		return NULL;
}

void ZEOptionManager::Load(const ZEString& FileName)
{
	ZERealPath Path = ZEPathInfo(FileName).GetRealPath();
	FILE* File = fopen(Path.Path, "r");
	
	char		LineBuffer[OPTIONS_FILE_MAX_LINE_LENGTH], 
				NameBuffer[OPTIONS_FILE_MAX_NAME_LENGTH], 
				ValueBuffer[OPTIONS_FILE_MAX_VALUE_LENGTH];

	ZEOptionSection*	CurrentSet;
	ZEOption*		Current;
	
	if (File != NULL)
	{
		while(fgets(LineBuffer, 1024, File))
		{
			if (MatchSet(LineBuffer, NameBuffer))
			{
				CurrentSet = GetSection(NameBuffer);
				continue;
			}

			if (CurrentSet != NULL)
			{
				MatchOption(LineBuffer, NameBuffer, ValueBuffer);
				if (NameBuffer[0] != '\n')
				{
					Current = CurrentSet->GetOption(NameBuffer);
					if (Current != NULL)
						if (Current->GetAttribute() != ZE_OA_INTERNAL)
							switch(Current->GetValueType())
							{
							case ZE_VRT_STRING:
								Current->SetValue(ValueBuffer);
								break;
							case ZE_VRT_INTEGER_32:
								Current->SetValue(atoi(ValueBuffer));
								break;			
							case ZE_VRT_FLOAT:
								Current->SetValue((float)atof(ValueBuffer));
								break;
							case ZE_VRT_BOOLEAN:
								if(_stricmp(ValueBuffer, "true") == 0)
									Current->SetValue(true);
								else if (_stricmp(ValueBuffer, "false") == 0)
									Current->SetValue(false);
								break;
							}
				}
			} 
			
		}
		fclose(File);
		ResetChanges();	
	}
	else
		zeError("Can not open option file. Could not load options from file.\r\n");
}

void ZEOptionManager::CommitChanges()
{
	ZEOptionSection* CurrSection;
	for (ZESize I = 0; I < Sections.GetCount(); I++)
	{
		CurrSection = Sections[I];
		if (CurrSection->HasChanges())
			CurrSection->CommitChanges();
	}
}

void ZEOptionManager::ResetChanges()
{
	ZEOptionSection* CurrSection;
	for (ZESize I = 0; I < Sections.GetCount(); I++)
	{
		CurrSection = Sections[I];
		if (CurrSection->HasChanges())
			CurrSection->ResetChanges();
	}
}

ZEOptionManager* ZEOptionManager::GetInstance()
{
	return ZECore::GetInstance()->GetOptions();
}

ZEOptionManager::ZEOptionManager()
{
	Commands.SetName("Options");
	Commands.AddCommand(new ZECommand("Save", ZECommandCallback::Create<ZEOptionManager, &ZEOptionManager::SaveCommand>(this)));
	Commands.AddCommand(new ZECommand("Load", ZECommandCallback::Create<ZEOptionManager, &ZEOptionManager::LoadCommand>(this)));
	Commands.AddCommand(new ZECommand("ListSections",  ZECommandCallback::Create<ZEOptionManager, &ZEOptionManager::ListSectionsCommand>(this)));
	Commands.AddCommand(new ZECommand("ListOptions",   ZECommandCallback::Create<ZEOptionManager, &ZEOptionManager::ListOptionsCommand>(this)));
	Commands.AddCommand(new ZECommand("CommitChanges", ZECommandCallback::Create<ZEOptionManager,  &ZEOptionManager::CommitChangesCommand>(this)));
	Commands.AddCommand(new ZECommand("ResetChanges",  ZECommandCallback::Create<ZEOptionManager,  &ZEOptionManager::ResetChangesCommand>(this)));
	ZECommandManager::GetInstance()->RegisterSection(&Commands);
}

ZEOptionManager::~ZEOptionManager()
{
	ZECommandManager::GetInstance()->UnregisterSection(&Commands);
}
