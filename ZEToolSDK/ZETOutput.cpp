//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETOutput.cpp
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

#include "output.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void (*Out)(const char*) = NULL;

#ifdef ZESDK_DEBUG_ENABLED
ZESDKOutputLevel OutputLevel = ZESDK_OL_DEBUG;
#else
ZESDKOutputLevel OutputLevel = ZESDK_OL_WARNINGS;
#endif

void ZESDKOutput::SetOutputCallback(void (*OutputCallback)(const char*) )
{
	Out = OutputCallback;
}

void ZESDKOutput::SetOutputLevel(ZESDKOutputLevel Level)
{
	OutputLevel = Level;
}

void ZESDKOutput::Output(const char* Text)
{
	if (Out != NULL)
		Out(Text);
}

const char* GetOutputLevelString(ZESDKOutputLevel Level)
{
	switch(Level)
	{
		case ZESDK_OL_ERROR:
			return "Error";
		case ZESDK_OL_WARNINGS:
			return "Warning";
		case ZESDK_OL_LOG:
			return "Log";
		case ZESDK_OL_NOTICE:
			return "Notice";
		case ZESDK_OL_DEBUG:
			return "Debug Output";
		default:
			return "";
	}
}

void ZESDKOutput::Output(const char* Module, const char* Format, ...)
{
	if (Out != NULL && OutputLevel != ZESDK_OL_NOOUTPUT)
	{
		char Buffer[1024];
		char Buffer2[1024];
		va_list VList;
		va_start(VList, Format);
		vsprintf_s(Buffer, 1024, Format, VList);
		sprintf_s(Buffer2, 1024, "[%s] : %s", Module, Buffer);
		va_end(VList);

		Out(Buffer2);
	}
}

void ZESDKOutput::Output(const char* Module, ZESDKOutputLevel Level, const char* Format, ...)
{
	if (Out != NULL && Level <= OutputLevel)
	{
		char Buffer[1024];
		char Buffer2[1024];
		va_list VList;
		va_start(VList, Format);
		vsprintf_s(Buffer, 1024, Format, VList);
		va_end(VList);
		sprintf_s(Buffer2, 1024, "[%s] %s : %s\r\n", Module, GetOutputLevelString(Level), Buffer);
		Out(Buffer2);
	}
}
void ZESDKOutput::DebugOutput(const char* File, const char* Function, int Line, bool Warning, const char* Format, ...)
{
	if (Out != NULL && ZESDK_OL_DEBUG <= OutputLevel)
	{
		char Buffer[1024];

		va_list VList;
		va_start(VList, Format);
		vsprintf_s(Buffer, 1024, Format, VList);
		va_end(VList);

		Output("%s : %s (File : \"%s\", Function : \"%s\", Line : %d)\r\n", (Warning ? "Debug Warning" : "Debug Error"), Buffer, Function, File, Line);
	}
}
