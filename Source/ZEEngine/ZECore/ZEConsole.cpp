//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEConsole.cpp
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

#include "ZECore.h"
#include "ZETypes.h"
#include "ZEOption.h"
#include "ZEConsole.h"
#include "ZECommandManager.h"
#include "ZEOptionManager.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#define ZE_CONSOLE_PARSER_MAX_IDENTIFIER_SIZE	50
#define ZE_CONSOLE_PARSER_MAX_STRING_SIZE		256

enum ZEConsoleParserState
{
	ZE_CPS_ERROR,
	ZE_CPS_SECTION,
	ZE_CPS_SCOPE,
	ZE_CPS_NAME,
	ZE_CPS_OPERATOR,
	ZE_CPS_PARAMETER,
	ZE_CPS_ASSINGMENT,
	ZE_CPS_DISPLAY,
	ZE_CPS_NEXT_COMMAND,
	ZE_CPS_FINISHED
};

ZEConsole* ZEConsole::Instance = NULL;

void ZEConsoleInterface::Input(const char* Input)
{
	ZEConsole::GetInstance()->Input(Input);
}

ZEConsoleInterface::~ZEConsoleInterface()
{

}

static void ParseEmptySpace(const char* Input, ZESize &Cursor)
{
	while(Input[Cursor] == ' ' || Input[Cursor] == '\t')
		Cursor++;
}

static bool ParseIdentifier(const char* Input, ZESize &Cursor, char* Output)
{
	ZESize Index = 0;

	ParseEmptySpace(Input, Cursor);

	if (!isalpha(Input[Cursor]))
		return false;
	else
		Output[Index++] = Input[Cursor++];


	while(isalnum(Input[Cursor]))
		if (Index > ZE_CONSOLE_PARSER_MAX_IDENTIFIER_SIZE - 1)
		{
			zeOutput("[Parser] Error : Indentifier is too big on position %Iu. Maximumum allowed indentifier size is %d.\r\n", Cursor, ZE_CONSOLE_PARSER_MAX_IDENTIFIER_SIZE);
			return false;
		}
		else
			Output[Index++] = Input[Cursor++];

	Output[Index] = '\0';

	return true;
}

static bool ParseConstant(const char* Input, ZESize &Cursor, ZEValue *Output)
{
	char Temp[ZE_CONSOLE_PARSER_MAX_STRING_SIZE];
	ZEInt I = 0;

	ParseEmptySpace(Input, Cursor);

	if(Input[Cursor] == '\"')
	{
		while(I < ZE_CONSOLE_PARSER_MAX_STRING_SIZE - 2)
		{
			 Cursor++;
			 if (Input[Cursor] == '\0')
			 {
				 zeOutput("[Parser] Error : String constant terminated unexpectedly on position %Iu.\r\n", Cursor);
				 return false;
			 }
			 else if (Input[Cursor] == '\"' && Input[Cursor - 1] != '\\')
			 {
				Temp[I] = '\0';
				Cursor++;
				Output->SetString(Temp);
				return true;	
			 }
			 Temp[I++] = Input[Cursor];
		}
		zeOutput("[Parser] Error : String constant is too big on position %Iu . Allowed maximum size is %d.\r\n", Cursor, ZE_CONSOLE_PARSER_MAX_STRING_SIZE);
		return false;
	}
 	else if (isdigit(Input[Cursor]))
	{
		bool IsFloat = false;
		Temp[I++] = Input[Cursor++];

		while(I < ZE_CONSOLE_PARSER_MAX_STRING_SIZE - 1)
		{
			if (isdigit(Input[Cursor]))
				Temp[I++] = Input[Cursor++];
			else if (IsFloat == false && Input[Cursor] == '.')
			{
				Temp[I++] = Input[Cursor++];
				IsFloat = true;
			}
			else
			{
				Temp[I] = '\0';
				if (IsFloat)
					Output->SetFloat((float)atof(Temp));
				else
					Output->SetInt32(atoi(Temp));
				return true;
			}
		}
		zeOutput("[Parser] Error : Numeric constant is too big on position %Iu . Allowed maximum size is %d.\r\n", Cursor, ZE_CONSOLE_PARSER_MAX_STRING_SIZE);
		return false;
	}
	else if (isalpha(Input[Cursor]))
	{
		if (ParseIdentifier(Input, Cursor, Temp))
		{
			if (_stricmp(Temp, "true") == 0)
			{
				Output->SetBoolean(true);
				return true;
			}
			else if (_stricmp(Temp, "false") == 0)
			{
				Output->SetBoolean(false);
				return true;
			}
			else if (_stricmp(Temp, "NULL") == 0)
			{
				Output->SetNull();
				return true;
			}
			else
			{
				zeOutput("[Parser] Error : Unexpected identifier on position %Iu.\r\n", Cursor);
				return false;
			}
		}
		else
			return false;
	}
	else
		return false;
}

static bool ParseParameters(const char* Input, ZESize &Cursor, ZECommandParameterList *Output)
{
	ZEValue Param;
	bool FirstParam = true;

	Output->Clear();
	
	while (true)
	{
		ParseEmptySpace(Input, Cursor);
		switch(Input[Cursor])
		{
			case ')':
				return true;
				break;

			case ',':
				Cursor++;
				if (ParseConstant(Input, Cursor, &Param))
					Output->Add(Param);
				else
					return false;
				break;
			case '\0':
				zeOutput("[Parser] Error : Syntax error parameter terminated unexpectedly on position %Iu.\r\n", Cursor);
				return false;
			default:
				if (FirstParam)
 					if (ParseConstant(Input, Cursor, &Param))
					{
						Output->Add(Param);
						FirstParam = false;
						break;
					}
				zeOutput("[Parser] Error : Syntax error parameter list on position %Iu.\r\n", Cursor);
				return false;
				break;
		}
	}
}

static ZEConsoleParserState ParseOperator(const char* Input, ZESize &Cursor)
{
	ParseEmptySpace(Input, Cursor);

	switch(Input[Cursor])
	{
		case '=':
			Cursor++;
			return ZE_CPS_ASSINGMENT;
		case '(':
			Cursor++;
			return ZE_CPS_PARAMETER;
		case ';':
		case '\0':
			return ZE_CPS_DISPLAY;
		default:
			zeOutput("[Parser] Error : Syntax error on position %Iu.\r\n", Cursor);
			return ZE_CPS_ERROR;
	}
}

static bool ParseScope(const char* Input, ZESize &Cursor)
{
	ParseEmptySpace(Input, Cursor);
	
	if (Input[Cursor++] == '.')
		return true;
	else 
	{	
		zeOutput("[Parser] Error : Sytax error on position %Iu.\r\n", Cursor);
		return false;
	}
}

bool ZEConsole::ParseInput(const char* Input)
{
	char Section[ZE_CONSOLE_PARSER_MAX_IDENTIFIER_SIZE], Name[ZE_CONSOLE_PARSER_MAX_IDENTIFIER_SIZE];
	ZECommandParameterList ParamList;
	ZEValue Constant;
	ZEOption* Opt;
	Output("> %s\r\n", Input);
	ZEConsoleParserState State = ZE_CPS_SECTION;
	ZESize Cursor = 0;

	while(State != ZE_CPS_FINISHED)
	{
		switch (State)
		{
			case ZE_CPS_ERROR:
				Output("\r\n");
				return false;
				break;

			case ZE_CPS_SECTION:
				if (ParseIdentifier(Input, Cursor, Section))
					State = ZE_CPS_SCOPE;
				else
				{
					Output("[Parser] Error : Identifier error on position %Iu.\r\n", Cursor);
					State =	ZE_CPS_ERROR;
				}
				break;

			case ZE_CPS_SCOPE:
				if (ParseScope(Input, Cursor))
					State = ZE_CPS_NAME;
				else
				{
					Output("[Parser] Error : Scope operator is expected on position %Iu.\r\n", Cursor);
					State = ZE_CPS_ERROR;
				}
				break;

			case ZE_CPS_NAME:
				if (ParseIdentifier(Input, Cursor, Name))
					State = ZE_CPS_OPERATOR;
				else
				{
					Output("[Parser] Error : Identifier error on position %Iu.\r\n", Cursor);
					State = ZE_CPS_ERROR;
				}
				break;
			
			case ZE_CPS_OPERATOR:
				State = ParseOperator(Input, Cursor);
				if (State == ZE_CPS_ERROR)
					Output("[Parser] Error : An operator is expected on position %Iu.\r\n", Cursor);
				break;

			case ZE_CPS_PARAMETER:
				if (ParseParameters(Input, Cursor, &ParamList))
				{				
					if (ZECommandManager::GetInstance()->ExecuteCommand(Section, Name, &ParamList))
						State = ZE_CPS_NEXT_COMMAND;
					else
					{
						Output("[Parser] Wrong command name.\r\n");
						State = ZE_CPS_ERROR;
					}
				}
				else
					State = ZE_CPS_ERROR;
				break;

			case ZE_CPS_ASSINGMENT:
				if (ParseConstant(Input, Cursor, &Constant))
				{	
					Opt = ZEOptionManager::GetInstance()->GetOption(Section, Name);
					if (Opt != NULL)
						if (Opt->GetValueType() == Constant.GetType())
						{
							Opt->SetValue(Constant);
							State = ZE_CPS_NEXT_COMMAND;
						}
						else
						{
							Output("[Parser] Error : Option value type does not match with the assignment type.\r\n");
							State = ZE_CPS_ERROR;
						}
					else
					{
						Output("[Parser] Error : There is no such an option or command named \"%s\".\r\n", Name);
						State = ZE_CPS_ERROR;
					}
				}
				else
					State = ZE_CPS_ERROR;
				break;

			case ZE_CPS_DISPLAY:
				Opt = ZEOptionManager::GetInstance()->GetOption(Section, Name);
				if (Opt != NULL)
				{
					switch(Opt->GetValueType())
					{
						case ZE_VRT_UNDEFINED:
							Output("%s.%s = UNDEFINED (Probably an error)\r\n");
							break;
						case ZE_VRT_INTEGER_32:
							Output("%s.%s = %d\r\n", Section, Name, Opt->GetValue().GetInt32());
							break;
						case ZE_VRT_FLOAT:
							Output("%s.%s = %lf\r\n", Section, Name, Opt->GetValue().GetFloat());
							break;
						case ZE_VRT_BOOLEAN:
							Output("%s.%s = %s\r\n", Section, Name, Opt->GetValue().GetBoolean() ? "true" : "false");
							break;
						case ZE_VRT_STRING:
							Output("%s.%s = \"%s\"\r\n", Section, Name, Opt->GetValue().GetString());
							break;
						case ZE_VRT_NULL:
							Output("%s.%s = NULL\r\n");
							break;
						default:
							Output("Internal error not supported variant type.");
							break;
					}
					State = ZE_CPS_NEXT_COMMAND;
					return true;
				}
				else
				{
					Output("[Parser] Wrong option name.\r\n");
					return false;
				}
				break;

			case ZE_CPS_NEXT_COMMAND:
					Output("\r\n");
			case ZE_CPS_FINISHED:
				return true;
				break;
		}
	}
	return false;		
}

static void LogCallback(const char* Module, ZELogType Type, const char* LogText, void* ExtraParameters)
{
	zeOutput("[%s] %s: %s\n", Module, ZELog::UtilityGetLogTypeString(Type), LogText);
}

void ZEConsole::SetConsoleInterface(ZEConsoleInterface* Interface)
{
	if (ConsoleInterface != NULL)
		ConsoleInterface->Deinitialize();

	ConsoleInterface = Interface;
	if (ConsoleInterface != NULL)
	{
		if (!ConsoleInterface->Initialize())
			return;

		if (InputEnabled)
			ConsoleInterface->EnableInput();
		else
			ConsoleInterface->DisableInput();
		if (Visible)
			ConsoleInterface->ShowConsole();
		else
			ConsoleInterface->HideConsole();
	}
}

bool ZEConsole::IsVisible()
{
	return Visible;
}

void ZEConsole::ShowConsole()
{
	Visible = true;
	if (ConsoleInterface != NULL)
		ConsoleInterface->ShowConsole();
}

void ZEConsole::HideConsole()
{
	Visible = false;
	if (ConsoleInterface != NULL)
		ConsoleInterface->HideConsole();
}

void ZEConsole::EnableInput()
{
	if (ConsoleInterface != NULL)
		ConsoleInterface->EnableInput();
	InputEnabled = true;
}

void ZEConsole::DisableInput()
{
	if (ConsoleInterface != NULL)
		ConsoleInterface->EnableInput();
	InputEnabled = false;
}

void ZEConsole::Log(const char* Module, const char* Format, ...)
{
	char Buffer[32768];
	va_list vlist;
	va_start(vlist, Format);
	vsprintf_s(Buffer, 32768, Format, vlist);
	va_end(vlist);
	
	char Buffer2[32768];
	sprintf_s(Buffer2, 32768, "[%s] %s\r\n", Module, Buffer);

	char* HistBuffer = new char[strlen(Buffer2) + 1];
	strncpy(HistBuffer, Buffer2, strlen(Buffer2) + 1);  
	OutputHistory.Add(HistBuffer);
	if (ConsoleInterface != NULL)
		ConsoleInterface->Output(Buffer2);
	#ifdef ZE_DEBUG_CHECK_HEAP
		if (!_CrtCheckMemory())
			OutputDebugString("Jackpot");
	#endif
}

void ZEConsole::Output(const char* Format, ...)
{
	char Buffer[32768];
	va_list vlist;
	va_start(vlist, Format);
	vsprintf_s(Buffer, 32768, Format, vlist);
	va_end(vlist);

	char* HistBuffer = new char[strlen(Buffer) + 1];
	strncpy(HistBuffer, Buffer, strlen(Buffer) + 1);  
	OutputHistory.Add(HistBuffer);
	if (ConsoleInterface != NULL)
		ConsoleInterface->Output(Buffer);
	#ifdef ZE_DEBUG_CHECK_HEAP
		if (!_CrtCheckMemory())
			OutputDebugString("Jackpot");
	#endif
}

void ZEConsole::Input(const char* Input)
{
	if(InputEnabled)
	{
		char* HistBuffer = new char[strlen(Input) + 1];
		strncpy(HistBuffer, Input, strlen(Input));  
		InputHistory.Add(HistBuffer);
		ParseInput(Input);
	}
}

void ZEConsole::Process()
{
	if (ConsoleInterface != NULL)
		ConsoleInterface->Process();
}

ZEConsole* ZEConsole::GetInstance()
{
	return ZECore::GetInstance()->GetConsole();
}

ZEConsole::ZEConsole()
{
	InputEnabled = true;
	Visible = false;
	Instance = this;
	ConsoleInterface = NULL;
	ZELog::GetInstance()->SetCallback(LogCallback);
}

ZEConsole::~ZEConsole()
{
	for (ZESize I = 0; I < OutputHistory.GetCount(); I++)
		delete[] OutputHistory[I];
	OutputHistory.Clear();

	for (ZESize I = 0; I < InputHistory.GetCount(); I++)
		delete[] InputHistory[I];
	InputHistory.Clear();
	#ifdef ZE_DEBUG_CHECK_HEAP
		if (!_CrtCheckMemory())
			OutputDebugString("Jackpot");
	#endif
	Instance = NULL;
}
