//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELogSession.cpp
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

#include "ZELogSession.h"

#include "ZEVersion.h"
#include "ZETimeStamp.h"
#include "ZEDS\ZEFormat.h"
#include "ZEFile\ZEPathManager.h"
#include "ZEFile\ZEFileInfo.h"

#ifdef ZE_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

ZESize ZELogSession::GetSessionID() const
{
	return SessionID;
}

ZESize ZELogSession::GetThreadID() const
{
	return ThreadID;
}

void ZELogSession::SetMinimumLevel(ZELogType Level)
{
	MinimumLevel = Level;
}

ZELogType ZELogSession::GetMinimumLevel() const
{
	return MinimumLevel;
}

void ZELogSession::SetBlock(bool Block)
{
	this->Block = Block;

}
bool ZELogSession::GetBlock() const
{
	return Block;
}

void ZELogSession::SetSink(bool Sink)
{
	this->Sink = Sink;
}
bool ZELogSession::GetSink() const
{
	return Sink;
}

void ZELogSession::SetCallback(ZELogCallback Callback)
{
	this->Callback = Callback;
}

ZELogCallback ZELogSession::GetCallback() const
{
	return Callback;
}

void ZELogSession::SetCallbackParameter(void* Parameter)
{
	CallbackParameter = Parameter;
}
void* ZELogSession::GetCallbackParameter() const
{
	return CallbackParameter;
}

const ZEString& ZELogSession::GetLogFileName() const
{
	return LogFileName;
}

void ZELogSession::SetLogFileMinLevel(ZELogType Level)
{
	LogFileMinLevel = Level;
}

ZELogType ZELogSession::GetLogFileMinLevel() const
{
	return LogFileMinLevel;
}

bool ZELogSession::OpenLogFile(const ZEString& FileName, const ZEString& Header, bool Append)
{
	ZERealPath Path = ZEPathManager::GetInstance()->TranslateToRealPath(FileName);
	if ((Path.Access & ZE_PA_WRITE) == 0)
		return false;

	const char* Mode = "w";
	if (Append)
		Mode = "a";

	LogFile = fopen(Path.Path, "a");
	if (LogFile == NULL)
		return false;

	LogFileName = FileName;
	fprintf((FILE*)LogFile, "%s\n\n", Header.ToCString());

	return true;
}

void ZELogSession::CloseLogFile()
{
	if (LogFile == NULL)
		return;

	fclose((FILE*)LogFile);
	LogFile = NULL;
}

void ZELogSession::BeginSession()
{
	if (SessionID != 0)
		return;

	ZELog::GetInstance()->BeginSession(this);
}

void ZELogSession::EndSession()
{
	CloseLogFile();

	if (SessionID == 0)
		return;
	
	ZELog::GetInstance()->EndSession();
}

ZELogSession::ZELogSession() : Link(this)
{
	SessionID = 0;
	ThreadID = 0;
	MinimumLevel = ZE_LOG_INFO;
	Sink = false;
	Block = false;
	LogFileMinLevel = ZE_LOG_INFO;
	LogFile = NULL;
	Callback = NULL;
	CallbackParameter = NULL;
}

ZELogSession::~ZELogSession()
{
	EndSession();
}

ZEString ZELogSession::GenerateLogFileName(const ZEString& BaseName)
{
	return ZEFormat::Format("{0}-{1}.log", BaseName, ZETimeStamp::Now().ToString("%Y%m%d%H%M"));
}

ZEString ZELogSession::GenerateLogFileHeader(const ZEString& ApplicationName)
{
	char ComputerName[256];
	DWORD Size = sizeof(ComputerName);
	GetComputerNameA(ComputerName, &Size);

	ZEFileInfo ExeFileInfo;
	if (ApplicationName.IsEmpty())
	{
		char ExeFileName[MAX_PATH];
		GetModuleFileName(NULL, ExeFileName, MAX_PATH);
		ExeFileInfo.SetPath(ExeFileName);	
	}

	return ZEFormat::Format( 
		" Zinek {0} Log File \n"
		"--------------------------------------------------------------------\n"
		"  Version       : {1}\n"
		"  Application   : {0}\n"
		"  Computer Name : {2}\n"
		"  Time Stamp    : {3}\n"
		"--------------------------------------------------------------------",
		(!ApplicationName.IsEmpty() ? ApplicationName : ExeFileInfo.GetName()),
		ZEVersion::GetZinekVersion().GetLongString().ToCString(),
		ComputerName,
		ZETimeStamp::Now().ToString());
}
