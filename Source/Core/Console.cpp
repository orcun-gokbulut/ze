//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Console.cpp
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

#include "Console.h"
#include "Command.h"
#include "Option.h"
#include "Core.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER PARSER //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
#define PARSER_MAX_IDENTIFIER_SIZE	50
#define PARSER_MAX_STRING_SIZE		256

enum EParserState
{
	PARSERSTATE_ERROR,
	PARSERSTATE_SECTION,
	PARSERSTATE_SCOPE,
	PARSERSTATE_NAME,
	PARSERSTATE_OPERATOR,
	PARSERSTATE_PARAMETER,
	PARSERSTATE_ASSINGMENT,
	PARSERSTATE_DISPLAY,
	PARSERSTATE_NEXTCOMMAND,
	PARSERSTATE_FINISHED
};

ZEConsole* ZEConsole::Instance = NULL;

void ZEConsoleInterface::Input(const char* Input)
{
	zeConsole->Input(Input);
}


inline void ZEConsole::ParseEmptySpace(const char* Input, int &Cursor)
{
	while(Input[Cursor] == L' ' || Input[Cursor] == L'\t')
		Cursor++;
}

inline bool ZEConsole::ParseConstant(const char* Input, int &Cursor, ZEVariant *Output)
{
	char Temp[PARSER_MAX_STRING_SIZE];
	int I = 0;

	ParseEmptySpace(Input, Cursor);

	if(Input[Cursor] == '\"')
	{
		while(I < PARSER_MAX_STRING_SIZE - 2)
		{
			 Cursor++;
			 if (Input[Cursor] == L'\0')
			 {
				 this->Output("[Parser] Error : String constant terminated unexpectedly on position %d.\r\n", Cursor);
				 return false;
			 }
			 else if (Input[Cursor] == L'\"' && Input[Cursor - 1] != L'\\')
			 {
				Temp[I] = L'\0';
				Cursor++;
				Output->SetString(Temp);
				return true;	
			 }
			 Temp[I++] = Input[Cursor];
		}
		this->Output("[Parser] Error : String constant is too big on position %d . Allowed maximum size is %d.\r\n", Cursor, PARSER_MAX_STRING_SIZE);
		return false;
	}
 	else if (isdigit(Input[Cursor]))
	{
		bool IsFloat = false;
		Temp[I++] = Input[Cursor++];

		while(I < PARSER_MAX_STRING_SIZE - 1)
		{
			if (isdigit(Input[Cursor]))
				Temp[I++] = Input[Cursor++];
			else if (IsFloat == false && Input[Cursor] == L'.')
			{
				Temp[I++] = Input[Cursor++];
				IsFloat = true;
			}
			else
			{
				Temp[I] = L'\0';
				if (IsFloat)
					Output->SetFloat((float)atof(Temp));
				else
					Output->SetInteger(atoi(Temp));
				return true;
			}
		}
		this->Output("[Parser] Error : Numeric constant is too big on position %d . Allowed maximum size is %d.\r\n", Cursor, PARSER_MAX_STRING_SIZE);
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
				this->Output("[Parser] Error : Unexpected indentifier on position %d.\r\n", Cursor);
				return false;
			}
		}
		else
			return false;
	}
	else
		return false;
}

inline bool ZEConsole::ParseParameters(const char* Input, int &Cursor, ZECommandParameterList *Output)
{
	ZEVariant Param;
	bool FirstParam = true;

	Output->Clear();
	
	while (true)
	{
		ParseEmptySpace(Input, Cursor);
		switch(Input[Cursor])
		{
			case L')':
				return true;
				break;

			case L',':
				Cursor++;
				if (ParseConstant(Input, Cursor, &Param))
					Output->Add(Param);
				else
					return false;
				break;
			case L'\0':
				this->Output("[Parser] Error : Sytax error parameter terminated unexpectedly on position %d.\r\n", Cursor);
				return false;
			default:
				if (FirstParam)
 					if (ParseConstant(Input, Cursor, &Param))
					{
						Output->Add(Param);
						FirstParam = false;
						break;
					}
				this->Output("[Parser] Error : Sytax error parameter list on position %d.\r\n", Cursor);
				return false;
				break;
		}
	}
}

inline int ZEConsole::ParseOperator(const char* Input, int &Cursor)
{
	ParseEmptySpace(Input, Cursor);

	switch(Input[Cursor])
	{
		case L'=':
			Cursor++;
			return PARSERSTATE_ASSINGMENT;
		case L'(':
			Cursor++;
			return PARSERSTATE_PARAMETER;
		case L';':
		case L'\0':
			return PARSERSTATE_DISPLAY;
		default:
			Output("[Parser] Error : Syntax error on position %d.\r\n", Cursor);
			return PARSERSTATE_ERROR;
	}
}

inline bool ZEConsole::ParseScope(const char* Input, int &Cursor)
{
	ParseEmptySpace(Input, Cursor);
	
	if (Input[Cursor++] == L'.')
		return true;
	else 
	{	
		Output("[Parser] Error : Sytax error on position %d.\r\n", Cursor);
		return false;
	}
}

inline bool ZEConsole::ParseIdentifier(const char* Input, int &Cursor, char* Output)
{
	int Index = 0;

	ParseEmptySpace(Input, Cursor);

	if (!isalpha(Input[Cursor]))
		return false;
	else
		Output[Index++] = Input[Cursor++];


	while(isalnum(Input[Cursor]))
		if (Index > PARSER_MAX_IDENTIFIER_SIZE - 1)
		{
			this->Output("[Parser] Error : Indentifier is too big on position %d. Maximumum allowed indentifier size is %d.\r\n", Cursor, PARSER_MAX_IDENTIFIER_SIZE);
			return false;
		}
		else
			Output[Index++] = Input[Cursor++];

	Output[Index] = L'\0';

	return true;
}


bool ZEConsole::ParseInput(const char* Input)
{
	char Section[PARSER_MAX_IDENTIFIER_SIZE], Name[PARSER_MAX_IDENTIFIER_SIZE];
	ZECommandParameterList ParamList;
	ZEVariant Constant;
	ZEOption* Opt;
	Output("> %s\r\n", Input);
	int State = PARSERSTATE_SECTION;
	int Cursor = 0;

	while(State != PARSERSTATE_FINISHED)
	{
		switch (State)
		{
			case PARSERSTATE_ERROR:
				Output("\r\n");
				return false;
				break;

			case PARSERSTATE_SECTION:
				if (ParseIdentifier(Input, Cursor, Section))
					State = PARSERSTATE_SCOPE;
				else
				{
					Output("[Parser] Error : Identifier error on position %d.\r\n", Cursor);
					State =	PARSERSTATE_ERROR;
				}
				break;

			case PARSERSTATE_SCOPE:
				if (ParseScope(Input, Cursor))
					State = PARSERSTATE_NAME;
				else
				{
					Output("[Parser] Error : Scope operator is expected on position %d.\r\n", Cursor);
					State = PARSERSTATE_ERROR;
				}
				break;

			case PARSERSTATE_NAME:
				if (ParseIdentifier(Input, Cursor, Name))
					State = PARSERSTATE_OPERATOR;
				else
				{
					Output("[Parser] Error : Identifier error on position %d.\r\n", Cursor);
					State = PARSERSTATE_ERROR;
				}
				break;
			
			case PARSERSTATE_OPERATOR:
				State = ParseOperator(Input, Cursor);
				if (State == PARSERSTATE_ERROR)
					Output("[Parser] Error : An operator is expected on position %d.\r\n", Cursor);
				break;

			case PARSERSTATE_PARAMETER:
				if (ParseParameters(Input, Cursor, &ParamList))
				{				
					if (zeCommands->ExecuteCommand(Section, Name, &ParamList))
						State = PARSERSTATE_NEXTCOMMAND;
					else
					{
						Output("[Parser] Wrong command name.\r\n");
						State = PARSERSTATE_ERROR;
					}
				}
				else
					State = PARSERSTATE_ERROR;
				break;

			case PARSERSTATE_ASSINGMENT:
				if (ParseConstant(Input, Cursor, &Constant))
				{	
					Opt = zeOptions->GetOption(Section, Name);
					if (Opt != NULL)
						if (Opt->GetValueType() == Constant.GetType())
						{
							Opt->SetValue(Constant);
							State = PARSERSTATE_NEXTCOMMAND;
						}
						else
						{
							Output("[Parser] Error : Option value type does not match with the assignment type.\r\n");
							State = PARSERSTATE_ERROR;
						}
					else
					{
						Output("[Parser] Error : There is no such an option or command named \"%s\".\r\n", Name);
						State = PARSERSTATE_ERROR;
					}
				}
				else
					State = PARSERSTATE_ERROR;
				break;

			case PARSERSTATE_DISPLAY:
				Opt = zeOptions->GetOption(Section, Name);
				if (Opt != NULL)
				{
					switch(Opt->GetValueType())
					{
						case ZEVARIANTTYPE_UNDEFINED:
							Output("%s.%s = UNDEFINED (Probably an error)\r\n");
							break;
						case ZEVARIANTTYPE_INTEGER:
							Output("%s.%s = %d\r\n", Section, Name, Opt->GetValue().GetInteger());
							break;
						case ZEVARIANTTYPE_FLOAT:
							Output("%s.%s = %lf\r\n", Section, Name, Opt->GetValue().GetFloat());
							break;
						case ZEVARIANTTYPE_BOOLEAN:
							Output("%s.%s = %s\r\n", Section, Name, Opt->GetValue().GetBoolean() ? "true" : "false");
							break;
						case ZEVARIANTTYPE_STRING:
							Output("%s.%s = \"%s\"\r\n", Section, Name, Opt->GetValue().GetString());
							break;
						case ZEVARIANTTYPE_NULL:
							Output("%s.%s = NULL\r\n");
							break;
						default:
							Output("Internal error not supported variant type.");
							break;
					}
					State = PARSERSTATE_NEXTCOMMAND;
					return true;
				}
				else
				{
					Output("[Parser] Wrong option name.\r\n");
					return false;
				}
				break;

			case PARSERSTATE_NEXTCOMMAND:
					Output("\r\n");
			case PARSERSTATE_FINISHED:
				return true;
				break;
		}
	}
	return false;		
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

#include <crtdbg.h>
void ZEConsole::Log(const char* Module, const char* Format, ...)
{
	char Buffer[32768];
	va_list vlist;
	va_start(vlist, Format);
	vsprintf_s(Buffer, 32768, Format, vlist);
	va_end(vlist);
	
	char Buffer2[32768];
	sprintf_s(Buffer2, 32768, "[%s] %s", Module, Buffer);

	char* HistBuffer = new char[strlen(Buffer2) + 1];
	strncpy(HistBuffer, Buffer2, strlen(Buffer2) + 1);  
	OutputHistory.Add(HistBuffer);
	if (ConsoleInterface != NULL)
		ConsoleInterface->Output(Buffer2);
	
	if (!_CrtCheckMemory())
		OutputDebugString("Jackpot");
#ifdef ZEDEBUG_ENABLED
	OutputDebugString(Buffer2);
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
#ifdef ZEDEBUG_ENABLED
	OutputDebugString(Buffer);
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

ZEConsole* ZEConsole::GetInstance()
{
	return ZECore::GetInstance()->GetConsole();
}

ZEConsole::ZEConsole()
{
	Visible = false;
	Instance = this;
	ConsoleInterface = NULL;
}

ZEConsole::~ZEConsole()
{
	Instance = NULL;
}
