//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETerminalUtils.cpp
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

#include "ZETerminalUtils.h"

#include "ZEPlatform.h"

#ifdef ZE_PLATFORM_WINDOWS

static bool Bold;
static ZETerminalColor ForegroundColor;
static ZETerminalColor BackgroundColor;


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void Update()
{
	WORD Attributes = 0;

	if (Bold)
		Attributes |= FOREGROUND_INTENSITY;

	switch (ForegroundColor)
	{
		default:
		case ZE_TC_DEFAULT:
		case ZE_TC_GRAY:
		case ZE_TC_DARK_GRAY:
			Attributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case ZE_TC_BLACK:
			break;

		case ZE_TC_RED:
			Attributes |= FOREGROUND_RED;
			break;

		case ZE_TC_GREEN:
			Attributes |= FOREGROUND_GREEN;
			break;

		case ZE_TC_BLUE:
			Attributes |= FOREGROUND_BLUE;
			break;

		case ZE_TC_YELLOW:
			Attributes |= FOREGROUND_RED | FOREGROUND_GREEN;
			break;

		case ZE_TC_MEGENTA:
			Attributes |= FOREGROUND_RED | FOREGROUND_BLUE;
			break;

		case ZE_TC_CYAN:
			Attributes |= FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_RED:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_RED;
			break;

		case ZE_TC_LIGHT_GREEN:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_GREEN;
			break;

		case ZE_TC_LIGHT_BLUE:
			Attributes |= FOREGROUND_INTENSITY |  FOREGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_YELLOW:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
			break;

		case ZE_TC_LIGHT_MEGENTA:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_CYAN:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case ZE_TC_WHITE:
			Attributes |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
	}

	switch (BackgroundColor)
	{
		default:
		case ZE_TC_DEFAULT:
		case ZE_TC_BLACK:
			break;

		case ZE_TC_GRAY:
		case ZE_TC_DARK_GRAY:
			Attributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;

		case ZE_TC_RED:
			Attributes |= BACKGROUND_RED;
			break;

		case ZE_TC_GREEN:
			Attributes |= BACKGROUND_GREEN;
			break;

		case ZE_TC_BLUE:
			Attributes |= BACKGROUND_BLUE;
			break;

		case ZE_TC_YELLOW:
			Attributes |= BACKGROUND_RED | BACKGROUND_GREEN;
			break;

		case ZE_TC_MEGENTA:
			Attributes |= BACKGROUND_RED | BACKGROUND_BLUE;
			break;

		case ZE_TC_CYAN:
			Attributes |= BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_RED:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_RED;
			break;

		case ZE_TC_LIGHT_GREEN:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_GREEN;
			break;

		case ZE_TC_LIGHT_BLUE:
			Attributes |= BACKGROUND_INTENSITY |  BACKGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_YELLOW:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
			break;

		case ZE_TC_LIGHT_MEGENTA:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
			break;

		case ZE_TC_LIGHT_CYAN:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;

		case ZE_TC_WHITE:
			Attributes |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
}

#elif defined(ZE_PLATFORM_UNIX)

#include <stdio.h>

void ZETerminalUtils::Update()
{
	if (Bold)
		printf("\x1B[1m")
	else
		printf("\x1B[21m");

	if (Underlined)
		printf("\x1B[4m")
	else
		printf("\x1B[24m");

	if (Blink)
		printf("\x1B[5m")
	else
		printf("\x1B[25m");

	switch (ForegroundColor)
	{
		default:
		case ZE_TC_DEFAULT:
			printf("\x1B[39m");
			break;

		case ZE_TC_BLACK:
			printf("\x1B[30m");
			break;

		case ZE_TC_RED:
			printf("\x1B[31m");
			break;

		case ZE_TC_GREEN:
			printf("\x1B[32m");
			break;

		case ZE_TC_BLUE:
			printf("\x1B[34m");
			break;

		case ZE_TC_YELLOW:
			printf("\x1B[33m");
			break;

		case ZE_TC_MEGENTA:
			printf("\x1B[35m");
			break;

		case ZE_TC_CYAN:
			printf("\x1B[36m");
			break;

		case ZE_TC_GRAY:
			printf("\x1B[37m");
			break;

		case ZE_TC_DARK_GRAY:
			printf("\x1B[90m");
			break;

		case ZE_TC_LIGHT_RED:
			printf("\x1B[91m");
			break;

		case ZE_TC_LIGHT_GREEN:
			printf("\x1B[92m");
			break;

		case ZE_TC_LIGHT_BLUE:
			printf("\x1B[94m");
			break;

		case ZE_TC_LIGHT_YELLOW:
			printf("\x1B[393m");
			break;

		case ZE_TC_LIGHT_MEGENTA:
			printf("\x1B[95m");
			break;

		case ZE_TC_LIGHT_CYAN:
			printf("\x1B[96m");
			break;

		case ZE_TC_WHITE:
			printf("\x1B[97m");
			break;
	}

	switch (BackgroundColor)
	{
		default:
		case ZE_TC_DEFAULT:
			printf("\x1B[49m");
			break;

		case ZE_TC_BLACK:
			printf("\x1B[40m");
			break;

		case ZE_TC_RED:
			printf("\x1B[41m");
			break;

		case ZE_TC_GREEN:
			printf("\x1B[42m");
			break;

		case ZE_TC_BLUE:
			printf("\x1B[44m");
			break;

		case ZE_TC_YELLOW:
			printf("\x1B[43m");
			break;

		case ZE_TC_MEGENTA:
			printf("\x1B[45m");
			break;

		case ZE_TC_CYAN:
			printf("\x1B[46m");
			break;

		case ZE_TC_GRAY:
			printf("\x1B[47m");
			break;

		case ZE_TC_DARK_GRAY:
			printf("\x1B[100m");
			break;

		case ZE_TC_LIGHT_RED:
			printf("\x1B[101m");
			break;

		case ZE_TC_LIGHT_GREEN:
			printf("\x1B[102m");
			break;

		case ZE_TC_LIGHT_BLUE:
			printf("\x1B[104m");
			break;

		case ZE_TC_LIGHT_YELLOW:
			printf("\x1B[393m");
			break;

		case ZE_TC_LIGHT_MEGENTA:
			printf("\x1B[105m");
			break;

		case ZE_TC_LIGHT_CYAN:
			printf("\x1B[106m");
			break;

		case ZE_TC_WHITE:
			printf("\x1B[107m");
			break;
	}
}

#endif

void ZETerminalUtils::SetBold(bool Enabled)
{
	#ifdef ZE_PLATFORM_WINDOWS
		Bold = Enabled;
		Update();
	#elif defined(ZE_PLATFORM_UNIX)
		if (Enabled)
			printf("\x1B[1m")
		else
			printf("\x1B[21m");
	#endif
}

void ZETerminalUtils::SetUnderlined(bool Enabled)
{
	#ifdef ZE_PLATFORM_UNIX
		if (Enabled)
			printf("\x1B[1m")
		else
			printf("\x1B[21m");
	#endif
}


void ZETerminalUtils::SetBlink(bool Enabled)
{
	#ifdef ZE_PLATFORM_UNIX
		if (Blink)
			printf("\x1B[5m")
		else
			printf("\x1B[25m");
	#endif
}

void ZETerminalUtils::SetForgroundColor(ZETerminalColor Color)
{
	#ifdef ZE_PLATFORM_WINDOWS
		ForegroundColor = Color;
		Update();
	#elif defined ZE_PLATFORM_UNIX
		switch (Color)
		{
			default:
			case ZE_TC_DEFAULT:
				printf("\x1B[39m");
				break;

			case ZE_TC_BLACK:
				printf("\x1B[30m");
				break;

			case ZE_TC_RED:
				printf("\x1B[31m");
				break;

			case ZE_TC_GREEN:
				printf("\x1B[32m");
				break;

			case ZE_TC_BLUE:
				printf("\x1B[34m");
				break;

			case ZE_TC_YELLOW:
				printf("\x1B[33m");
				break;

			case ZE_TC_MEGENTA:
				printf("\x1B[35m");
				break;

			case ZE_TC_CYAN:
				printf("\x1B[36m");
				break;

			case ZE_TC_GRAY:
				printf("\x1B[37m");
				break;

			case ZE_TC_DARK_GRAY:
				printf("\x1B[90m");
				break;

			case ZE_TC_LIGHT_RED:
				printf("\x1B[91m");
				break;

			case ZE_TC_LIGHT_GREEN:
				printf("\x1B[92m");
				break;

			case ZE_TC_LIGHT_BLUE:
				printf("\x1B[94m");
				break;

			case ZE_TC_LIGHT_YELLOW:
				printf("\x1B[393m");
				break;

			case ZE_TC_LIGHT_MEGENTA:
				printf("\x1B[95m");
				break;

			case ZE_TC_LIGHT_CYAN:
				printf("\x1B[96m");
				break;

			case ZE_TC_WHITE:
				printf("\x1B[97m");
				break;
		}
	#endif
}

void ZETerminalUtils::SetBackgroundColor(ZETerminalColor Color)
{
	if (BackgroundColor == Color)
		return;

	BackgroundColor = Color;	
	Update();
}

void ZETerminalUtils::SetCursorPosition(ZEUInt x, ZEUInt y)
{
	#ifdef ZE_PLATFORM_WINDOWS
		COORD CursorPos;
		CursorPos.X = x;
		CursorPos.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPos);
	#endif
}

void ZETerminalUtils::GetCursorPosition(ZEUInt& x, ZEUInt& y)
{
	#ifdef ZE_PLATFORM_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO Info;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Info);
		x = Info.dwCursorPosition.X;
		y = Info.dwCursorPosition.Y;
	#endif
}

void ZETerminalUtils::SetCursorPositionX(ZEUInt x)
{
	ZEUInt Temp, y;
	GetCursorPosition(Temp, y);
	SetCursorPosition(x, y);
}

ZEUInt ZETerminalUtils::GetCursorPositionX()
{
	ZEUInt x, y;
	GetCursorPosition(x, y);
	return x;
}

void ZETerminalUtils::SetCursorPositionY(ZEUInt y)
{
	ZEUInt x, Temp;
	GetCursorPosition(x, Temp);
	SetCursorPosition(x, y);
}

ZEUInt ZETerminalUtils::GetCursorPositionY()
{
	ZEUInt x, y;
	GetCursorPosition(x, y);
	return y;
}

void ZETerminalUtils::Reset()
{
	#ifdef ZE_PLATFORM_WINDOWS
		Bold = false;
		ForegroundColor = ZE_TC_DEFAULT;
		BackgroundColor = ZE_TC_DEFAULT;
		Update();
	#else
		printf("\e[0m");
	#endif
}
